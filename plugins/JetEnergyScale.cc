#include "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "FWCore/Framework/interface/Event.h"

JetEnergyScale::JetEnergyScale(const edm::ParameterSet& cfg):
  inputJets_           (cfg.getParameter<edm::InputTag>("inputJets"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           )),
  scaleFactor_         (cfg.getParameter<double>       ("scaleFactor"         )),
  scaleType_           (cfg.getParameter<std::string>  ("scaleType"           )),  
  jetPTThresholdForMET_(cfg.getParameter<double>       ("jetPTThresholdForMET")),
  jetEMLimitForMET_    (cfg.getParameter<double>       ("jetEMLimitForMET"    ))
{
  // use label of input to create label for output
  outputJets_ = inputJets_.label();
  outputMETs_ = inputMETs_.label(); 
  
  // register products
  produces<std::vector<pat::Jet> >(outputJets_);
  produces<std::vector<pat::MET> >(outputMETs_); 
}


void
JetEnergyScale::beginJob()
{ 
  // check if scaleType is ok
  if(scaleType_.compare("abs")!=0 && scaleType_.compare("rel")!=0){
    edm::LogError("JetEnergyScale") << "Unknown scaleType: " << scaleType_;
	throw cms::Exception("Configuration Error");
  }		
}


void
JetEnergyScale::produce(edm::Event& event, const edm::EventSetup& setup)
{
  edm::Handle<std::vector<pat::Jet> > jets;
  event.getByLabel(inputJets_, jets);

  edm::Handle<std::vector<pat::MET> > mets;
  event.getByLabel(inputMETs_, mets);
  
  std::auto_ptr<std::vector<pat::Jet> > pJets(new std::vector<pat::Jet>);
  std::auto_ptr<std::vector<pat::MET> > pMETs(new std::vector<pat::MET>);

  double dPx    = 0.;
  double dPy    = 0.;
  double dSumEt = 0.;

  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
  
    pat::Jet scaledJet = *jet;
    
    if(scaleType_.compare("abs")==0){
      scaledJet.scaleEnergy( scaleFactor_ );    
    }        
    else{
      scaledJet.scaleEnergy( fabs(scaledJet.eta())*scaleFactor_ );    
    } 
               
    pJets->push_back( scaledJet );   
    
    // consider jet scale shift only if the raw jet pt and emf 
    // is above the thresholds given in the module definition
    if((jet->isCaloJet() || jet->isJPTJet())
       && jet->correctedJet("raw").pt() > jetPTThresholdForMET_
       && jet->emEnergyFraction() < jetEMLimitForMET_) {
      dPx    += scaledJet.px() - jet->px();
      dPy    += scaledJet.py() - jet->py();
      dSumEt += scaledJet.et() - jet->et();
    }
  }

  // scale MET accordingly
  pat::MET met = *(mets->begin());
  double scaledMETPx = met.px() - dPx;
  double scaledMETPy = met.py() - dPy;
  pat::MET scaledMET(reco::MET(met.sumEt()+dSumEt, reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)), reco::MET::Point(0,0,0)));
  pMETs->push_back( scaledMET );
  event.put(pJets, outputJets_);
  event.put(pMETs, outputMETs_);
}
