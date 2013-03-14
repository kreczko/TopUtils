import FWCore.ParameterSet.Config as cms
from RecoJets.Configuration.RecoGenJets_cff import ak5GenJets
from TopAnalysis.TopUtils.GenJetParticles_cff import genParticlesForJetsPlusBHadronNoNu

ak5GenJetsPlusHadron = ak5GenJets.clone()
ak5GenJetsPlusHadron.src = "genParticlesForJetsPlusBHadronNoNu"

improvedHadronMatchingSequence = cms.Sequence(genParticlesForJetsPlusBHadronNoNu * ak5GenJetsPlusHadron)

