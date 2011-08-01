import FWCore.ParameterSet.Config as cms

# =============================
#  Cfg File for PU Reweighting
# =============================

process = cms.Process("USER")

# ================
#  Message Logger
# ================

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

# ==========================
#  Configuration of Process
# ==========================

#### Input Files

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring("/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02719D6B-1398-E011-AA71-001A92971B94.root"))

#process.load("TopAnalysis.Configuration.samples.Spring11_WJets_PATtuple_cff")

#### Number of Events

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
    )

#### Process Options

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

#### Definiton for Module

process.load("TopAnalysis.TopUtils.EventWeightPU_cfi")

process.eventWeightPU.MCSampleFile      = "TopAnalysis/TopUtils/data/MC_PUDist_Summer11_TTJets_TuneZ2_7TeV_madgraph_tauola.root"
process.eventWeightPU.MCSampleHistoName = "MCPUDistribution/pileup"
process.eventWeightPU.DataFile          = "TopAnalysis/TopUtils/data/Data_PUDist_160404-163869_7TeV_May10ReReco_Collisions11_v2_and_165088-167913_7TeV_PromptReco_Collisions11.root"
process.eventWeightPU.DataHistoName     = "pileup"    
process.eventWeightPU.PUSysShiftUp      =  0.6
process.eventWeightPU.PUSysShiftDown    = -0.6

#### Output Module

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string("myOutputFile.root"),
                               outputCommands = cms.untracked.vstring('keep *_*_*_*') 
                               )
#### Path

process.USER    = cms.Path(process.makeWeightsPU)
process.outpath = cms.EndPath(process.out)



