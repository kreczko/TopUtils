import FWCore.ParameterSet.Config as cms

triggerMatchedMuons = cms.EDProducer( "TriggerMatchedCandsProducer",
  src          = cms.InputTag( "selectedPatMuons" ),
  matches      = cms.string( 'muonTriggerMatchHLTMuons' ),
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
)
