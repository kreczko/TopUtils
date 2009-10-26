#include "TopAnalysis/TopUtils/interface/JetCalibrationVariables.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h" 
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/Common/interface/RefProd.h" 
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/RefHolder.h"
#include "DataFormats/Common/interface/Holder.h"


using namespace reco;

namespace {
  struct dictionary {
    edm::RefToBaseVector<pat::Jet> jrtbv;
    edm::Wrapper<edm::RefToBaseVector<pat::Jet> > jrtbv_w;
    edm::reftobase::BaseVectorHolder<pat::Jet> * bvhj_p;
  };
}
