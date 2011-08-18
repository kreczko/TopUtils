import FWCore.ParameterSet.Config as cms
import os

# usage of this file is documented in
# https://twiki.cern.ch/twiki/bin/view/CMS/HamburgWikiAnalysisTopPrependPF2PATSequence

def prependPF2PATSequence(process, pathnames = [''], options = dict()):

    ## set defaults if variable was not given to function
    options.setdefault('runOnMC', True)
    options.setdefault('runOnAOD', False)
    options.setdefault('switchOffEmbedding', True)
    options.setdefault('addResolutions', True)
    #options.setdefault('postfix', '')
    options.setdefault('runOnOLDcfg', False)
    options.setdefault('cutsMuon', 'pt > 10. & abs(eta) < 2.5')
    options.setdefault('cutsElec', 'et > 20. & abs(eta) < 2.5')
    options.setdefault('cutsJets', 'pt > 10. & abs(eta) < 5.0')
    options.setdefault('electronIDs', 'CiC') ## can be set to CiC, classical
    options.setdefault('pfIsoConeMuon', 0.3)
    options.setdefault('pfIsoConeElec', 0.3)
    options.setdefault('pfIsoValMuon', 0.2)
    options.setdefault('pfIsoValElec', 0.2)
    options.setdefault('skipIfNoPFMuon', False)
    options.setdefault('skipIfNoPFElec', False)
    options.setdefault('addNoCutPFMuon', False)
    options.setdefault('addNoCutPFElec', False)
    options.setdefault('noMuonTopProjection', False)
    options.setdefault('noElecTopProjection', False)
    options.setdefault('analyzersBeforeMuonIso', cms.Sequence())
    options.setdefault('analyzersBeforeElecIso', cms.Sequence())
    options.setdefault('excludeElectronsFromWsFromGenJets', False)
    options.setdefault('METCorrectionLevel', 0)

    if 'applyMETCorrections' in options:
        raise KeyError, "The option 'applyMETCorrections' is not supported anymore by prependPF2PATSequence, please use 'METCorrectionLevel'! 'METCorrectionLevel' may be set to 0,1,2 (no correction, TypeI, TypeI+TypeII corrections)"

    ## tool to replace all input tags in a given sequence
    from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag

    ## postfixes are NOT supported right now
    postfix='' #options['postfix']

    # check if needed standard sequences are available, else load them
    if not hasattr(process, 'XMLIdealGeometryESSource'):
        process.load("Configuration.StandardSequences.Geometry_cff")

    if not hasattr(process, 'magfield'):
        process.load("Configuration.StandardSequences.MagneticField_cff")

    ## standard PAT configuration file
    process.load("PhysicsTools.PatAlgos.patSequences_cff")

    ## create a good vertex collection
    pvSelection = cms.PSet( minNdof = cms.double( 4.)
                          , maxZ    = cms.double(24.)
                          , maxRho  = cms.double( 2.)
                          )

    process.goodOfflinePrimaryVertices = cms.EDFilter( "PrimaryVertexObjectFilter" # checks for fake PVs automatically
                                                     , filterParams = pvSelection
                                                     , filter       = cms.bool(True) # use as producer and filter
                                                     , src          = cms.InputTag('offlinePrimaryVertices')
                                                     )

    process.goodOfflinePrimaryVerticesWithBS = cms.EDFilter( "PrimaryVertexObjectFilter" # checks for fake PVs automatically
                                                           , filterParams = pvSelection
                                                           , filter       = cms.bool(False) # use only as producer
                                                           , src          = cms.InputTag('offlinePrimaryVerticesWithBS')
                                                           )

    ## only added as the usePF2PAT function needs it to work, is deleted immediately afterwards
    process.out = cms.OutputModule( "PoolOutputModule"
                                  , fileName = cms.untracked.string('dummyFile.root')
                                  , outputCommands = cms.untracked.vstring('drop *')
                                  )

    ## choose correct set of jec levels for MC and data
    if options['runOnMC']:
        jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
    else:
        jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']

    ## adaptions needed to run with CMSSW 41X
    if os.getenv('CMSSW_VERSION').startswith('CMSSW_4_1_'):
        ## run the full PF2PAT sequence
        from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
        usePF2PAT( process
                 , runPF2PAT      = True
                 , runOnMC        = options['runOnMC']
                 , jetAlgo        = 'AK5'
                 , postfix        = postfix
                 )

        getattr(process,'patJetCorrFactors'+postfix).levels = cms.vstring(jecLevels)
        getattr(process,'patJetCorrFactors'+postfix).payload = 'AK5PFchs'

        getattr(process,'patDefaultSequence'+postfix).remove(getattr(process,'selectedPatTaus'+postfix))

    ## for all other releases use the new configuration
    else:
        ## run the full PF2PAT sequence
        from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
        usePF2PAT( process
                 , runPF2PAT      = True
                 , runOnMC        = options['runOnMC']
                 , jetAlgo        = 'AK5'
                 , postfix        = postfix
                 , jetCorrections = ('AK5PFchs',cms.vstring(jecLevels))
                 )

    delattr(process,'out')

    ##
    ## customize PAT
    ##

    if not os.getenv('CMSSW_VERSION').startswith('CMSSW_4_1_'):
        process.pfPileUp.checkClosestZVertex = False

    ## remove taus and photons from pat sequence
    from PhysicsTools.PatAlgos.tools.coreTools import removeSpecificPATObjects, removeCleaning
    removeSpecificPATObjects( process
                            , names = ['Taus', 'Photons']
                            , outputInProcess = False
                            , postfix = postfix
                            )
    removeCleaning( process
                  , outputInProcess = False
                  , postfix = postfix
                  )

    ## make selection for the pf candidates

    ## minimal restriction to charged hadrons
    pfSelectedChargedHadrons = cms.EDFilter( "GenericPFCandidateSelector"
                                           , src = cms.InputTag("pfAllChargedHadrons")
                                           , cut = cms.string("")
                                           )

    ## minimal restriction to neutral hadrons
    pfSelectedNeutralHadrons = cms.EDFilter( "GenericPFCandidateSelector"
                                           , src = cms.InputTag("pfAllNeutralHadrons")
                                           , cut = cms.string("") # et>1 # not yet official
                                           )

    ## minimal restriction to photons
    pfSelectedPhotons = cms.EDFilter( "GenericPFCandidateSelector"
                                    , src = cms.InputTag("pfAllPhotons")
                                    , cut = cms.string("") # et>1 # not yet official
                                    )

    setattr(process,'pfSelectedChargedHadrons'+postfix, pfSelectedChargedHadrons)
    setattr(process,'pfSelectedNeutralHadrons'+postfix, pfSelectedNeutralHadrons)
    setattr(process,'pfSelectedPhotons'       +postfix, pfSelectedPhotons       )

    ## add selected pf candidates to the sequence
    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfAllPhotons'+postfix), getattr(process,'pfAllPhotons'+postfix)
                                                        * getattr(process,'pfSelectedChargedHadrons'+postfix)
                                                        * getattr(process,'pfSelectedNeutralHadrons'+postfix)
                                                        * getattr(process,'pfSelectedPhotons'+postfix)
                                                        )

    ## removal of unnecessary modules
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'patPFParticles'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'patCandidateSummary'+postfix))

    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'selectedPatPFParticles'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'selectedPatCandidateSummary'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'countPatElectrons'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'countPatMuons'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'countPatLeptons'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'countPatJets'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'countPatPFParticles'+postfix))

    ## embedding of resolutions into the patObjects
    if options['addResolutions']:
        process.load("TopQuarkAnalysis.TopObjectResolutions.stringResolutions_etEtaPhi_cff")


    ##
    ## customize muons
    ##

    ## disable muon top projections
    if options['noMuonTopProjection']:
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoMuon'+postfix))

    ## switch muons to be originating from the primary vertex
    getattr(process,'pfSelectedMuons'+postfix).src = 'pfMuonsFromVertex'+postfix

    ## apply selection for muons
    getattr(process,'pfSelectedMuons'+postfix).cut = options['cutsMuon']

    ## calculate isolation only for (kinematically) selected pfMuons
    getattr(process,'isoDepMuonWithCharged'+postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'isoDepMuonWithNeutral'+postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'isoDepMuonWithPhotons'+postfix).src = 'pfSelectedMuons'+postfix
    ## calculate isolation only based on selected pfCandidates
    getattr(process,'isoDepMuonWithCharged'+postfix).ExtractorPSet.inputCandView = 'pfSelectedChargedHadrons'
    getattr(process,'isoDepMuonWithNeutral'+postfix).ExtractorPSet.inputCandView = 'pfSelectedNeutralHadrons'
    getattr(process,'isoDepMuonWithPhotons'+postfix).ExtractorPSet.inputCandView = 'pfSelectedPhotons'

    ## adapt isolation cone for muons
    getattr(process,'isoValMuonWithNeutral'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'isoValMuonWithCharged'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'isoValMuonWithPhotons'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])

    ## adapt isolation cut
    getattr(process,'pfIsolatedMuons'+postfix).combinedIsolationCut = options['pfIsoValMuon']

    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfIsolatedMuons'+postfix)
                                                        , options['analyzersBeforeMuonIso'] * getattr(process,'pfIsolatedMuons'+postfix)
                                                        )

    ## filter event if no isolated muon found
    if options['skipIfNoPFMuon']:
        process.isoMuonFilter = cms.EDFilter( "CandViewCountFilter"
                                            , src = cms.InputTag("pfIsolatedMuons"+postfix)
                                            , minNumber = cms.uint32(1)
                                            )

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfIsolatedMuons'+postfix), getattr(process,'pfIsolatedMuons'+postfix)
                                                            * process.isoMuonFilter
                                                            )

    ## use beamspot for dB calculation
    getattr(process,'patMuons'+postfix).usePV = False

    ## adding of resolutions into the patObjects
    if options['addResolutions']:
        getattr(process,'patMuons'+postfix).addResolutions = True
        getattr(process,'patMuons'+postfix).resolutions = cms.PSet( default = cms.string("muonResolution") )

    ## switch off embeding of collections
    if options['switchOffEmbedding']:
        getattr(process,'patMuons'+postfix).embedTpfmsMuon          = False
        #getattr(process,'patMuons'+postfix).embedHighLevelSelection = False
        getattr(process,'patMuons'+postfix).embedCaloMETMuonCorrs   = False
        getattr(process,'patMuons'+postfix).embedPFCandidate        = False
        getattr(process,'patMuons'+postfix).embedStandAloneMuon     = False
        getattr(process,'patMuons'+postfix).embedTrack              = False
        getattr(process,'patMuons'+postfix).embedTcMETMuonCorrs     = False
        getattr(process,'patMuons'+postfix).embedCombinedMuon       = False
        getattr(process,'patMuons'+postfix).embedGenMatch           = False
        getattr(process,'patMuons'+postfix).embedPickyMuon          = False

    ## no cut muons
    if options['addNoCutPFMuon']:
        setattr(process,'noCutIsoDepMuonWithCharged'+postfix, getattr(process,'isoDepMuonWithCharged'+postfix).clone(src = 'pfAllMuons'+postfix))
        setattr(process,'noCutIsoDepMuonWithNeutral'+postfix, getattr(process,'isoDepMuonWithNeutral'+postfix).clone(src = 'pfAllMuons'+postfix))
        setattr(process,'noCutIsoDepMuonWithPhotons'+postfix, getattr(process,'isoDepMuonWithPhotons'+postfix).clone(src = 'pfAllMuons'+postfix))

        noCutIsoDepMuon = cms.PSet( pfNeutralHadrons = cms.InputTag("noCutIsoDepMuonWithNeutral"+postfix)
                                  , pfChargedHadrons = cms.InputTag("noCutIsoDepMuonWithCharged"+postfix)
                                  , pfPhotons        = cms.InputTag("noCutIsoDepMuonWithPhotons"+postfix)
                                  )

        setattr(process,'noCutIsoValMuonWithCharged'+postfix, getattr(process,'isoValMuonWithCharged'+postfix).clone())
        setattr(process,'noCutIsoValMuonWithNeutral'+postfix, getattr(process,'isoValMuonWithNeutral'+postfix).clone())
        setattr(process,'noCutIsoValMuonWithPhotons'+postfix, getattr(process,'isoValMuonWithPhotons'+postfix).clone())

        getattr(process,'noCutIsoValMuonWithCharged'+postfix).deposits[0].src = 'noCutIsoDepMuonWithCharged'+postfix
        getattr(process,'noCutIsoValMuonWithNeutral'+postfix).deposits[0].src = 'noCutIsoDepMuonWithNeutral'+postfix
        getattr(process,'noCutIsoValMuonWithPhotons'+postfix).deposits[0].src = 'noCutIsoDepMuonWithPhotons'+postfix

        noCutIsoValMuon = cms.PSet( pfNeutralHadrons = cms.InputTag("noCutIsoValMuonWithNeutral"+postfix)
                                  , pfChargedHadrons = cms.InputTag("noCutIsoValMuonWithCharged"+postfix)
                                  , pfPhotons        = cms.InputTag("noCutIsoValMuonWithPhotons"+postfix)
                                  )

        noCutIsolationValueMapsMuon = cms.VInputTag( cms.InputTag("noCutIsoValMuonWithCharged")
                                                   , cms.InputTag("noCutIsoValMuonWithNeutral")
                                                   , cms.InputTag("noCutIsoValMuonWithPhotons")
                                                   )

        setattr(process,'noCutPfIsolatedMuons'+postfix, getattr(process,'pfIsolatedMuons'+postfix).clone( src = 'pfAllMuons'+postfix
                                                                                                        , isolationValueMaps   = noCutIsolationValueMapsMuon
                                                                                                        , combinedIsolationCut = 1e30
                                                                                                        ))

        setattr(process,'noCutMuonMatch'+postfix, getattr(process,'muonMatch'+postfix).clone(src = 'noCutPfIsolatedMuons'+postfix))

        setattr(process,'noCutPatMuons'+postfix, getattr(process,'patMuons'+postfix).clone( pfMuonSource     = 'noCutPfIsolatedMuons'+postfix
                                                                                          , genParticleMatch = 'noCutMuonMatch'+postfix
                                                                                          , isoDeposits      = noCutIsoDepMuon
                                                                                          , isolationValues  = noCutIsoValMuon
                                                                                          ))

        noCutPatMuonsSequence = cms.Sequence( getattr(process,'noCutIsoDepMuonWithCharged'+postfix)
                                            * getattr(process,'noCutIsoDepMuonWithNeutral'+postfix)
                                            * getattr(process,'noCutIsoDepMuonWithPhotons'+postfix)
                                            * getattr(process,'noCutIsoValMuonWithCharged'+postfix)
                                            * getattr(process,'noCutIsoValMuonWithNeutral'+postfix)
                                            * getattr(process,'noCutIsoValMuonWithPhotons'+postfix)
                                            * getattr(process,'noCutPfIsolatedMuons'+postfix)
                                            * getattr(process,'noCutMuonMatch'+postfix)
                                            * getattr(process,'noCutPatMuons'+postfix)
                                            )

        if not options['runOnMC']:
            noCutPatMuonsSequence.remove(getattr(process,'noCutMuonMatch'+postfix))

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfAllMuons'+postfix), getattr(process,'pfAllMuons'+postfix)
                                                            * noCutPatMuonsSequence
                                                            )


    ##
    ## customize electrons
    ##

    ## disable electron top projections
    if options['noElecTopProjection']:
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoElectron'+postfix))

    ## switch muons to be originating from the primary vertex
    getattr(process,'pfSelectedElectrons'+postfix).src = 'pfElectronsFromVertex'+postfix

    ## apply selection for electrons (they are the source of the pat::electrons)
    getattr(process,'pfSelectedElectrons'+postfix).cut = options['cutsElec']

    ## calculate isolation only for (kinematically) selected pfElectrons
    getattr(process,'isoDepElectronWithCharged'+postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'isoDepElectronWithNeutral'+postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'isoDepElectronWithPhotons'+postfix).src = 'pfSelectedElectrons'+postfix
    ## calculate isolation only based on selected pfCandidates
    getattr(process,'isoDepElectronWithCharged'+postfix).ExtractorPSet.inputCandView = 'pfSelectedChargedHadrons'
    getattr(process,'isoDepElectronWithNeutral'+postfix).ExtractorPSet.inputCandView = 'pfSelectedNeutralHadrons'
    getattr(process,'isoDepElectronWithPhotons'+postfix).ExtractorPSet.inputCandView = 'pfSelectedPhotons'

    ## adapt isolation cone for electrons
    getattr(process,'isoValElectronWithNeutral'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'isoValElectronWithCharged'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'isoValElectronWithPhotons'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])

    ## adapt isolation cut
    getattr(process,'pfIsolatedElectrons'+postfix).combinedIsolationCut = options['pfIsoValElec']

    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfIsolatedElectrons'+postfix)
                                                        , options['analyzersBeforeElecIso'] * getattr(process,'pfIsolatedElectrons'+postfix)
                                                        )

    ## filter event if no isolated electron found
    if options['skipIfNoPFElec']:
        process.isoElectronFilter = cms.EDFilter("CandViewCountFilter"
                                                , src = cms.InputTag("pfIsolatedElectrons"+postfix)
                                                , minNumber = cms.uint32(1)
                                                )

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfIsolatedElectrons'+postfix), getattr(process,'pfIsolatedElectrons'+postfix)
                                                            * process.isoElectronFilter
                                                            )

    ## save eleID sources
    eleIDs = cms.PSet()

    ## add CiC electron ID
    if options['electronIDs'].count('CiC') > 0 :
        process.load('RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi')
        process.eidCiCSequence = cms.Sequence( process.eidVeryLooseMC
                                             * process.eidLooseMC
                                             * process.eidMediumMC
                                             * process.eidTightMC
                                             * process.eidSuperTightMC
                                             * process.eidHyperTight1MC
                                             )

        ## embed CiC electron ID into patElectrons
        eleIDsCiC = cms.PSet( eidVeryLooseMC   = cms.InputTag("eidVeryLooseMC")
                            , eidLooseMC       = cms.InputTag("eidLooseMC")
                            , eidMediumMC      = cms.InputTag("eidMediumMC")
                            , eidTightMC       = cms.InputTag("eidTightMC")
                            , eidSuperTightMC  = cms.InputTag("eidSuperTightMC")
                            , eidHyperTight1MC = cms.InputTag("eidHyperTight1MC")
                            )

        for name in eleIDsCiC.parameterNames_():
            setattr(eleIDs,name,getattr(eleIDsCiC,name))


    ## add classical electron ID
    if options['electronIDs'].count('classical') > 0 :
        process.load("ElectroWeakAnalysis.WENu.simpleEleIdSequence_cff")

        eleIDsClassical = cms.PSet( simpleEleId95relIso = cms.InputTag("simpleEleId95relIso")
                                  , simpleEleId90relIso = cms.InputTag("simpleEleId90relIso")
                                  , simpleEleId85relIso = cms.InputTag("simpleEleId85relIso")
                                  , simpleEleId80relIso = cms.InputTag("simpleEleId80relIso")
                                  , simpleEleId70relIso = cms.InputTag("simpleEleId70relIso")
                                  , simpleEleId60relIso = cms.InputTag("simpleEleId60relIso")
                                  , simpleEleId95cIso   = cms.InputTag("simpleEleId95cIso")
                                  , simpleEleId90cIso   = cms.InputTag("simpleEleId90cIso")
                                  , simpleEleId85cIso   = cms.InputTag("simpleEleId85cIso")
                                  , simpleEleId80cIso   = cms.InputTag("simpleEleId80cIso")
                                  , simpleEleId70cIso   = cms.InputTag("simpleEleId70cIso")
                                  , simpleEleId60cIso   = cms.InputTag("simpleEleId60cIso")
                                  )

        for name in eleIDsClassical.parameterNames_():
            setattr(eleIDs,name,getattr(eleIDsClassical,name))

    ## if no electronID is needed, don't try to add it to pat Electron
    if options['electronIDs'].count('classical') == 0 and options['electronIDs'].count('CiC') == 0 :
        getattr(process,'patElectrons'+postfix).addElectronID = False

    ## add all eleIDs to the pat electron
    getattr(process,'patElectrons'+postfix).electronIDSources = eleIDs

    ## use beamspot for dB calculation
    getattr(process,'patElectrons'+postfix).usePV = False

    ## embedding of resolutions into the patObjects
    if options['addResolutions']:
        getattr(process,'patElectrons'+postfix).addResolutions = True
        getattr(process,'patElectrons'+postfix).resolutions = cms.PSet( default = cms.string("elecResolution") )

    ## switch off embeding of collections
    if options['switchOffEmbedding']:
        #getattr(process,'patElectrons'+postfix).embedHighLevelSelection = False
        getattr(process,'patElectrons'+postfix).embedGsfElectronCore    = False
        getattr(process,'patElectrons'+postfix).embedSuperCluster       = False
        getattr(process,'patElectrons'+postfix).embedPFCandidate        = False
        getattr(process,'patElectrons'+postfix).embedGsfTrack           = False
        getattr(process,'patElectrons'+postfix).embedTrack              = False
        getattr(process,'patElectrons'+postfix).embedGenMatch           = False

    ## no cut elecs
    if options['addNoCutPFElec']:
        setattr(process,'noCutPfAllElectrons'+postfix, getattr(process,'pfAllElectrons'+postfix).clone(src = 'pfNoPileUp'+postfix))

        setattr(process,'noCutIsoDepElectronWithCharged'+postfix, getattr(process,'isoDepElectronWithCharged'+postfix).clone(src = 'noCutPfAllElectrons'+postfix))
        setattr(process,'noCutIsoDepElectronWithNeutral'+postfix, getattr(process,'isoDepElectronWithNeutral'+postfix).clone(src = 'noCutPfAllElectrons'+postfix))
        setattr(process,'noCutIsoDepElectronWithPhotons'+postfix, getattr(process,'isoDepElectronWithPhotons'+postfix).clone(src = 'noCutPfAllElectrons'+postfix))

        noCutIsoDepElectron = cms.PSet( pfNeutralHadrons = cms.InputTag("noCutIsoDepElectronWithNeutral"+postfix)
                                      , pfChargedHadrons = cms.InputTag("noCutIsoDepElectronWithCharged"+postfix)
                                      , pfPhotons        = cms.InputTag("noCutIsoDepElectronWithPhotons"+postfix)
                                      )

        setattr(process,'noCutIsoValElectronWithCharged'+postfix, getattr(process,'isoValElectronWithCharged'+postfix).clone())
        setattr(process,'noCutIsoValElectronWithNeutral'+postfix, getattr(process,'isoValElectronWithNeutral'+postfix).clone())
        setattr(process,'noCutIsoValElectronWithPhotons'+postfix, getattr(process,'isoValElectronWithPhotons'+postfix).clone())

        getattr(process,'noCutIsoValElectronWithCharged'+postfix).deposits[0].src = 'noCutIsoDepElectronWithCharged'+postfix
        getattr(process,'noCutIsoValElectronWithNeutral'+postfix).deposits[0].src = 'noCutIsoDepElectronWithNeutral'+postfix
        getattr(process,'noCutIsoValElectronWithPhotons'+postfix).deposits[0].src = 'noCutIsoDepElectronWithPhotons'+postfix

        noCutIsoValElectron = cms.PSet( pfNeutralHadrons = cms.InputTag("noCutIsoValElectronWithNeutral"+postfix)
                                      , pfChargedHadrons = cms.InputTag("noCutIsoValElectronWithCharged"+postfix)
                                      , pfPhotons        = cms.InputTag("noCutIsoValElectronWithPhotons"+postfix)
                                      )

        noCutIsolationValueMapsElectron = cms.VInputTag( cms.InputTag("noCutIsoValElectronWithCharged")
                                                       , cms.InputTag("noCutIsoValElectronWithNeutral")
                                                       , cms.InputTag("noCutIsoValElectronWithPhotons")
                                                       )

        setattr(process,'noCutPfIsolatedElectrons'+postfix, getattr(process,'pfIsolatedElectrons'+postfix).clone( src = 'noCutPfAllElectrons'+postfix
                                                                                                                , isolationValueMaps   = noCutIsolationValueMapsElectron
                                                                                                                , combinedIsolationCut = 1e30
                                                                                                                ))

        ## right now no new matcher is needed as the gsfElectrons need to be matched
        #setattr(process,'noCutElectronMatch'+postfix, getattr(process,'electronMatch'+postfix).clone())
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'electronMatch'+postfix))

        setattr(process,'noCutPatElectrons'+postfix, getattr(process,'patElectrons'+postfix).clone( pfElectronSource = 'noCutPfIsolatedElectrons'+postfix
                                                                                                  #, genParticleMatch = 'noCutElectronMatch'+postfix
                                                                                                  , isoDeposits      = noCutIsoDepElectron
                                                                                                  , isolationValues  = noCutIsoValElectron
                                                                                                  ))

        noCutPatElectronsSequence = cms.Sequence( getattr(process,'noCutPfAllElectrons'+postfix)
                                                * getattr(process,'noCutIsoDepElectronWithCharged'+postfix)
                                                * getattr(process,'noCutIsoDepElectronWithNeutral'+postfix)
                                                * getattr(process,'noCutIsoDepElectronWithPhotons'+postfix)
                                                * getattr(process,'noCutIsoValElectronWithCharged'+postfix)
                                                * getattr(process,'noCutIsoValElectronWithNeutral'+postfix)
                                                * getattr(process,'noCutIsoValElectronWithPhotons'+postfix)
                                                * getattr(process,'noCutPfIsolatedElectrons'+postfix)
                                                #* getattr(process,'noCutElectronMatch'+postfix)
                                                * getattr(process,'electronMatch'+postfix)
                                                * getattr(process,'noCutPatElectrons'+postfix)
                                                )

        if not options['runOnMC']:
            noCutPatElectronsSequence.remove(getattr(process,'electronMatch'+postfix))

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfAllElectrons'+postfix)
                                                            , noCutPatElectronsSequence * getattr(process,'pfAllElectrons'+postfix)
                                                            )


    ##
    ## customize taus
    ##

    ## remove the full pftau sequence as it is not needed for us
    if not os.getenv('CMSSW_VERSION').startswith('CMSSW_4_1_'):
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauPFJets08Region'+postfix))
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauPileUpVertices'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauTagInfoProducer'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsLegacyTaNCPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsLegacyHPSPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBase'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBaseDiscriminationByLeadingTrackFinding'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBaseDiscriminationByIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBaseDiscriminationByLeadingPionPtCut'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTaus'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoTau'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingTrackFinding'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingTrackPtCut'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingPionPtCut'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTrackIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByECALIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationAgainstElectron'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationAgainstMuon'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNC'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrOnePercent'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrHalfPercent'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrTenthPercent'+postfix))

    ##
    ## customize photons
    ##

    ## remove the photons match
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'photonMatch'+postfix))

    ##
    ## customize jets
    ##

    ## activate jet area calculation needed for L1FastJet corrections
    getattr(process,'pfJets'+postfix).doAreaFastjet = True

    ## source of PFJets
    getattr(process,'pfJets'+postfix).src = 'pfNoElectron'

    ## switchmodules to correct sources
    massSearchReplaceAnyInputTag(getattr(process,'patPF2PATSequence'+postfix),'pfNoTau'+postfix,'pfJets'+postfix)

    ## remove soft lepton taggers, which would have needed more RECO collections as input
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'softMuonTagInfosAOD'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'softMuonBJetTagsAOD'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'softMuonByPtBJetTagsAOD'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'softMuonByIP3dBJetTagsAOD'+postfix))

    getattr(process,'patJets'+postfix).tagInfoSources = []
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonBJetTagsAOD"+postfix))
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonByPtBJetTagsAOD"+postfix))
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonByIP3dBJetTagsAOD"+postfix))

    ## embedding of resolutions into the patObjects
    if options['addResolutions']:
        getattr(process,'patJets'+postfix).addResolutions = True
        getattr(process,'patJets'+postfix).resolutions = cms.PSet( default = cms.string("udscResolutionPF")
                                                                 , bjets = cms.string("bjetResolutionPF")
                                                                 )

    ## remove obsolte genJet clustering sequences
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'genParticlesForJetsNoNu'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'iterativeCone5GenJetsNoNu'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'ak5GenJetsNoNu'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'ak7GenJetsNoNu'+postfix))

    ## switch genJetMatch of patJet to correct input collection, already produced at production
    if options['runOnAOD']:
        getattr(process,'patJetGenJetMatch'+postfix).matched = cms.InputTag('ak5GenJets','',process.name_())
    else:
        getattr(process,'patJetGenJetMatch'+postfix).matched = cms.InputTag('ak5GenJets','','PAT')

    ## switch off embeding of collections
    if options['switchOffEmbedding']:
        getattr(process,'patJets'+postfix).embedPFCandidates   = False
        getattr(process,'patJets'+postfix).embedGenJetMatch    = False
        getattr(process,'patJets'+postfix).embedGenPartonMatch = False

    ## apply selection for pat jets
    getattr(process,'selectedPatJets'+postfix).cut = options['cutsJets']

    ## this needs to be re-run to compute rho for the L1Fastjet corrections
    from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
    process.kt6PFJets = kt4PFJets.clone(src=getattr(process,'pfJets'+postfix).src, doAreaFastjet=True, doRhoFastjet=True, rParam=0.6)

    ## add kt6PFJets for rho calculation needed for L1FastJet correction
    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfMET'+postfix)
                                                        , process.kt6PFJets * getattr(process,'pfMET'+postfix)
                                                        )


    ##
    ## customize MET
    ##

    ## re-configure and create MET
    getattr(process,'pfMET'+postfix).src = 'pfNoPileUp'+postfix

    if options['METCorrectionLevel'] == 0 and not str(options['METCorrectionLevel']) == 'False':
        print "Raw MET will be used, no corrections will be applied!"
    elif options['METCorrectionLevel'] == 1 and not str(options['METCorrectionLevel']) == 'True' or options['METCorrectionLevel'] == 2 :
        ## create jet correctors for MET corrections
        from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import ak5PFL1Fastjet, ak5PFL2Relative, ak5PFL3Absolute, ak5PFResidual
        ## L1FastJet
        process.ak5PFL1FastjetChs = ak5PFL1Fastjet.clone( algorithm = 'AK5PFchs'
                                                        , srcRho    = cms.InputTag('kt6PFJets'+postfix,'rho')
                                                        )

        ## L2Relative
        process.ak5PFL2RelativeChs = ak5PFL2Relative.clone( algorithm = 'AK5PFchs' )

        ## L3Absolute
        process.ak5PFL3AbsoluteChs = ak5PFL3Absolute.clone( algorithm = 'AK5PFchs' )

        ## Residual
        process.ak5PFResidualChs = ak5PFResidual.clone( algorithm = 'AK5PFchs' )

        ## combinded corrections
        process.combinedCorrector = cms.ESSource( 'JetCorrectionServiceChain'
                                                , correctors = cms.vstring('ak5PFL1FastjetChs','ak5PFL2RelativeChs','ak5PFL3AbsoluteChs','ak5PFResidualChs')
                                                )

        ## remove residual corrections from MET corrections for MC
        if options['runOnMC']:
            process.combinedCorrector.correctors.remove('ak5PFResidualChs')

        ## configuration of MET corrections
        from JetMETCorrections.Type1MET.MetType1Corrections_cff import metJESCorAK5PFJet
        setattr(process,'corMet'+postfix,metJESCorAK5PFJet.clone())
        getattr(process,'corMet'+postfix).metType = 'PFMET'
        getattr(process,'corMet'+postfix).inputUncorMetLabel = 'pfMET'
        ## configuration of TypeI corrections
        getattr(process,'corMet'+postfix).inputUncorJetsLabel = 'pfJets'+postfix
        getattr(process,'corMet'+postfix).jetPTthreshold = cms.double(10.0)
        getattr(process,'corMet'+postfix).corrector = cms.string('combinedCorrector')
        ## decide which MET correction level to use
        if options['METCorrectionLevel'] == 1 :
            print "TypeI corrected MET will be used, corrections for JEC will be applied!"
            getattr(process,'corMet'+postfix).useTypeII = False
        elif options['METCorrectionLevel'] == 2 :
            print "TypeII corrected MET will be used, corrections for JEC and unclustered energy will be applied!"
            getattr(process,'corMet'+postfix).useTypeII = True
        ## configuration of TypeII corrections
        getattr(process,'corMet'+postfix).UscaleA = cms.double(1.5)
        getattr(process,'corMet'+postfix).UscaleB = cms.double(0)
        getattr(process,'corMet'+postfix).UscaleC = cms.double(0)
        getattr(process,'corMet'+postfix).inputUncorUnlusteredLabel = cms.untracked.InputTag('pfNoJet'+postfix)

        ## add MET corrections to sequence
        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfMET'+postfix), getattr(process,'pfMET'+postfix)
                                                            * getattr(process,'corMet'+postfix)
                                                            )

        ## change uncorrected MET to corrected MET in PAT
        getattr(process,'patMETs'+postfix).metSource = 'corMet'+postfix

    else:
        raise ValueError, "Wrong config: 'METCorrectionLevel' may only be set to 0,1,2 (no correction, TypeI, TypeI+TypeII corrections) but was set to *"+str(options['METCorrectionLevel'])+"*!"

    ## embedding of resolutions into the patObjects
    if options['addResolutions']:
        getattr(process,'patMETs'+postfix).addResolutions = True
        getattr(process,'patMETs'+postfix).resolutions = cms.PSet( default = cms.string("metResolutionPF") )


    ##
    ## let it run
    ##

    ## prepare everything to run on AOD samples instead of pre-computed skims
    if options['runOnAOD']:
        if options['runOnMC']:
            ## needed for redoing the ak5GenJets
            process.load("TopAnalysis.TopUtils.GenJetParticles_cff")
            process.load("RecoJets.Configuration.RecoGenJets_cff")

            if options['excludeElectronsFromWsFromGenJets']:
                process.genParticlesForJets.excludeFromResonancePids = [11, 12, 13, 14, 16]

        ## do proper event filters when running on non-pre-computed data skims
        else:
            ## HBHE noise filter
            process.load("CommonTools.RecoAlgos.HBHENoiseFilter_cfi")
            ## s. https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1196.html
            process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
            process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
            process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999.
            labelOfHBHENoiseFilter = cms.InputTag("")
            if type(process.HBHENoiseFilter.label) == cms.InputTag:
                labelOfHBHENoiseFilter = process.HBHENoiseFilter.label
                delattr(process.HBHENoiseFilter, 'label')
            #if str(type(process.HBHENoiseFilter.label)) == "<type 'instancemethod'>":

            ## event scraping filter
            process.scrapingFilter = cms.EDFilter( "FilterOutScraping"
                                                 , applyfilter = cms.untracked.bool( True )
                                                 , debugOn     = cms.untracked.bool( False )
                                                 , numtrack    = cms.untracked.uint32( 10 )
                                                 , thresh      = cms.untracked.double( 0.25 )
                                                 )

    ## disable muon and / or electron top projections
    if options['noElecTopProjection']:
        massSearchReplaceAnyInputTag(getattr(process,'patPF2PATSequence'+postfix),'pfNoElectron'+postfix,'pfNoMuon'+postfix)
    if options['noMuonTopProjection']:
        massSearchReplaceAnyInputTag(getattr(process,'patPF2PATSequence'+postfix),'pfNoMuon'+postfix,'pfNoPileUp'+postfix)

    ## define sequence with all modules in
    process.pf2pat = cms.Sequence( process.goodOfflinePrimaryVertices
                                 * process.goodOfflinePrimaryVerticesWithBS
                                 )

    ## prepare running of sequence if input is AOD
    if options['runOnAOD']:
        if options['runOnMC']:
            process.pf2pat += process.genJetParticles
            process.pf2pat += process.ak5GenJets
        else:
            process.pf2pat += process.HBHENoiseFilter
            process.pf2pat += process.scrapingFilter

    ## add electronIDs that should be added to the par electron
    if options['electronIDs'].count('CiC') > 0 :
        process.pf2pat += process.eidCiCSequence
    if options['electronIDs'].count('classical') > 0 :
        process.pf2pat += process.simpleEleIdSequence

    ## run PF2PAT sequence
    process.pf2pat += getattr(process,'patPF2PATSequence'+postfix)

    ## use selected collection of good primary vertices in pf2pat sequence
    massSearchReplaceAnyInputTag(process.pf2pat, 'offlinePrimaryVertices'      , 'goodOfflinePrimaryVertices')
    massSearchReplaceAnyInputTag(process.pf2pat, 'offlinePrimaryVerticesWithBS', 'goodOfflinePrimaryVerticesWithBS')
    process.goodOfflinePrimaryVertices.src       = 'offlinePrimaryVertices'
    process.goodOfflinePrimaryVerticesWithBS.src = 'offlinePrimaryVerticesWithBS'


    ##
    ## output all options and set defaults
    ##

    print '==================================================='
    print '|||||||||||||||||||||||||||||||||||||||||||||||||||'
    print '==================================================='
    print 'options used by prependPF2PATSequence:'
    print 'runOnMC:', options['runOnMC']
    print 'runOnAOD:', options['runOnAOD']
    print 'switchOffEmbedding:', options['switchOffEmbedding']
    print 'addResolutions:', options['addResolutions']
    #print 'postfix:', options.setdefault('postfix', '')
    print 'runOnOLDcfg:', options['runOnOLDcfg']
    print 'cutsMuon:', options['cutsMuon']
    print 'cutsElec:', options['cutsElec']
    print 'cutsJets:', options['cutsJets']
    print 'electronIDs:', options['electronIDs']
    print 'pfIsoConeMuon:', options['pfIsoConeMuon']
    print 'pfIsoConeElec:', options['pfIsoConeElec']
    print 'pfIsoValMuon:', options['pfIsoValMuon']
    print 'pfIsoValElec:', options['pfIsoValElec']
    print 'skipIfNoPFMuon:', options['skipIfNoPFMuon']
    print 'skipIfNoPFElec:', options['skipIfNoPFElec']
    print 'addNoCutPFMuon:', options['addNoCutPFMuon']
    print 'addNoCutPFElec:', options['addNoCutPFElec']
    print 'noMuonTopProjection:', options['noMuonTopProjection']
    print 'noElecTopProjection:', options['noElecTopProjection']
    print 'analyzersBeforeMuonIso:', options['analyzersBeforeMuonIso']
    print 'analyzersBeforeElecIso:', options['analyzersBeforeElecIso']
    print 'excludeElectronsFromWsFromGenJets:', options['excludeElectronsFromWsFromGenJets']
    print 'METCorrectionLevel:', options['METCorrectionLevel']
    print '==================================================='
    print '|||||||||||||||||||||||||||||||||||||||||||||||||||'
    print '==================================================='

    ## append pf2pat sequence to all paths in pathnames
    if pathnames == ['']:
        pathnames = process.paths_().keys()

    print 'prepending PF2PAT sequence to paths:', pathnames
    for pathname in pathnames:
        ## use only good vertices
        massSearchReplaceAnyInputTag(getattr(process,pathname), 'offlinePrimaryVertices'      , 'goodOfflinePrimaryVertices')
        massSearchReplaceAnyInputTag(getattr(process,pathname), 'offlinePrimaryVerticesWithBS', 'goodOfflinePrimaryVerticesWithBS')

        ## replace everything from the old selection with the newly generated collections
        if options['runOnOLDcfg']:
            massSearchReplaceAnyInputTag(getattr(process,pathname), 'selectedPatJetsAK5PF', 'selectedPatJets')
            massSearchReplaceAnyInputTag(getattr(process,pathname), 'patMETsPF', 'patMETs')
            massSearchReplaceAnyInputTag(getattr(process,pathname), cms.InputTag('scaledJetEnergy', 'selectedPatJetsAK5PF', process.name_()), cms.InputTag('scaledJetEnergy', 'selectedPatJets', process.name_()))

        ## finally insert the sequence into all (given) paths
        getattr(process, pathname).insert(0,process.pf2pat)

    ## BUGFIX FOR STUPID ERROR IN HBHENoiseFilter
    if options['runOnAOD']:
        if not options['runOnMC']:
            if not labelOfHBHENoiseFilter == cms.InputTag(""):
                setattr(process.HBHENoiseFilter, 'label', labelOfHBHENoiseFilter)

    if 'postfix' in options:
        print 'POSTFIXES ARE NOT SUPPORTED AT THE MOMENT, THIS OPTION IS IGNORED'
