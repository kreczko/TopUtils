import FWCore.ParameterSet.Config as cms

signalCatcher = cms.EDFilter("SignalCatcher")

runSignalCatcher = cms.Path(signalCatcher)

