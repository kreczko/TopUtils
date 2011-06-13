import FWCore.ParameterSet.Config as cms

process = cms.Process("TOP")

## MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")

## Options and Output Report
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

## Input Source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:patTuple.root')
)
## Maximal Number of Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

## Geometry and Detector Conditions (needed for jet track association)
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#from Configuration.AlCa.autoCond import autoCond 
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
process.load("Configuration.StandardSequences.MagneticField_cff")

## Standard Pat Configuration File
process.load("PhysicsTools.PatAlgos.patSequences_cff")

#from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string('testFile.root'),
#                               # save only events passing the full path
#                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                               # save PAT Layer 1 output; you need a '*' to
#                               # unpack the list of commands 'patEventContent'
#                               outputCommands = cms.untracked.vstring('drop *', *patEventContent )
#                               )
#
#process.outpath = cms.EndPath(process.out)


## this is just an example selection for muons
process.trackMuons = process.selectedPatMuons.clone(
    src = 'selectedPatMuons',
    cut = 'isGlobalMuon & isTrackerMuon &'
    'pt > 20. &'
    'abs(eta) < 2.1 &'
    'innerTrack.numberOfValidHits >= 11 &'
    'globalTrack.normalizedChi2 < 10.0  &'
    'globalTrack.hitPattern.numberOfValidMuonHits>0 &'
    'abs(dB)<0.02 &'
    'innerTrack.hitPattern.pixelLayersWithMeasurement>=1 &'
    'numberOfMatches>1'
)


## load configs for pf jet clustering and MET
process.load("PhysicsTools.PFCandProducer.pfMET_cfi")
process.load("PhysicsTools.PFCandProducer.pfJets_cff")

## cutomized top projectors for pat muons and electrons
process.load("TopAnalysis.TopUtils.patMuonTopProjector_cfi")
process.load("TopAnalysis.TopUtils.patElectronTopProjector_cfi")

## this needs to be re-run to compute rho for the L1Fastjet corrections
process.load("RecoJets.JetProducers.kt4PFJets_cfi")
process.kt6PFJets = process.kt4PFJets.clone(src='pfNoTrackMuons', doAreaFastjet=True, doRhoFastjet=True, rParam=0.6)

## cutomize the jet clustering (the default for pfJets is AK5 already,
## the input should be the new top-projected pfNoPileUp collection)
process.pfNoTrackMuons = process.pfNoPatMuons.clone(topCollection = "trackMuons")
process.pfJets.src = 'pfNoTrackMuons'


## customize the pat jets
from PhysicsTools.PatAlgos.tools.jetTools import *
switchJetCollection(
    process,cms.InputTag('pfJets'),
    doJTA        = True,
    doBTagging   = True,
    jetCorrLabel = ('AK5PF', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute'])),
    doType1MET   = False,
    genJetCollection=cms.InputTag("ak5GenJets"),
    doJetID      = False,
    outputModule = ''
)

## no calo towers for pfJets
process.patJets.embedCaloTowers     = False
## not needed, we keep all pfCandidates in the tuple
process.patJets.embedPFCandidates   = False

## remove soft lepton taggers, which would have needed more RECO collections as input
process.patDefaultSequence.remove(process.softMuonBJetTagsAOD)
process.patDefaultSequence.remove(process.softMuonByPtBJetTagsAOD)
process.patDefaultSequence.remove(process.softMuonByIP3dBJetTagsAOD)
process.patDefaultSequence.remove(process.softMuonTagInfosAOD)

process.patJets.tagInfoSources = []
process.patJets.discriminatorSources.remove(cms.InputTag("softMuonBJetTagsAOD"))
process.patJets.discriminatorSources.remove(cms.InputTag("softMuonByPtBJetTagsAOD"))
process.patJets.discriminatorSources.remove(cms.InputTag("softMuonByIP3dBJetTagsAOD"))

## remove all but jets from the sequence
from PhysicsTools.PatAlgos.tools.coreTools import *
removeAllPATObjectsBut(process, ['Jets'], False)
removeCleaning(process, False)
process.patDefaultSequence.remove(process.countPatLeptons)
process.patDefaultSequence.remove(process.countPatJets)

## re-configure and create MET
process.pfMET.src = 'pfNoPileup'
process.patMETs.metSource = "pfMET"
process.patMETs.addMuonCorrections = False

## let it run
process.pf2pat = cms.Sequence(
  # this one is only for testing
    process.trackMuons *
    process.pfNoTrackMuons *
  # jet clustering
    process.pfJetSequence *
    process.kt6PFJets *
    process.pfMET *
    process.patDefaultSequence
)
