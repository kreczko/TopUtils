import FWCore.ParameterSet.Config as cms

isolatedGenMuons = cms.EDProducer("GenCandSelector",
  ## input collection
  src = cms.InputTag("genParticles"),
  ## configuration of target particle
  target = cms.PSet(
    pdgId  = cms.vstring("13"),
    status = cms.int32( 1) 
  ),
  ## configuration of mother particle
  ancestor = cms.PSet(
    pdgId = cms.vstring("24")
  )
)

