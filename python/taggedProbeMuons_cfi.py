import FWCore.ParameterSet.Config as cms

taggedProbeMuons = cms.EDProducer( "ResonanceTagProbeProducer",
  tags    = cms.InputTag( "selectedPatMuons" ),
  probes  = cms.InputTag( "selectedPatMuons" ),
  mass   = cms.double( 90 ),
  deltaM = cms.double(  5 )
)
