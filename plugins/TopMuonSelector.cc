#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/TopMuonSelector.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"

/// default constructor
TopMuonSelector::TopMuonSelector(const edm::ParameterSet& cfg):
  inputCollection_( cfg.getParameter<edm::InputTag>("inputCollection") ),
  ttSemiLeptonicEvent_( cfg.getParameter<edm::InputTag>("ttSemiLeptonicEvent") ),
  hypoKey_( cfg.getParameter<std::string>("hypoKey") )
{
  // reco level match 
  produces<std::vector<pat::Muon> >("rec");
  // generator level match 
  produces<std::vector<pat::Muon> >("gen");
}

/// default destructor
TopMuonSelector::~TopMuonSelector()
{
}

/// produce function
void
TopMuonSelector::produce(edm::Event& event, const edm::EventSetup& setup)
{
  // fetch the input collection from the event content
  edm::Handle<std::vector<pat::Muon> > muons; 
  event.getByLabel(inputCollection_, muons);

  // fetch TtSemiLeptonicEvent from the event content
  edm::Handle<TtSemiLeptonicEvent> tops; 
  event.getByLabel(ttSemiLeptonicEvent_, tops);

  // add reco level match from TtSemiLeptonicEvent
  std::auto_ptr<std::vector<pat::Muon> > recPtr(new std::vector<pat::Muon>);  
  std::auto_ptr<std::vector<pat::Muon> > genPtr(new std::vector<pat::Muon>);  

  // add reco level match from TtSemiLeptonicEvent
  if( tops->isHypoValid(hypoKey_) ){
    recPtr->push_back( (*muons)[tops->jetLeptonCombination(hypoKey_)[TtSemiLepEvtPartons::Lepton]] );
  }

  // add generator level match from TtSemiLeptonicEvent
  if( tops->genEvent()->isSemiLeptonic(WDecay::kMuon) ){
    double minDR = -1;
    int genIndex = -1;
    for(unsigned int i=0; i<muons->size(); ++i){
      if( minDR<0 || deltaR<const pat::Muon, const reco::GenParticle>((*muons)[i], *tops->singleLepton())<minDR ){
	minDR = deltaR<const pat::Muon, const reco::GenParticle>((*muons)[i], *tops->singleLepton());
	genIndex=i;
      }
    }
    if(genIndex>=0){
      genPtr->push_back( (*muons)[genIndex] );
    }
  }
  
  // add reco level match to the event
  event.put(recPtr, "rec");  
  // add generator level match to the event
  event.put(genPtr, "gen");  
}
