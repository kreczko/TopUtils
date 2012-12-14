#include <algorithm>

#include "FWCore/Framework/interface/Event.h"
#include "TopAnalysis/TopUtils/plugins/PFElectronGSFMomentum.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

PFElectronGSFMomentum::PFElectronGSFMomentum(const edm::ParameterSet& cfg):
  inputElectrons_      (cfg.getParameter<edm::InputTag>("inputElectrons"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           ))
{
  // use label of input to create label for output
  outputElectrons_ = inputElectrons_.label();
  outputMETs_ = inputMETs_.label(); 

  // register products
  produces<std::vector<pat::Electron> >(outputElectrons_);
  produces<std::vector<pat::MET> >(outputMETs_); 
}

void
PFElectronGSFMomentum::beginJob()
{ 
}

void
PFElectronGSFMomentum::produce(edm::Event& event, const edm::EventSetup& setup)
{
  // access jets
  edm::Handle<std::vector<pat::Electron> > electrons;
  event.getByLabel(inputElectrons_, electrons);
  // access MET
  edm::Handle<edm::View<pat::MET> > mets;
  event.getByLabel(inputMETs_, mets);
  
  // create two new collections for jets and MET
  std::auto_ptr<std::vector<pat::Electron> > pElectrons(new std::vector<pat::Electron>);
  std::auto_ptr<std::vector<pat::MET> > pMETs(new std::vector<pat::MET>);

  // loop ans rescale jets
  double dPx = 0., dPy = 0., dSumEt = 0.;
  for(std::vector<pat::Electron>::const_iterator electron=electrons->begin(); electron!=electrons->end(); ++electron){
    pat::Electron scaledElectron = *electron;


    pat::Electron::LorentzVector LVElectron = scaledElectron.ecalDrivenMomentum();
    
    std::cout<<"GSF E: "<<LVElectron.E()<<"GSF Px: "<<LVElectron.px()<<"GSF Py: "<<LVElectron.py()<<"GSF Pz: "<<LVElectron.pz()<<std::endl;
    std::cout<<"Nom E: "<<scaledElectron.energy()<<"Nom Px: "<<scaledElectron.px()<<"Nom Py: "<<scaledElectron.py()<<"Nom Pz: "<<scaledElectron.pz()<<std::endl;

    scaledElectron.setP4(LVElectron);
    pElectrons->push_back( scaledElectron );

    dPx    += scaledElectron.px() - electron->px();
    dPy    += scaledElectron.py() - electron->py();
    dSumEt += scaledElectron.et() - electron->et();
  }
  
  // scale MET accordingly
  pat::MET met = *(mets->begin());
  double scaledMETPx = met.px() - dPx;
  double scaledMETPy = met.py() - dPy;
  met.setP4(reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)));
  pMETs->push_back( met );
  event.put(pElectrons, outputElectrons_);
  event.put(pMETs, outputMETs_);
}

