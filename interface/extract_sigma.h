#include <stdio.h>
#include "TH1.h"
#include "TF1.h"
#include <vector>
#include <iostream>

vector<double> extract_sigma(TH1* theHisto, unsigned int verbose=0)
{
  int initialIgnoreLevel=gErrorIgnoreLevel;
  if(verbose==0) gErrorIgnoreLevel=kWarning;

  double lowLim = theHisto->GetMean()-theHisto->GetRMS(); 
  double upLim = theHisto->GetMean()+theHisto->GetRMS();  

  TF1 *f1 = new TF1("f1","gaus");
  f1->SetRange(lowLim,upLim);
  theHisto->Fit("f1","RLQ");

  double mean =  f1->GetParameter(1);
  double deviation = f1->GetParameter(2); 

  vector<double> output;

  lowLim = mean - (2.0*deviation);
  upLim  = mean + (2.0*deviation);

  f1->SetRange(lowLim,upLim);
  theHisto->Fit("f1","RLQ");
  mean =  f1->GetParameter(1);
  deviation = f1->GetParameter(2);

  //cout << "quality is Chi^2/NDF = " << f1->GetChisquare() << "/" << f1->GetNDF() << " = " <<  f1->GetChisquare()/f1->GetNDF() << endl;
  //cout << "mean value: " << mean << "+- " << deviation << endl;
  output.push_back(f1->GetParameter(2));
  output.push_back(f1->GetParError(2));
  delete f1;
  gErrorIgnoreLevel=initialIgnoreLevel;
  return output;
}
