#ifndef EventWeightPU_cc
#define EventWeightPU_cc

#include "EventWeightPU.h"
#include "DataFormats/Common/interface/View.h"

// =============
//  Constructor
// =============

EventWeightPU::EventWeightPU(const edm::ParameterSet& cfg):
  inTag_MCSampleFile(cfg.getParameter<edm::FileInPath>("MCSampleFile") ),
  inTag_MCSampleHistoName(cfg.getParameter<std::string>("MCSampleHistoName") ),
  inTag_DataFile(cfg.getParameter<edm::FileInPath>("DataFile") ),
  inTag_DataHistoName(cfg.getParameter<std::string>("DataHistoName") ),
  inTag_PUSource(cfg.getParameter<edm::InputTag>("PUSource") ),
  inTag_PUSysShiftUp(cfg.getParameter<double>("PUSysShiftUp") ),
  inTag_PUSysShiftDown(cfg.getParameter<double>("PUSysShiftDown") )
{

  LumiWeights_ = edm::LumiReWeighting(inTag_MCSampleFile.fullPath(),inTag_DataFile.fullPath(),inTag_MCSampleHistoName,inTag_DataHistoName);
  
  PUShiftUp_   = reweight::PoissonMeanShifter(inTag_PUSysShiftUp);
  PUShiftDown_ = reweight::PoissonMeanShifter(inTag_PUSysShiftDown);

  produces<double>("eventWeightPU");
  produces<double>("eventWeightPUUp");
  produces<double>("eventWeightPUDown");
  produces<double>("eventWeightPU3BX");  
  produces<double>("eventWeightPU3BXUp");
  produces<double>("eventWeightPU3BXDown");
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
  std::auto_ptr<double> eventWeightPU_Up(new double); 
  std::auto_ptr<double> eventWeightPU_Down(new double); 
  std::auto_ptr<double> eventWeightPU3BX(new double);
  std::auto_ptr<double> eventWeightPU3BX_Up(new double);
  std::auto_ptr<double> eventWeightPU3BX_Down(new double);
  
  edm::Handle<edm::View<PileupSummaryInfo> > pPUInfo;
  evt.getByLabel(inTag_PUSource, pPUInfo);

  edm::View<PileupSummaryInfo>::const_iterator iterPU;

  // default values to allow for tracing errors
  
  wght_ = -1;
  *eventWeightPU = wght_;

  wght3BX_ = -1;
  *eventWeightPU3BX = wght3BX_;   

  float sum_nvtx = 0;
  
  for(iterPU = pPUInfo->begin(); iterPU != pPUInfo->end(); ++iterPU)  // vector size is 3
  { 
    sum_nvtx += iterPU->getPU_NumInteractions();

    if (iterPU->getBunchCrossing()==0) // -1: previous BX, 0: current BX,  1: next BX
    {
      int nvtx = iterPU->getPU_NumInteractions();

      wght_      = LumiWeights_.weight(nvtx);
      wght_Up_   = wght_ * PUShiftUp_.ShiftWeight(nvtx);
      wght_Down_ = wght_ * PUShiftDown_.ShiftWeight(nvtx);
    }
  }

  wght3BX_      = LumiWeights_.weight3BX(sum_nvtx/3.0);
  wght3BX_Up_   = wght3BX_ * PUShiftUp_.ShiftWeight(float(sum_nvtx/3.0));
  wght3BX_Down_ = wght3BX_ * PUShiftDown_.ShiftWeight(float(sum_nvtx/3.0));

  (*eventWeightPU)         = wght_;
  (*eventWeightPU_Up)      = wght_Up_;
  (*eventWeightPU_Down)    = wght_Down_;
  (*eventWeightPU3BX)      = wght3BX_;
  (*eventWeightPU3BX_Up)   = wght3BX_Up_;
  (*eventWeightPU3BX_Down) = wght3BX_Down_;

  evt.put(eventWeightPU,"eventWeightPU");
  evt.put(eventWeightPU_Up,"eventWeightPUUp");
  evt.put(eventWeightPU_Down,"eventWeightPUDown");
  evt.put(eventWeightPU3BX,"eventWeightPU3BX");  
  evt.put(eventWeightPU3BX_Up,"eventWeightPU3BXUp");  
  evt.put(eventWeightPU3BX_Down,"eventWeightPU3BXDown");  
}

#endif
