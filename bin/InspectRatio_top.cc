#include <memory>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>

#include "TopAnalysis/TopUtils/interface/RootSystem.h"
#include "TopAnalysis/TopUtils/interface/RootHistograms.h"
#include "TopAnalysis/TopUtils/interface/RootPostScript.h"
#include "TopAnalysis/TopUtils/interface/ManipHist.h"
#include "TopAnalysis/TopUtils/bin/NiceStyle.cc"

using namespace std;

class TopInspectRatio : public ManipHist {
 public:
  TopInspectRatio(){};
  ~TopInspectRatio(){};
  virtual void readConfig( std::string );
  void writeOutput(){CompHist::writeOutput(kFirstOnly);};
};

void TopInspectRatio::readConfig( std::string name )
{
  ConfigFile cfg( name, "=", "$" );  
  configBlockIO    ( cfg );
  configBlockHist  ( cfg );
  configBlockDivide( cfg );
}

int main(int argc, char* argv[])
{
  setNiceStyle();
  gStyle->SetOptStat( 0 );

  if( argc<2 ){
    cerr << "ERROR:" 
	 << " Missing argument" << endl;
    return 1;
  }

  TopInspectRatio plots;
  try{
    plots.readConfig( argv[1] );
    plots.loadHistograms();
    
    //depending on style draw ps/eps/jpg
    if( !strcmp(plots.writeAs().c_str(), "ps") ){
      plots.divideAndDrawPs();
    } else if( !strcmp(plots.writeAs().c_str(), "eps") ){
      plots.divideAndDrawEps();
    } else{
      cerr << "ERROR:"
	   << " Unknown file format requested: "
	   << plots.writeAs() << endl; 
      return -1;
    }
    plots.writeOutput();
  }
  catch(char* str){
    cerr << "ERROR: " << str << endl;
    return 1;
  }
  catch(...){
    cerr << "ERROR: this one is new...";
      return 1;
  }
  cout << "Thanx and GoodBye " << endl;
  return 0;
}
