#ifndef BTagSFEventWeight_h
#define BTagSFEventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TString.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoBTag/Records/interface/BTagPerformanceRecord.h"
#include "RecoBTag/PerformanceDB/interface/BtagPerformance.h"
#include "CondFormats/PhysicsToolsObjects/interface/BinningPointByMap.h"
#include "FWCore/Framework/interface/ESHandle.h"

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
  std::string version_;
  std::string sysVar_;
  double shapeVarPtThreshold_;
  double shapeVarEtaThreshold_;
  double uncertaintySFb_;
  double shapeDistortionFactor_;
  int verbose_;
  edm::FileInPath filename_;
  double maxPtDB_;
  double maxPt11004_;
  double maxPt2012_;
  double maxPtMisTag_;
  double maxEta_;
  
  /// to load database
  std::map<std::string,PerformanceResult::ResultType> measureMap_;
  edm::ESHandle<BtagPerformance> perfHBTag;
  edm::ESHandle<BtagPerformance> perfHMisTag;
  
  /// histogram container
  /// for output
  std::map<std::string, TH1F*> hists_;
  /// efficiency histos as input
  //std::map<std::string, TH1F*> effHists_;
  std::map<std::string, TH2F*> effHists_;
  
  /// file with histos
  TFile * file_;

  double effBTagSF11004(double);
  double effBTagSFerr11004(double);
  double effMisTagSF11004(double, double, TString);
  double effBTagSF2012(double);
  double effBTagSFerr2012(double);
  double effMisTagSF2012(double, double, TString);
  double effBTag    (double, double);
  double effBTagSF  (double, double, bool);
  double effBTagCjet(double, double);
  double effMisTag  (double, double);
  double effMisTagSF(double, double);
  double effBTagEvent(std::vector<double> &, std::vector<double> &);
  
};

#endif
