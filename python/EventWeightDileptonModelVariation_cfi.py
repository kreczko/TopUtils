import FWCore.ParameterSet.Config as cms

eventWeightDileptonModelVariation = cms.EDProducer("EventWeightDileptonModelVariation",
    ttGenEvent = cms.InputTag('genEvt'),
    weightVariable = cms.string('toppt'), #valid values: toppt, topeta, topmass
    slope = cms.double(0),
    weight1x = cms.double(200), #position where weight is 1
    minWeight = cms.double(0.1), #low cut-off, at least 0.1 event weight
    maxWeight = cms.double(2),  #high cut-off, at most 2 event weight
)



