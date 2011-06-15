import FWCore.ParameterSet.VarParsing as VarParsing
import sys

# setup 'standard' options
options = VarParsing.VarParsing ('standard')
## decide whether to run on MC or data
options.register('runOnMC', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "decide to run on MC or data")
## is the MC ttbar (has no effect once runOnMC==False)
options.register('isTTBarMC', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "decide to produce ttGenEvent")
## global Tag to be used (if left empty pat default tag is used)
options.register('globalTag', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which globalTag should be used")
## tag of TriggerResults collection
options.register('triggerTag', 'HLT', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "tag used for the triggerResults")

# get and parse the command line arguments
if( hasattr(sys, "argv") ):
    for args in sys.argv :
        arg = args.split(',')
        for val in arg:
            val = val.split('=')
            if(len(val)==2):
                setattr(options,val[0], val[1])

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

if not options.globalTag == '':
    process.GlobalTag.globaltag = options.globalTag + '::All'

### test configuration
#process.source.fileNames = cms.untracked.vstring()
#process.source.fileNames += 'file:/scratch/hh/current/cms/user/eschliec/TTBar_pythia6_Summer11/7ABCBB94-E67D-E011-9B16-00151796D80C.root',
#process.source.fileNames += 'file:/scratch/hh/current/cms/user/eschliec/TTBar_pythia6_Summer11/4A5D814A-E67D-E011-9052-0015178C0190.root',
#process.source.fileNames += '/store/data/Run2011A/MultiJet/AOD/PromptReco-v4/000/166/841/0C96BC23-F195-E011-A8AB-0030487C608C.root',
#process.source.fileNames += '/store/data/Run2011A/MultiJet/AOD/PromptReco-v4/000/166/033/76F06A70-348E-E011-B6DA-001D09F25438.root',
#process.source.fileNames += '/store/data/Run2011A/MultiJet/AOD/PromptReco-v4/000/166/863/F8487AF6-3496-E011-9A68-001D09F23A20.root',
#process.source.fileNames += '/store/data/Run2011A/MultiJet/AOD/PromptReco-v4/000/165/103/76696D89-EE80-E011-AEF7-003048F024E0.root',
process.maxEvents.input = cms.untracked.int32(-1)
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

if options.runOnMC:

    # reduce size
    process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
    process.genParticles = cms.EDProducer(
        #see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGenParticlePruner
        "GenParticlePruner",
        src = cms.InputTag("genParticles"),
        select = cms.vstring(
            "drop  *  ",
            "++keep++ pdgId = {Z0}",
            "++keep++ pdgId = {W+}",   "++keep++ pdgId = {W-}",
            "++keep++ pdgId = {t}",    "++keep++ pdgId = {tbar}",
            "++keep++ pdgId = {b}",    "++keep++ pdgId = {bbar}",
            "++keep++ pdgId = {c}",    "++keep++ pdgId = {cbar}",
            # "drop pdgId = {Z0} & status = 2"
            "++keep++ pdgId = {e+}",   "++keep++ pdgId = {e-}",
            "++keep++ pdgId = {mu+}",  "++keep++ pdgId = {mu-}",
            "++keep++ pdgId = {tau+}", "++keep++ pdgId = {tau-}",
            )
        )
    
    ## needed for redoing the ak5GenJets
    process.load("TopAnalysis.TopUtils.GenJetParticles_cff")
    process.load("RecoJets.Configuration.RecoGenJets_cff")

    ## make sure the genJets are clustered from the full genParticle collection and not the pruned one
    process.genParticlesForJets.src = cms.InputTag("genParticles","","HLT")
    
    if options.isTTBarMC:
        ## std sequence to produce the ttGenEvt
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

        ## make sure the ttGenEvent is made from the full genParticle collection and not the pruned one
        process.initSubset.src  = cms.InputTag("genParticles","","HLT")
        process.decaySubset.src = cms.InputTag("genParticles","","HLT")

## selection for events from data

if not options.runOnMC:
    ## HBHE noise filter
    process.load("CommonTools.RecoAlgos.HBHENoiseFilter_cfi")
    ## s. https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1196.html
    process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
    process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
    process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999.

    ## event scraping filter
    process.scrapingFilter = cms.EDFilter( "FilterOutScraping"
                                         , applyfilter = cms.untracked.bool( True )
                                         , debugOn     = cms.untracked.bool( False )
                                         , numtrack    = cms.untracked.uint32( 10 )
                                         , thresh      = cms.untracked.double( 0.25 )
                                         )

    ## high level trigger filter (non existing Triggers are ignored)
    process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
    process.trigger = process.hltHighLevel.clone(HLTPaths = [#2010 trigger ('v*' to be immune to version changes)
                                                             '*',
                                                             #2011 1E33 trigger ('v*' to be immune to version changes)
                                                             '*',
                                                             #2011 1E33-2E33 trigger ('v*' to be immune to version changes)
                                                             '*'],
                                                 TriggerResultsTag = cms.InputTag("TriggerResults","",options.triggerTag),
                                                 throw = False)
    

## let it run
process.p = cms.Path()

if not options.runOnMC:
    process.p += process.HBHENoiseFilter
    process.p += process.scrapingFilter
    process.p += process.trigger

if options.runOnMC:
    if options.isTTBarMC:
        process.p += process.makeGenEvt

    process.p += process.genParticles
    process.p += process.genJetParticles
    process.p += process.ak5GenJets

process.out.outputCommands+= ['keep *DcsStatus*_*_*_*']
## all tracks in the event
process.out.outputCommands+= ['keep recoTracks_generalTracks_*_*']
## beamspot und primary vertices
process.out.outputCommands+= ['keep *_offlineBeamSpot_*_*']
process.out.outputCommands+= ['keep *_offlinePrimaryVertices*_*_*']
process.out.outputCommands+= ['keep *_goodOfflinePrimaryVertices*_*_*']
## collections for PF2PAT
process.out.outputCommands+= ['keep *_particleFlow_*_*']
## trigger information
process.out.outputCommands+= ['keep edmTriggerResults_*_*_*']
process.out.outputCommands+= ['keep *_hltTriggerSummaryAOD_*_*']
## data needed for muons
process.out.outputCommands+= ['keep *_muons__*']
process.out.outputCommands+= ['keep *_*_muCorrData_*']
process.out.outputCommands+= ['keep *_tevMuons_*_*']
process.out.outputCommands+= ['keep *_globalMuons__*']
process.out.outputCommands+= ['keep *_standAloneMuons_*_*']
## data needed for electrons
process.out.outputCommands+= ['keep *_*Gsf*_*_*']
process.out.outputCommands+= ['keep *_*gsf*_*_*']
process.out.outputCommands+= ['keep *SuperCluster*_*_*_*']
## generator information
if options.runOnMC:
    ## skimmed genParticles
    process.out.outputCommands+= ['keep recoGenParticles_genParticles_*_PAT']
    ## ak5GenJets
    process.out.outputCommands+= ['keep recoGenJets_ak5GenJets_*_PAT']
    ## genMET
    process.out.outputCommands+= ["keep *_genMetTrue_*_*"]
    ## pile-up information
    process.out.outputCommands+= ['keep *_addPileupInfo_*_*']
    ## further info on generated event (like pt-hat)
    process.out.outputCommands+= ['keep GenEventInfoProduct_*_*_*']
    ## ttGenEvent
    if options.isTTBarMC:
        process.out.outputCommands+= ['keep *_genEvt_*_*']
        process.out.outputCommands+= ['keep *_initSubset_*_*']
        process.out.outputCommands+= ['keep *_decaySubset_*_*']
