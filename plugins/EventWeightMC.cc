#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightMC.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

EventWeightMC::EventWeightMC(const edm::ParameterSet& cfg)
{  
  edm::Service<TFileService> fs;
  if( !fs ) throw edm::Exception( edm::errors::Configuration, "TFile Service is not registered in cfg file" );
  hists_["eventWeightMC"]     = fs->make<TH1F>( "eventWeightMC", "eventWeightMC", 600, -300., 300. );
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
  hists_.find("eventWeightMC")->second->Fill(weight_);
  std::auto_ptr<double> eventWeight(new double);
  *eventWeight = weight_ ;

  evt.put(eventWeight);
}
