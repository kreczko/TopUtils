//#include <iostream>
//#include <algorithm>
#include "TopAnalysis/TopUtils/plugins/GenCandSelector.h"

GenCandSelector::GenCandSelector(const edm::ParameterSet& cfg):
  src_( cfg.getParameter<edm::InputTag>("src") )
{
  // buffer for string->std::pair<int,int> conversion from target/ancestor 
  // configuration 
  std::vector<std::string> buffer;//, bufferIgnore;

  // configure target particle(s); daughterIds_ is filled with pdgIds of 
  // target particle(s) and potential daughter particle(s) if existing; if
  // no ':' is found in the configruation string the second list element 
  // is set to '0'; '0' is a wildcart
  edm::ParameterSet target=cfg.getParameter<edm::ParameterSet>("target");
  buffer = target.getParameter<std::vector<std::string> >("pdgId" );
  for(std::vector<std::string>::const_iterator pdgId=buffer.begin(); pdgId!=buffer.end(); ++pdgId){
    daughterIds_.push_back(std::make_pair(atoi(firstElement(*pdgId).c_str()), secondElement(*pdgId).empty()?0:atoi(secondElement(*pdgId).c_str())) );
  }

  // configure status of the target particle
  status_= target.getParameter<int>("status");
  
  // configure ancestor particle(s); ancestorIds_ is filled with pdgIds 
  // of ancestor particle(s) and their further ancestor particle(s) if 
  // existing. If no ':' is found in the configuration string the first 
  // list element is set to '0'; '0' is a wildcart
  edm::ParameterSet ancestor=cfg.getParameter<edm::ParameterSet>("ancestor");
  buffer = ancestor.getParameter<std::vector<std::string> >("pdgId" );
  for(std::vector<std::string>::const_iterator pdgId=buffer.begin(); pdgId!=buffer.end(); ++pdgId){
    ancestorIds_.push_back(std::make_pair(secondElement(*pdgId).empty()?0:atoi(firstElement(*pdgId).c_str()), secondElement(*pdgId).empty()?atoi(firstElement(*pdgId).c_str()):atoi(secondElement(*pdgId).c_str())) );
  }

  // configure excluded ancestor particle(s); ancestorIgnoreIds_ is filled with pdgIds 
  // of ancestor particle(s) and their further ancestor particle(s) if 
  // existing. If no ':' is found in the configuration string the first 
  // list element is set to '0'; '0' is a wildcart
  //edm::ParameterSet ancestorIgnore=cfg.getParameter<edm::ParameterSet>("ancestorIgnore");
  //bufferIgnore = ancestorIgnore.getParameter<std::vector<std::string> >("pdgId" );
  //for(std::vector<std::string>::const_iterator pdgId=bufferIgnore.begin(); pdgId!=bufferIgnore.end(); ++pdgId){
  //  ancestorIgnoreIds_.push_back(std::make_pair(secondElement(*pdgId).empty()?0:atoi(firstElement(*pdgId).c_str()), secondElement(*pdgId).empty()?atoi(firstElement(*pdgId).c_str()):atoi(secondElement(*pdgId).c_str())) );
  //}

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
    if( contained(daughterIds_.begin(), daughterIds_.end(), &(*p)) ){
      if(descendant(daughterIds_.begin(), daughterIds_.end(), &(*p))){
	if(p->numberOfMothers()>0){
	  if( ancestor( ancestorIds_.begin(), ancestorIds_.end(), p->mother()) ){
	    if(p->status()==status_){
	      //if( !ancestor( ancestorIgnoreIds_.begin(), ancestorIgnoreIds_.end(), p->mother()) ){
		print(&(*p)); out->push_back(*p);
		//}
	    }
	  }
	}
      }
    }
  }
  // push out vector into the event
  evt.put(out);
}

bool 
GenCandSelector::descendant(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p) const 
{
  int index=find(first, last, p);
  if( index<(last-first) ){
    if((first+index)->second == 0){ 
      // found save element; note that here the 
      // second elements of the list are checked
      return true; 
    }
    else if( p->numberOfDaughters() ){
      for(unsigned int i=0; i<p->numberOfDaughters(); ++i){
	if( contained(first, last, p) && contained(first, last, p->daughter(i), false) ){ 	  
	  // catch as part of the daughter list 
	  return true;
	}
      }
    }
    else{
      // if element is not save (i.e. a special
      // daughter is still required) drop down
      for(unsigned int i=0; i<p->numberOfDaughters(); ++i){
	if(descendant(first, last, p->daughter(i))){
	  return true;
	}
      }
    }
  }
  // no success go home...
  return false;
}

bool 
GenCandSelector::ancestor(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p) const 
{
  int index=find(first, last, p, false);
  if( abs(p->pdgId())>99 ){
    // this is a no go; if we arrive here p is
    // hadron or the the daughter of a hadron
    return false;
  }
  if( index<(last-first) ){
    if((first+index)->first == 0){ 
      // found save element; note that here the 
      // first elements of the list are checked 
      return true; 
    }
    else{
      // if element is not save (i.e. a special 
      // ancestor is still required) bubble up
      for(unsigned int i=0; i<p->numberOfMothers(); ++i){
	// does p have an adaequte mother? Other
	// wise bubble further up
	if(abs(p->mother(i)->pdgId()) == (first+index)->first){//&&p->mother(i)->status()==3){
	  return true;
	}
	else{
	  if(ancestor(first, last, p->mother(i))) return true;
	}
      }
    }
  }
  else{
    // if p itself is not the required ancestor
    // particle bubble up (recursion) 
    for(unsigned int i=0; i<p->numberOfMothers(); ++i){
      return ancestor(first, last, p->mother(i));
    }
  }
  // no success go home...
  return false;
}

void 
GenCandSelector::print(const reco::Candidate* p) const
{
  edm::LogVerbatim log("GenCandSelector");
  log << "-----------------------------------------------------" << "\n"
      << ">> found object that fits to requirements:         <<" << "\n"
      << "-----------------------------------------------------" << "\n";
  if(p->mother()->numberOfMothers()>0){
    log << "ancestors : ";
    MotherPrinter(log, p->mother());
  }
  log << "\n";
  log << "mother   : " << p->mother()->pdgId() << "(status " << p->mother()->status() << ")\n"
      << "part     : " << p->pdgId() << " [status " << p->status() << "]\n";
  if(p->begin()!=p->end()){
    log	<< "daugther : ";
    for(reco::GenParticle::const_iterator d=p->begin(); d!=p->end(); ++d){
      log << d->pdgId() << " [";
      for(reco::GenParticle::const_iterator dd=d->begin(); dd!=d->end(); ++dd){
	log << dd->pdgId() << ",";
      }
      log << "], ";
    } log << "\n";
  }
}

void 
GenCandSelector::MotherPrinter(edm::LogVerbatim& log, const reco::Candidate* p) const
{
  if(p->numberOfMothers()>0){
    log << ", " << p->mother()->pdgId() << "(status " << p->mother()->status() << ")";
    if(p->mother()->numberOfMothers()>0) MotherPrinter(log, p->mother());
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenCandSelector);
