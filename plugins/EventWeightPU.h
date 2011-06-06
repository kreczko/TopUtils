#ifndef EventWeightPU_h
#define EventWeightPU_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

class EventWeightPU : public edm::EDProducer {

   public:
      explicit EventWeightPU(const edm::ParameterSet&);
      ~EventWeightPU();

   private:

      virtual void produce(edm::Event&, const edm::EventSetup&);

      std::string inTag_MCSampleFile;
      std::string inTag_MCSampleHistoName;
      std::string inTag_DataFile;
      std::string inTag_DataHistoName;

      edm::InputTag inTag_PUSource;

      edm::LumiReWeighting LumiWeights_;

      double wght_;

};

#endif
