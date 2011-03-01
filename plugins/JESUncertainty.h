#ifndef JESUncertainty_h
#define JESUncertainty_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"

class JESUncertainty : public edm::EDProducer {

 public:
  /// default constructor
  explicit JESUncertainty(const edm::ParameterSet&);
  /// default destructor
  ~JESUncertainty(){};
  
 private:
  /// check settings
  virtual void beginJob();
  /// rescale jet energy and recalculated MET
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  /// jet input collection 
  edm::InputTag inputJets_;
  /// met input collection
  edm::InputTag inputMETs_;
  /// uncertainty of b-scale
  double bScaleUnc_;
  /// uncertainty from PU
  double puUnc_;
  /// uncertainty from sw version
  double swUnc_;
  /// up or down scaling
  bool direction_;  
};

#endif
