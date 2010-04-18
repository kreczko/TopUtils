import FWCore.ParameterSet.Config as cms

## make parton matched calo jets
from TopAnalysis.TopUtils.genParticleMatching_cff import jetPartonMatch
from TopAnalysis.TopUtils.GenMatchedCandidates_cfi import partonMatchedJets
makePartonMatchedJets = cms.Sequence(jetPartonMatch * partonMatchedJets)

## make gen jet matched calo jets
from TopAnalysis.TopUtils.genParticleMatching_cff import jetGenJetMatch
from TopAnalysis.TopUtils.GenMatchedCandidates_cfi import genJetMatchedJets
makeGenJetMatchedJets = cms.Sequence(jetGenJetMatch * genJetMatchedJets)

# module to create edm::Association between pat muons and
# generator Muons originating from prompt W or Z boson
# decays
from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch
muonMatch.src = "selectedPatMuons"
muonMatch.matched = "prunedGenParticles"
muonMatch.mcStatus = [1, 2, 3]

## make gen matched muons
from TopAnalysis.TopUtils.GenMatchedCandidates_cfi import genMatchedMuons
makeGenMatchedMuons = cms.Sequence(muonMatch * genMatchedMuons)
