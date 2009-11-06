#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/UtilAlgos/interface/PhysObjectMatcher.h"
#include "CommonTools/UtilAlgos/interface/DummyMatchSelector.h"

typedef reco::PhysObjectMatcher<edm::View<reco::Candidate>, edm::View<reco::Candidate>, reco::DummyMatchSelector<reco::Candidate, reco::Candidate> > TagAndProbeAssociator;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TagAndProbeAssociator );
