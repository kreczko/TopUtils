import FWCore.ParameterSet.Config as cms

eventWeightDileptonSF = cms.EDProducer("EventWeightDileptonSF",
    electrons    = cms.InputTag('filterDiLeptonMassQCDveto'),
    muons        = cms.InputTag('filterDiLeptonMassQCDveto')
)



