#ifndef EventWeightDileptonSF_h
#define EventWeightDileptonSF_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

class EventWeightDileptonSF : public edm::EDProducer {

 public:
  explicit EventWeightDileptonSF(const edm::ParameterSet&);
  ~EventWeightDileptonSF();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  double getLeptonSF(const reco::Candidate&);

  /// electrons to calculate SF
  edm::InputTag electrons_;
  /// muons to calculate SF
  edm::InputTag muons_;
};

#endif
