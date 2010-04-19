#include <algorithm>
#include "TopAnalysis/TopUtils/plugins/GenCandSelector.h"

GenCandSelector::GenCandSelector(const edm::ParameterSet& cfg): ancestor_(0),
  src_( cfg.getParameter<edm::InputTag>("src") )
{
  // configure target particle
  edm::ParameterSet target=cfg.getParameter<edm::ParameterSet>("target");
  pdgId_ = target.getParameter<unsigned int>("pdgId" );
  status_= target.getParameter<unsigned int>("status");

  edm::ParameterSet mother=cfg.getParameter<edm::ParameterSet>("mother");
  pdgIds_ = mother.getParameter<std::vector<unsigned int> >("pdgIds" );
  if( mother.exists("ancestor") ){
    ancestor_= mother.getParameter<unsigned int>("ancestor");
  }
  // register output
  produces<std::vector<reco::GenParticle> >();
}

void
GenCandSelector::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // read collection from file
  edm::Handle<reco::GenParticleCollection> src; 
  evt.getByLabel(src_, src);
  
  // prepare output vector
  std::auto_ptr<reco::GenParticleCollection> out(new reco::GenParticleCollection);

  // loop input collection
  for(reco::GenParticleCollection::const_iterator p=src->begin(); p!=src->end(); ++p){
    if(abs(p->pdgId())==(int)pdgId_ && p->status()==(int)status_){
      // check whether pdgId of the first generation mother 
      // particle is part of the allowed pdgIds
      if( std::find( pdgIds_.begin(), pdgIds_.end(), (unsigned int)abs(p->begin()->pdgId()) )!=pdgIds_.end() ){
	if(ancestor_==0){ 
	  out->push_back(*p);
	}
	else{
	  // check whether p has an ancestor of type ancestor_
	  if( findAncestor(p->begin(), ancestor_) ){
	    out->push_back(*p);
	  }
	}
      }
    }  
  }
  // push out vector into the event
  evt.put(out);
}

// find ancestor ofr given type upstream the particle chain
bool 
GenCandSelector::findAncestor(reco::GenParticle::const_iterator part, int& type)
{
  if(part->begin()==part->end()){
    return false;
  }
  else{
    if(abs(part->begin()->pdgId())==type){
      return true;
    }
    else{
      return findAncestor(part->begin(), type);
    }
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenCandSelector);
