import FWCore.ParameterSet.Config as cms

# =============================
#  Cfg File for PU Reweighting
# =============================

process = cms.Process("UserTest")

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

#process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring())
process.load("TopAnalysis.Configuration.samples.Spring11_WJets_PATtuple_cff")

#### Number of Events

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )


#### Process Options

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.load("TopAnalysis.TopUtils.EventWeightPU_cfi")

process.eventWeightPU.MCSampleFile      = "../data/MC_PUDist.root"
process.eventWeightPU.MCSampleHistoName = "pileup"
process.eventWeightPU.DataFile          = "../data/Data_PUDist_110527.root"
process.eventWeightPU.DataHistoName     = "pileup"    

# process.myProducerLabel = cms.EDProducer('EventWeightPU')

#### Output Module

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string("myOutputFile.root"),
                               outputCommands = cms.untracked.vstring('keep *_*_*_*') 
                               )

#### Path

process.UserTest = cms.Path(process.makeWeightsPU)
process.outpath  = cms.EndPath(process.out)



