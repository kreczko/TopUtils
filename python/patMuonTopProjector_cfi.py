import FWCore.ParameterSet.Config as cms

pfNoPatMuons = cms.EDProducer(
    "TPPatMuonsOnPFCandidates",
    enable =  cms.bool( True ),
    verbose = cms.untracked.bool( False ),
    name = cms.untracked.string("noTrackMuon"),
    topCollection = cms.InputTag("selectedPatMuons"),
    bottomCollection = cms.InputTag("pfNoPileUp"),
)
