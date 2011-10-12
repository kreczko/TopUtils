#include "TopAnalysis/TopUtils/plugins/EventWeightDileptonKinEffSF.h"

EventWeightDileptonKinEffSF::EventWeightDileptonKinEffSF(const edm::ParameterSet& cfg):
  electrons_ (cfg.getParameter<edm::InputTag>("electrons")),
  muons_     (cfg.getParameter<edm::InputTag>("muons")) 
{
  produces<double>("eventWeightDileptonKinEffSF");
}

EventWeightDileptonKinEffSF::~EventWeightDileptonKinEffSF()
{
}

double EventWeightDileptonKinEffSF::getLeptonSF(const reco::Candidate& lep){
  double sf  = 1.;
  double pt  = lep.pt();  

  if(pt<20.)
    sf = 1.;
  else if(pt<40.)
    sf = 0.9940;
  else if(pt<70.)
    sf = 0.9736;    
  else if(pt<120.)
    sf = 0.9728;
  else if(pt<180.)
    sf = 0.9499;     
  else
    sf = 0.8201;       

  return sf;
}

void EventWeightDileptonKinEffSF::produce(edm::Event& evt, const edm::EventSetup& setup)
{
    std::auto_ptr<double> eventWeight(new double);  

    edm::Handle<std::vector<pat::Electron> > electrons;
    evt.getByLabel(electrons_, electrons);    
    
    edm::Handle<std::vector<pat::Muon> > muons;
    evt.getByLabel(muons_, muons);

    *eventWeight = 1.;
    
    // consistency check: number of leptons should be 2
    if(electrons->size() + muons->size() != 2){
      edm::LogWarning ( "DileptonKinEffEventWeight" ) << "cannot unambiguously identify decay channel (wrong input collections?!)";      
    }
    
    for(std::vector<pat::Electron>::const_iterator el = electrons->begin(); el!= electrons->end(); ++el) {
      double elsf = getLeptonSF(*el);
      *eventWeight *= elsf;
    }    
    
    for(std::vector<pat::Muon>::const_iterator mu = muons->begin(); mu!= muons->end(); ++mu) {
      double musf = getLeptonSF(*mu);
      *eventWeight *= musf;  
    }
    
    *eventWeight = TMath::Sqrt(*eventWeight);
   
    evt.put(eventWeight,"eventWeightDileptonKinEffSF"); 
}
