#ifndef JetEnergyScale_h
#define JetEnergyScale_h

#include "FWCore/Framework/interface/EDProducer.h"

class JetEnergyScale : public edm::EDProducer {

 public:
  explicit JetEnergyScale(const edm::ParameterSet&);
  ~JetEnergyScale(){};
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag inputJets_;
  edm::InputTag inputMETs_;

  std::string outputJets_;
  std::string outputMETs_;

  double scaleFactor_;

  double jetPTthresholdForMET_;
  double jetEMlimitForMET_;
};

#endif
