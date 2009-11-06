import FWCore.ParameterSet.Config as cms

#-------------------------------------------------
# cfg file for the analysis of selected muon
# quantities
#-------------------------------------------------
process = cms.Process("UserTest")

## configure message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
#process.MessageLogger.cerr.FwkReport.reportEvery = 100

#-------------------------------------------------
# process configuration
#-------------------------------------------------

## define input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/user/rwolf/ttbar09/patTuple_all_0_ttbar09.root'
    ## add your favourite file here
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
process.load("TopAnalysis.TopUtils.TagAndProbeAssociator_cfi")

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('patTuple.root'),
    eventContent = cms.PSet(
      outputCommands = cms.untracked.vstring('keep *_*_*_*')  ## keep all
    )
)

process.p1 = cms.Path(process.testMap *
                      process.out
                      )
