import FWCore.ParameterSet.Config as cms

produceGenLevelBJets = cms.EDProducer('GenLevelBJetProducer',
  ttGenEvent = cms.InputTag('genEvt'),
  genJets = cms.InputTag('ak5GenJets', '', 'HLT'),
  deltaR = cms.double(5.0),
  noBBbarResonances = cms.bool(True)
)
