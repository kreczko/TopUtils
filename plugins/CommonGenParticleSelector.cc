#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Common/interface/RefVector.h"
#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleElementCollectionSelector.h"


typedef SingleObjectSelector<std::vector<reco::GenParticle>, StringCutObjectSelector<reco::GenParticle> > CommonGenParticleSelector;
DEFINE_FWK_MODULE(CommonGenParticleSelector);
