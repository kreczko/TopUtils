#ifndef JetEnergyScale_h
#define JetEnergyScale_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

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

   inputJets            : input collection for jets (expecting patJets).
   inputMETs            : input collection for  MET (expecting patMET).
   scaleFactor          : scale factor to which to shift the JES.
   scaleType            : type of scaling; you can choose between _abs_ (normal scale 
                          factor), _rel_ (a scale factor relative in eta?), _jes_ (un-
			  certainty UP and DOWN scaling according to the estimated JES
			  uncertainties as derived from JetMET) and _top_ (here we are 
			  following the suggestion of the TopPAG: on top of the normal 
			  JES uncertainty as given by JetMET we add the following un-
			  certainties on top: 
			  + PU: add an equivalent of 0.2GeV*0.8(jetArea)*2.2(<PU>))/pt.
			  + 0.02 (for 50<pt(jet)<200 && |eta(jet)|<2) and 0.03 else.
			  + release differences and calibration changes (configurable).
   resolutionFactor     : factor to rescale the jet resolution. Increasing the JER by 
                          10% requires a resolutionFactor of 1.1
   jetPTThresholdForMET : pt threshold for (uncorrected!) jets considered for Type1 MET 
                          corrections. 
   jetEMLimitForMET     : limit in em fraction for Type1 MET correction. 

   The scaleType jes will turn the parameter scaleFactor invalid, as the scales are taken
   from JetMET. 
   For expected parameter values for _jetPTThresholdForMET_ and _jetEMLimitForMET_ have a 
   look at: JetMETCorrections/Type1MET/python/MetType1Corrections_cff.py. Two output 
   collections are written to file with instance labels corresponding to the input label 
   of the jet and met input collections. 
*/

class JetEnergyScale : public edm::EDProducer {

 public:
  /// default constructor
  explicit JetEnergyScale(const edm::ParameterSet&);
  /// default destructor
  ~JetEnergyScale(){};
  
 private:
  /// check settings
  virtual void beginJob();
  /// rescale jet energy and recalculated MET
  virtual void produce(edm::Event&, const edm::EventSetup&);
  /// rescale the resolution of the jet
  double resolutionFactor(const pat::Jet&);
  /// scale all energies of the jet
  void scaleJetEnergy(pat::Jet&, double);

 private:
  /// electron input collection 
  edm::InputTag inputElectrons_;
  /// jet input collection 
  edm::InputTag inputJets_;
  /// met input collection
  edm::InputTag inputMETs_;

  /// electron output collection 
  std::string outputElectrons_;
  /// jet output collection 
  std::string outputJets_;
  /// MET output collection 
  std::string outputMETs_;
  /// payload for scaleType jes
  std::string payload_;
  /// absolute scaling or relative in eta
  std::string scaleType_;
  /// if scaleType is set to use a single source, here's the name of the source to be used
  std::string sourceName_;
  /// scale factor for the rescaling of JES
  double scaleFactor_;
  /// scale factor bJES/JES
  double scaleFactorB_;
  /// scale factors for the energy resolution of jets
  std::vector<double> resolutionFactor_;
  /// valid |eta| ranges for the energy resolution scale factors
  std::vector<double> resolutionRanges_;
  /// JECUncertaintySource File
  edm::FileInPath JECUncSrcFile_;
  /// threshold on (raw!) jet pt for Type1 MET corrections 
  double jetPTThresholdForMET_;
  /// limit on the emf of the jet for Type1 MET corrections 
  double jetEMLimitForMET_;
  /// allowed scaleTypes
  std::vector<std::string> allowedTypes_;
};

#endif
