import FWCore.ParameterSet.Config as cms

pfNoPatElectrons = cms.EDProducer(
    "TPPatElectronsOnPFCandidates",
    enable =  cms.bool( True ),
    verbose = cms.untracked.bool( False ),
    name = cms.untracked.string("noTrackElectron"),
    topCollection = cms.InputTag("selectedPatElectrons"),
    bottomCollection = cms.InputTag("pfNoPileUp"),
)
