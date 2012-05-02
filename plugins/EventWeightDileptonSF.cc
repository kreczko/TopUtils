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
        sf = 1.0080;
      else if(pt<40.)
        sf = 1.0061;	
      else if(pt<50.)
        sf = 1.0060; 
      else
        sf = 1.0016; 	   
    } else if(fabs(eta)<=1.6){
      if(pt<30.)
        sf = 1.0057;
      else if(pt<40.)
        sf = 1.0175;	
      else if(pt<50.)
        sf = 1.0126; 
      else
        sf = 1.0046;         
    } else if(fabs(eta)<=2.4){
      if(pt<30.)
        sf = 1.0603;
      else if(pt<40.)
        sf = 1.0330;	
      else if(pt<50.)
        sf = 1.0160; 
      else
        sf = 1.0081;         
    }  
  } else if(lep.isMuon()) {
    if(fabs(eta)<=1.4){
      if(pt<30.)
        sf = 0.9963;
      else if(pt<40.)
        sf = 0.9936;	
      else if(pt<50.)
        sf = 0.9912; 
      else
        sf = 0.9852; 	   
    } else if(fabs(eta)<=1.6){
      if(pt<30.)
        sf = 0.9841;
      else if(pt<40.)
        sf = 0.9806;	
      else if(pt<50.)
        sf = 0.9772; 
      else
        sf = 0.9709;         
    } else if(fabs(eta)<=2.4){
      if(pt<30.)
        sf = 0.9918;
      else if(pt<40.)
        sf = 0.9820;	
      else if(pt<50.)
        sf = 0.9779; 
      else
        sf = 0.9646;         
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
