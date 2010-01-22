import FWCore.ParameterSet.Config as cms

# module to create edm::Association between pat muons and
# generator Muons originating from prompt W or Z boson
# decays
from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch
muonMatch.src = "selectedLayer1Muons"
muonMatch.matched = "prunedGenParticles"
muonMatch.mcStatus = [1, 2, 3]

## make gen matched muons
from TopAnalysis.TopUtils.GenMatchedMuons_cfi import genMatchedMuons

makeGenMatchedMuons = cms.Sequence(muonMatch *
                                   genMatchedMuons
                                   )
