#ifndef ResonanceTagProbeProducer_h
#define ResonanceTagProbeProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class ResonanceTagProbeProducer : public edm::EDProducer {

 public:
  explicit ResonanceTagProbeProducer(const edm::ParameterSet&);
  ~ResonanceTagProbeProducer(){};
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag tags_;
  edm::InputTag probes_;
  double mass_;
  double deltaM_;
};

#endif
