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
# C o n f i g u r e   P A T   T r i g g e r 
#
# - - -

## needed for L1 lookup tables
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('START36_V4::All')

## trigger sequences
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )

## define HLT_Mu9 matches
process.muonTriggerMatchHLTMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "selectedPatMuons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerMuon' ),
    filterIds      = cms.vint32 ( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_Mu9' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( True )
)

## take obsolete matches out of the patTriggerMatcher sequence
## and add the match that is relevant for this analysis
process.patTriggerMatcher += process.muonTriggerMatchHLTMuons
process.patTriggerMatcher.remove( process.patTriggerMatcherElectron )
process.patTriggerMatcher.remove( process.patTriggerMatcherMuon )
process.patTriggerMatcher.remove( process.patTriggerMatcherTau )

## configure patTrigger & patTriggerEvent
process.patTrigger.onlyStandAlone = False
process.patTriggerEvent.patTriggerMatches = [ "muonTriggerMatchHLTMuons" ]



## create triggerMetchedMuons
process.load( "TopAnalysis.TopUtils.triggerMatchedMuons_cfi" )

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
    process.patTriggerSequence *
    process.triggerMatchedMuons
   #process.content *
   #process.out
)
