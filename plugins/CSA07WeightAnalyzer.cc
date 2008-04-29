#include "TopAnalysis/TopUtils/plugins/CSA07WeightAnalyzer.h"

#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"


CSA07WeightAnalyzer::CSA07WeightAnalyzer(const edm::ParameterSet& cfg):
  hist_ ( cfg.getParameter<std::string>( "hist" ) )
{
}

void
CSA07WeightAnalyzer::analyze(const edm::Event& evt, const edm::EventSetup& setup)
{
  edm::Handle<double> weight; 
  evt.getByLabel("weight", weight);

  edm::Handle<double> gridEff; 
  evt.getByLabel("gridEff", gridEff);

  edm::Handle<int> procID;
  evt.getByLabel( "genEventProcID", procID );
  
  edm::Handle<double> scale;
  evt.getByLabel( "genEventScale", scale );

  evtWeight_->Fill( *weight );
  evtScale_ ->Fill( *scale  );
  evtWghtVsScale_->Fill( *scale, (1./(*gridEff)*(*weight)) );

  if( *procID != 4){//Pythia Events
    edm::Handle<double> filterEff;
    evt.getByLabel( "genEventRunInfo", "FilterEfficiency", filterEff );
    
    edm::Handle<double> autoXSec;
    evt.getByLabel( "genEventRunInfo", "AutoCrossSection", autoXSec); 

    edm::Handle<double> preXSec;
    evt.getByLabel( "genEventRunInfo", "PreCalculatedCrossSection", preXSec); 

    evtPreXSec_  ->Fill( *preXSec   );
    evtAutoXSec_ ->Fill( *autoXSec  );
    evtFilterEff_->Fill( *filterEff );
  } 
}

void 
CSA07WeightAnalyzer::beginJob(const edm::EventSetup&)
{
  if( hist_.empty() )
    return;

  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  
  NameScheme wgt("wgt");
  ofstream hist(hist_.c_str(), std::ios::out);

  evtWeight_     = fs->make<TH1F>(wgt.name(hist, "wgt"   ), wgt.name("wgt"  ), 1000000, 0.,  1E6);
  evtScale_      = fs->make<TH1F>(wgt.name(hist, "scale" ), wgt.name("scale"),     500, 0., 5000);
  evtPreXSec_    = fs->make<TH1F>(wgt.name(hist, "prexs" ), wgt.name("prexs"),    1100, 0., 1E11);
  evtAutoXSec_   = fs->make<TH1F>(wgt.name(hist, "autxs" ), wgt.name("autxs"),    1100, 0., 1E11);
  evtFilterEff_  = fs->make<TH1F>(wgt.name(hist, "eff"   ), wgt.name("eff"  ),     100, 0.,   1.);
  evtWghtVsScale_= fs->make<TH1F>(wgt.name(hist, "wghtVsScale" ), wgt.name("wgt"), 500, 0., 5000);
}

void 
CSA07WeightAnalyzer::endJob() 
{
}
