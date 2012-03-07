#ifndef EventWeightPU_h
#define EventWeightPU_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/View.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "PhysicsTools/Utilities/interface/Lumi3DReWeighting.h"

class EventWeightPU : public edm::EDProducer {

   public:
      explicit EventWeightPU(const edm::ParameterSet&);
      ~EventWeightPU();

   private:

      int error_code;

      virtual void produce(edm::Event&, const edm::EventSetup&);

      edm::InputTag   inTag_PUSource;
      std::string     inTag_WeightName;

      std::string     inTag_MCSampleTag;

      edm::FileInPath inTag_MCSampleFile;
      std::string     inTag_MCSampleHistoName;
      edm::FileInPath inTag_DataFile;
      std::string     inTag_DataHistoName;
     
      bool            inTag_CreateWeight3DHisto;
      edm::FileInPath inTag_Weight3DHistoFile;

      edm::LumiReWeighting   LumiWeights_;
      edm::Lumi3DReWeighting LumiWeights3D_;

      double wght_;
      double wght3D_;
};

#endif
