#ifndef BaseMacro_h
#define BaseMacro_h

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>

#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

#include <TAxis.h>
#include <TAttLine.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TPostScript.h>

#include "FWCore/ParameterSet/interface/ParameterSet.h"


/**
   \class   BaseMacro BaseMacro.h "TopAnalysis/TopUtils/interface/BaseMacro.h"

   \brief   Basic macro to provide a skeleton and some base functionality for analysis macros

   Basic macro to provide a skeleton and some base functionality for analysis macros. Foreseen
   functionalities are o[pen and close files and histogram handlig. 
*/


class BaseMacro {
 public:
  /// this descibes a set of input files, corresponding labels and scaling factors to be used; 
  /// the expected structure is:
  /// + input file
  /// + sample description (also used for legend)
  /// + luminosity normalization for MC (for data this should be 1.)
  typedef std::vector< std::pair< TFile*, std::pair<std::string, float> > > SampleCollection;
  /// this describes a map of histograms, corresponding to a histogram name and a std::vector 
  /// of TH1*; for each sample one TH1* is expected in the vector
  typedef std::map<std::string, std::vector<TH1*> > HistMap;
  
 public:
  /// default constructor; opens a set of files corresponding to the input samples, loads a set of a set
  /// histograms of interest and applies normalization to these histograms; the normalization can be set 
  /// to -1. for this step to have no effect
  BaseMacro(const edm::ParameterSet& cfg);
  /// default destructor
  ~BaseMacro();

 protected:
  /// save a set of Canvases to a single ps file of name fileName; it is foreseen that cnavs is a memeber 
  /// of a derived class, which is passed on to this function
  void save(const std::vector<TCanvas*>& canvs, std::string& fileName) const;
  /// save a set of histograms to a root file of name fileName; it is foreseen that hists is a member of 
  /// a derived class, which is passed on to this function
  void save(const std::vector<TH1*>& hists, const std::string& fileName) const;
  /// save single canvas to a file of type fileType; the name of the file will be the name of the Canvas;
  /// it is foreseen that canv is either an element of a std::vector<TCanvas*>, which is a member of a
  /// derived class or that it is a member of the derived class itself 
  void save(const TCanvas& canv, const std::string& fileType) const;

 protected:
  /// collection of input files
  SampleCollection samples_;
  /// map for histogram management
  HistMap hists_;
};

#endif
