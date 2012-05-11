import FWCore.ParameterSet.Config as cms

process = cms.Process("ResolutionTreeWriter")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.GlobalTag.globaltag = cms.string('START42_V17::All')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/mc/Fall11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S6_START42_V14B-v2/0000/0040932D-A80F-E111-BBF7-00304867BFAA.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('ResolutionTree.root')
)

from TopAnalysis.TopUtils.ResolutionTreeWriter_cfi import writeResolutionTree
process.resolutionTreeWriter = writeResolutionTree

process.p = cms.Path(process.resolutionTreeWriter)

from TopAnalysis.TopUtils.usePatTupleWithParticleFlow_cff import prependPF2PATSequence
PFoptions = {
    'runOnAOD': True
    }
prependPF2PATSequence(process, 'p', PFoptions)
