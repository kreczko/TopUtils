import FWCore.ParameterSet.Config as cms

#
# produce mvaComputer with all necessary ingredients
#

## import MVA trainer cfi
from TopAnalysis.TopUtils.QCDBkgMVATrainer_cfi import *

## ------------------------------------------------------------------------------------------
## configuration of event looper for mva taining; take care to make the 
## looper known to the process. The way to do that is to add the following
## lines to your cfg.py
##
## from TopQuarkAnalysis.TopJetCombination.TtSemiLepJetCombMVATrainTreeSaver_Muons_cff import looper
## process.looper = looper
## ------------------------------------------------------------------------------------------ 
looper = cms.Looper("TtSemiLepSignalSelectorMVATrainerLooper",
    trainers = cms.VPSet(cms.PSet(
        monitoring = cms.untracked.bool(False),
        loadState  = cms.untracked.bool(False),
        saveState  = cms.untracked.bool(True),
        calibrationRecord = cms.string('ttSemiLepSignalSelectorMVA'),
        trainDescription = cms.untracked.string(
            'TopAnalysis/TopAnalyzer/data/QCDBkgMVATrainTreeSaver_Muons.xml')
    ))
)

## provide a sequence to save a tree for the training
saveQCDBkgMVATrainTree = cms.Sequence(trainQCDBkg)

