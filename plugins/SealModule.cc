#include "FWCore/Framework/interface/MakerMacros.h"

#include "TopAnalysis/TopUtils/plugins/EventWeight.h"
#include "TopAnalysis/TopUtils/plugins/CSA07WeightAnalyzer.h"
#include "TopAnalysis/TopUtils/plugins/EvtWeightFromSigmaProducer.h"

DEFINE_FWK_MODULE( EventWeight );
DEFINE_FWK_MODULE( CSA07WeightAnalyzer );
DEFINE_FWK_MODULE( EvtWeightFromSigmaProducer );
