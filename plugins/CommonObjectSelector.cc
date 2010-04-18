#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/Common/interface/RefVector.h"
#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleElementCollectionSelector.h"


typedef SingleObjectSelector<std::vector<reco::GenJet>, StringCutObjectSelector<reco::GenJet> > CommonGenJetSelector;
DEFINE_FWK_MODULE(CommonGenJetSelector);
