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
  )#,
  ## configuration of excluded mother particle                               
    #ancestorIgnore = cms.PSet(
    #pdgId = cms.vstring("5")
  #)
)

isolatedGenElectrons = cms.EDProducer("GenCandSelector",
  ## input collection
  src = cms.InputTag("genParticles"),
  ## configuration of target particle
  target = cms.PSet(
    pdgId  = cms.vstring("11"),
    status = cms.int32( 1) 
  ),
  ## configuration of mother particle
  ancestor = cms.PSet(
    pdgId = cms.vstring("24")
  )#,
  ## configuration of excluded mother particle                               
    #ancestorIgnore = cms.PSet(
    #pdgId = cms.vstring("5")
  #)
)

