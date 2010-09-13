#ifndef JetEnergyScale_h
#define JetEnergyScale_h

#include "FWCore/Framework/interface/EDProducer.h"

/**
   \class   JetEnergyScale JetEnergyScale.h "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"

   \brief   Plugin to shift the jet energy scale and recalculate the MET accordingly

   Plugin to shift the jet energy scale and recalculate the MET accordingly. The module 
   mimics the assumption that the jet energy scale (JES) has been estimated wrong by a
   factor of _scaleFactor_, corresponding to a L2L3 corrected jet. The p4 of the patJet 
   is beeing rescaled. All other patJet properties stay the same. The MET is recalculated 
   taking the shifted JES into account for the Type1 MET correction. For the patMET the 
   rescaled sumET and the p4 are stored. The different correction levels are lost for 
   the new collection. The module has the following parameters: 

  inputJets            : input collection for  MET (expecting patMET).
  inputMETs            : input collection for jets (expecting patJets).
  scaleFactor          : scale factor to which to shift the JES.
  jetPTThresholdForMET : pt threshold for (uncorrected!) jets considered for Type1 MET 
                         corrections. 
  jetEMLimitForMET     : limit in em fraction for Type1 MET correction. 

  For expected parameters for _jetPTThresholdForMET_ and _jetEMLimitForMET_ have a look 
  at: JetMETCorrections/Type1MET/python/MetType1Corrections_cff.py. Two output collections 
  are written to file with instance label corresponding to the input label of the jet 
  and met input collections. 
*/

class JetEnergyScale : public edm::EDProducer {

 public:
  /// default constructor
  explicit JetEnergyScale(const edm::ParameterSet&);
  /// default destructor
  ~JetEnergyScale(){};
  
 private:
  /// rescale jet energy and recalculated MET
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  /// jet input collection 
  edm::InputTag inputJets_;
  /// met input collection
  edm::InputTag inputMETs_;
  /// jet output collection 
  std::string outputJets_;
  /// MET output collection 
  std::string outputMETs_;
  /// scale factor for the rescaling
  double scaleFactor_;
  /// threshold on (raw!) jet pt for Type1 MET corrections 
  double jetPTThresholdForMET_;
  /// limit on the emf of the jet for Type1 MET corrections 
  double jetEMLimitForMET_;
};

#endif
