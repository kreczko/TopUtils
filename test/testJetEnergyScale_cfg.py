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
    '/store/mc/Spring10/TTbarJets-madgraph/AODSIM/START3X_V26_S09-v1/0005/0210B899-9C46-DF11-A10F-003048C69294.root'
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
process.GlobalTag.globaltag = cms.string('START38_V14::All')

## pat sequences
process.load("PhysicsTools.PatAlgos.patSequences_cff")

## configure JetEnergyScale tool
process.load("TopAnalysis.TopUtils.JetEnergyScale_cff")
from TopAnalysis.TopUtils.JetEnergyScale_cff import *
#scalePatJetsEnergy(process, 1.1)
scaledJetEnergy.scaleType   = "top:up"
scaledJetEnergy.scaleFactor = 1.053
scaledJetEnergy.payload     = "AK5Calo"
scaledJetEnergy.resolutionFactor = 1.1

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
run36xOn35xInput(process)
    
process.load("RecoJets.Configuration.GenJetParticles_cff")
process.load("RecoJets.Configuration.RecoGenJets_cff")

process.p = cms.Path(process.genJetParticles *
                     process.ak5GenJets *
                     process.patDefaultSequence *
                     process.scaledJetEnergy
                     )

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    SelectEvents   = cms.untracked.PSet(SelectEvents = cms.vstring('p') ),                               
    fileName = cms.untracked.string('testJetEnergyScale.root'),
    outputCommands = cms.untracked.vstring('drop *')
)
process.out.outputCommands += ['keep *_scaledJetEnergy_*_*']

## output path
process.outpath = cms.EndPath(process.out)
process.options.wantSummary=False
