import FWCore.ParameterSet.Config as cms
from TopAnalysis.TopUtils.GenJetParticles_cfi import *


genJetParticlesPlusBHadron = cms.Sequence(genParticlesForJetsPlusBHadron)

hiGenParticlesForJetsPlusBHadron = genParticlesForJetsPlusBHadron.clone()
hiGenParticlesForJetsPlusBHadron.src = cms.InputTag("hiGenParticles")

genParticlesForJetsPlusBHadronNoNu = genParticlesForJetsPlusBHadron.clone()
genParticlesForJetsPlusBHadronNoNu.ignoreParticleIDs += cms.vuint32( 12,14,16)

genParticlesForJetsPlusBHadronNoMuNoNu = genParticlesForJetsPlusBHadron.clone()
genParticlesForJetsPlusBHadronNoMuNoNu.ignoreParticleIDs += cms.vuint32( 12,13,14,16)
