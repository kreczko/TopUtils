era = 'Jec11_V10'

import FWCore.ParameterSet.Config as cms
process = cms.Process('jecdb')
process.load('CondCore.DBCommon.CondDBCommon_cfi')
process.CondDBCommon.connect = 'sqlite_file:'+era+'_private.db'
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source('EmptySource')
process.PoolDBOutputService = cms.Service('PoolDBOutputService',
   process.CondDBCommon,
   toPut = cms.VPSet(
      cms.PSet(
         record = cms.string('AK5PFchs'), 
         tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PFchs'), 
         label  = cms.string('AK5PFchs') 
      )#,
      #cms.PSet(
      #   record = cms.string('AK5PF'), 
      #   tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PF'), 
      #   label  = cms.string('AK5PF') 
      #)
   )
)

process.dbWriterAK5PFchs = cms.EDAnalyzer('JetCorrectorDBWriter',
   era    = cms.untracked.string(era),
   algo   = cms.untracked.string('AK5PFchs')
)
process.dbWriterAK5PF = cms.EDAnalyzer('JetCorrectorDBWriter',
   era    = cms.untracked.string(era),
   algo   = cms.untracked.string('AK5PF')
)

## save JEC and uncertainties to files
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('GR_R_42_V23::All')
process.readAK5PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                                      # below is the communication to the database 
                                      payloadName    = cms.untracked.string('AK5PFchs'),
                                      # this is used ONLY for the name of the printed txt files. You can use any name that you like, 
                                      # but it is recommended to use the GT name that you retrieved the files from.
                                      globalTag      = cms.untracked.string(era),
                                      printScreen    = cms.untracked.bool(False),
                                      createTextFile = cms.untracked.bool(True),
                                   )
process.readAK5PF = cms.EDAnalyzer('JetCorrectorDBReader',
                                   # below is the communication to the database 
                                   payloadName    = cms.untracked.string('AK5PF'),
                                   # this is used ONLY for the name of the printed txt files. You can use any name that you like, 
                                   # but it is recommended to use the GT name that you retrieved the files from.
                                   globalTag      = cms.untracked.string(era),
                                   printScreen    = cms.untracked.bool(False),
                                   createTextFile = cms.untracked.bool(True),
                                   )

process.p = cms.Path(
    #process.readAK5PFchs *
    #process.readAK5PF
    process.dbWriterAK5PFchs #*
    #process.dbWriterAK5PF
)
