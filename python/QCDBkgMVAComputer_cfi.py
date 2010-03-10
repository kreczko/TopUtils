import FWCore.ParameterSet.Config as cms

#
# module to make the mvaComputer
#
findQCDBkgMVA = cms.EDProducer("TtSemiLepSignalSelectorMVAComputer",
    jets     = cms.InputTag("selectedPatJets"),
    muons  = cms.InputTag("selectedPatMuons"),
    METs     = cms.InputTag("patMETs"),
    electrons  = cms.InputTag("selectedPatElectrons")
)
