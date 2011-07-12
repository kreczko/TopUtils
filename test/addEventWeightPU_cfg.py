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

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring("/store/mc/Summer11/QCD_Pt-15to3000_TuneD6T_Flat_7TeV_pythia6/AODSIM/PU_S3_START42_V11-v2/0000/84488536-2B7E-E011-989D-001A92810A94.root"))
#process.load("TopAnalysis.Configuration.samples.Spring11_WJets_PATtuple_cff")

#### Number of Events

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
    )


#### Process Options

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.load("TopAnalysis.TopUtils.EventWeightPU_cfi")

process.eventWeightPU.MCSampleFile      = "TopAnalysis/TopUtils/data/MC_PUDist_TTJets_TuneZ2_7TeV_madgraph_tauola.root"
process.eventWeightPU.MCSampleHistoName = "MCPUDistribution/pileup"
process.eventWeightPU.DataFile          = "TopAnalysis/TopUtils/data/Data_PUDist_160404-166861_7TeV_PromptReco_Collisions11.root"
process.eventWeightPU.DataHistoName     = "pileup"    
process.eventWeightPU.PUSysShiftUp      = 0.6
process.eventWeightPU.PUSysShiftDown    = -0.6

# process.myProducerLabel = cms.EDProducer('EventWeightPU')

#### Output Module

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string("myOutputFile.root"),
                               outputCommands = cms.untracked.vstring('keep *_*_*_*') 
                               )
#### Path

process.USER    = cms.Path(process.makeWeightsPU)
process.outpath = cms.EndPath(process.out)



