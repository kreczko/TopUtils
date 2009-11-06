import FWCore.ParameterSet.Config as cms

testMap = cms.EDFilter("TagAndProbeAssociator",
  ## target collection
  src     =  cms.InputTag("selectedLayer1Muons"),
  ## collcetion to be matched to the target collection
  matched = cms.InputTag("selectedLayer1Muons"),
  ## maximal delta value for match
  maxDeltaR = cms.double(0.2),
  ## switch to determine whether match ambiguities should be resolved or not
  resolveAmbiguities    = cms.bool(True),
  ## switch to determine whether ambiguities should be resolved by deltaR (match quality) or by pt ordering
  resolveByMatchQuality = cms.bool(True)
)
