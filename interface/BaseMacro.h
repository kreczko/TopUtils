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
   functionalities are open and close files and histogram handlig. We make use of the edm::PSet
   for configure our macros. The expected basic configuration is expected to contain the 
   following parameters (of fwlite configuration style):

   import FWCore.ParameterSet.Config as cms
   process = cms.Process("Plots")

   process.BasicMacro = cms.PSet(
    ## luminosity in pb-1
    lumi = cms.double(10)
    ## vector of input samples
    inputs = cms.VPSet(
      ## input file
      file  = cms.string('file:Sample1.root'),
      ## label in legend
      label = cms.string('legend entry'),
      ## normalization scale (expected to be to 1pb)
      scale = cms.double(0.123)
    ),
    ## histograms of interest
    hists = cms.vstring('fullpath/tohist',)

    This block may be imported from a central file and partiially replaced. In the BaseMacro class 
    it is foreseen that histograms of interest may be written inpto a single ps file (for overview 
    files), into single picture files (like .png -- nor validity check is applied yet) or into a
    new root file for further processing. It is expected that the (vectors of) Canvases and histo-
    grams are members of small derived classes with not more than 150 lines of code. They should 
    serve simple functionalities. Examples are given below:

    + plot a certain (set of) histogram(s) (comaprison of data and MC/theory).
    + read a certain (set of) histogram(s) and print a readable output to file/shell (e.g. a table 
      of cross sections).
    + take a (set of) histogram(s) from different inputs divide them (to determine an efficiency) 
      and write the result into a new file.
    + take a (set of) histogram(s) from different inputs calculate a cross section from it and 
      write the result into a new file.
    + take a set of pre-evaluated systematic variations, which apply on an efficiency, combine them 
      binwise and write the result into a new file.
    + take a (set of) cross section histogram(s), apply bin center corrections and write the result 
      into a new file.
    + take a (set of) cross section histogram(s), add systematic uncertainties to it and write the 
      result inot a new file.

    It is recommended to divide each processing on the way to a cross section in as small portions as
    possible and to make the result of this step persistent in a new output file. You may thus apply 
    very localized changes and redo the calculations without doing everything from scratch over and 
    over again. It is further on recommended to have a (set of) script(s) at hand to run the basic 
    processes on a routine basis. It might be useful to keep the calculation of a central cross sec-
    tion value separated from the full maschienery for the evaluation of systematic uncertainties.
)

*/


class BaseMacro {
 public:
  /// this descibes a set of input files, corresponding labels and scaling factors to be used; the 
  /// expected structure is:
  /// + sample description (also used for legend)
  /// + input file
  /// + luminosity normalization for MC (for data this should be 1.)
  typedef std::vector< std::pair< std::string, std::pair<TFile*, float> > > InputCollection;
  /// this describes a map of histograms, corresponding to a histogram name and a std::pair of TH1*
  /// corresponding to each input sample; the std::stringthe the  corresponding TH1* is expected to be 
  /// the sample description string of the InputCollection
  typedef std::map<std::string, std::map<std::string, TH1*> > HistMap;
  
 public:
  /// default constructor; opens a set of files corresponding to the input samples, loads a set of a set
  /// histograms of interest and applies normalization to these histograms; the normalization can be set 
  /// to -1. for this step to have no effect
  BaseMacro(const edm::ParameterSet& cfg);
  /// default destructor
  ~BaseMacro(){};
 protected:
  /// save a set of Canvases to a single ps file of name fileName; it is foreseen that cnavs is a memeber 
  /// of a derived class, which is passed on to this function
  void save(const std::vector<TCanvas*>& canvs, std::string fileName) const;
  /// save a set of histograms to a root file of name fileName; it is foreseen that hists is a member of 
  /// a derived class, which is passed on to this function
  void save(const std::vector<TH1*>& hists, const std::string& fileName) const;
  /// save single canvas to a file of type fileType; the name of the file will be the name of the Canvas;
  /// it is foreseen that canv is either an element of a std::vector<TCanvas*>, which is a member of a
  /// derived class or that it is a member of the derived class itself 
  void save(const TCanvas& canv, const std::string& fileType) const;

 protected:
  /// collection of input files
  InputCollection inputs_;
  /// map for histogram management
  HistMap hists_;
  /// luminosity
  double lumi_;
  /// verbosiry level
  bool verbose_;

};

#endif
