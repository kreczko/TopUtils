import FWCore.ParameterSet.Config as cms
from RecoJets.Configuration.RecoGenJets_cff import ak5GenJets
from TopAnalysis.TopUtils.GenJetParticles_cff import genParticlesForJetsPlusBHadron
from TopAnalysis.TopUtils.GenJetParticles_cff import genParticlesForJetsPlusBHadronNoNu

ak5GenJetsPlusHadron = ak5GenJets.clone()
ak5GenJetsPlusHadron.src = "genParticlesForJetsPlusBHadron"

ak5GenJetsNoNuPlusHadron = ak5GenJets.clone()
ak5GenJetsNoNuPlusHadron.src = "genParticlesForJetsPlusBHadronNoNu"

improvedJetHadronQuarkMatchingSequence = cms.Sequence(genParticlesForJetsPlusBHadron * ak5GenJetsPlusHadron)
improvedJetNoNuHadronQuarkMatchingSequence = cms.Sequence(genParticlesForJetsNoNuPlusBHadron * ak5GenJetsNoNuPlusHadron)

