#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightMC.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

EventWeightMC::EventWeightMC(const edm::ParameterSet& cfg)
{
  produces<double>();
}

void
EventWeightMC::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  //---------------------------------------------
  // get event weight
  //---------------------------------------------
  edm::Handle<GenEventInfoProduct> evt_info;
  evt.getByType(evt_info);
  weight_ = evt_info->weight();
  
  std::auto_ptr<double> eventWeight(new double);
  *eventWeight = weight_ ;

  evt.put(eventWeight);
}
