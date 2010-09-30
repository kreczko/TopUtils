#ifndef ResidualJetCorrector_h
#define ResidualJetCorrector_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

class ResidualJetCorrector : public edm::EDProducer {

 public:
  explicit ResidualJetCorrector(const edm::ParameterSet&);
  ~ResidualJetCorrector(){
    delete ResJetCorPar;
    delete JEC;
  };
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag jets_;
  std::string corrections_;

  std::string JEC_PATH;
  edm::FileInPath fipRes;
  JetCorrectorParameters *ResJetCorPar;
  std::vector<JetCorrectorParameters> vParam;
  FactorizedJetCorrector *JEC;

};

#endif
