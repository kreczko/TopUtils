#include "FWCore/Framework/interface/MakerMacros.h"

#include "TopAnalysis/TopUtils/plugins/EventWeight.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightFromXSec.h"
#include "TopAnalysis/TopUtils/plugins/CSA07WeightAnalyzer.h"

DEFINE_FWK_MODULE( EventWeight );
DEFINE_FWK_MODULE( EventWeightFromXSec );
DEFINE_FWK_MODULE( CSA07WeightAnalyzer );

