import FWCore.ParameterSet.Config as cms
from TopAnalysis.TopUtils.GenLevelBJetProducer_cfi import *

produceGenLevelBJetsPlusHadron = produceGenLevelBJets.clone()
produceGenLevelBJetsPlusHadron.genJets = cms.InputTag('ak5GenJetsPlusHadron','','')

