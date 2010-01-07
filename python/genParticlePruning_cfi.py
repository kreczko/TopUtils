import FWCore.ParameterSet.Config as cms

from SimGeneral.HepPDTESSource.pythiapdt_cfi import *

## collection of generator level muons that originate
## from W-bosons
prunedGenParticles = cms.EDProducer("GenParticlePruner",
  src = cms.InputTag("genParticles"),
  select = cms.vstring(
  "keep+ pdgId = {W-}",   ## keep W-  and its next daughter generation
  "keep+ pdgId = {W+}",   ## keep W+  and its next daughter generation
  "keep+ pdgId = {Z0}"    ## keep Z0  and its next daughter generation
  )
)
