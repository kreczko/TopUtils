#include "FWCore/Framework/interface/MakerMacros.h"
#include "TopAnalysis/TopUtils/plugins/GenMatchedCandsProducer.h"

#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


typedef GenMatchedCandsProducer<reco::Jet, reco::GenJet> GenJetMatchedJetsProducer;
DEFINE_FWK_MODULE(GenJetMatchedJetsProducer);

typedef GenMatchedCandsProducer<pat::Jet, reco::GenParticle> PartonMatchedJetsProducer;
DEFINE_FWK_MODULE(PartonMatchedJetsProducer);

typedef GenMatchedCandsProducer<pat::Muon, reco::GenParticle> GenMatchedMuonsProducer;
DEFINE_FWK_MODULE(GenMatchedMuonsProducer);
