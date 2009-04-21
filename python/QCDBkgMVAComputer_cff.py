import FWCore.ParameterSet.Config as cms

#
# produce mvaComputer with all necessary ingredients
#
from TopAnalysis.TopUtils.QCDBkgMVAComputer_cfi import *

## path for mva input file
TtSemiLepSignalSelectorMVAFileSource = cms.ESSource("TtSemiLepSignalSelectorMVAFileSource",
    ttSemiLepSignalSelectorMVA = cms.FileInPath('mvatraining/current.mva')
)
