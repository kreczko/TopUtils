import FWCore.ParameterSet.Config as cms

process = cms.Process("JetCalibrationTreeWriter")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0010/BE7DBA08-28F9-DD11-A7B1-001CC4A65D04.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0010/6C2D0BBF-E6F8-DD11-90A4-00144F0D6BE8.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0010/6AF22C5E-2BF9-DD11-97DF-001CC4C0B0DC.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0010/40EC05AA-2CF9-DD11-877E-001CC47D7BE0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0010/30D4C1B4-E3F8-DD11-ACF0-00144F0DC2A0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/FC824064-28F7-DD11-86FA-001CC47BCFDC.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/F4C07636-3AF7-DD11-A0FF-001F29082E7E.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/F033E90A-25F7-DD11-96F8-001F2908AED8.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/EEF966B8-35F7-DD11-8254-001E0BECB5C0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/EEBBAE0C-37F7-DD11-8B29-001F2907EF7E.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/ECC96598-38F7-DD11-AB2B-001A4BD0CF54.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/E0F8F810-37F7-DD11-81B1-001A4BD0CF54.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/DAF13ADA-29F7-DD11-BF21-001F2908AE9E.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/D6342D54-2EF7-DD11-8557-001F2907DA48.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/C8CEF23C-2BF7-DD11-8404-001F2908BE72.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/C626E93F-31F7-DD11-9580-001F290789D6.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/C2FF143A-31F7-DD11-BA89-001E0B1CA8A0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/C2FA74C1-35F7-DD11-B46C-001F2908AECC.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/C00B213D-31F7-DD11-A302-001A4BD0CF54.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/BC3292C2-35F7-DD11-8EC3-001E0B616BB2.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/B8BF6BA2-34F7-DD11-9A77-001CC47D2FA4.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/B82A7DA3-34F7-DD11-80E3-001CC47DFC00.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/B2A41DC1-2FF7-DD11-996E-001F290860E4.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/AC91727D-8CF7-DD11-9D3B-00144F0D68CA.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/AADF5A2D-26F7-DD11-8333-001F2908BE30.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/9EA3E1F6-90F7-DD11-A886-001CC4A7D032.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/9A8AB235-3AF7-DD11-AE8B-001F29079F98.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/985E82BC-2FF7-DD11-8D03-001F2908AED8.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/96A10682-81F7-DD11-8A8F-001CC4BF7AC4.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/88AADCC8-32F7-DD11-A847-001CC47D7BE0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/8858DABB-2FF7-DD11-9578-001F2907EF7E.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/884881C5-2FF7-DD11-BCE8-001F2907FC08.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/8638E165-28F7-DD11-9CEA-001CC47D185A.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/7E97A50E-37F7-DD11-96CB-001F2908CFBC.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/7AA8CC11-37F7-DD11-8D80-001CC416D644.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/76AD7DDB-29F7-DD11-9758-001E0BECB5C0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/6E0E414C-2EF7-DD11-AA50-001F2908BE72.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/66860FBC-2FF7-DD11-A974-001F29088E72.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/62E36C32-3AF7-DD11-AA36-001F2907EF7E.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/5EBEF02F-1EF7-DD11-A17B-001F29082E68.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/569DE2C7-35F7-DD11-A733-00144F0D68C6.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/54315627-3AF7-DD11-A402-00144F0D68C0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/4EDED21E-3DF7-DD11-88DF-00144F0D7E90.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/4AFC0FC0-35F7-DD11-8D0C-001F2907DA48.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/463DD566-28F7-DD11-AF6A-001CC416D644.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/46012453-2EF7-DD11-A4A4-001F2907FC08.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/3E39E931-1EF7-DD11-A787-001CC47BCE82.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/3CE4124F-2EF7-DD11-90AD-001CC4A68C80.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/38473DDC-21F7-DD11-8EC9-001F2908F0E4.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/2ACC454C-37F7-DD11-AE5A-00144F0D68C0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/26E5D9DB-21F7-DD11-9FFC-001F2908BE72.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/20C7B190-3BF7-DD11-8328-001F29079F98.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/1CB60330-1EF7-DD11-8069-001F29071CFA.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/0E6C3A39-90F7-DD11-9D77-001CC4A6ABA8.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/06B00B4D-2EF7-DD11-8C08-001E0BECB5C0.root',
        '/store/mc/Summer08/TauolaTTbar/GEN-SIM-RECO/IDEAL_V9_v2/0009/0054812D-26F7-DD11-99D7-001F2908F0E4.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

## configure geometry
process.load("Configuration.StandardSequences.Geometry_cff")

## configure conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('STARTUP_V4::All')

## Magnetic field now needs to be in the high-level py
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("PhysicsTools.PatAlgos.patSequences_cff")
process.p0 = cms.Path(process.patDefaultSequence)

## necessary fixes to run 2.2.X on 2.1.X data
## comment this when running on samples produced
## with 22X
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run22XonSummer08AODSIM
run22XonSummer08AODSIM(process)
process.patDefaultSequence.remove(process.patPFCandidateIsoDepositSelection)
process.patDefaultSequence.remove(process.patPFTauIsolation) 

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('allJetCalibrationTree.root')
)

process.jetCalibrationTreeWriter = cms.EDAnalyzer('JetCalibrationTreeWriter',
     jetTag      = cms.untracked.InputTag("patJets")
                                                  
)

process.dump = cms.EDAnalyzer('EventContentAnalyzer'
)

process.p = cms.Path(process.jetCalibrationTreeWriter)
##process.p = cms.Path(process.dump)
