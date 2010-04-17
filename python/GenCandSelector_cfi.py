import FWCore.ParameterSet.Config as cms

isolatedGenMuons = cms.EDProducer("GenCanSelector",
  ## input collection
  src = cms.InputTag("genParticles"),

  ## configuration of target particle
  target = cms.PSet(
    pdgId  = cms.int32(13),
    status = cms.int32( 3) 
  ),

  ## configuration of mother particle
  mother = cms.PSet(
    pdgIds = cms.vint32(23, 24),
    ancestor = cms.int32(6)
  )
)

