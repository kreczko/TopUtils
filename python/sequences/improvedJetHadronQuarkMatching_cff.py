import FWCore.ParameterSet.Config as cms
from RecoJets.Configuration.RecoGenJets_cff import ak5GenJets
from TopAnalysis.TopUtils.GenJetParticles_cff import genParticlesForJetsPlusHadron
from TopAnalysis.TopUtils.GenJetParticles_cff import genParticlesForJetsNoNuPlusHadron

ak5GenJetsPlusHadron = ak5GenJets.clone()
ak5GenJetsPlusHadron.src = "genParticlesForJetsPlusHadron"

ak5GenJetsNoNuPlusHadron = ak5GenJets.clone()
ak5GenJetsNoNuPlusHadron.src = "genParticlesForJetsNoNuPlusHadron"

improvedJetHadronQuarkMatchingSequence = cms.Sequence(genParticlesForJetsPlusHadron * ak5GenJetsPlusHadron)
improvedJetNoNuHadronQuarkMatchingSequence = cms.Sequence(genParticlesForJetsNoNuPlusHadron * ak5GenJetsNoNuPlusHadron)

