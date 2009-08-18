import FWCore.ParameterSet.Config as cms

## produce top generated event
from TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff import *
## produce top reconstructed event
from TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff import *
##  produce selectedLayer1TopMuons
from TopAnalysis.TopUtils.TopMuonSelector_cfi import *

makeSelectedLayer1TopMuons = cms.Sequence(makeGenEvt *
                                          makeTtSemiLepEvent *
                                          selectedLayer1TopMuons
                                          )

