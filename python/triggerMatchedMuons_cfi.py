import FWCore.ParameterSet.Config as cms

triggerMatchedMuons = cms.EDProducer( "TriggerMatchedCandsProducer",
  src          = cms.InputTag( "selectedPatMuons" ),
  matches      = cms.string( 'muonTriggerMatchHLTMuons' ),
  trigger      = cms.InputTag( "patTrigger" ),
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
)
