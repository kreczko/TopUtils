#ifndef UnclusteredMETScale_h
#define UnclusteredMETScale_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"

class UnclusteredMETScale : public edm::EDProducer {

 public:
  /// default constructor
  explicit UnclusteredMETScale(const edm::ParameterSet&);
  /// default destructor
  ~UnclusteredMETScale(){};
  
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
  /// scale factor for the rescaling
  double scaleFactor_;
};

#endif
