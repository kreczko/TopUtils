#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "TopAnalysis/TopUtils/plugins/GenMatchedCandsProducerBase.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
typedef GenMatchedCandsProducerBase<pat::Muon, reco::GenParticle> GenMatchedMuonsProducer;
DEFINE_FWK_MODULE(GenMatchedMuonsProducer);
