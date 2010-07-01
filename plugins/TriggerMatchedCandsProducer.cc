#include "DataFormats/PatCandidates/interface/Muon.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "TopAnalysis/TopUtils/plugins/TriggerMatchedCandsProducer.h"

// default constructor
TriggerMatchedCandsProducer::TriggerMatchedCandsProducer(const edm::ParameterSet& cfg):
  src_( cfg.getParameter<edm::InputTag>("src" ) ),
  matches_( cfg.getParameter< std::string >( "matches" ) ),
  triggerEvent_( cfg.getParameter< edm::InputTag >( "triggerEvent" ) )
{
  // produce pat::Muon collection
  produces<std::vector<pat::Muon> >();
}

void
TriggerMatchedCandsProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // input collection
  edm::Handle<std::vector<pat::Muon> > src; 
  evt.getByLabel(src_, src);
  // trigger event
  edm::Handle< pat::TriggerEvent > triggerEvent;
  evt.getByLabel( triggerEvent_, triggerEvent );
 
  // pat trigger helper to recieve for trigger 
  // matching information
  const pat::helper::TriggerMatchHelper matchHelper;

  // prepare vector of output vector
  std::auto_ptr<std::vector<pat::Muon> > matched(new std::vector<pat::Muon>);

  // receive the TriggerObjectMatch from the triggerEvent
  const pat::TriggerObjectMatch* triggerMatch( triggerEvent->triggerObjectMatchResult( matches_ ) );
  // loop over candidate references
  for( size_t idx=0; idx<src->size(); ++idx){ 
    const reco::CandidateBaseRef candBaseRef( edm::Ref<std::vector<pat::Muon> >( src, idx ) );
    const pat::TriggerObjectRef trigRefTag( matchHelper.triggerMatchObject( candBaseRef, triggerMatch, evt, *triggerEvent ) );
    if( trigRefTag.isAvailable() ){ matched->push_back((*src)[idx]); }
  }
  evt.put(matched);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TriggerMatchedCandsProducer );
