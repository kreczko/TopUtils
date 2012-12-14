#ifndef PFElectronGSFMomentum_h
#define PFElectronGSFMomentum_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/**
   \class   PFElectronGSFMomentum PFElectronGSFMomentum.h "TopAnalysis/TopUtils/plugins/PFElectronGSFMomentum.h"

   \brief   Replace the PFElectron Momentum with GSFElectron Momentum and scale the MET accordingly

*/

class PFElectronGSFMomentum : public edm::EDProducer {

 public:
  /// default constructor
  explicit PFElectronGSFMomentum(const edm::ParameterSet&);
  /// default destructor
  ~PFElectronGSFMomentum(){};
  
 private:
  /// check settings
  virtual void beginJob();
  /// rescale  recalculated MET
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  /// jet input collection 
  edm::InputTag inputElectrons_;
  /// met input collection
  edm::InputTag inputMETs_;
  /// jet output collection 
  std::string outputElectrons_;
  /// MET output collection 
  std::string outputMETs_;
};

#endif
