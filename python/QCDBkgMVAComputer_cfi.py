import FWCore.ParameterSet.Config as cms

#
# module to make the mvaComputer
#
findQCDBkgMVA = cms.EDProducer("TtSemiLepSignalSelectorMVAComputer",
    jets     = cms.InputTag("selectedLayer1Jets"),
    muons  = cms.InputTag("selectedLayer1Muons"),
    METs     = cms.InputTag("layer1METs"),
    electrons  = cms.InputTag("selectedLayer1Electrons")
)
