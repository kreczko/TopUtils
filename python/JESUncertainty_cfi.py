import FWCore.ParameterSet.Config as cms

scaleJetEnergy = cms.EDProducer("JESUncertainty",
    inputJets            = cms.InputTag("selectedPatJetsAK5PF"),
    inputMETs            = cms.InputTag("patMETsPF"),
    bScaleUncertainty    = cms.double(0.050),
    puUncertainty        = cms.double(0.010),    
    swUncertainty        = cms.double(0.015),    
    direction            = cms.bool(True)  # 1 for up, 0 for down   
)
