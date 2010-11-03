#ifndef EventListing_h
#define EventListing_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/**
   \class   EventListing EventListing.cc "TopAnalysis/TopUtils/plugins/EventListing.cc"

   \brief   EDAnalyzer which only prints a list of selected events at the end of a cmsRun job 

   EDAnalyzer which only prints a list of selected events at the end of a cmsRun job.
   Just put it somewhere in your path at a certain selection step and you will get a list of all
   event which survive this selection.
*/

class EventListing : public edm::EDAnalyzer {

 public:
  explicit EventListing(const edm::ParameterSet&);
  ~EventListing(){};
  
 private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  std::vector<int> evtlist;
};

EventListing::EventListing(const edm::ParameterSet& cfg)
{      
}

void
EventListing::analyze(const edm::Event& evt, const edm::EventSetup& setup)
{  
  evtlist.push_back(evt.id().run());
  evtlist.push_back(evt.id().luminosityBlock());
  evtlist.push_back(evt.id().event());
}

void
EventListing::endJob()
{
  std::cout << "SELECTED EVENTS" << std::endl;
  std::cout << "Run\t Lumi\t Event" << std::endl;
  for(size_t i=0;i<evtlist.size();++i){
    std::cout << evtlist[i];
    if((i+1)%3==0) std::cout << std::endl;
    else std::cout << "\t";
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EventListing );

#endif
