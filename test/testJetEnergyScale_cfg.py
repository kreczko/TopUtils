import FWCore.ParameterSet.Config as cms

#-------------------------------------------------
# test cfg file for scaling the jet energy
#-------------------------------------------------
process = cms.Process("TEST")

## add message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'

#-------------------------------------------------
# process configuration
#-------------------------------------------------

## define input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    ## test-file at lxplus
    'file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_1_X_2008-07-08_STARTUP_V4-AODSIM.100.root'
    ## one file from the madgraph ttbar sample at desy
    #'/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V9_v2/0000/027B51E9-8EED-DD11-9045-0015C5E9C0E1.root'
    ## one file from the tauola ttbar sample at desy
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

## configure geometry
process.load("Configuration.StandardSequences.Geometry_cff")

## configure conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('IDEAL_V9::All')

## load magnetic field
process.load("Configuration.StandardSequences.MagneticField_cff")

#-------------------------------------------------
# tqaf configuration
#-------------------------------------------------

## std sequence for tqaf layer1
process.load("TopQuarkAnalysis.TopObjectProducers.tqafLayer1_cff")

## define two new sequences that split up the default patLayer1 sequence
## (will be obsolete with PAT_v2)
from PhysicsTools.PatAlgos.patLayer1_cff import *
process.patLayer1_allObjects = cms.Sequence(allLayer1Muons +
                                            allLayer1Electrons +
                                            allLayer1Taus +
                                            allLayer1Photons +
                                            allLayer1Jets +
                                            allLayer1METs)
process.patLayer1_selectedObjects = cms.Sequence(selectedLayer1Muons * countLayer1Muons *
                                                 selectedLayer1Electrons * countLayer1Electrons *
                                                 selectedLayer1Taus * countLayer1Taus *
                                                 selectedLayer1Photons * countLayer1Photons *
                                                 selectedLayer1Jets * countLayer1Jets *
                                                 selectedLayer1METs *
                                                 selectedLayer1Hemispheres)

## configure JetEnergyScale tool
process.load("TopAnalysis.TopUtils.JetEnergyScale_cfi")
process.scaledJetEnergy.scaleFactor = 1.1
process.selectedLayer1Jets.src = "scaledJetEnergy:allLayer1Jets"
process.selectedLayer1METs.src = "scaledJetEnergy:allLayer1METs"

## necessary fixes to run 2.2.X on 2.1.X data
## comment this when running on samples produced with 22X
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)

#-------------------------------------------------
# process paths
#-------------------------------------------------

process.p = cms.Path(process.patLayer0_patTuple *
                     process.patLayer1_allObjects *
                     process.scaledJetEnergy *
                     process.patLayer1_selectedObjects)
