#include "FWCore/Framework/interface/MakerMacros.h"

#include "TopAnalysis/TopUtils/plugins/EventWeight.h"
DEFINE_FWK_MODULE( EventWeight );

#include "TopAnalysis/TopUtils/plugins/EventWeightMC.h"
DEFINE_FWK_MODULE( EventWeightMC );

#include "TopAnalysis/TopUtils/plugins/EventWeightPlain.h"
DEFINE_FWK_MODULE( EventWeightPlain );

#include "TopAnalysis/TopUtils/plugins/EventWeightPU.h"
DEFINE_FWK_MODULE( EventWeightPU );

#include "TopAnalysis/TopUtils/plugins/EventWeightDileptonSF.h"
DEFINE_FWK_MODULE( EventWeightDileptonSF );

#include "TopAnalysis/TopUtils/plugins/EventWeightDileptonKinEffSF.h"
DEFINE_FWK_MODULE( EventWeightDileptonKinEffSF );

#include "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"
DEFINE_FWK_MODULE( JetEnergyScale );

#include "TopAnalysis/TopUtils/plugins/JESUncertainty.h"
DEFINE_FWK_MODULE( JESUncertainty );

#include "TopAnalysis/TopUtils/plugins/UnclusteredMETScale.h"
DEFINE_FWK_MODULE( UnclusteredMETScale );

#include "TopAnalysis/TopUtils/plugins/TtSemiLepSignalSelectorMVATrainer.h"
DEFINE_FWK_MODULE( TtSemiLepSignalSelectorMVATrainer );

#include "TopAnalysis/TopUtils/plugins/TtSemiLepSignalSelectorMVAComputer.h"
DEFINE_FWK_MODULE( TtSemiLepSignalSelectorMVAComputer );

#include "TopAnalysis/TopUtils/plugins/CandidateCleaner.h"
//namespace pat {
  typedef CandidateCleaner<reco::GenJet> PATGenJetCleaner;
//}
DEFINE_FWK_MODULE(PATGenJetCleaner);
