import FWCore.ParameterSet.Config as cms

selectedLayer1TopMuons = cms.EDFilter("TopMuonSelector",
  ## input collection for the selector
  inputCollection = cms.InputTag("selectedPatMuons"),
  ## ttSemileptonicEvent
  ttSemiLeptonicEvent = cms.InputTag("ttSemiLepEvent"),
  ## hypothesis key 
  hypoKey = cms.string("kGenMatch")
)

