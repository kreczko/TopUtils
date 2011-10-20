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
  
  // for pt below 20 and abs(eta) > 2.5 no scale factors are available
  if(pt<20. && fabs(eta)>2.5)
    return sf;
  //SF taken from AN-11-225: numbers = scale factor identification*scale factor isolation
  if(lep.isElectron()){  
    if(fabs(eta)<=1.5){
      if(pt<30.)
        sf = 1.0094;
      else if(pt<40.)
        sf = 1.0027;	
      else if(pt<50.)
        sf = 0.9981; 
      else
        sf = 0.9990; 	   
    } else if(fabs(eta)<=2.5){
      if(pt<30.)
        sf = 1.0537;
      else if(pt<40.)
        sf = 1.0324;	
      else if(pt<50.)
        sf = 1.0110; 
      else
        sf = 1.0056;         
    }  
  } else if(lep.isMuon()) {
    if(fabs(eta)<=1.0){
      if(pt<30.)
        sf = 1.0030;
      else if(pt<40.)
        sf = 0.9945;	
      else if(pt<50.)
        sf = 0.9988; 
      else
        sf = 1.0022; 	   
    } else if(fabs(eta)<=2.1){
      if(pt<30.)
        sf = 0.9884;
      else if(pt<40.)
        sf = 0.9921;	
      else if(pt<50.)
        sf = 0.9901; 
      else
        sf = 0.9896;         
    } else if(fabs(eta)<=2.5){
      if(pt<30.)
        sf = 0.9870;
      else if(pt<40.)
        sf = 0.9811;	
      else if(pt<50.)
        sf = 0.9924; 
      else
        sf = 0.9947;         
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
