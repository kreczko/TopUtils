import FWCore.ParameterSet.Config as cms

pfNoPatElectrons = cms.EDProducer(
    "TPPatMuonsOnPFCandidates",
    enable =  cms.bool( True ),
    verbose = cms.untracked.bool( False ),
    name = cms.untracked.string("noTrackMuon"),
    topCollection = cms.InputTag("selectedPatElectrons"),
    bottomCollection = cms.InputTag("pfNoPileUp"),
)
