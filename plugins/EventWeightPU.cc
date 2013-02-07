#ifndef EventWeightPU_cc
#define EventWeightPU_cc

#include "EventWeightPU.h"

// =============
//  Constructor
// =============

EventWeightPU::EventWeightPU(const edm::ParameterSet& cfg):
  inTag_PUSource(cfg.getParameter<edm::InputTag>("PUSource")),
  inTag_WeightName(cfg.getParameter<std::string>("WeightName")),

  inTag_MCSampleTag(cfg.getParameter<std::string>("MCSampleTag")),

  inTag_MCSampleFile(cfg.getParameter<edm::FileInPath>("MCSampleFile")),
  inTag_MCSampleHistoName(cfg.getParameter<std::string>("MCSampleHistoName")),
  inTag_DataFile(cfg.getParameter<edm::FileInPath>("DataFile")),
  inTag_DataHistoName(cfg.getParameter<std::string>("DataHistoName")),

  inTag_CreateWeight3DHisto(cfg.getParameter<bool>("CreateWeight3DHisto")), 
  inTag_Weight3DHistoFile(cfg.getParameter<edm::FileInPath>("Weight3DHistoFile"))
{

  error_code = 0;

  if(inTag_MCSampleTag == "Fall11" || inTag_MCSampleTag == "Summer12"){

    LumiWeights3D_ = edm::Lumi3DReWeighting();
    LumiWeights_   = edm::LumiReWeighting(inTag_MCSampleFile.fullPath(),inTag_DataFile.fullPath(),
					  inTag_MCSampleHistoName,inTag_DataHistoName);
  }
  else if(inTag_MCSampleTag == "Summer11"){
    
    LumiWeights_   = edm::LumiReWeighting();
    LumiWeights3D_ = edm::Lumi3DReWeighting(inTag_MCSampleFile.fullPath(),inTag_DataFile.fullPath(),
					    inTag_MCSampleHistoName,inTag_DataHistoName,
					    inTag_Weight3DHistoFile.fullPath());
    
    if(inTag_CreateWeight3DHisto){ 

      std::cout << "Creating new 3D matrix." << std::endl;
      LumiWeights3D_.weight3D_init(1.0);
    }
    else LumiWeights3D_.weight3D_init(inTag_Weight3DHistoFile.fullPath());
  }
  else {

    std::cout << " ERROR: Non-valid tag for MC sample, all event weights will be 1. " << std::endl;
    error_code = -1;    
  }
    
  produces<double>(inTag_WeightName);
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

  if(error_code != 0) wght_ = 1;
  else{
  
    edm::Handle<edm::View<PileupSummaryInfo> > pPUInfo;
    evt.getByLabel(inTag_PUSource, pPUInfo);
    
    edm::View<PileupSummaryInfo>::const_iterator iterPU;
    
    // default values to allow for tracing errors
    
    wght_   = -1;
    wght3D_ = -1;
    
    int nvtx_m = -1; 
    int nvtx   = -1; 
    int nvtx_p = -1;
    
    // for the true number of interactions float are used
    // as this is the mean of a Poisson distribution
    // (only the current BX needed)
    float fnvtx = -1.;
    
    for(iterPU = pPUInfo->begin(); iterPU != pPUInfo->end(); ++iterPU)  // vector size is 3
    { 
      int BX = iterPU->getBunchCrossing(); // -1: previous BX, 0: current BX,  1: next BX
      
      if (inTag_MCSampleTag == "Fall11" || inTag_MCSampleTag == "Summer12" ){
	if (BX ==  0) fnvtx = iterPU->getTrueNumInteractions();
      }
      else if (inTag_MCSampleTag == "Summer11"){
	if      (BX == -1) nvtx_m = iterPU->getPU_NumInteractions();
	else if (BX ==  0) nvtx   = iterPU->getPU_NumInteractions();
	else if (BX ==  1) nvtx_p = iterPU->getPU_NumInteractions();  
      }
    }

    if      (inTag_MCSampleTag == "Fall11" || inTag_MCSampleTag == "Summer12") wght_ = LumiWeights_.weight(fnvtx);
    else if (inTag_MCSampleTag == "Summer11") wght_ = LumiWeights3D_.weight3D(nvtx_m, nvtx, nvtx_p);
  } 

  (*eventWeightPU) = wght_;
  evt.put(eventWeightPU,inTag_WeightName);
}

#endif
