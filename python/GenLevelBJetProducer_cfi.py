import FWCore.ParameterSet.Config as cms

produceGenLevelBJets = cms.EDProducer("GenLevelBJetProducer",
    ttGenEvent = cms.InputTag('genEvt'),
    genJets = cms.InputTag('ak5GenJets','','SIM'), 
    deltaR = cms.double(0.5),
    resolveParticleName = cms.bool(False),
    requireTopBquark = cms.bool(False),
    noBBbarResonances = cms.bool(True)
)



