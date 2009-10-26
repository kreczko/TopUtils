import FWCore.ParameterSet.Config as cms

## make parton matched calo jets
from TopAnalysis.TopUtils.genParticleMatching_cff import jetPartonMatch
from TopAnalysis.TopUtils.GenMatchedJets_cfi import partonMatchedJets
makePartonMatchedJets = cms.Sequence(jetPartonMatch * partonMatchedJets)

## make gen jet matched calo jets
from TopAnalysis.TopUtils.genParticleMatching_cff import jetGenJetMatch
from TopAnalysis.TopUtils.GenMatchedJets_cfi import genJetMatchedJets
makeGenJetMatchedJets = cms.Sequence(jetGenJetMatch * genJetMatchedJets)
