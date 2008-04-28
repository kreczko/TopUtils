#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "TopAnalysis/TopUtils/interface/FitMethods.h"

using std::cerr;
using std::cout;
using std::endl;

double Gauss(double *x, double *par){
  double arg=0;
  if(par[2]!=0){
    arg = (x[0]-par[1])/par[2];
  }
  return par[0]*TMath::Exp(-0.5*arg*arg);
}

StableFit::StableFit(const char* funcName, int funcType, double lowerBound, double upperBound):
  funcType_  (funcType  ),
  funcName_  (funcName  ),
  lowerBound_(lowerBound),
  upperBound_(upperBound)
{
  if(funcType_==0){
    func_ = new TF1(funcName_, Gauss, lowerBound_, upperBound_, 3);  
    func_->SetParNames( "Const", "Mean", "Sigma" );
  }
  else{
    cout << "Sorry: not yet implemented" << endl;
  }
}

void
StableFit::fit(TH1F& hfit)
{
  //set start values for first iteration
  if(funcType_==0){
    double maxValue=hfit.GetBinCenter(hfit.GetMaximumBin());
    func_->SetParameter(1, maxValue);
    func_->SetParameter(2, hfit.GetRMS());
  }

  //set parameter limits
  if(funcType_==0){
    func_->SetParLimits(1, lowerBound_, upperBound_);
    func_->SetParLimits(2, 0., 5.*hfit.GetRMS());
  }

  //do the fit
  mean_ = func_->GetParameter(1);
  sigma_= func_->GetParameter(2);

  hfit.Fit( "func", "RE0", "", (mean_-2.*sigma_), (mean_+2.*sigma_) );
  if(hfit.GetFunction("func")){
    //get mean and sigma 
    //from first iteration
    mean_ = hfit.GetFunction("func")->GetParameter(1);
    sigma_= hfit.GetFunction("func")->GetParameter(2);

    //set start values for 
    //second iteration
    func_->SetParameter(1, mean_ );
    func_->SetParameter(2, sigma_);
    hfit.Fit( func_, "MEL", "", (mean_-1.5*sigma_), (mean_+1.5*sigma_) );
  }
  else{
    std::cout << "sorry... no fit function found..." << std::endl;
  }
}
