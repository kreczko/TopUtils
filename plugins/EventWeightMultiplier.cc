#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TopAnalysis/TopUtils/plugins/EventWeightMultiplier.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"



EventWeightMultiplier::EventWeightMultiplier(const edm::ParameterSet& cfg):
  eventWeightTags_  ( cfg.getParameter< std::vector<edm::InputTag> > ( "eventWeightTags") ),
  verbose_          ( cfg.getParameter<int>                          ( "verbose" ) )
{
  // additionalFactor is an optional parameter:
  additionalFactor_ = 1.;
  if(cfg.exists("additionalFactor")) additionalFactor_ = cfg.getParameter<double> ("additionalFactor");
  
  produces<double>();
  
  /// laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for b tag eff SF
  hists_["eventWeight"]     = fs->make<TH1F>( "eventWeight"    , "eventWeight"    , 100, 0, 10 );
  hists_["eventWeightMean"] = fs->make<TH1F>( "eventWeightMean", "eventWeightMean", 1  , 0,  1 );
  
}

EventWeightMultiplier::~EventWeightMultiplier()
{
}

void
EventWeightMultiplier::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  double finalWeight = 1.;
  
  // loop over all eventWeightTags and multiply weights
  for(unsigned iWeight=0; iWeight < eventWeightTags_.size(); iWeight++){
    
    // get weight from the CMSSW event
    edm::Handle<double> wgt;
    evt.getByLabel(eventWeightTags_[iWeight], wgt);
    // ignore non existing weights
    if(wgt.isValid()){
      if(verbose_>=1) std::cout<<"  eventWeight "<<iWeight<<": "<<*wgt<<std::flush;
      // multiply finalWeight with this weight
      finalWeight *= *wgt;
    }
    else{
      edm::LogInfo("weightNotFound") << "eventWeight " << iWeight << " not found";
    }
  }
  // if additional factor exists, multply with it
  if(additionalFactor_!=1){
    if(verbose_>=1) std::cout<<"; additionalFactor: "<<additionalFactor_<<std::flush;
    finalWeight *= additionalFactor_;
  }
  
  if(verbose_>=1) std::cout<<"; finalWeight= "<<finalWeight<<std::endl;
   
  hists_.find("eventWeight" )->second->Fill( finalWeight );

  std::auto_ptr<double> result(new double);
  *result = finalWeight;
  evt.put(result);
}

/// executed at the end after looping over all events
void 
    EventWeightMultiplier::endJob() 
{
  double eventWeightMean = hists_.find("eventWeight" )->second->GetMean();
  hists_.find("eventWeightMean" )->second->Fill(0.5, eventWeightMean );
  if(verbose_>=1) std::cout<<"Mean eventWeight = "<<eventWeightMean<<std::endl;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EventWeightMultiplier );

