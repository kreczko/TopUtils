import FWCore.ParameterSet.Config as cms

pFElectronGSFMomentum = cms.EDProducer("PFElectronGSFMomentum",
    inputElectrons       = cms.InputTag("selectedPatElectrons"),
    inputMETs            = cms.InputTag("scaledPatMETs"),
)
