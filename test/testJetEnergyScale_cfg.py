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
    ## test-file on cern afs
    'file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_1_X_2008-07-08_STARTUP_V4-AODSIM.100.root'
    ## one file from the madgraph ttbar sample
    #'/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V9_v2/0000/027B51E9-8EED-DD11-9045-0015C5E9C0E1.root'
    ## one file from the tauola ttbar sample
    #'/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/0054812D-26F7-DD11-99D7-001F2908F0E4.root'
    )
)

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

## configure geometry & conditions
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')

## pat sequences
process.load("PhysicsTools.PatAlgos.patSequences_cff")

## configure JetEnergyScale tool
process.load("TopAnalysis.TopUtils.JetEnergyScale_cfi")
process.scaledJetEnergy.scaleFactor = 1.1
process.selectedLayer1Jets.src = "scaledJetEnergy:allLayer1Jets"
## don't forget to use "scaledJetEnergy:layer1METs" as InputTag for your subsequent modules

## necessary fixes to run 2.2.X on 2.1.X data
## comment this when running on samples produced with 22X
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)
process.patDefaultSequence.remove(process.patPFCandidateIsoDepositSelection)
process.patDefaultSequence.remove(process.patPFTauIsolation) 

process.p = cms.Path(process.beforeLayer1Objects *   # part of the patDefaultSequence
                     process.allLayer1Objects *      # part of the patDefaultSequence
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
