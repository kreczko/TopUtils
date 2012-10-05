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
  // collect them in vector
  //std::cout << "BHadJetIndices: "  << std::endl;
  for (int i=0; i<(int) BHadJetIndex->size()    ; ++i){
    // avoid double listing of indices
    if(i==0||(BHadJetIndex->at(i-1)!=BHadJetIndex->at(i))){
      VBHadJetIdx    .push_back(    BHadJetIndex->at(i));
      //std::cout << BHadJetIndex->at(i)  << std::endl;
    }
  };
  //std::cout << "AntiBHadJetIndices: "  << std::endl;
  for (int i=0; i<(int) AntiBHadJetIndex->size(); ++i){
    // avoid double listing of indices within each list
    if(i==0||(AntiBHadJetIndex->at(i-1)!=AntiBHadJetIndex->at(i))){
      VAntiBHadJetIdx.push_back(AntiBHadJetIndex->at(i));
      //std::cout << AntiBHadJetIndex->at(i) << std::endl;
    }
  };
//   // deal with more than one bjet index:
//   // avoid using the same jets
//   if(VBHadJetIdx.size()>1&&VAntiBHadJetIdx.size()==1){
//     // scan B-jet for overlapping entries
//     for(int i=0; i<(int) VBHadJetIdx.size(); ++i){
//       if(VBHadJetIdx[i]==VAntiBHadJetIdx[0]){
// 	// rm overlapping entry in collection with more than one entry
// 	VBHadJetIdx.erase(VBHadJetIdx.begin()+i);
//       }
//     }
//   }
//   if(VAntiBHadJetIdx.size()>1&&VBHadJetIdx.size()==1){
//     // scan ANTI-B-jet for overlapping entries
//     for(int i=0; i<(int) VAntiBHadJetIdx.size(); ++i){
//       if(VAntiBHadJetIdx[i]==VBHadJetIdx[0]){
// 	// rm overlapping entry in collection with more than one entry
// 	VAntiBHadJetIdx.erase(VAntiBHadJetIdx.begin()+i);
//       }
//     }
//   }
  // deal with bjet indices appearing in both index lists
  if(VAntiBHadJetIdx.size()>1&&VBHadJetIdx.size()>1){
    for(int i=0; i<(int) VBHadJetIdx.size(); ++i){
      for(int j=0; j<(int) VAntiBHadJetIdx.size(); ++j){
	if(VAntiBHadJetIdx[j]==VBHadJetIdx[i]){
	  // rm overlapping entry in collection with more than one entry
	  if(VBHadJetIdx.size()==1&&VAntiBHadJetIdx.size()>1) VAntiBHadJetIdx.erase(VAntiBHadJetIdx.begin()+j); 
	  else if(VBHadJetIdx.size()>1&&VAntiBHadJetIdx.size()==1) VBHadJetIdx.erase(VBHadJetIdx.begin()+i);
	  else{
	    VAntiBHadJetIdx.erase(VAntiBHadJetIdx.begin()+j);
	  }
	}
      }
    }
  }
  // choose B-jet index from list
  if(VBHadJetIdx.size()>0){
    for(int i=0; i<(int) VBHadJetIdx.size(); ++i){
      // use indices of jets satisfying the PS cuts if possible
      for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
	int currentIndex=p-genJet->begin();
	// scan for jet index and check kinematic range
	if(currentIndex==VBHadJetIdx[i]&&std::abs(p->eta())<2.1&&p->pt()>30) bIX=VBHadJetIdx[i];    
      }
    }
    // if not: take leading index
    if(bIX==-1)  bIX   =VBHadJetIdx    [0];
  }
  // choose Anti-B-jet index from list
  if(VAntiBHadJetIdx.size()>0){
    for(int i=0; i<(int) VAntiBHadJetIdx.size(); ++i){
      // use indices of jets satisfying the PS cuts if possible
      for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
	int currentIndex=p-genJet->begin();
	// scan for jet index and check kinematic range
	if(currentIndex==VAntiBHadJetIdx[i]&&std::abs(p->eta())<2.1&&p->pt()>30) bbarIX=VAntiBHadJetIdx[i]; 
      }
    }
    // if not: take leading index
    if(bbarIX==-1)  bbarIX   =VAntiBHadJetIdx    [0];
  }

  // loop input collection to find b-jet
  for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
    int currentIndex=p-genJet->begin();
    //std::cout << "checking jet no. " << currentIndex << std::endl;
    if(currentIndex==bIX){
      //std::cout << "candidate found!" << std::endl;
      if(std::abs(p->eta())<eta_&&p->pt()>pt_){
	//std::cout << std::endl << "selected b-jet! index " << currentIndex << ", ";
	//std::cout << "pt=" << p->pt() << ", eta=" << p->eta() << std::endl;
	out->push_back(*p);
      }
    }
  }
  // loop input collection to find bbar-jet
  for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
    int currentIndex=p-genJet->begin();
    //std::cout << "checking jet no. " << currentIndex << std::endl;
    if(currentIndex==bbarIX){
      //std::cout << "candidate found!" << std::endl;
      if(std::abs(p->eta())<eta_&&p->pt()>pt_){
	//std::cout << std::endl << "selected anti b-jet! index " << currentIndex << ", ";
	//std::cout << "pt=" << p->pt() << ", eta=" << p->eta() << std::endl;
	out->push_back(*p);
      }
    }
  }

  // push out vector into the event
  evt.put(out);
}

bool checkKinematicRange(int index, edm::Handle<edm::View<reco::GenJet> > genJet){
  // loop input collection
  for(edm::View<reco::GenJet>::const_iterator p=genJet->begin(); p!=genJet->end(); ++p){
    int currentIndex=p-genJet->begin();
    // scan for jet index
    if(currentIndex==index){
      // check keinematic range
      if(std::abs(p->eta())<2.1&&p->pt()>30) return true;
      else return false;
    }
  }
  return false;
}



#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenJetSelector);
