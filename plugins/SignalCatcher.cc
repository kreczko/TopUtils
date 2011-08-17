// -*- C++ -*-
//
// Package:    SignalCatcher
// Class:      SignalCatcher
//
/**\class SignalCatcher SignalCatcher.cc TopAnalysis/TopUtils/plugins/SignalCatcher.cc

Description: catches SIGXCPU and SIGUSR1 and turns them into SIGINT
*/
//
// Original Author:  Benjamin Lutz,,,DESY
//         Created:  Wed Aug 17 13:29:00 CET 2011
// $Id: SignalCatcher.cc,v 1.1 2011/08/17 11:49:55 blutz Exp $
//
//


// system include files
#include <memory>
#include <csignal>

// user include files
//#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class SignalCatcher : public edm::EDFilter {
public:
  explicit SignalCatcher(const edm::ParameterSet&);
  ~SignalCatcher();

private:
  virtual void beginJob() ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  static void signalHandler(int sig);

  // ----------member data ---------------------------
  edm::RunNumber_t _minRunNumber;
  edm::RunNumber_t _maxRunNumber;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

void SignalCatcher::signalHandler(int sig) {

  edm::LogWarning(__FILE__) << ":" << __FUNCTION__ << ":" << __LINE__ << " "
                            << "received signal " << sig << " "
                            << "initiating program termination!";

  kill(getpid(),SIGINT);

}

//
// constructors and destructor
//
SignalCatcher::SignalCatcher(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed

  signal(SIGUSR1, &signalHandler); // USR1
  signal(SIGXCPU, &signalHandler); // XCPU

}


SignalCatcher::~SignalCatcher()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
SignalCatcher::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  return false;
}

// ------------ method called once each job just before starting event loop  ------------
void
SignalCatcher::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
SignalCatcher::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(SignalCatcher);
