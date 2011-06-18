import FWCore.ParameterSet.Config as cms

taggedProbeMuons = cms.EDProducer( "ResonanceTagProbeProducer",
  tags    = cms.InputTag( "selectedPatMuons" ),
  probes  = cms.InputTag( "selectedPatMuons" ),
  mass   = cms.double( 90 ),
  deltaM = cms.double(  5 ),
  leptonId = cms.int32(13)  ## 11 for electrons, 13 for muons
)
