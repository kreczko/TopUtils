import FWCore.ParameterSet.Config as cms

writeResolutionTree = cms.EDAnalyzer('ResolutionTreeWriter',
     jetTag      = cms.untracked.InputTag("selectedPatJets"),
     electronTag = cms.untracked.InputTag("selectedPatElectrons"),
     muonTag     = cms.untracked.InputTag("selectedPatMuons"),
     METTag      = cms.untracked.InputTag("patMETs")
)
