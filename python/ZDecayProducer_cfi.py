import FWCore.ParameterSet.Config as cms

ZDecayProducer = cms.EDProducer("ZDecayProducer",
    src = cms.InputTag("genParticles"))
