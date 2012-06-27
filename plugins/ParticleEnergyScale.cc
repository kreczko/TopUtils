#include "TopAnalysis/TopUtils/plugins/ParticleEnergyScale.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

typedef ParticleEnergyScale<pat::Electron>  ElectronEnergyScale;
typedef ParticleEnergyScale<pat::Photon>    PhotonEnergyScale;
typedef ParticleEnergyScale<pat::Muon>      MuonEnergyScale;
typedef ParticleEnergyScale<pat::Tau>       TauEnergyScale;

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ElectronEnergyScale);
DEFINE_FWK_MODULE(PhotonEnergyScale);
DEFINE_FWK_MODULE(MuonEnergyScale);
DEFINE_FWK_MODULE(TauEnergyScale);

