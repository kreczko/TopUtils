import FWCore.ParameterSet.Config as cms

selectedLayer1TopMuon = cms.EDFilter("TopMuonSelector",
  ## input collection for the selector
  inputCollection = cms.InputTag("selectedLayer1Muons"),
  ## ttSemileptonicEvent
  ttSemiLeptonicEvent = cms.InputTag("ttSemiLepEvent"),
  ## hypothesis key 
  hypoKey = cms.string("kGenMatch")
)

