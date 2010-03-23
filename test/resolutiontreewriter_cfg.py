import FWCore.ParameterSet.Config as cms

process = cms.Process("ResolutionTreeWriter")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
    '/store/user/henderle/7TeV/TTbar/PATtuple_1.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('ResolutionTree.root')
)

process.resolutionTreeWriter = cms.EDAnalyzer('ResolutionTreeWriter',
     jetTag      = cms.untracked.InputTag("selectedPatJets"),
     electronTag = cms.untracked.InputTag("selectedPatElectrons"),
     muonTag     = cms.untracked.InputTag("selectedPatMuons"),
     METTag      = cms.untracked.InputTag("patMETs")                                           
)

process.dump = cms.EDAnalyzer('EventContentAnalyzer'
)

process.p = cms.Path(process.resolutionTreeWriter)
#process.p = cms.Path(process.dump)
