import FWCore.ParameterSet.Config as cms

scaledMET = cms.EDProducer("UnclusteredMETScale",
    inputJets            = cms.InputTag("patJets"),
    inputMETs            = cms.InputTag("patMETs"),
    inputElectrons       = cms.InputTag("patElectrons"),
    inputMuons           = cms.InputTag("patMuons"),
    scaleFactor          = cms.double(1.0)
)
