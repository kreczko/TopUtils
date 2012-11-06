#ifndef MyPatJetCorrFactorsProducer_h
#define MyPatJetCorrFactorsProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

class MyPatJetCorrFactorsProducer : public edm::EDProducer {

 public:
  explicit MyPatJetCorrFactorsProducer(const edm::ParameterSet&);
  ~MyPatJetCorrFactorsProducer();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag jetSrc_;

};

#endif
