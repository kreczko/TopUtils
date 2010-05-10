import FWCore.ParameterSet.Config as cms

#-------------------------------------------------
# cfg file for the analysis of selected muon
# quantities
#-------------------------------------------------
process = cms.Process("UserTest")

## configure message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.categories.append('GenCandSelector')
process.MessageLogger.cerr.GenCandSelector = cms.untracked.PSet(
    limit = cms.untracked.int32(100)
)
#process.MessageLogger.cerr.threshold = 'INFO'
#process.MessageLogger.cerr.FwkReport.reportEvery = 100

#-------------------------------------------------
# process configuration
#-------------------------------------------------

## define input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    ## add your favourite file here
    '/store/user/henderle/7TeV/TTbar/PATtuple_1.root'
    #'/store/user/henderle/samples/Wjets_madgraph_10TeV/PATtuple_117.root'
    )
)

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

#-------------------------------------------------
# add event weight
#-------------------------------------------------

## add event weight information
process.load("TopAnalysis.TopUtils.GenCandSelector_cfi")

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('patTuple_test.root'),
    eventContent = cms.PSet(
      outputCommands = cms.untracked.vstring('keep *_*_*_*')  ## keep all
    )
)

process.p1 = cms.Path(process.isolatedGenMuons ## *
##                    process.out
                      )
