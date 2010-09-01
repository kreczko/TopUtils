import FWCore.ParameterSet.Config as cms

eventWeight = cms.EDProducer("EventWeightPlain",
  nevts = cms.uint32(  1),    ## add the maximal number of events in the sample
  xsec  = cms.double(1.0),    ## add the xsection for production in pb
  eff   = cms.double(1.0),    ## add filter efficiencies
  lumi  = cms.double(1.0)     ## add the target lumi you aim for
)

makeWeights = cms.Sequence(eventWeight)

