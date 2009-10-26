#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "TopAnalysis/TopUtils/plugins/GenMatchedCandsProducerBase.h"

#include "DataFormats/JetReco/interface/GenJet.h"
typedef GenMatchedCandsProducerBase<reco::Jet, reco::GenJet> GenJetMatchedJetsProducer;
DEFINE_FWK_MODULE(GenJetMatchedJetsProducer);

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
typedef GenMatchedCandsProducerBase<pat::Jet, reco::GenParticle> PartonMatchedJetsProducer;
DEFINE_FWK_MODULE(PartonMatchedJetsProducer);
