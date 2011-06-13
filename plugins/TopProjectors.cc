#include "PhysicsTools/PFCandProducer/plugins/TopProjector.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

typedef TopProjector<pat::Muon, reco::PFCandidate> TPPatMuonsOnPFCandidates;
typedef TopProjector<pat::Electron, reco::PFCandidate> TPPatElectronsOnPFCandidates;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TPPatMuonsOnPFCandidates);
DEFINE_FWK_MODULE(TPPatElectronsOnPFCandidates);
