import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

## configure message logger
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( False )
)

## define input
process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/user/henderle/Spring10/WJets_MAD/PATtuple_8_1.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1000 )
)

# - - -
#
# D e f i n e   T a g   a n d   P r o b e   C o l l e c t i o n
#
# - - -

## import muon selector
process.load( "PhysicsTools.PatAlgos.selectionLayer1.muonSelector_cfi" )

## define tag collection
process.tagMuons = process.selectedPatMuons.clone(
    src = "selectedPatMuons",
    cut = "isTrackerMuon=1 & isGlobalMuon=1 & "
          "innerTrack.numberOfValidHits>=11 & "
          "globalTrack.normalizedChi2<10.0  & "
          "globalTrack.hitPattern.numberOfValidMuonHits>0 & "
          "abs(dB)<0.02 & "
          "(trackIso+caloIso)/pt<0.05"
)

## define probe muons
process.probeMuons = process.selectedPatMuons.clone(
    src = "selectedPatMuons",
    cut = "isTrackerMuon=1 & isGlobalMuon=1"
)

## configure module
process.load( "TopAnalysis.TopUtils.taggedProbeMuons_cfi" )
process.taggedProbeMuons.tags   = "tagMuons"
process.taggedProbeMuons.probes = "probeMuons"
process.taggedProbeMuons.mass   =  90
process.taggedProbeMuons.deltaM =   5

## check content
process.content = cms.EDAnalyzer("EventContentAnalyzer")

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('patTuple.root'),
    eventContent = cms.PSet(
      outputCommands = cms.untracked.vstring('keep *_*_*_*')  ## keep all
    )
)

process.p = cms.Path(
    process.tagMuons +
    process.probeMuons *
    process.taggedProbeMuons
   #process.content *
   #process.out
)
