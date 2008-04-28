#ifndef FitMethods_h
#define FitMethods_h

#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "TopAnalysis/TopUtils/interface/RootSystem.h"
#include "TopAnalysis/TopUtils/interface/RootHistograms.h"

class StableFit {

 public:
  StableFit(const char*, int, double, double);
  ~StableFit(){ delete func_;};
  void fit(TH1F&);

 private:
  TF1* func_;
  double mean_;
  double sigma_;
  int funcType_;
  const char* funcName_;
  double lowerBound_;
  double upperBound_;
};

#endif
