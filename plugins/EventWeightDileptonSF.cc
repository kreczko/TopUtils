#include "TopAnalysis/TopUtils/plugins/EventWeightDileptonSF.h"

EventWeightDileptonSF::EventWeightDileptonSF(const edm::ParameterSet& cfg):
  electrons_ (cfg.getParameter<edm::InputTag>("electrons")),
  muons_     (cfg.getParameter<edm::InputTag>("muons")) 
{
  produces<double>("eventWeightDileptonSF");
}

EventWeightDileptonSF::~EventWeightDileptonSF()
{
}

double EventWeightDileptonSF::getLeptonSF(const reco::Candidate& lep){
  double sf  = 1.;
  double pt  = lep.pt();
  double eta = lep.eta();
  
  // for pt below 20 and abs(eta) > 2.4 no scale factors are available
  if(pt<20. && fabs(eta)>2.4)
    return sf;
  //SF taken from AN-12-075: numbers = scale factor identification*scale factor isolation
  if(lep.isElectron()){  
    if(fabs(eta)<=1.4){
      if(pt<30.)
        sf = 0.9671;
      else if(pt<40.)
        sf = 0.9831;	
      else if(pt<50.)
        sf = 0.9924; 
      else
        sf = 0.9961; 	   
    } else if(fabs(eta)<=1.6){
      if(pt<30.)
        sf = 0.9546;
      else if(pt<40.)
        sf = 0.9896;	
      else if(pt<50.)
        sf = 0.9920; 
      else
        sf = 0.9957;         
    } else if(fabs(eta)<=2.4){
      if(pt<30.)
        sf = 1.0129;
      else if(pt<40.)
        sf = 0.9964;	
      else if(pt<50.)
        sf = 0.9941; 
      else
        sf = 0.9964;         
    }  
  } else if(lep.isMuon()) {
    if(fabs(eta)<=1.4){
      if(pt<30.)
        sf = 0.9898;
      else if(pt<40.)
        sf = 0.9915;	
      else if(pt<50.)
        sf = 0.9912; 
      else
        sf = 0.9847; 	   
    } else if(fabs(eta)<=1.6){
      if(pt<30.)
        sf = 0.9844;
      else if(pt<40.)
        sf = 0.9806;	
      else if(pt<50.)
        sf = 0.9761; 
      else
        sf = 0.9704;         
    } else if(fabs(eta)<=2.4){
      if(pt<30.)
        sf = 0.9822;
      else if(pt<40.)
        sf = 0.9786;	
      else if(pt<50.)
        sf = 0.9751; 
      else
        sf = 0.9597;         
    }    
  }    
  
  return sf;
}

void EventWeightDileptonSF::produce(edm::Event& evt, const edm::EventSetup& setup)
{
    std::auto_ptr<double> eventWeight(new double);  

    edm::Handle<std::vector<pat::Electron> > electrons;
    evt.getByLabel(electrons_, electrons);    
    
    edm::Handle<std::vector<pat::Muon> > muons;
    evt.getByLabel(muons_, muons);

    *eventWeight = 1.;
    
    // consistency check: number of leptons should be 2
    if(electrons->size() + muons->size() != 2){
      edm::LogWarning ( "DileptonEventWeight" ) << "cannot unambiguously identify decay channel (wrong input collections?!)";      
    }
    
    for(std::vector<pat::Electron>::const_iterator el = electrons->begin(); el!= electrons->end(); ++el) {
      double elsf = getLeptonSF(*el);
      *eventWeight *= elsf;
    }    
    
    for(std::vector<pat::Muon>::const_iterator mu = muons->begin(); mu!= muons->end(); ++mu) {
      double musf = getLeptonSF(*mu);
      *eventWeight *= musf;  
    }
   
    evt.put(eventWeight,"eventWeightDileptonSF"); 
}
