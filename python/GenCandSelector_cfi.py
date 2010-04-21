import FWCore.ParameterSet.Config as cms

## TO BE DONE: need additional selection for lepton+jets correction:collection of muons coming from tau which has a W or Z as mother

isolatedGenMuons = cms.EDProducer("GenCandSelector",
  ## input collection
  src = cms.InputTag("genParticles"),

  ## configuration of target particle
  target = cms.PSet(
    pdgId  = cms.uint32(13),
    status = cms.uint32( 3) 
  ),

  ## configuration of mother particle
  mother = cms.PSet(
    pdgIds = cms.vuint32(23, 24),
    # comment out if you do not want to use the ancestor-option
    #ancestor = cms.uint32(6)
  )
)

