import FWCore.ParameterSet.Config as cms

produceGenLevelBJets = cms.EDProducer('GenLevelBJetProducer',
  ttGenEvent = cms.InputTag('genEvt'),
  genJets = cms.InputTag('ak5GenJets', '', 'SIM'), #REPLACED HLT FOR SIM
  deltaR = cms.double(5.0),
  noBBbarResonances = cms.bool(True)
)
