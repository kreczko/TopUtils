import FWCore.ParameterSet.Config as cms

scaledJetEnergy = cms.EDProducer("JetEnergyScale",
    inputJets            = cms.InputTag("allLayer1Jets"),
    inputMETs            = cms.InputTag("layer1METs"),
    scaleFactor          = cms.double(1.0),
    jetPTthresholdForMET = cms.double(20.),
    jetEMlimitForMET     = cms.double(0.9)
)
