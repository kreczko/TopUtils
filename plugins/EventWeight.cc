#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EventWeight.h"

EventWeight::EventWeight(const edm::ParameterSet& cfg):
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
  edm::Handle<double> weight; 
  evt.getByLabel(weight_, weight);
  
  std::auto_ptr<double> wgt(new double);
  if(gridEff_<=0.)
    throw edm::Exception( edm::errors::Configuration,"Grid Efficiency is found to be <=0" );
  else{
    *wgt=1./gridEff_*(*weight);
  }
  evt.put(wgt);
}
