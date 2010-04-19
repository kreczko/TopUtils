import FWCore.ParameterSet.Config as cms

genFinalStateSelector = cms.EDFilter("GenFinalStateSelector",
  ## input collection for the selector
  src = cms.InputTag("genParticles"),
  ## number of expected electrons in the final state
  elecs = cms.int32(1),
  ## number of expected muons in the final state
  muons = cms.int32(1),
  ## invert the selection
  invert=cms.bool(False)
)

