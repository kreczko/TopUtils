#include "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

JetEnergyScale::JetEnergyScale(const edm::ParameterSet& cfg):
  inputJets_           (cfg.getParameter<edm::InputTag>("inputJets"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           )),
  scaleFactor_         (cfg.getParameter<double>       ("scaleFactor"         )),
  jetPTthresholdForMET_(cfg.getParameter<double>       ("jetPTthresholdForMET")),
  jetEMlimitForMET_    (cfg.getParameter<double>       ("jetEMlimitForMET"    ))
{
  // use label of input to create label for output
  outputJets_ = inputJets_.label();
  outputMETs_ = inputMETs_.label();
  //convert first letter to upper case
  outputJets_.replace(0, 1, 1, (char)toupper(outputJets_[0]));
  outputMETs_.replace(0, 1, 1, (char)toupper(outputMETs_[0]));
  // put "scaled" at the beginning of the label
  outputJets_.insert(0, "scaled");
  outputMETs_.insert(0, "scaled");
  // register products
  produces<std::vector<pat::Jet> >(outputJets_);
  produces<std::vector<pat::MET> >(outputMETs_);
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
    scaledJet.scaleEnergy( scaleFactor_ );
    pJets->push_back( scaledJet );
    if(jet->originalObject()->pt() > jetPTthresholdForMET_
       && jet->emEnergyFraction() < jetEMlimitForMET_) {
      dPx    += scaledJet.px() - jet->px();
      dPy    += scaledJet.py() - jet->py();
      dSumEt += scaledJet.et() - jet->et();
    }
  }

  pat::MET met = *(mets->begin());
  double scaledMETPx = met.px() - dPx;
  double scaledMETPy = met.py() - dPy;
  pat::MET scaledMET = met;
  scaledMET.setP4( math::XYZTLorentzVector(scaledMETPx, scaledMETPy, 0,
					   sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)) );
  // scaledMET.setSumEt( met.sumEt() + dSumEt );
  pMETs->push_back( scaledMET );

  event.put(pJets, outputJets_);
  event.put(pMETs, outputMETs_);
}
