#ifndef CSA07WeightAnalyzer_h
#define CSA07WeightAnalyzer_h

#include <memory>
#include <string>
#include <vector>

#include "TH1F.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TopAnalysis/TopUtils/interface/NameScheme.h"

class CSA07WeightAnalyzer : public edm::EDAnalyzer {

 public:

  explicit CSA07WeightAnalyzer(const edm::ParameterSet&);
  ~CSA07WeightAnalyzer(){};
  
 private:

  virtual void beginJob(const edm::EventSetup&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:

  std::string  hist_;

  TH1F *evtWeight_;   
  TH1F *evtScale_;  
  TH1F *evtPreXSec_;   
  TH1F *evtAutoXSec_; 
  TH1F *evtFilterEff_;
  TH1F *evtWghtVsScale_;        
};

#endif
