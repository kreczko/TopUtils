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

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

class EventWeightPU : public edm::EDProducer {

   public:
      explicit EventWeightPU(const edm::ParameterSet&);
      ~EventWeightPU();

   private:

      virtual void produce(edm::Event&, const edm::EventSetup&);

      edm::FileInPath inTag_MCSampleFile;
      std::string inTag_MCSampleHistoName;
      edm::FileInPath inTag_DataFile;
      std::string inTag_DataHistoName;

      edm::InputTag inTag_PUSource;

      float inTag_PUSysShiftUp;
      float inTag_PUSysShiftDown;

      edm::LumiReWeighting LumiWeights_;

      reweight::PoissonMeanShifter PUShiftUp_;
      reweight::PoissonMeanShifter PUShiftDown_;

      double wght_;
      double wght_Up_;
      double wght_Down_;
      double wght3BX_;
      double wght3BX_Up_;
      double wght3BX_Down_;
};

#endif
