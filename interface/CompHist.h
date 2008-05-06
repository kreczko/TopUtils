#ifndef CompHist_h
#define CompHist_h

#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "TopAnalysis/TopUtils/interface/ConfigFile.h"
#include "TopAnalysis/TopUtils/interface/RootSystem.h"
#include "TopAnalysis/TopUtils/interface/RootHistograms.h"


namespace HistStyle{
  static const int Line   = 0;
  static const int Marker = 1;
  static const int Filled = 2; 
}

template <class T> 
void readVector(std::string s, std::vector<T>& vec)
{
  std::stringstream stream( s );
  T buffer;
  while (!stream.eof()) {
    stream >> buffer;
    if(!stream.str().empty()){
      vec.push_back( buffer );
    }
  }
}

class CompHist{
 public:
  enum RootOutput{ kFirstOnly, kLastOnly, kAll};
  enum HistFilter{ kBeginsWith, kEndsWith, kContains};

 public:
  CompHist():verbose_(false){};
  CompHist(bool verbose):verbose_(verbose){};
  //~CompHist(){ file_->Close(); };
  virtual ~CompHist(){};
  virtual void readConfig( std::string )=0;

  std::string writeAs(){return writeAs_;}
  void setVerbose(bool val) {verbose_=val; return;}
  bool verbose() {return verbose_;}
  void loadHistograms();
  void drawPs();
  void drawEps();
  void draw(TCanvas&, TLegend&, int&);
  bool histFilter(TString&);
  bool histFilter(TString&, CompHist::HistFilter);
  void writeOutput(CompHist::RootOutput);
  void writeOutput(){writeOutput(kAll);};

 protected:
  // io helpers
  void configBlockIO(ConfigFile&);
  void configBlockHist(ConfigFile&);
  void readLabels(std::string, std::vector<std::string>&);
  bool isOutputRequested(){return rootOutDir_.size()>1;};
  void readHistogramList();

  // save way to get members
  std::string legend(int);

  // histogram manipulators
  void setLegendStyle(TLegend&);
  void setCanvasStyle(TCanvas&);
  void setCanvLog(TCanvas&, int);
  void setCanvGrid(TCanvas&, int);
  void setAxesStyle(TH1&, const char*, const char*);
  void setHistStyles(TH1F&, int, int);
  void setHistLabels(TH1F&, int);
  void setHistScale(TH1F&, int);
  void setHistMax(TH1F&, int);
  void setHistMin(TH1F&, int);
  void setLineWidth (TH1F&, int);
  void setLineStyle(TH1F&, int);
  void setLineColor(TH1F&, int);
  void setFillStyle(TH1F&, int);
  void setFillColor(TH1F&, int);
  void setMarkerStyle(TH1F&, int);
  void setMarkerColor(TH1F&, int);
  void setMarkerSize(TH1F&, int);
  double findMaximum(int);

 protected:
  //---------------------------------------------
  // Interface
  //---------------------------------------------

  // define input/output
  std::string filterOpt_;                    // filter option for comparison
  std::string histFile_;                     // name of .hist file
  std::vector<std::string> dirNameList_;     // list of directory names in .root file(s)
  std::vector<std::string> fileNameList_;    // list of .root files
  std::vector<std::string> histFilterList_;  // list of filter patterns for histograms
  std::string output_;                       // potential root output
  std::string rootOutDir_;                   // root directory for potential root output
  std::string writeTo_, writeAs_;            // type (ps/eps) and directory of output
  std::vector<std::string> outputLabelList_; // list of .root files

  // define histogram design
  std::vector<short int> gridX_, gridY_;     // grid in x/y to be set?
  std::vector<short int> logX_,  logY_;      // log  in x/y to be set? 
  std::vector<short int> histStyle_;         // line, marker, filled?
  std::vector<short int> histColor_;         // line/fill color?
  std::vector<short int> commonStyle_;       // fill or line style
  std::vector<short int> commonWidth_;       // line width 
  std::vector<short int> markerStyle_;       // marker style
  std::vector<double> markerSize_;           // marker size
  std::vector<std::string> xAxes_, yAxes_;   // x/y axes labels
  std::vector<double> min_, max_;            // min/max of histogram (to be set by hand)
  std::vector<double> scale_;                // scale if wanted
  std::vector<short int> errors_;            // show histogram with errors?

  // define legend design
  double legXLeft_,  legXRight_;             // legend boundaries (common to all histograms)
  double legYLower_, legYUpper_;             // legend boundaries (common to all histograms)
  std::vector<std::string> legendEntries_;   // list of legend entries

  //---------------------------------------------
  // Internals
  //---------------------------------------------
  bool verbose_;
  std::vector<TFile*> fileList_;
  std::vector<TObjArray> sampleList_;
  std::vector<std::string> histList_;
};
#endif
