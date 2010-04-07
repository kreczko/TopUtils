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
    ## add your favourite file here
    "/store/user/henderle/samples/TTbar_madgraph_10TeV/PATtuple_1.root"
    )
)

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

#-------------------------------------------------
# add event weight
#-------------------------------------------------

## produce top generated event
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEventFilters_cff")
## branch A
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchB.electron = True
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchB.muon     = False
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchB.tau      = True
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchB.restrictTauDecays = cms.PSet(leptonic = cms.bool(True))
## branch B
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.electron = False
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.muon     = True
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.tau      = True
process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.restrictTauDecays = cms.PSet(leptonic = cms.bool(True))

## filter event asccording to expected final state configuration on gen level
process.load("TopAnalysis.TopUtils.GenFinalStateSelector_cfi")

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('patTuple.root'),
    eventContent = cms.PSet(
      outputCommands = cms.untracked.vstring('keep *_*_*_*')  ## keep all
    )
)

process.p1 = cms.Path(process.makeGenEvt *
                     #process.ttSemiLeptonicFilter  *
                      process.genFinalStateSelector *
                      process.out
                      )
