#ifndef EventWeightPU_cc
#define EventWeightPU_cc

#include "EventWeightPU.h"

// =============
//  Constructor
// =============

EventWeightPU::EventWeightPU(const edm::ParameterSet& cfg):
  inTag_PUSource(cfg.getParameter<edm::InputTag>("PUSource") ),
  inTag_WeightName(cfg.getParameter<std::string>("WeightName") ),
  inTag_Weight3DName(cfg.getParameter<std::string>("Weight3DName") ),

  inTag_MCSampleFile(cfg.getParameter<edm::FileInPath>("MCSampleFile") ),
  inTag_MCSampleHistoName(cfg.getParameter<std::string>("MCSampleHistoName") ),
  inTag_DataFile(cfg.getParameter<edm::FileInPath>("DataFile") ),
  inTag_DataHistoName(cfg.getParameter<std::string>("DataHistoName") ),

  inTag_MCSample3DFile(cfg.getParameter<edm::FileInPath>("MCSample3DFile") ),
  inTag_MCSample3DHistoName(cfg.getParameter<std::string>("MCSample3DHistoName") ),
  inTag_Data3DFile(cfg.getParameter<edm::FileInPath>("Data3DFile") ),
  inTag_Data3DHistoName(cfg.getParameter<std::string>("Data3DHistoName") ),

  inTag_CreateWeight3DHisto(cfg.getParameter<bool>("CreateWeight3DHisto") ), 
  inTag_Weight3DHistoFile(cfg.getParameter<std::string>("Weight3DHistoFile") )
{

  // ohject for standard re-weighting, in-time-pileup only

  LumiWeights_   = edm::LumiReWeighting(inTag_MCSampleFile.fullPath(),inTag_DataFile.fullPath(),
					inTag_MCSampleHistoName,inTag_DataHistoName);

  // object for 3D-reweighting, includig out-of-time pileup

  LumiWeights3D_ = edm::Lumi3DReWeighting(inTag_MCSample3DFile.fullPath(),inTag_Data3DFile.fullPath(),
					  inTag_MCSample3DHistoName,inTag_Data3DHistoName,
					  inTag_Weight3DHistoFile);
  
  if (inTag_CreateWeight3DHisto)
  { 
    std::cout << "Create new 3D matrix." << std::endl;
    LumiWeights3D_.weight3D_init(1.0);
  }
  else LumiWeights3D_.weight3D_init(inTag_Weight3DHistoFile);

  produces<double>(inTag_WeightName);
  produces<double>(inTag_Weight3DName);  
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
  std::auto_ptr<double> eventWeightPU3D(new double);
  
  edm::Handle<edm::View<PileupSummaryInfo> > pPUInfo;
  evt.getByLabel(inTag_PUSource, pPUInfo);

  edm::View<PileupSummaryInfo>::const_iterator iterPU;

  // default values to allow for tracing errors
  
  wght_   = -1;
  wght3D_ = -1;
 
  int nvtx_m     = -1; 
  int nvtx       = -1; 
  int nvtx_p     = -1;

  for(iterPU = pPUInfo->begin(); iterPU != pPUInfo->end(); ++iterPU)  // vector size is 3
  { 
    int BX = iterPU->getBunchCrossing(); // -1: previous BX, 0: current BX,  1: next BX
    
    if      (BX == -1) nvtx_m = iterPU->getPU_NumInteractions();
    else if (BX ==  0) nvtx   = iterPU->getPU_NumInteractions();
    else if (BX ==  1) nvtx_p = iterPU->getPU_NumInteractions();  
  }

  wght_   = LumiWeights_.weight(nvtx);
  wght3D_ = LumiWeights3D_.weight3D(nvtx_m, nvtx, nvtx_p);

  (*eventWeightPU)   = wght_;
  (*eventWeightPU3D) = wght3D_;

  evt.put(eventWeightPU,inTag_WeightName);
  evt.put(eventWeightPU3D,inTag_Weight3DName);  
}

#endif
