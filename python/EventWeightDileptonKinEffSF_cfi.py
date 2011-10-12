import FWCore.ParameterSet.Config as cms

eventWeightDileptonKinEffSF = cms.EDProducer("EventWeightDileptonKinEffSF",
    electrons    = cms.InputTag('filterDiLeptonMassQCDveto'),
    muons        = cms.InputTag('filterDiLeptonMassQCDveto')
)



