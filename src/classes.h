#include "TopAnalysis/TopUtils/interface/ResolutionVariables.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h" 
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Candidate/interface/Candidate.h"


#include "DataFormats/Common/interface/RefProd.h" 
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/RefHolder.h"
#include "DataFormats/Common/interface/Holder.h"


using namespace reco;

namespace {
  struct dictionary {
    edm::Wrapper<std::vector<std::vector<int> > > bla;
    std::vector<std::vector<int> > bla_vector;  
    
    edm::RefToBaseVector<pat::Muon> murtbv;
    edm::Wrapper<edm::RefToBaseVector<pat::Muon> > murtbv_w;
    edm::reftobase::BaseVectorHolder<pat::Muon>* bvhmu_p;

    edm::RefToBaseVector<pat::Jet> jrtbv;
    edm::Wrapper<edm::RefToBaseVector<pat::Jet> > jrtbv_w;
    edm::reftobase::BaseVectorHolder<pat::Jet>* bvhj_p;

    edm::Association<edm::View<reco::Candidate> > arcv;
    edm::Wrapper<edm::Association<edm::View<reco::Candidate> > > arcv_w;
    edm::RefProd<edm::View<reco::Candidate> > rcv_rp;
  };
}
