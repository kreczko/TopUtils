import FWCore.ParameterSet.Config as cms

genMatchedMuons = cms.EDProducer('GenMatchedMuonsProducer',
  src = cms.InputTag('selectedPatMuons'),
  match = cms.InputTag('muonMatch')
)
