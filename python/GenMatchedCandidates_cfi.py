import FWCore.ParameterSet.Config as cms

partonMatchedJets = cms.EDProducer('PartonMatchedJetsProducer',
  src = cms.InputTag('selectedPatJetsAK5'),
  match = cms.InputTag('jetPartonMatch')
)

genJetMatchedJets = cms.EDProducer('GenJetMatchedJetsProducer',
  src = cms.InputTag('selectedPatJetsAK5'),
  match = cms.InputTag('jetGenJetMatch')
)

genMatchedMuons = cms.EDProducer('GenMatchedMuonsProducer',
  src = cms.InputTag('selectedPatMuons'),
  match = cms.InputTag('muonMatch')
)
