#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EventWeight.h"

EventWeight::EventWeight(const edm::ParameterSet& cfg):
  useWght_( cfg.getParameter<bool>("useWght") ),
  gridEff_( cfg.getParameter<double>("gridEff") ),
  weight_ ( cfg.getParameter<edm::InputTag>("weight") )
{
  produces<double>();
}

void
EventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  //---------------------------------------------
  // get event weight
  //---------------------------------------------
  double dummy=1.;
  edm::Handle<double> weight; 
  if(useWght_){
    evt.getByLabel(weight_, weight);
    dummy=*weight;
  }
  std::auto_ptr<double> wgt(new double);
  if(gridEff_<=0.)
    throw edm::Exception( edm::errors::Configuration,"Grid Efficiency is found to be <=0" );
  else{
    *wgt=1./gridEff_*dummy;
  }
  evt.put(wgt);
}
