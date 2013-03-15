import FWCore.ParameterSet.Config as cms
from TopAnalysis.TopUtils.GenJetParticles_cfi import *


genJetParticlesPlusHadron = cms.Sequence(genParticlesForJetsPlusHadron)

hiGenParticlesForJetsPlusHadron = genParticlesForJetsPlusHadron.clone()
hiGenParticlesForJetsPlusHadron.src = cms.InputTag("hiGenParticles")

genParticlesForJetsNoNuPlusHadron = genParticlesForJetsPlusHadron.clone()
genParticlesForJetsNoNuPlusHadron.ignoreParticleIDs += cms.vuint32( 12,14,16)

genParticlesForJetsNoMuNoNuPlusHadron = genParticlesForJetsPlusHadron.clone()
genParticlesForJetsNoMuNoNuPlusHadron.ignoreParticleIDs += cms.vuint32( 12,13,14,16)
