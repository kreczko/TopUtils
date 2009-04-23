#include "FWCore/Framework/interface/MakerMacros.h"

#include "TopAnalysis/TopUtils/plugins/EventWeight.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightPlain.h"
#include "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"
#include "TopAnalysis/TopUtils/plugins/TtSemiLepSignalSelectorMVAComputer.h"

DEFINE_FWK_MODULE( EventWeight );
DEFINE_FWK_MODULE( EventWeightPlain );
DEFINE_FWK_MODULE( JetEnergyScale );
DEFINE_FWK_MODULE( TtSemiLepSignalSelectorMVAComputer );

