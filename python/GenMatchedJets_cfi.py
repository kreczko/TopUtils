import FWCore.ParameterSet.Config as cms

partonMatchedJets = cms.EDProducer('PartonMatchedJetsProducer',
  src = cms.InputTag('selectedLayer1JetsAK5'),
  match = cms.InputTag('jetPartonMatch')
)

genJetMatchedJets = cms.EDProducer('GenJetMatchedJetsProducer',
  src = cms.InputTag('selectedLayer1JetsAK5'),
  match = cms.InputTag('jetGenJetMatch')
)
