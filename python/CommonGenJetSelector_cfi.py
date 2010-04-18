import FWCore.ParameterSet.Config as cms

# module to select GenJets
# See https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
# on how to use the cut-string
#
selectedGenJets = cms.EDFilter("CommonGenJetSelector",
    src = cms.InputTag("ak5GenJets"),
    cut = cms.string("")
)
