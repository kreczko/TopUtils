import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( False )
)

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/user/henderle/Spring10/WJets_MAD/PATtuple_8_1.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1000 )
)

process.load( "PhysicsTools.PatAlgos.selectionLayer1.muonSelector_cfi" )
process.tagMuons = process.selectedPatMuons.clone(
    src = "selectedPatMuons",
    cut = "isTrackerMuon=1 & isGlobalMuon=1 & "
    "innerTrack.numberOfValidHits>=11 & globalTrack.normalizedChi2<10.0 & "
    "globalTrack.hitPattern.numberOfValidMuonHits>0 & abs(dB)<0.02 & "
    "(trackIso+caloIso)/pt<0.05"
)

process.load( "TopAnalysis.TopUtils.taggedProbeMuons_cfi" )
process.taggedProbeMuons.tags = "tagMuons"

process.content = cms.EDAnalyzer("EventContentAnalyzer")

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('patTuple.root'),
    eventContent = cms.PSet(
      outputCommands = cms.untracked.vstring('keep *_*_*_*')  ## keep all
    )
)

process.p = cms.Path(
   #process.content *
    process.tagMuons *
    process.taggedProbeMuons *
    process.out
)
