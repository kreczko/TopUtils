import FWCore.ParameterSet.Config as cms

#-------------------------------------------------
# test cfg file for scaling the jet energy
#-------------------------------------------------
process = cms.Process("TEST")

## add message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'

## define input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/mc/Summer09/TTbar/AODSIM/MC_31X_V3_AODSIM-v1/0026/F899CDFB-6C89-DE11-B2FD-00304894558A.root'
    )
)

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

## configure geometry & conditions
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('MC_31X_V3::All')

## pat sequences
process.load("PhysicsTools.PatAlgos.patSequences_cff")

## configure JetEnergyScale tool
process.load("TopAnalysis.TopUtils.JetEnergyScale_cfi")
process.scaledJetEnergy.scaleFactor = 1.1
process.selectedLayer1Jets.src = "scaledJetEnergy:allLayer1Jets"
## don't forget to use "scaledJetEnergy:layer1METs" as InputTag for your subsequent modules

## necessary to run with 33X on 31X AOD samples
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
run33xOn31xMC(process)
restrictInputToAOD31X(process)

process.p = cms.Path(process.allLayer1Objects *      # part of the patDefaultSequence
                     process.scaledJetEnergy *
                     process.selectedLayer1Objects * # part of the patDefaultSequence
                     process.cleanLayer1Objects *    # part of the patDefaultSequence
                     process.countLayer1Objects)     # part of the patDefaultSequence

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    SelectEvents   = cms.untracked.PSet(SelectEvents = cms.vstring('p') ),                               
    fileName = cms.untracked.string('testJetEnergyScale.root'),
    outputCommands = cms.untracked.vstring('drop *')
)
process.out.outputCommands += ['keep *_scaledJetEnergy_*_*']

## output path
process.outpath = cms.EndPath(process.out)
