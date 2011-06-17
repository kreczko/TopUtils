#ifndef EventWeightPU_cc
#define EventWeightPU_cc

#include "EventWeightPU.h"

// =============
//  Constructor
// =============

EventWeightPU::EventWeightPU(const edm::ParameterSet& cfg):
  inTag_MCSampleFile(cfg.getParameter<edm::FileInPath>("MCSampleFile") ),
  inTag_MCSampleHistoName(cfg.getParameter<std::string>("MCSampleHistoName") ),
  inTag_DataFile(cfg.getParameter<edm::FileInPath>("DataFile") ),
  inTag_DataHistoName(cfg.getParameter<std::string>("DataHistoName") ),
  inTag_PUSource(cfg.getParameter<edm::InputTag>("PUSource") )
{

  LumiWeights_ = edm::LumiReWeighting(inTag_MCSampleFile.fullPath(),inTag_DataFile.fullPath(),inTag_MCSampleHistoName,inTag_DataHistoName);
  
  produces<double>();
}

// =============
//  Destructor
// =============

EventWeightPU::~EventWeightPU() {}

// =======================================================
//  Calculate and assign event weights for PU reweighting
// =======================================================

void EventWeightPU::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  std::auto_ptr<double> eventWeightPU(new double); 
  
  edm::Handle<std::vector<PileupSummaryInfo> > pPUInfo;
  evt.getByLabel(inTag_PUSource, pPUInfo);

  std::vector<PileupSummaryInfo>::const_iterator iterPU;

  wght_ = -1;
  *eventWeightPU = wght_;  // default value to allow for tracing errors
  
  for(iterPU = pPUInfo->begin(); iterPU != pPUInfo->end(); ++iterPU)  // vector size is 3
   
    if (iterPU->getBunchCrossing()==0) // -1: previous BX, 0: current BX,  1: next BX
    {
      wght_ = LumiWeights_.weight(iterPU->getPU_NumInteractions());
      (*eventWeightPU) = wght_;
      break;
    }
  
  evt.put(eventWeightPU);
  
}

#endif
