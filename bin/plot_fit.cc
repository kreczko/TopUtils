#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "TopAnalysis/TopUtils/interface/RootSystem.h"
#include "TopAnalysis/TopUtils/interface/RootHistograms.h"
#include "TopAnalysis/TopUtils/interface/RootPostScript.h"
#include "TopAnalysis/TopUtils/interface/FitHist.h"
#include "TopAnalysis/TopUtils/bin/NiceStyle.cc"

using namespace std;

class TopInspectFit : public FitHist {
 public:
  TopInspectFit():FitHist(false){};
  ~TopInspectFit(){};
  virtual void readConfig( std::string );
};


void TopInspectFit::readConfig( std::string name )
{
  ConfigFile cfg( name, "=", "$" );  
  configBlockIO  ( cfg );
  configBlockHist( cfg );
  configBlockFit ( cfg );
}

int main(int argc, char* argv[])
{
  setNiceStyle();
  gStyle->SetOptStat( 0 );
  gStyle->SetOptFit ( 0 );

  gStyle->SetStatColor(0);
  gStyle->SetStatBorderSize(0);
  gStyle->SetStatX(0.93);
  gStyle->SetStatY(0.93);
  gStyle->SetStatW(0.18);
  gStyle->SetStatH(0.18);

  if( argc<2 ){
    cerr << "ERROR:" 
	 << " Missing argument" << endl;
    return 1;
  }

  TopInspectFit plots;
  try{
    plots.readConfig( argv[1] );
    plots.loadHistograms();

    //depending on style draw ps/eps/jpg
    if( !strcmp(plots.writeAs().c_str(), "ps") ){
      plots.fitAndDrawPs();
      plots.fillTargetHistograms();
      plots.drawPs();
    } else if( !strcmp(plots.writeAs().c_str(), "eps") ){
      plots.fitAndDrawEps();
      plots.fillTargetHistograms();
      plots.drawEps();
    } else{
      cerr << "ERROR:"
	   << " Unknown file format requested: "
	   << plots.writeAs() << endl; 
      return -1;
    }
    plots.writeFitOutput();
  }
  catch(char* str){
    cerr << "ERROR: " << str << endl;
    return 1;
  }
  catch(...){
    cerr << "ERROR: this one is new...";
      return 1;
  }
  cout << "works " << "thanx and GoodBye " << endl; 
  return 0;
}
