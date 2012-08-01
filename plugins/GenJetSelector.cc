#include "TopAnalysis/TopUtils/plugins/GenJetSelector.h"

GenJetSelector::GenJetSelector(const edm::ParameterSet& cfg):
  genJet_ ( cfg.getParameter<edm::InputTag>("genJet" )),
  bHadJetIdx_    (cfg.getParameter<edm::InputTag> ("BHadJetIndex"    )),
  antibHadJetIdx_(cfg.getParameter<edm::InputTag> ("AntiBHadJetIndex")),
  pt_     ( cfg.getParameter<double       >("pt"     )),
  eta_    ( cfg.getParameter<double       >("eta"    ))
{
  // register output
  produces<std::vector<reco::GenJet> >();
}

void
GenJetSelector::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // read collection from file
  edm::Handle<edm::View<reco::GenJet> > genJet; 
  evt.getByLabel(genJet_, genJet);
  
  // prepare output vector
  std::auto_ptr<std::vector<reco::GenJet> > out(new std::vector<reco::GenJet>);
  
  // get indices (e.g. produced by TopAnalysis/TopUtils/GenLevelBJetProducer.cc)
  bIX   =-1;
  bbarIX=-1;
  std::vector<int> VBHadJetIdx, VAntiBHadJetIdx;
  edm::Handle<std::vector<int> > BHadJetIndex, AntiBHadJetIndex;
  evt.getByLabel(bHadJetIdx_    , BHadJetIndex    );
  evt.getByLabel(antibHadJetIdx_, AntiBHadJetIndex);
  // save them in vector
  for (int i=0; i<(int) BHadJetIndex->size()    ; ++i){VBHadJetIdx    .push_back(    BHadJetIndex->at(i));};
  for (int i=0; i<(int) AntiBHadJetIndex->size(); ++i){VAntiBHadJetIdx.push_back(AntiBHadJetIndex->at(i));};
  // use leading vector entries finally 
  if(VBHadJetIdx    .size()>0) bIX   =VBHadJetIdx    [0];
  if(VAntiBHadJetIdx.size()>0) bbarIX=VAntiBHadJetIdx[0];
  //allIndices.push_back(bIX   );
  //allIndices.push_back(bbarIX);
  //std::cout << std::endl << "(bIX,bbarIX) = (" << bIX << "," << bbarIX << ")"<< std::endl;

  // loop input collection
  for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
    int currentIndex=p-genJet->begin();
    //std::cout << "checking jet no. " << currentIndex << std::endl;
    if(currentIndex==bbarIX||currentIndex==bIX){
      //std::cout << "candidate found!" << std::endl;
      if(p->eta()<eta_&&p->pt()>pt_){
	//std::cout << std::endl << "selected jet! index" << currentIndex << ", ";
	//std::cout << "pt= " << p->pt() << ", eta= " << p->eta() << std::endl;
	out->push_back(*p);
      }
    }
  }
  // push out vector into the event
  evt.put(out);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenJetSelector);
