#include "TopAnalysis/TopUtils/plugins/UnclusteredMETScale.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

UnclusteredMETScale::UnclusteredMETScale(const edm::ParameterSet& cfg):
  inputJets_           (cfg.getParameter<edm::InputTag>("inputJets"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           )),
  scaleFactor_         (cfg.getParameter<double>       ("scaleFactor"         ))
{  
  // register products
  produces<std::vector<pat::MET> >("scaledMETs"); 
}


void
UnclusteredMETScale::beginJob()
{ 		
}


void
UnclusteredMETScale::produce(edm::Event& event, const edm::EventSetup& setup)
{
  edm::Handle<std::vector<pat::Jet> > jets;
  event.getByLabel(inputJets_, jets);

  edm::Handle<std::vector<pat::MET> > mets;
  event.getByLabel(inputMETs_, mets);

  std::auto_ptr<std::vector<pat::MET> > pMETs(new std::vector<pat::MET>);

  pat::MET met = *(mets->begin());
  double scaledMETPx = met.px();
  double scaledMETPy = met.py();
  double scaledMETEt = met.et();  

  // get unclustered MET by subtracting clustered energy (i.e. adding the jet energies)
  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
     scaledMETPx += jet->px();
     scaledMETPy += jet->py();
     scaledMETEt += jet->et();
  }
  
  // met_x and met_y are now unclustered energy
  // apply the 10% on the unclustered energy. "factor" is either 0.9 or 1.1, for MET_minus or MET_plus, resp.
  scaledMETPx *= scaleFactor_;
  scaledMETPy *= scaleFactor_;
  scaledMETEt *= scaleFactor_;

  // readd the clustered enery
  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
     scaledMETPx -= jet->px();
     scaledMETPy -= jet->py();
     scaledMETEt -= jet->et();
  }

  pat::MET scaledMET(reco::MET(scaledMETEt, reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)), reco::MET::Point(0,0,0)));
  
  pMETs->push_back( scaledMET );  
  event.put(pMETs, "scaledMETs");
}
