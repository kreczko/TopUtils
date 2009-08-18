import FWCore.ParameterSet.Config as cms

selectedLayer1TopJets = cms.EDFilter("TopJetsSelector",
  ## input collection for the selector
  inputCollection = cms.InputTag("selectedLayer1Jets"),
  ## generatorMatch label
  generatorMatch = cms.InputTag("ttSemiLepJetPartonMatch"),                                     
  ## ttSemileptonicEvent
  ttSemiLeptonicEvent = cms.InputTag("ttSemiLepEvent"),
  ## hypothesis key 
  hypoKey = cms.string("kGenMatch"),
  ## specify the jetType you want to select for. You can choose from
  ## 'All', 'BJet', 'LightQuark'. Take care of capital letters.                                     
  jetType = cms.string("All"),
  ## specify whether you like to have the jets of the hypothesis
  ## chosen or the oposite
  partOfTheHypothesis = cms.bool(True)                                     
)

