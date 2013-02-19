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
    options.setdefault('resolutionsVersion', 'tqaf') ## can be set to 'tqaf', 'summer11', and 'fall11'
    #options.setdefault('postfix', '')
    options.setdefault('runOnOLDcfg', False)
    options.setdefault('cutsMuon', 'pt > 10. & abs(eta) < 2.5')
    options.setdefault('cutsElec', 'et > 20. & abs(eta) < 2.5')
    options.setdefault('cutsJets', 'pt > 10. & abs(eta) < 5.0')
    options.setdefault('electronIDs', 'CiC') ## can be set to CiC, classical, MVA
    options.setdefault('pfIsoConeMuon', 0.4)
    options.setdefault('pfIsoConeElec', 0.4)
    options.setdefault('pfIsoValMuon', 0.2)
    options.setdefault('pfIsoValElec', 0.2)
    options.setdefault('doDeltaBetaCorrMuon', False)
    options.setdefault('doDeltaBetaCorrElec', False)
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
    options.setdefault('JECEra' , '')
    options.setdefault('JECFile', '')
    options.setdefault('additionalJECLevels', [])
    options.setdefault('addGluonTags', False)

    if 'applyMETCorrections' in options:
        raise KeyError, "The option 'applyMETCorrections' is not supported anymore by prependPF2PATSequence, please use 'METCorrectionLevel'! 'METCorrectionLevel' may be set to 0,1,2 (no correction, TypeI, TypeI+TypeII corrections)"

    ## tool to replace all input tags in a given sequence
    from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag

    ## postfixes are NOT supported right now
    postfix='' #options['postfix']

    ## check if needed standard sequences are available, else load them
    if not hasattr(process, 'XMLIdealGeometryESSource') and not hasattr(process, 'XMLFromDBSource'):
        process.load('Configuration.StandardSequences.GeometryDB_cff')
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

    ## read JEC from local SQLite file if given
    if not options['JECEra'] == '' or not options['JECFile'] == '':
        if not options['JECEra'] == '' and not options['JECFile'] == '':
            #process.load("CondCore.DBCommon.CondDBCommon_cfi")
            #from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup
            process.jec = cms.ESSource( "PoolDBESSource"
                                      , DBParameters = cms.PSet(messageLevel = cms.untracked.int32(0))
                                      , timetype = cms.string('runnumber')
                                      , toGet = cms.VPSet(cms.PSet( record = cms.string('JetCorrectionsRecord')
                                                                  , tag    = cms.string('JetCorrectorParametersCollection_'+options['JECEra']+'_AK5PFchs')
                                                                  , label  = cms.untracked.string('AK5PFchs')
                                                                  )
                                                          ## here you add as many jet types as you need
                                                          ## note that the tag name is specific for the particular sqlite file 
                                                         )
                                      , connect = cms.string('sqlite:'+options['JECFile'])
                                      )
            ## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
            process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')
        else:
            raise ValueError, "The options 'JECEra' and 'JECFile' may only be set both (using the give SQLite file and JEC era) or none (taking the JEC from the global tag) and not only one of them!"
            
    ## choose correct set of jec levels for MC and data
    if options['runOnMC']:
        jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
    else:
        jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']

    ## add additional JEC levels after recommended minimum of JECs
    jecLevels += options['additionalJECLevels']

    ## run the full PF2PAT sequence
    from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
    usePF2PAT( process
             , runPF2PAT      = True
             , runOnMC        = options['runOnMC']
             , jetAlgo        = 'AK5'
             , outputModules  = []
             , postfix        = postfix
             , jetCorrections = ('AK5PFchs',cms.vstring(jecLevels))
             )

    ##
    ## customize PAT
    ##

    #????????????????????????????????????
    #??? ??? ??? STILL NEEDED ??? ??? ???
    #????????????????????????????????????
    process.pfPileUp.checkClosestZVertex = False
    process.pfPileUpIso.checkClosestZVertex = False

    ## remove taus and photons from pat sequence
    from PhysicsTools.PatAlgos.tools.coreTools import removeSpecificPATObjects, removeCleaning
    removeSpecificPATObjects( process
                            , names = ['Taus', 'Photons']
                            , outputModules = []
                            , postfix = postfix
                            )
    removeCleaning( process
                  , outputModules = []
                  , postfix = postfix
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
        if options['resolutionsVersion'] == 'tqaf'     :
          process.load("TopQuarkAnalysis.TopObjectResolutions.stringResolutions_etEtaPhi_cff")
        elif options['resolutionsVersion'] == 'summer11' :
          process.load("TopAnalysis.Configuration.stringResolutions_etEtaPhi_Summer11_cff")
        elif options['resolutionsVersion'] == 'fall11' :
          process.load("TopAnalysis.Configuration.stringResolutions_etEtaPhi_Fall11_cff")
        else :
          raise ValueError, "Wrong config: 'resolutionsVersion' may only be set to 'tqaf' or 'summer11', not to *"+str(options['resolutionsVersion'])+"*!"


    ##
    ## customize muons
    ##

    ## default muons modules:
    # pfAllMuons
    # pfMuonsFromVertex
    # pfSelectedMuons
    # muPFIsoDepositCharged
    # muPFIsoDepositChargedAll
    # muPFIsoDepositGamma
    # muPFIsoDepositNeutral
    # muPFIsoDepositPU
    # muPFIsoValueCharged03
    # muPFIsoValueChargedAll03
    # muPFIsoValueGamma03
    # muPFIsoValueNeutral03
    # muPFIsoValueGammaHighThreshold03
    # muPFIsoValueNeutralHighThreshold03
    # muPFIsoValuePU03
    # muPFIsoValueCharged04
    # muPFIsoValueChargedAll04
    # muPFIsoValueGamma04
    # muPFIsoValueNeutral04
    # muPFIsoValueGammaHighThreshold04
    # muPFIsoValueNeutralHighThreshold04
    # muPFIsoValuePU04
    # pfIsolatedMuons
    # pfMuons
    # pfNoMuon
    # muonMatch
    # patMuons
    # selectedPatMuons

    ## disable muon top projections
    if options['noMuonTopProjection']:
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoMuon'+postfix))

    ## apply selection for muons
    getattr(process,'pfSelectedMuons'+postfix).cut = options['cutsMuon']

    ## calculate isolation only for (kinematically) selected pfMuons
    getattr(process,'muPFIsoDepositCharged'   +postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'muPFIsoDepositChargedAll'+postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'muPFIsoDepositGamma'     +postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'muPFIsoDepositNeutral'   +postfix).src = 'pfSelectedMuons'+postfix
    getattr(process,'muPFIsoDepositPU'        +postfix).src = 'pfSelectedMuons'+postfix

    ## we want out own isolation cones:
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueCharged04'             +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueChargedAll04'          +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueGamma04'               +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueNeutral04'             +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueGammaHighThreshold04'  +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueNeutralHighThreshold04'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValuePU04'                  +postfix))

    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueGammaHighThreshold03'  +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'muPFIsoValueNeutralHighThreshold03'+postfix))

    ## adapt isolation cone for muons
    setattr(process,'muPFIsoValueCharged'             +postfix, getattr(process, 'muPFIsoValueCharged03'             +postfix).clone())
    setattr(process,'muPFIsoValueChargedAll'          +postfix, getattr(process, 'muPFIsoValueChargedAll03'          +postfix).clone())
    setattr(process,'muPFIsoValueGamma'               +postfix, getattr(process, 'muPFIsoValueGamma03'               +postfix).clone())
    setattr(process,'muPFIsoValueNeutral'             +postfix, getattr(process, 'muPFIsoValueNeutral03'             +postfix).clone())
    #setattr(process,'muPFIsoValueGammaHighThreshold'  +postfix, getattr(process, 'muPFIsoValueGammaHighThreshold03'  +postfix).clone())
    #setattr(process,'muPFIsoValueNeutralHighThreshold'+postfix, getattr(process, 'muPFIsoValueNeutralHighThreshold03'+postfix).clone())
    setattr(process,'muPFIsoValuePU'                  +postfix, getattr(process, 'muPFIsoValuePU03'                  +postfix).clone())

    getattr(process,'muPFIsoValueCharged'             +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'muPFIsoValueChargedAll'          +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'muPFIsoValueGamma'               +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'muPFIsoValueNeutral'             +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    #getattr(process,'muPFIsoValueGammaHighThreshold'  +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    #getattr(process,'muPFIsoValueNeutralHighThreshold'+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,'muPFIsoValuePU'                  +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])

    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueCharged03'             +postfix),getattr(process,'muPFIsoValueCharged'             +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueChargedAll03'          +postfix),getattr(process,'muPFIsoValueChargedAll'          +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueGamma03'               +postfix),getattr(process,'muPFIsoValueGamma'               +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueNeutral03'             +postfix),getattr(process,'muPFIsoValueNeutral'             +postfix))
    #getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueGammaHighThreshold03'  +postfix),getattr(process,'muPFIsoValueGammaHighThreshold'  +postfix))
    #getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValueNeutralHighThreshold03'+postfix),getattr(process,'muPFIsoValueNeutralHighThreshold'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'muPFIsoValuePU03'                  +postfix),getattr(process,'muPFIsoValuePU'                  +postfix))

    ## adapt isolation collection / cut
    getattr(process,'pfMuons'+postfix).isolationValueMapsCharged  = ['muPFIsoValueCharged'+postfix]
    getattr(process,'pfMuons'+postfix).deltaBetaIsolationValueMap =  'muPFIsoValuePU'+postfix
    getattr(process,'pfMuons'+postfix).isolationValueMapsNeutral  = ['muPFIsoValueNeutral'+postfix,'muPFIsoValueGamma'+postfix]
    getattr(process,'pfIsolatedMuons'+postfix).isolationValueMapsCharged  = ['muPFIsoValueCharged'+postfix]
    getattr(process,'pfIsolatedMuons'+postfix).deltaBetaIsolationValueMap =  'muPFIsoValuePU'+postfix
    getattr(process,'pfIsolatedMuons'+postfix).isolationValueMapsNeutral  = ['muPFIsoValueNeutral'+postfix,'muPFIsoValueGamma'+postfix]
    getattr(process,'pfIsolatedMuons'+postfix).isolationCut = options['pfIsoValMuon']
    getattr(process,'pfIsolatedMuons'+postfix).doDeltaBetaCorrection = options['doDeltaBetaCorrMuon']

    ## adapt isolation src for pat muons
    getattr(process,'patMuons'+postfix).isolationValues.pfNeutralHadrons   = 'muPFIsoValueNeutral'   +postfix
    getattr(process,'patMuons'+postfix).isolationValues.pfChargedAll       = 'muPFIsoValueChargedAll'+postfix
    getattr(process,'patMuons'+postfix).isolationValues.pfPUChargedHadrons = 'muPFIsoValuePU'        +postfix
    getattr(process,'patMuons'+postfix).isolationValues.pfPhotons          = 'muPFIsoValueGamma'     +postfix
    getattr(process,'patMuons'+postfix).isolationValues.pfChargedHadrons   = 'muPFIsoValueCharged'   +postfix

    ## remove *pfMuons* as they are not needed
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfMuons'+postfix))

    if not len(options['analyzersBeforeMuonIso'].moduleNames()) == 0:
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
        ## HighLevelSelection is no real embedding, keep it TRUE at all times !!!
        getattr(process,'patMuons'+postfix).embedHighLevelSelection = True
        getattr(process,'patMuons'+postfix).embedTpfmsMuon          = False
        getattr(process,'patMuons'+postfix).embedCaloMETMuonCorrs   = False
        getattr(process,'patMuons'+postfix).embedPFCandidate        = False
        getattr(process,'patMuons'+postfix).embedStandAloneMuon     = False
        getattr(process,'patMuons'+postfix).embedTrack              = False
        getattr(process,'patMuons'+postfix).embedTcMETMuonCorrs     = False
        getattr(process,'patMuons'+postfix).embedCombinedMuon       = False
        getattr(process,'patMuons'+postfix).embedGenMatch           = False
        getattr(process,'patMuons'+postfix).embedPickyMuon          = False

    ## no cut muons
    ## these muons are still originating from the PV -> no PU muons
    ## no further requirments are made (except for being a PF muon)
    if options['addNoCutPFMuon']:
        ## calculate isolation for all pfMuons from PV
        setattr(process,'noCutMuPFIsoDepositCharged'   +postfix, getattr(process,'muPFIsoDepositCharged'   +postfix).clone(src = 'pfMuonsFromVertex'+postfix))
        setattr(process,'noCutMuPFIsoDepositChargedAll'+postfix, getattr(process,'muPFIsoDepositChargedAll'+postfix).clone(src = 'pfMuonsFromVertex'+postfix))
        setattr(process,'noCutMuPFIsoDepositGamma'     +postfix, getattr(process,'muPFIsoDepositGamma'     +postfix).clone(src = 'pfMuonsFromVertex'+postfix))
        setattr(process,'noCutMuPFIsoDepositNeutral'   +postfix, getattr(process,'muPFIsoDepositNeutral'   +postfix).clone(src = 'pfMuonsFromVertex'+postfix))
        setattr(process,'noCutMuPFIsoDepositPU'        +postfix, getattr(process,'muPFIsoDepositPU'        +postfix).clone(src = 'pfMuonsFromVertex'+postfix))

        noCutIsoDepMuon = cms.PSet( pfNeutralHadrons   = cms.InputTag("noCutMuPFIsoDepositNeutral")
                                  , pfChargedAll       = cms.InputTag("noCutMuPFIsoDepositChargedAll")
                                  , pfPUChargedHadrons = cms.InputTag("noCutMuPFIsoDepositPU")
                                  , pfPhotons          = cms.InputTag("noCutMuPFIsoDepositGamma")
                                  , pfChargedHadrons   = cms.InputTag("noCutMuPFIsoDepositCharged")
                                  )
       
        setattr(process,'noCutMuPFIsoValueCharged'             +postfix, getattr(process, 'muPFIsoValueCharged'             +postfix).clone())
        setattr(process,'noCutMuPFIsoValueChargedAll'          +postfix, getattr(process, 'muPFIsoValueChargedAll'          +postfix).clone())
        setattr(process,'noCutMuPFIsoValueGamma'               +postfix, getattr(process, 'muPFIsoValueGamma'               +postfix).clone())
        setattr(process,'noCutMuPFIsoValueNeutral'             +postfix, getattr(process, 'muPFIsoValueNeutral'             +postfix).clone())
        #setattr(process,'noCutMuPFIsoValueGammaHighThreshold'  +postfix, getattr(process, 'muPFIsoValueGammaHighThreshold'  +postfix).clone())
        #setattr(process,'noCutMuPFIsoValueNeutralHighThreshold'+postfix, getattr(process, 'muPFIsoValueNeutralHighThreshold'+postfix).clone())
        setattr(process,'noCutMuPFIsoValuePU'                  +postfix, getattr(process, 'muPFIsoValuePU'                  +postfix).clone())
        
        getattr(process,'noCutMuPFIsoValueCharged'             +postfix).deposits[0].src = 'noCutMuPFIsoDepositCharged'   +postfix
        getattr(process,'noCutMuPFIsoValueChargedAll'          +postfix).deposits[0].src = 'noCutMuPFIsoDepositChargedAll'+postfix
        getattr(process,'noCutMuPFIsoValueGamma'               +postfix).deposits[0].src = 'noCutMuPFIsoDepositGamma'     +postfix
        getattr(process,'noCutMuPFIsoValueNeutral'             +postfix).deposits[0].src = 'noCutMuPFIsoDepositNeutral'   +postfix
        #getattr(process,'noCutMuPFIsoValueGammaHighThreshold'  +postfix).deposits[0].src = 'noCutMuPFIsoDepositGamma'     +postfix
        #getattr(process,'noCutMuPFIsoValueNeutralHighThreshold'+postfix).deposits[0].src = 'noCutMuPFIsoDepositNeutral'   +postfix
        getattr(process,'noCutMuPFIsoValuePU'                  +postfix).deposits[0].src = 'noCutMuPFIsoDepositPU'        +postfix

        noCutIsoValMuon = cms.PSet( pfNeutralHadrons   = cms.InputTag("noCutMuPFIsoValueNeutral")
                                  , pfChargedAll       = cms.InputTag("noCutMuPFIsoValueChargedAll")
                                  , pfPUChargedHadrons = cms.InputTag("noCutMuPFIsoValuePU")
                                  , pfPhotons          = cms.InputTag("noCutMuPFIsoValueGamma")
                                  , pfChargedHadrons   = cms.InputTag("noCutMuPFIsoValueCharged")
                                  )
        
        setattr(process,'noCutPfIsolatedMuons'+postfix, getattr(process,'pfIsolatedMuons'+postfix).clone( src = 'pfMuonsFromVertex'+postfix
                                                                                                        , isolationValueMapsCharged  = ['noCutMuPFIsoValueCharged'+postfix]
                                                                                                        , deltaBetaIsolationValueMap =  'noCutMuPFIsoValuePU'+postfix
                                                                                                        , isolationValueMapsNeutral  = ['noCutMuPFIsoValueNeutral'+postfix,'noCutMuPFIsoValueGamma'+postfix]
                                                                                                        , isolationCut = 1e30
                                                                                                        ))
        
        setattr(process,'noCutMuonMatch'+postfix, getattr(process,'muonMatch'+postfix).clone(src = 'pfMuonsFromVertex'+postfix))

        setattr(process,'noCutPatMuons'+postfix, getattr(process,'patMuons'+postfix).clone( pfMuonSource     = 'pfMuonsFromVertex'+postfix
                                                                                          , genParticleMatch = 'noCutMuonMatch'+postfix
                                                                                          , isoDeposits      = noCutIsoDepMuon
                                                                                          , isolationValues  = noCutIsoValMuon
                                                                                          ))

        process.noCutPatMuonsSequence = cms.Sequence( getattr(process,'noCutMuPFIsoDepositCharged'   +postfix)
                                                    * getattr(process,'noCutMuPFIsoDepositChargedAll'+postfix)
                                                    * getattr(process,'noCutMuPFIsoDepositGamma'     +postfix)
                                                    * getattr(process,'noCutMuPFIsoDepositNeutral'   +postfix)
                                                    * getattr(process,'noCutMuPFIsoDepositPU'        +postfix)
                                                    * getattr(process,'noCutMuPFIsoValueCharged'             +postfix)
                                                    * getattr(process,'noCutMuPFIsoValueChargedAll'          +postfix)
                                                    * getattr(process,'noCutMuPFIsoValueGamma'               +postfix)
                                                    * getattr(process,'noCutMuPFIsoValueNeutral'             +postfix)
                                                    #* getattr(process,'noCutMuPFIsoValueGammaHighThreshold'  +postfix)
                                                    #* getattr(process,'noCutMuPFIsoValueNeutralHighThreshold'+postfix)
                                                    * getattr(process,'noCutMuPFIsoValuePU'                  +postfix)
                                                    * getattr(process,'noCutPfIsolatedMuons'+postfix)
                                                    * getattr(process,'noCutMuonMatch'+postfix)
                                                    * getattr(process,'noCutPatMuons'+postfix)
                                                    )

        if not options['runOnMC']:
            process.noCutPatMuonsSequence.remove(getattr(process,'noCutMuonMatch'+postfix))

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfMuonsFromVertex'+postfix), getattr(process,'pfMuonsFromVertex'+postfix)
                                                            * process.noCutPatMuonsSequence
                                                            )


    ##
    ## customize electrons
    ##

    ## default electron modules
    # pfAllElectrons
    # pfElectronsFromVertex
    # pfSelectedElectrons
    # elPFIsoDepositCharged
    # elPFIsoDepositChargedAll
    # elPFIsoDepositGamma
    # elPFIsoDepositNeutral
    # elPFIsoDepositPU
    # elPFIsoValueCharged03
    # elPFIsoValueChargedAll03
    # elPFIsoValueGamma03
    # elPFIsoValueNeutral03
    # elPFIsoValuePU03
    # elPFIsoValueCharged04
    # elPFIsoValueChargedAll04
    # elPFIsoValueGamma04
    # elPFIsoValueNeutral04
    # elPFIsoValuePU04
    # pfIsolatedElectrons
    # pfElectrons
    # pfNoElectron
    # electronMatch
    # patElectrons
    # selectedPatElectrons

    ## disable electron top projections
    if options['noElecTopProjection']:
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoElectron'+postfix))

    ## apply selection for electrons (they are the source of the pat::electrons)
    getattr(process,'pfSelectedElectrons'+postfix).cut = options['cutsElec']

    ## calculate isolation only for (kinematically) selected pfElectrons
    getattr(process,'elPFIsoDepositCharged'   +postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'elPFIsoDepositChargedAll'+postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'elPFIsoDepositGamma'     +postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'elPFIsoDepositNeutral'   +postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process,'elPFIsoDepositPU'        +postfix).src = 'pfSelectedElectrons'+postfix

    ## we want out own isolation cones:
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'elPFIsoValueCharged04PFId'   +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'elPFIsoValueChargedAll04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'elPFIsoValueGamma04PFId'     +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'elPFIsoValueNeutral04PFId'   +postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'elPFIsoValuePU04PFId'        +postfix))

    ## adapt isolation cone for electrons
    setattr(process,'elPFIsoValueCharged'             +postfix, getattr(process, 'elPFIsoValueCharged03PFId'   +postfix).clone())
    setattr(process,'elPFIsoValueChargedAll'          +postfix, getattr(process, 'elPFIsoValueChargedAll03PFId'+postfix).clone())
    setattr(process,'elPFIsoValueGamma'               +postfix, getattr(process, 'elPFIsoValueGamma03PFId'     +postfix).clone())
    setattr(process,'elPFIsoValueNeutral'             +postfix, getattr(process, 'elPFIsoValueNeutral03PFId'   +postfix).clone())
    setattr(process,'elPFIsoValuePU'                  +postfix, getattr(process, 'elPFIsoValuePU03PFId'        +postfix).clone())

    getattr(process,'elPFIsoValueCharged'             +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'elPFIsoValueChargedAll'          +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'elPFIsoValueGamma'               +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'elPFIsoValueNeutral'             +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,'elPFIsoValuePU'                  +postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])

    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'elPFIsoValueCharged03PFId'   +postfix),getattr(process,'elPFIsoValueCharged'             +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'elPFIsoValueChargedAll03PFId'+postfix),getattr(process,'elPFIsoValueChargedAll'          +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'elPFIsoValueGamma03PFId'     +postfix),getattr(process,'elPFIsoValueGamma'               +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'elPFIsoValueNeutral03PFId'   +postfix),getattr(process,'elPFIsoValueNeutral'             +postfix))
    getattr(process,'patPF2PATSequence'+postfix).replace(getattr(process,'elPFIsoValuePU03PFId'        +postfix),getattr(process,'elPFIsoValuePU'                  +postfix))

    ## adapt isolation cut
    getattr(process,'pfElectrons'+postfix).isolationValueMapsCharged  = ['elPFIsoValueCharged'+postfix]
    getattr(process,'pfElectrons'+postfix).deltaBetaIsolationValueMap =  'elPFIsoValuePU'+postfix
    getattr(process,'pfElectrons'+postfix).isolationValueMapsNeutral  = ['elPFIsoValueNeutral'+postfix,'elPFIsoValueGamma'+postfix]
    getattr(process,'pfIsolatedElectrons'+postfix).isolationValueMapsCharged  = ['elPFIsoValueCharged'+postfix]
    getattr(process,'pfIsolatedElectrons'+postfix).deltaBetaIsolationValueMap =  'elPFIsoValuePU'+postfix
    getattr(process,'pfIsolatedElectrons'+postfix).isolationValueMapsNeutral  = ['elPFIsoValueNeutral'+postfix,'elPFIsoValueGamma'+postfix]
    getattr(process,'pfIsolatedElectrons'+postfix).isolationCut = options['pfIsoValElec']
    getattr(process,'pfIsolatedElectrons'+postfix).doDeltaBetaCorrection = options['doDeltaBetaCorrElec']

    ## adapt isolation src for pat electrons
    getattr(process,'patElectrons'+postfix).isolationValues.pfNeutralHadrons   = 'elPFIsoValueNeutral'   +postfix
    getattr(process,'patElectrons'+postfix).isolationValues.pfChargedAll       = 'elPFIsoValueChargedAll'+postfix
    getattr(process,'patElectrons'+postfix).isolationValues.pfPUChargedHadrons = 'elPFIsoValuePU'        +postfix
    getattr(process,'patElectrons'+postfix).isolationValues.pfPhotons          = 'elPFIsoValueGamma'     +postfix
    getattr(process,'patElectrons'+postfix).isolationValues.pfChargedHadrons   = 'elPFIsoValueCharged'   +postfix

    if not len(options['analyzersBeforeElecIso'].moduleNames()) == 0:
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

    ## add MVA electron ID
    if options['electronIDs'].count('MVA') > 0 :
        process.load("EGamma.EGammaAnalysisTools.electronIdMVAProducer_cfi")
        process.eidMVASequence = cms.Sequence( process.mvaTrigV0
                                             * process.mvaNonTrigV0
                                             )

        ## embed MVA electron ID into patElectrons
        eleIDsMVA = cms.PSet( mvaTrigV0    = cms.InputTag("mvaTrigV0")
                            , mvaNonTrigV0 = cms.InputTag("mvaNonTrigV0")
                            )

        for name in eleIDsMVA.parameterNames_():
            setattr(eleIDs,name,getattr(eleIDsMVA,name))

    ## if no electronID is needed, don't try to add it to pat Electron
    if options['electronIDs'].count('classical') == 0 and options['electronIDs'].count('CiC') == 0 and options['electronIDs'].count('MVA') == 0 :
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
        ## HighLevelSelection is no real embedding, keep it TRUE at all times !!!
        getattr(process,'patElectrons'+postfix).embedHighLevelSelection = True
        getattr(process,'patElectrons'+postfix).embedGsfElectronCore    = False
        getattr(process,'patElectrons'+postfix).embedSuperCluster       = False
        getattr(process,'patElectrons'+postfix).embedSeedCluster        = False
        getattr(process,'patElectrons'+postfix).embedPFCandidate        = False
        getattr(process,'patElectrons'+postfix).embedGsfTrack           = False
        getattr(process,'patElectrons'+postfix).embedTrack              = False
        getattr(process,'patElectrons'+postfix).embedGenMatch           = False
        getattr(process,'patElectrons'+postfix).embedRecHits            = False

    ## no cut elecs
    if options['addNoCutPFElec']:
        setattr(process,'noCutPfAllElectrons'       +postfix, getattr(process,'pfAllElectrons'       +postfix).clone(src = 'pfNoPileUp'         +postfix))
        setattr(process,'noCutPfElectronsFromVertex'+postfix, getattr(process,'pfElectronsFromVertex'+postfix).clone(src = 'noCutPfAllElectrons'+postfix))
        
        ## calculate isolation for all pfElectrons from PV
        setattr(process,'noCutElPFIsoDepositCharged'   +postfix, getattr(process,'elPFIsoDepositCharged'   +postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))
        setattr(process,'noCutElPFIsoDepositChargedAll'+postfix, getattr(process,'elPFIsoDepositChargedAll'+postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))
        setattr(process,'noCutElPFIsoDepositGamma'     +postfix, getattr(process,'elPFIsoDepositGamma'     +postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))
        setattr(process,'noCutElPFIsoDepositNeutral'   +postfix, getattr(process,'elPFIsoDepositNeutral'   +postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))
        setattr(process,'noCutElPFIsoDepositPU'        +postfix, getattr(process,'elPFIsoDepositPU'        +postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))

        noCutIsoDepElectron = cms.PSet( pfNeutralHadrons   = cms.InputTag("noCutElPFIsoDepositNeutral")
                                      , pfChargedAll       = cms.InputTag("noCutElPFIsoDepositChargedAll")
                                      , pfPUChargedHadrons = cms.InputTag("noCutElPFIsoDepositPU")
                                      , pfPhotons          = cms.InputTag("noCutElPFIsoDepositGamma")
                                      , pfChargedHadrons   = cms.InputTag("noCutElPFIsoDepositCharged")
                                      )
       
        setattr(process,'noCutElPFIsoValueCharged'             +postfix, getattr(process, 'elPFIsoValueCharged'             +postfix).clone())
        setattr(process,'noCutElPFIsoValueChargedAll'          +postfix, getattr(process, 'elPFIsoValueChargedAll'          +postfix).clone())
        setattr(process,'noCutElPFIsoValueGamma'               +postfix, getattr(process, 'elPFIsoValueGamma'               +postfix).clone())
        setattr(process,'noCutElPFIsoValueNeutral'             +postfix, getattr(process, 'elPFIsoValueNeutral'             +postfix).clone())
        setattr(process,'noCutElPFIsoValuePU'                  +postfix, getattr(process, 'elPFIsoValuePU'                  +postfix).clone())
        
        getattr(process,'noCutElPFIsoValueCharged'             +postfix).deposits[0].src = 'noCutElPFIsoDepositCharged'   +postfix
        getattr(process,'noCutElPFIsoValueChargedAll'          +postfix).deposits[0].src = 'noCutElPFIsoDepositChargedAll'+postfix
        getattr(process,'noCutElPFIsoValueGamma'               +postfix).deposits[0].src = 'noCutElPFIsoDepositGamma'     +postfix
        getattr(process,'noCutElPFIsoValueNeutral'             +postfix).deposits[0].src = 'noCutElPFIsoDepositNeutral'   +postfix
        getattr(process,'noCutElPFIsoValuePU'                  +postfix).deposits[0].src = 'noCutElPFIsoDepositPU'        +postfix

        noCutIsoValElectron = cms.PSet( pfNeutralHadrons   = cms.InputTag("noCutElPFIsoValueNeutral")
                                      , pfChargedAll       = cms.InputTag("noCutElPFIsoValueChargedAll")
                                      , pfPUChargedHadrons = cms.InputTag("noCutElPFIsoValuePU")
                                      , pfPhotons          = cms.InputTag("noCutElPFIsoValueGamma")
                                      , pfChargedHadrons   = cms.InputTag("noCutElPFIsoValueCharged")
                                      )
        
        setattr(process,'noCutPfIsolatedElectrons'+postfix, getattr(process,'pfIsolatedElectrons'+postfix).clone( src = 'noCutPfElectronsFromVertex'+postfix
                                                                                                                , isolationValueMapsCharged  = ['noCutElPFIsoValueCharged'+postfix]
                                                                                                                , deltaBetaIsolationValueMap =  'noCutElPFIsoValuePU'+postfix
                                                                                                                , isolationValueMapsNeutral  = ['noCutElPFIsoValueNeutral'+postfix,'noCutElPFIsoValueGamma'+postfix]
                                                                                                                , isolationCut = 1e30
                                                                                                                ))
        
        ## right now no new matcher is needed as the gsfElectrons need to be matched
        #setattr(process,'noCutElectronMatch'+postfix, getattr(process,'electronMatch'+postfix).clone(src = 'noCutPfElectronsFromVertex'+postfix))
        getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'electronMatch'+postfix))

        setattr(process,'noCutPatElectrons'+postfix, getattr(process,'patElectrons'+postfix).clone( pfElectronSource     = 'noCutPfElectronsFromVertex'+postfix
                                                                                                  #, genParticleMatch = 'noCutElectronMatch'+postfix
                                                                                                  , isoDeposits      = noCutIsoDepElectron
                                                                                                  , isolationValues  = noCutIsoValElectron
                                                                                                  ))

        process.noCutPatElectronsSequence = cms.Sequence( getattr(process,'noCutPfAllElectrons'          +postfix)
                                                        * getattr(process,'noCutPfElectronsFromVertex'   +postfix)
                                                        * getattr(process,'noCutElPFIsoDepositCharged'   +postfix)
                                                        * getattr(process,'noCutElPFIsoDepositChargedAll'+postfix)
                                                        * getattr(process,'noCutElPFIsoDepositGamma'     +postfix)
                                                        * getattr(process,'noCutElPFIsoDepositNeutral'   +postfix)
                                                        * getattr(process,'noCutElPFIsoDepositPU'        +postfix)
                                                        * getattr(process,'noCutElPFIsoValueCharged'     +postfix)
                                                        * getattr(process,'noCutElPFIsoValueChargedAll'  +postfix)
                                                        * getattr(process,'noCutElPFIsoValueGamma'       +postfix)
                                                        * getattr(process,'noCutElPFIsoValueNeutral'     +postfix)
                                                        * getattr(process,'noCutElPFIsoValuePU'          +postfix)
                                                        * getattr(process,'noCutPfIsolatedElectrons'     +postfix)
                                                        #* getattr(process,'noCutElectronMatch'           +postfix)
                                                        * getattr(process,'electronMatch'                +postfix)
                                                        * getattr(process,'noCutPatElectrons'            +postfix)
                                                        )
        
        if not options['runOnMC']:
            process.noCutPatElectronsSequence.remove(getattr(process,'electronMatch'+postfix))

        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfAllElectrons'+postfix)
                                                            , process.noCutPatElectronsSequence * getattr(process,'pfAllElectrons'+postfix)
                                                            )


    ##
    ## customize taus
    ##

    ## default tau modules:
    # pfTauPFJets08Region
    # pfTauPileUpVertices
    # pfTauTagInfoProducer
    # pfJetsPiZeros
    # pfJetsLegacyTaNCPiZeros
    # pfJetsLegacyHPSPiZeros
    # pfTausBase
    # hpsSelectionDiscriminator
    # hpsPFTauProducerSansRefs
    # hpsPFTauProducer
    # pfTausBaseDiscriminationByDecayModeFinding
    # pfTausBaseDiscriminationByLooseCombinedIsolationDBSumPtCorr
    # pfTaus
    # pfNoTau
    # hpsPFTauDiscriminationByDecayModeFinding
    # hpsPFTauDiscriminationByVLooseChargedIsolation
    # hpsPFTauDiscriminationByLooseChargedIsolation
    # hpsPFTauDiscriminationByMediumChargedIsolation
    # hpsPFTauDiscriminationByTightChargedIsolation
    # hpsPFTauDiscriminationByVLooseIsolation
    # hpsPFTauDiscriminationByLooseIsolation
    # hpsPFTauDiscriminationByMediumIsolation
    # hpsPFTauDiscriminationByTightIsolation
    # hpsPFTauDiscriminationByVLooseIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByLooseIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByMediumIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByTightIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByRawChargedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByRawGammaIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr
    # hpsPFTauDiscriminationByLooseElectronRejection
    # hpsPFTauDiscriminationByMediumElectronRejection
    # hpsPFTauDiscriminationByTightElectronRejection
    # hpsPFTauDiscriminationByMVAElectronRejection
    # hpsPFTauDiscriminationByLooseMuonRejection
    # hpsPFTauDiscriminationByMediumMuonRejection
    # hpsPFTauDiscriminationByTightMuonRejection


    ## remove the full pftau sequence as it is not needed for us
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauPFJets08Region'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauPileUpVertices'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTauTagInfoProducer'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsLegacyTaNCPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfJetsLegacyHPSPiZeros'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBase'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsSelectionDiscriminator'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauProducerSansRefs'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauProducer'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBaseDiscriminationByDecayModeFinding'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTausBaseDiscriminationByLooseCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfTaus'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfNoTau'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByDecayModeFinding'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByVLooseChargedIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseChargedIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumChargedIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightChargedIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByVLooseIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightIsolation'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByVLooseIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByRawChargedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByRawGammaIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseElectronRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumElectronRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightElectronRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMVAElectronRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByLooseMuonRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByMediumMuonRejection'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'hpsPFTauDiscriminationByTightMuonRejection'+postfix))

    ##
    ## customize photons
    ##

    ## default photon modules:
    #pfAllPhotons
    #pfSelectedPhotons
    #phPFIsoDepositCharged
    #phPFIsoDepositChargedAll
    #phPFIsoDepositGamma
    #phPFIsoDepositNeutral
    #phPFIsoDepositPU
    #phPFIsoValueCharged03
    #phPFIsoValueChargedAll03
    #phPFIsoValueGamma03
    #phPFIsoValueNeutral03
    #phPFIsoValuePU03
    #phPFIsoValueCharged04
    #phPFIsoValueChargedAll04
    #phPFIsoValueGamma04
    #phPFIsoValueNeutral04
    #phPFIsoValuePU04
    #pfIsolatedPhotons

    #getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfAllPhotons'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfSelectedPhotons'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoDepositCharged'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoDepositChargedAll'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoDepositGamma'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoDepositNeutral'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoDepositPU'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueCharged03PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueChargedAll03PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueGamma03PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueNeutral03PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValuePU03PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueCharged04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueChargedAll04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueGamma04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValueNeutral04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'phPFIsoValuePU04PFId'+postfix))
    getattr(process,'patPF2PATSequence'+postfix).remove(getattr(process,'pfIsolatedPhotons'+postfix))

    ##
    ## customize jets
    ##

    ## default jet modules:
    # pfJets
    # pfNoJet
    # pfJetTracksAssociatorAtVertex
    # kt6PFJets
    # patJetCorrFactors
    # jetTracksAssociatorAtVertex
    # impactParameterTagInfosAOD
    # secondaryVertexTagInfosAOD
    # softMuonTagInfosAOD
    # jetBProbabilityBJetTagsAOD
    # jetProbabilityBJetTagsAOD
    # trackCountingHighPurBJetTagsAOD
    # trackCountingHighEffBJetTagsAOD
    # simpleSecondaryVertexHighEffBJetTagsAOD
    # simpleSecondaryVertexHighPurBJetTagsAOD
    # combinedSecondaryVertexBJetTagsAOD
    # combinedSecondaryVertexMVABJetTagsAOD
    # softMuonBJetTagsAOD
    # softMuonByPtBJetTagsAOD
    # softMuonByIP3dBJetTagsAOD
    # patJetCharge
    # patJetPartonMatch
    # genParticlesForJetsNoNu
    # iterativeCone5GenJetsNoNu
    # ak5GenJetsNoNu
    # ak7GenJetsNoNu
    # patJetGenJetMatch
    # patJetPartons
    # patJetPartonAssociation
    # patJetFlavourAssociation
    # patJets
    # selectedPatJets

    ## activate jet area calculation needed for L1FastJet corrections
    # is the default now, no switch needed anymore
    getattr(process,'pfJets'+postfix).doAreaFastjet = True

    ## switchmodules to correct sources
    massSearchReplaceAnyInputTag(getattr(process,'patPF2PATSequence'+postfix),'pfNoTau'+postfix,'pfJets'+postfix)

    ## remove soft lepton taggers, which would have needed more RECO collections as input
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

    ## when flavor corrections are needed use the ones derived from top events
    getattr(process,'patJetCorrFactors'+postfix).flavorType = 'T'

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
    ## use rho from ALL PFcandidates to be consistent with the JEC
    from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
    setattr(process, 'kt6PFJetsUsePATWithPFlow'+postfix, kt4PFJets.clone(src='particleFlow', doAreaFastjet=True, doRhoFastjet=True, rParam=0.6))

    ## add kt6PFJets for rho calculation needed for L1FastJet correction
    getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfMET'+postfix)
                                                        , getattr(process,'kt6PFJetsUsePATWithPFlow'+postfix) * getattr(process,'pfMET'+postfix)
                                                        )

    ## do gluon tagging for jets
    ## further information can be found here: https://twiki.cern.ch/twiki/bin/view/CMS/GluonTag
    if options['addGluonTags']:
        from QuarkGluonTagger.EightTeV.QGTagger_RecoJets_cff import goodOfflinePrimaryVerticesQG, kt6PFJetsQG, kt6PFJetsIsoQG, QGTagger
        setattr(process,'goodOfflinePrimaryVerticesQG'+postfix, goodOfflinePrimaryVerticesQG.clone())
        setattr(process,'kt6PFJetsQG'                 +postfix, kt6PFJetsQG.clone())
        setattr(process,'kt6PFJetsIsoQG'              +postfix, kt6PFJetsIsoQG.clone())
        setattr(process,'QGTagger'                    +postfix, QGTagger.clone( srcJets   = cms.InputTag('pfJets'+postfix)
                                                                              , useCHS    = cms.untracked.bool(True)
                                                                              , jec       = cms.untracked.string('combinedCorrector')
                                                                              , srcRho    = cms.InputTag('kt6PFJetsQG'+postfix,'rho')
                                                                              , srcRhoIso = cms.InputTag('kt6PFJetsIsoQG'+postfix,'rho')
                                                                              ))
        setattr(process,'QuarkGluonTagger'+postfix, cms.Sequence( getattr(process,'goodOfflinePrimaryVerticesQG'+postfix)
                                                                * getattr(process,'kt6PFJetsQG'                 +postfix)
                                                                * getattr(process,'kt6PFJetsIsoQG'              +postfix)
                                                                * getattr(process,'QGTagger'                    +postfix)
                                                                ))

        getattr(process,'patJets'+postfix).userData.userFloats.src = getattr(process,'patJets'+postfix).userData.userFloats.src + [cms.InputTag("QGTagger"+postfix,"qgLikelihood"), cms.InputTag("QGTagger"+postfix,"qgMLP")]
        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'kt6PFJetsUsePATWithPFlow'+postfix)
                                                              ## put after kt6JetUsePATWithPFlows as they are needed by the JEC
                                                            , getattr(process,'kt6PFJetsUsePATWithPFlow'+postfix) * getattr(process,'QuarkGluonTagger'+postfix))
        

    ## jet corrector is needed for TypeI MET and QuarkGluonTagger
    if options['addGluonTags'] or options['METCorrectionLevel'] == 1 or options['METCorrectionLevel'] == 2:
        ## create jet correctors for MET corrections
        from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import ak5PFL1Fastjet, ak5PFL2Relative, ak5PFL3Absolute, ak5PFResidual
        ## L1FastJet
        process.ak5PFL1FastjetChs = ak5PFL1Fastjet.clone( algorithm = 'AK5PFchs'
                                                        , srcRho    = cms.InputTag('kt6PFJetsUsePATWithPFlow','rho',process.name_())
                                                        )

        ## L2Relative
        process.ak5PFL2RelativeChs = ak5PFL2Relative.clone( algorithm = 'AK5PFchs' )

        ## L3Absolute
        process.ak5PFL3AbsoluteChs = ak5PFL3Absolute.clone( algorithm = 'AK5PFchs' )

        ## Residual
        process.ak5PFResidualChs = ak5PFResidual.clone( algorithm = 'AK5PFchs' )

        ## combinded corrections
        process.combinedCorrector = cms.ESProducer( 'JetCorrectionESChain'
                                                  , correctors = cms.vstring('ak5PFL1FastjetChs','ak5PFL2RelativeChs','ak5PFL3AbsoluteChs','ak5PFResidualChs')
                                                  )

        ## remove residual corrections from MET corrections for MC
        if options['runOnMC']:
            process.combinedCorrector.correctors.remove('ak5PFResidualChs')


    ##
    ## customize MET
    ##

    ## default met modules:
    # pfMET
    # patMETs

    ## re-configure and create MET
    ## use MET from all PF candidates here !!!
    getattr(process,'pfMET'+postfix).src = 'particleFlow'

    if options['METCorrectionLevel'] == 0 and not str(options['METCorrectionLevel']) == 'False':
        print "Raw MET will be used, no corrections will be applied!"
    elif options['METCorrectionLevel'] == 1 and not str(options['METCorrectionLevel']) == 'True' or options['METCorrectionLevel'] == 2 :
        ## configuration of MET corrections
        from JetMETCorrections.Type1MET.pfMETCorrections_cff import pfchsMETcorr, pfJetMETcorr, pfCandMETcorr, pfType1p2CorrectedMet
        setattr(process,'metCorrType0'+postfix,pfchsMETcorr.clone())
        getattr(process,'metCorrType0'+postfix).src = 'goodOfflinePrimaryVertices'
        setattr(process,'metCorrTypeI'+postfix,pfJetMETcorr.clone())
        getattr(process,'metCorrTypeI'+postfix).src = 'pfJets'+postfix
        getattr(process,'metCorrTypeI'+postfix).jetCorrLabel = 'combinedCorrector'
        getattr(process,'metCorrTypeI'+postfix).offsetCorrLabel = 'ak5PFL1FastjetChs'
        #getattr(process,'metCorrTypeI'+postfix).type1JetPtThreshold = 10.0
        #getattr(process,'metCorrTypeI'+postfix).skipEMfractionThreshold = 0.9
        setattr(process,'metCorrTypeII'+postfix,pfCandMETcorr.clone())
        getattr(process,'metCorrTypeII'+postfix).src = 'pfNoJet'+postfix
        setattr(process,'corMet'+postfix,pfType1p2CorrectedMet.clone())
        getattr(process,'corMet'+postfix).src                 = 'pfMET'+postfix
        getattr(process,'corMet'+postfix).srcCHSSums          = [cms.InputTag('metCorrType0'+postfix,'type0')]
        getattr(process,'corMet'+postfix).srcType1Corrections = [cms.InputTag('metCorrTypeI'+postfix,'type1')]
        if options['METCorrectionLevel'] == 1 :
            print "TypeI corrected MET will be used, corrections for JEC will be applied!"
            getattr(process,'corMet'+postfix).applyType2Corrections = False
            setattr(process,'metCorrections'+postfix, cms.Sequence(getattr(process,'metCorrType0'+postfix) * getattr(process,'metCorrTypeI'+postfix)))
        elif options['METCorrectionLevel'] == 2 :
            print "TypeII corrected MET will be used, corrections for JEC and unclustered energy will be applied!"
            getattr(process,'corMet'+postfix).applyType2Corrections = True
            getattr(process,'corMet'+postfix).srcUnclEnergySums = [cms.InputTag('metCorrTypeI'+postfix, 'type2'), cms.InputTag('metCorrTypeI'+postfix, 'offset'), cms.InputTag('metCorrTypeII')]
            setattr(process,'metCorrections'+postfix, cms.Sequence(getattr(process,'metCorrType0'+postfix) * getattr(process,'metCorrTypeI'+postfix) * getattr(process,'metCorrTypeII'+postfix)))
        ## add MET corrections to sequence
        getattr(process,'patPF2PATSequence'+postfix).replace( getattr(process,'pfMET'+postfix), getattr(process,'pfMET'+postfix)
                                                            * getattr(process,'metCorrections'+postfix) * getattr(process,'corMet'+postfix)
                                                            )

        ## change uncorrected MET to corrected MET in PAT
        getattr(process,'patMETs'+postfix).metSource = 'corMet'+postfix

    else:
        raise ValueError, "Wrong config: 'METCorrectionLevel' may only be set to 0,1,2 (no correction, Type0+TypeI, Type0+TypeI+TypeII corrections) but was set to *"+str(options['METCorrectionLevel'])+"*!"

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
    if options['electronIDs'].count('MVA') > 0 :
        process.pf2pat += process.eidMVASequence

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
    print 'resolutionsVersion:', options['resolutionsVersion']
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
    print 'doDeltaBetaCorrMuon:', options['doDeltaBetaCorrMuon']
    print 'doDeltaBetaCorrElec:', options['doDeltaBetaCorrElec']
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
    print 'JECEra:', options['JECEra']
    print 'JECFile:', options['JECFile']
    print 'additionalJECLevels:', options['additionalJECLevels']
    print '==================================================='
    print '|||||||||||||||||||||||||||||||||||||||||||||||||||'
    print '==================================================='

    ## append pf2pat sequence to all paths in pathnames
    if pathnames == ['']:
        pathnames = process.paths_().keys()

    print 'prepending PF2PAT sequence to paths/sequences:', pathnames
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

    if 'postfix' in options:
        print 'POSTFIXES ARE NOT SUPPORTED AT THE MOMENT, THIS OPTION IS IGNORED'
