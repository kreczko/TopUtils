#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/TopJetsSelector.h"

//#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"

/// default constructor
TopJetsSelector::TopJetsSelector(const edm::ParameterSet& cfg):
  inputCollection_( cfg.getParameter<edm::InputTag>("inputCollection") ),
  generatorMatch_ ( cfg.getParameter<edm::InputTag>("generatorMatch" ) ),
  ttSemiLeptonicEvent_( cfg.getParameter<edm::InputTag>("ttSemiLeptonicEvent") ),
  hypoKey_( cfg.getParameter<std::string>("hypoKey") ),
  jetType_( cfg.getParameter<std::string>("jetType") ),
  partOfTheHypothesis_( cfg.getParameter<bool>("partOfTheHypothesis") )
{
  if( !(jetType_.compare("BJet")==0 || jetType_.compare("LightQuark")==0 || jetType_.compare("All")==0) ){
    throw cms::Exception("UnknownParamerterValue") 
      << "Parameter jetType specified with unknown type '" << jetType_ 
      << "'. Available types are 'BJet', 'LightQuark', 'All'.";
  }
  // reco level match 
  produces<std::vector<pat::Jet> >("rec");
  // generator level match 
  produces<std::vector<pat::Jet> >("gen");
}

/// default destructor
TopJetsSelector::~TopJetsSelector()
{
}

/// produce function
void
TopJetsSelector::produce(edm::Event& event, const edm::EventSetup& setup)
{
  // fetch the input collection from the event content
  edm::Handle<std::vector<pat::Jet> > jets; 
  event.getByLabel(inputCollection_, jets);

  // fetch TtSemiLeptonicEvent from the event content
  edm::Handle<TtSemiLeptonicEvent> tops; 
  event.getByLabel(ttSemiLeptonicEvent_, tops);

  // fetch TtSemiLeptonicEvent from the event content
  edm::Handle<std::vector<std::vector<int> > > match; 
  event.getByLabel(generatorMatch_, match);

  // add reco level match from TtSemiLeptonicEvent
  std::auto_ptr<std::vector<pat::Jet> > recPtr(new std::vector<pat::Jet>);  
  std::auto_ptr<std::vector<pat::Jet> > genPtr(new std::vector<pat::Jet>);  

  unsigned int idx=0;
  for(std::vector<pat::Jet>::const_iterator jet=jets->begin(); jet!=jets->end(); ++jet, ++idx){
    // still needs to be x-checked whether idx==0 is visted or not
    std::cout << idx << std::endl;
    
    // add reco level match from TtSemiLeptonicEvent
    if( tops->isHypoValid(hypoKey_) ){
      if(pushElement(idx, tops->jetLeptonCombination(hypoKey_)) ) recPtr->push_back( (*jet) );
    }
    // add generator level match from TtSemiLeptonicEvent
    if( tops->genEvent()->isSemiLeptonic(WDecay::kMuon) && !match->empty() ){
      if(pushElement(idx, match->front()) ) genPtr->push_back( (*jet) );
    }
  }
  
  // add reco level match to the event
  event.put(recPtr, "rec");  
  // add generator level match to the event
  event.put(genPtr, "gen");  
}

/// helper function to decide whether the element should be pushed or not
bool
TopJetsSelector::pushElement(const int& idx, const std::vector<int>& match)
{
  bool push = false;
  if( jetType_.compare("BJet")==0 || jetType_.compare("All")==0 ){
    if(idx == match[TtSemiLepEvtPartons::HadB] ) push = partOfTheHypothesis_;
    if(idx == match[TtSemiLepEvtPartons::LepB] ) push = partOfTheHypothesis_;
  }
  if( jetType_.compare("LighQuark")==0 || jetType_.compare("All")==0 ){
    if(idx == match[TtSemiLepEvtPartons::LightQ] ) push = partOfTheHypothesis_;
    if(idx == match[TtSemiLepEvtPartons::LightQBar] ) push = partOfTheHypothesis_;
  }
  return push;
}
