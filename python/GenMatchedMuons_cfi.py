import FWCore.ParameterSet.Config as cms

genMatchedMuons = cms.EDProducer('GenMatchedMuonsProducer',
  src = cms.InputTag('selectedLayer1Muons'),
  match = cms.InputTag('muonMatch')
)
