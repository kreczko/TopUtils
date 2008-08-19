#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/interface/EvtWeightFromSigmaProducer.h"

EvtWeightFromSigmaProducer::EvtWeightFromSigmaProducer(const edm::ParameterSet& cfg):
  crossSection_( cfg.getParameter<double>("crossSection" ) ),
  nEvents_     ( cfg.getParameter<int>   ("nEvents"      ) ), 
  targetLumi_  ( cfg.getParameter<double>("targetLumi"   ) )    
{
  produces<double>("eventWeight");
}

EvtWeightFromSigmaProducer::~EvtWeightFromSigmaProducer()
{
}

void
EvtWeightFromSigmaProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  std::auto_ptr<double> eventWeight(new double);  
  prodLumi = nEvents_/crossSection_;
  *eventWeight = targetLumi_/prodLumi;    
 
  std::cout << "Weight = " << *eventWeight << std::endl;
      
  evt.put(eventWeight, "eventWeight");  
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_ANOTHER_FWK_MODULE( EvtWeightFromSigmaProducer );
