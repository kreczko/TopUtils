#include <stdio.h>
#include "TH1.h"
#include "TF1.h"
#include <vector>
#include <iostream>

vector<double> extract_sigma(TH1* theHisto)
{
  TF1 *f1 = new TF1("f1","gaus");
  theHisto->Fit("f1","LQ");

  double mean =  f1->GetParameter(1);
  double deviation = f1->GetParameter(2); 

  double lowLim; 
  double upLim;  

  double degrade = 0.1;
  unsigned int iteration = 0;

  vector<double> output;

  cout << "first fit quality is : " << f1->GetChisquare()/f1->GetNDF() << endl;

  while( iteration == 0 || (f1->GetChisquare()/f1->GetNDF() > 3. && iteration <= 5) )
    {
      lowLim = mean - (2.0*deviation*(1-degrade*iteration));
      upLim  = mean + (2.0*deviation*(1-degrade*iteration));
      //      cout << "mean is " << mean << " deviation is " << deviation << endl;
      //      cout << "lowLim/upLim = " << lowLim << "/" << upLim << endl;
      
      f1->SetRange(lowLim,upLim);
      theHisto->Fit("f1","RL");
      mean =  f1->GetParameter(1);
      deviation = f1->GetParameter(2);
      cout << "iteration " << iteration << endl;
      iteration++;
    }
  
  cout << "quality is Chi^2/NDF = " << f1->GetChisquare() << "/" << f1->GetNDF() << " = " <<  f1->GetChisquare()/f1->GetNDF() << endl;
  cout << "mean value: " << mean << "+- " << deviation << endl;
  output.push_back(f1->GetParameter(2));
  output.push_back(f1->GetParError(2));
  delete f1;
  return output;
}
