#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightFromXSec.h"

EventWeightFromXSec::EventWeightFromXSec(const edm::ParameterSet& cfg):
  eff_( cfg.getParameter<double>("eff") ),
  xsec_( cfg.getParameter<double>("xsec") ),
  lumi_( cfg.getParameter<double>("lumi") ),
  nevts_( cfg.getParameter<unsigned>("nevts") )
{
  produces<double>("eventWeight");
}

EventWeightFromXSec::~EventWeightFromXSec()
{
}

void
EventWeightFromXSec::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  std::auto_ptr<double> eventWeight(new double);  
  wght_=lumi_*xsec_*eff_/nevts_;
  *eventWeight = wght_;    
  evt.put(eventWeight, "eventWeight");  
}
