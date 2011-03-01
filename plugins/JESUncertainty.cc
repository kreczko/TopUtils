#include "TopAnalysis/TopUtils/plugins/JESUncertainty.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "FWCore/Framework/interface/EventSetup.h"

JESUncertainty::JESUncertainty(const edm::ParameterSet& cfg):
  inputJets_           (cfg.getParameter<edm::InputTag>("inputJets"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           )),
  bScaleUnc_           (cfg.getParameter<double>       ("bScaleUncertainty"   )),
  puUnc_               (cfg.getParameter<double>       ("puUncertainty"       )),  
  swUnc_               (cfg.getParameter<double>       ("swUncertainty"       )),
  direction_           (cfg.getParameter<bool>         ("direction"           ))
{  
  // register products
  produces<std::vector<pat::Jet> >("scaledJets");   
  produces<std::vector<pat::MET> >("scaledMETs"); 
}


void
JESUncertainty::beginJob()
{ 	
}


void
JESUncertainty::produce(edm::Event& event, const edm::EventSetup& setup)
{
  edm::Handle<std::vector<pat::Jet> > jets;
  event.getByLabel(inputJets_, jets);

  edm::Handle<std::vector<pat::MET> > mets;
  event.getByLabel(inputMETs_, mets);

  edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
  setup.get<JetCorrectionsRecord>().get("AK5PF",JetCorParColl);
  JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];
  JetCorrectionUncertainty *jecUnc = new JetCorrectionUncertainty(JetCorPar);	 

  std::auto_ptr<std::vector<pat::Jet> > pJets(new std::vector<pat::Jet>);
  std::auto_ptr<std::vector<pat::MET> > pMETs(new std::vector<pat::MET>);
  
  pat::MET met = *(mets->begin());
  double scaledMETPx = met.px();
  double scaledMETPy = met.py();
  double scaledMETEt = met.et(); 

  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
    pat::Jet scaledJet = *jet;
    
    jecUnc->setJetEta(scaledJet.eta());
    jecUnc->setJetPt(scaledJet.pt() );
     
    scaledMETPx += scaledJet.correctedJet("RAW").px();
    scaledMETPy += scaledJet.correctedJet("RAW").py();
    scaledMETEt += scaledJet.correctedJet("RAW").et();
     
    double unc = jecUnc->getUncertainty(direction_);   
       
    unc = sqrt(unc * unc + bScaleUnc_ * bScaleUnc_);
    unc = sqrt(unc * unc + puUnc_     * puUnc_    );     
    unc = sqrt(unc * unc + swUnc_     * swUnc_    );   
             
    double ptscaleunc = 1.;
    if(direction_) ptscaleunc = 1. + unc;
    else ptscaleunc = 1.- unc;
     
    scaledJet.scaleEnergy(ptscaleunc);
    
    scaledJet.correctedJet("RAW").scaleEnergy(ptscaleunc);
    scaledMETPx -= scaledJet.correctedJet("RAW").px();
    scaledMETPy -= scaledJet.correctedJet("RAW").py();
    scaledMETEt -= scaledJet.correctedJet("RAW").et();

    pJets->push_back( scaledJet ); 
  }
    
  pat::MET scaledMET(reco::MET(scaledMETEt, reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)), reco::MET::Point(0,0,0)));
    
  event.put(pJets, "scaledJets");    
  event.put(pMETs, "scaledMETs");
}


