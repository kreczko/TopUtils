import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.pfMuons_cff import *
from CommonTools.ParticleFlow.pfNoPileUp_cff import *
from CommonTools.ParticleFlow.pfElectrons_cff import *
from CommonTools.ParticleFlow.ParticleSelectors.pfSortByType_cff import *

## minimal restriction to charged hadrons
pfSelectedChargedHadrons = cms.EDFilter(
    "GenericPFCandidateSelector",
    src = cms.InputTag("pfAllChargedHadrons"),
    cut = cms.string("")
)

## minimal restriction to neutral hadrons
pfSelectedNeutralHadrons = cms.EDFilter(
    "GenericPFCandidateSelector",
    src = cms.InputTag("pfAllNeutralHadrons"),
    cut = cms.string("et>1")
)

## minimal restriction to photons
pfSelectedPhotons = cms.EDFilter(
    "GenericPFCandidateSelector",
    src = cms.InputTag("pfAllPhotons"),
    cut = cms.string("et>1")
)

## isolation configuration for the muons
pfAllMuons.src = 'pfNoPileUp'
pfSelectedMuons.src = 'pfAllMuons'
isoDepMuonWithCharged.src = 'pfAllMuons'
isoDepMuonWithNeutral.src = 'pfAllMuons'
isoDepMuonWithPhotons.src = 'pfAllMuons'
isoDepMuonWithCharged.ExtractorPSet.inputCandView = 'pfSelectedChargedHadrons'
isoDepMuonWithNeutral.ExtractorPSet.inputCandView = 'pfSelectedNeutralHadrons'
isoDepMuonWithPhotons.ExtractorPSet.inputCandView = 'pfSelectedPhotons'

## isolation configuration for the electrons
pfAllElectrons.src = 'pfNoPileUp'
pfSelectedElectrons.src = 'pfAllElectrons'
isoDepElectronWithCharged.src = 'pfAllElectrons'
isoDepElectronWithNeutral.src = 'pfAllElectrons'
isoDepElectronWithPhotons.src = 'pfAllElectrons'
isoDepElectronWithCharged.ExtractorPSet.inputCandView = 'pfSelectedChargedHadrons'
isoDepElectronWithNeutral.ExtractorPSet.inputCandView = 'pfSelectedNeutralHadrons'
isoDepElectronWithPhotons.ExtractorPSet.inputCandView = 'pfSelectedPhotons'

particleFlow = cms.Sequence(
  # pileup and candidate classification for isolation
    pfNoPileUpSequence
  + pfSortByTypeSequence
  + pfSelectedChargedHadrons
  + pfSelectedNeutralHadrons
  + pfSelectedPhotons
  # configuration of electrons
  + pfAllElectrons
  + pfSelectedElectrons
  + pfElectronIsolationSequence
  # configuration of muons
  + pfAllMuons
  + pfSelectedMuons
  + pfMuonIsolationSequence
)
