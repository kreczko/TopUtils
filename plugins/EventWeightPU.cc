#ifndef EventWeightPU_cc
#define EventWeightPU_cc

#include "EventWeightPU.h"

// =============
//  Constructor
// =============

EventWeightPU::EventWeightPU(const edm::ParameterSet& cfg):
  inTag_MCSampleFile(cfg.getParameter<std::string>("MCSampleFile") ),
  inTag_MCSampleHistoName(cfg.getParameter<std::string>("MCSampleHistoName") ),
  inTag_DataFile(cfg.getParameter<std::string>("DataFile") ),
  inTag_DataHistoName(cfg.getParameter<std::string>("DataHistoName") ),
  inTag_PUSource(cfg.getParameter<edm::InputTag>("PUSource") )
{

  LumiWeights_ = edm::LumiReWeighting(inTag_MCSampleFile,inTag_DataFile,inTag_MCSampleHistoName,inTag_DataHistoName);
  
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
  
  for(iterPU = pPUInfo->begin(); iterPU != pPUInfo->end(); ++iterPU)
    
    if (iterPU->getBunchCrossing()==0)
    {
      wght_ = LumiWeights_.weight(iterPU->getPU_NumInteractions());
      *eventWeightPU = wght_;
    }
    else *eventWeightPU = -1; // must be changed for proper out-of time PU handling
  
  evt.put(eventWeightPU);
  
}

#endif
