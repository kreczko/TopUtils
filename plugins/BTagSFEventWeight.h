#ifndef BTagSFEventWeight_h
#define BTagSFEventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/// This module calculates a b tag scale factor (SF) for the whole event with >=2 b tags,
/// which is put into the CMSSW event
/// as a double, which can be used as an event weight in the analyzers of interest.
/// cfg parameters:
/// jets_             jet collection
/// bTagAlgo_         name of the b tag algorithm (supported: SSVHEM)
/// sysVar_           name of the systematic variation (noSys, bTagSFUp, bTagSFDown, misTagSFUp, misTagSFDown)
/// verbose_          set to 0 if no output on terminal is desired, 1 for moderate and 2 for detailed output
/// filename_         if not set to "", efficiencies are loaded from histos in filename_, which have been
///                   created with BTagEfficiencyAnalyzer.cc

class BTagSFEventWeight : public edm::EDProducer {

 public:
  explicit BTagSFEventWeight(const edm::ParameterSet&);
  ~BTagSFEventWeight();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  edm::InputTag jets_;
  std::string bTagAlgo_;
  std::string sysVar_;
  int verbose_;
  std::string filename_;
  
  /// histogram container
  /// for output
  std::map<std::string, TH1F*> hists_;
  /// efficiency histos as input
  std::map<std::string, TH1F*> effHists_;
  
  /// file with histos
  TFile * file_;

  
  double effBTag    (double, double);
  double effBTagSF  (double, double);
  double effBTagCjet(double, double);
  double effMisTag  (double, double);
  double effMisTagSF(double, double);
  double effBTagEvent(std::vector<double> &, std::vector<double> &);
  
};

#endif
