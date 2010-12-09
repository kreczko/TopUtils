import FWCore.ParameterSet.Config as cms

scaledJetEnergy = cms.EDProducer("JetEnergyScale",
    inputJets            = cms.InputTag("patJets"),
    inputMETs            = cms.InputTag("patMETs"),
    scaleFactor          = cms.double(1.0),
    scaleType            = cms.string("abs"), #abs or rel
    jetPTThresholdForMET = cms.double(20.),
    jetEMLimitForMET     = cms.double(0.9),
    resolutionFactor     = cms.double(1.0)
)
