#include "TopAnalysis/TopUtils/plugins/GenFinalStateSelector.h"
#include "AnalysisDataFormats/TopObjects/interface/TopGenEvent.h"


/// constructor
GenFinalStateSelector::GenFinalStateSelector(const edm::ParameterSet& cfg) :
  src_( cfg.getParameter<edm::InputTag>("src"))
{
  cfg.existsAs<int >("muons" ) ?  muons_ =cfg.getParameter<int >("muons" ) : -1;
  cfg.existsAs<int >("elecs" ) ?  elecs_ =cfg.getParameter<int >("elecs" ) : -1;
  cfg.existsAs<bool>("invert") ?  invert_=cfg.getParameter<bool>("invert") : false;
}

/// destructor
GenFinalStateSelector::~GenFinalStateSelector()
{
}

/// all that needs to done during the event loop
bool
GenFinalStateSelector::filter(edm::Event& event, const edm::EventSetup& setup)
{
  // recieve particle listing
  edm::Handle<edm::View<reco::GenParticle> > src; 
  event.getByLabel(src_, src);

  // major loop in the event; all information
  // needed for the selection is collected here.
  int iElec=0,iMuon=0;
  for(edm::View<reco::GenParticle>::const_iterator top=src->begin(); top!=src->end(); ++top){
    if( abs(top->pdgId())==TopDecayID::tID ){
      for(reco::GenParticle::const_iterator td=top->begin(); td!=top->end(); ++td){
	if( abs(td->pdgId())==TopDecayID::WID ){
	  for(reco::GenParticle::const_iterator wd=td->begin(); wd!=td->end(); ++wd){
	    if( abs(wd->pdgId())==TopDecayID::elecID ){
	      ++iElec;
	    }
	    if( abs(wd->pdgId())==TopDecayID::muonID ){
	      ++iMuon;
	    }
	    if( abs(wd->pdgId())==TopDecayID::tauID  ){ 
              for(reco::GenParticle::const_iterator taud=wd->begin()->begin(); taud!=wd->begin()->end(); ++taud){
	  	if( abs(taud->pdgId())==TopDecayID::elecID ){
	          ++iElec;
	        }
	        if( abs(taud->pdgId())==TopDecayID::muonID ){
	          ++iMuon;
	        }
              }
	    }
	  }
	}
      }
    }
  }
  bool accept=(elecs_>=0 ? iElec>=elecs_:true) && (muons_>=0 ? iMuon>=muons_:true);
  //if(( (!invert_ && accept) || (!(!invert_)&& !accept) ) ){
  //  std::cout << "iElec  = " << iElec << "(" << elecs_ << ")" << std::endl;
  //  std::cout << "iMuon  = " << iMuon << "(" << muons_ << ")" << std::endl;
  //}
  return ( (!invert_ && accept) || (!(!invert_)&& !accept) ); 
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenFinalStateSelector);
