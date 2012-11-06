#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/MyPatJetCorrFactorsProducer.h"

//#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "PhysicsTools/PatAlgos/plugins/JetCorrFactorsProducer.h"

MyPatJetCorrFactorsProducer::MyPatJetCorrFactorsProducer(const edm::ParameterSet& cfg):
  jetSrc_( cfg.getParameter<edm::InputTag>("jetSrc") )
{
  produces<pat::JetCorrFactorsProducer::JetCorrFactorsMap>();
}

MyPatJetCorrFactorsProducer::~MyPatJetCorrFactorsProducer()
{
}

void
MyPatJetCorrFactorsProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  edm::Handle<edm::View<reco::Jet> > genJets;
  evt.getByLabel(jetSrc_, genJets);

  std::vector<pat::JetCorrFactors> jcfs;
  for(edm::View<reco::Jet>::const_iterator jet = genJets->begin(); jet != genJets->end(); ++jet){
    std::vector<std::pair<std::string, std::vector<float> > > jecHelper;
    jecHelper.push_back(std::make_pair("L1FastJet" ,std::vector<float>(1,1.)));
    jecHelper.push_back(std::make_pair("L2Relative",std::vector<float>(1,1.)));
    jecHelper.push_back(std::make_pair("L3Absolute",std::vector<float>(1,1.)));
    pat::JetCorrFactors jecs("",jecHelper);
    jcfs.push_back(jecs);
  }

  std::auto_ptr<pat::JetCorrFactorsProducer::JetCorrFactorsMap> jetCorrsMap(new pat::JetCorrFactorsProducer::JetCorrFactorsMap());
  pat::JetCorrFactorsProducer::JetCorrFactorsMap::Filler filler(*jetCorrsMap);
  // jets and jetCorrs have their indices aligned by construction
  filler.insert(genJets, jcfs.begin(), jcfs.end());
  filler.fill(); // do the actual filling
  // put our produced stuff in the event
  evt.put(jetCorrsMap);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( MyPatJetCorrFactorsProducer );
