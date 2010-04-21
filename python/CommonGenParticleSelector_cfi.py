import FWCore.ParameterSet.Config as cms

# module to select GenParticles, basic collection needed as input
# See https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
# on how to use the cut-string
#
selectedGenParticles = cms.EDFilter("CommonGenParticleSelector",
                                    src = cms.InputTag("genParticles"),
                                    cut = cms.string("")
                                    )
