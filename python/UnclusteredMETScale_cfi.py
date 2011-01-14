import FWCore.ParameterSet.Config as cms

scaledMET = cms.EDProducer("UnclusteredMETScale",
    inputJets            = cms.InputTag("patJets"),
    inputMETs            = cms.InputTag("patMETs"),
    scaleFactor          = cms.double(1.0)
)
