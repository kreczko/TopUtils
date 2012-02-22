#include "FWCore/Utilities/interface/EDMException.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "TopAnalysis/TopUtils/plugins/ResonanceTagProbeProducer.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

ResonanceTagProbeProducer::ResonanceTagProbeProducer(const edm::ParameterSet& cfg):
  tags_  ( cfg.getParameter< edm::InputTag >("tags"    ) ),
  probes_( cfg.getParameter< edm::InputTag >( "probes" ) ),
  mass_  ( cfg.getParameter< double >( "mass"   ) ),
  deltaM_( cfg.getParameter< double >( "deltaM" ) ),
  leptonId_ ( cfg.getParameter< int >( "leptonId" ) )
{
  // produce pat::Muon collection
  //produces<std::vector<pat::Muon> >();
  if(leptonId_ == 11){
    produces<std::vector<pat::Electron> >();
    std::cout<<"Tag and Probe for electrons!"<<std::endl;
  }
  else if(leptonId_ == 13){
    produces<std::vector<pat::Muon> >();
    std::cout<<"Tag and Probe for muons!"<<std::endl;
  }
  else throw edm::Exception( edm::errors::Configuration,
			     "leptonId is not 11 (e) or 13 (mu)" );
  
  
  // load TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
                          "TFile Service is not registered in cfg file" );
  }
  hists_["taggedProbesCounterEv"]=fs->make<TH1F>("taggedProbesCounterEv", "taggedProbesCounterEv", 10,0.,10.);
  hists_["taggedProbesCounterTg"]=fs->make<TH1F>("taggedProbesCounterTg", "taggedProbesCounterTg", 10,0.,10.);
  hists_["MultTag"]=fs->make<TH1F>("MultTag", "MultTag", 10,0.,10.);
  hists_["MultGlobal"]=fs->make<TH1F>("MultGlobal", "MultGlobal", 10,0.,10.);
  hists_["lepLepMass"]=fs->make<TH1F>("lepLepMass", "lepLepMass", 40,0.,200.);
  hists_["lepLepCharge"]=fs->make<TH1F>("lepLepCharge", "lepLepCharge", 2,-1.5,1.5);
}

void
    ResonanceTagProbeProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  /// in the case of electrons
  if(leptonId_ == 11){
    // tag muon collection
    edm::Handle<edm::View<pat::Electron> > tags; 
    evt.getByLabel(tags_, tags);
    // probe muon collection
    edm::Handle<edm::View<pat::Electron> > probes; 
    evt.getByLabel(probes_, probes);
  
    // prepare vector of output vector
    std::auto_ptr<std::vector<pat::Electron> > taggedProbes(new std::vector<pat::Electron>);
    
    // count how many times per event a probe is tagged
    int taggedProbesCounterEv=0;
  
    // loop over tag muons
    for(edm::View<pat::Electron>::const_iterator tag=tags->begin(); tag!=tags->end(); ++tag){
      // count how many times per tagged muon a probe is tagged
      int taggedProbesCounterTg=0;
      // loop over probe muons
      for(edm::View<pat::Electron>::const_iterator probe=probes->begin(); probe!=probes->end(); ++probe){
	// check and skip overlaps
	if( probe->originalObjectRef() != tag->originalObjectRef() ){
	  // select as probe muon if in resonance mass range and of opposite charge
	  if((tag->charge()*probe->charge()) < 0){
	    if( fabs((tag->p4()+probe->p4()).mass()-mass_)<deltaM_){
	      taggedProbes->push_back(*probe);
	      taggedProbesCounterEv++;
	      taggedProbesCounterTg++;
	      }
	  hists_.find("lepLepMass")->second->Fill((tag->p4()+probe->p4()).mass()); //histo with di-muon mass
	  }
	  //histo with multiplied charge to check the ratio between same and opposite charge leptons
	  if( fabs((tag->p4()+probe->p4()).mass()-mass_)<deltaM_) hists_.find("lepLepCharge")->second->Fill(tag->charge()*probe->charge());
	}
      }
      hists_.find("taggedProbesCounterTg")->second->Fill(taggedProbesCounterTg);
    }
    hists_.find("taggedProbesCounterEv")->second->Fill(taggedProbesCounterEv);
    hists_.find("MultTag")->second->Fill(tags->size()); // multiplicity of tagged muon per event
    hists_.find("MultGlobal")->second->Fill(probes->size()); // multiplicity of global (i.e. probe cand) muon per event
    evt.put(taggedProbes);
  }
  
  /// in the case of muons
  else if(leptonId_ == 13){
    // tag muon collection
    edm::Handle<edm::View<pat::Muon> > tags; 
    evt.getByLabel(tags_, tags);
    // probe muon collection
    edm::Handle<edm::View<pat::Muon> > probes; 
    evt.getByLabel(probes_, probes);
  
    // prepare vector of output vector
    std::auto_ptr<std::vector<pat::Muon> > taggedProbes(new std::vector<pat::Muon>);
    
    // count how many times per event a probe is tagged
    int taggedProbesCounterEv=0;
  
    // loop over tag muons
    for(edm::View<pat::Muon>::const_iterator tag=tags->begin(); tag!=tags->end(); ++tag){
      // count how many times per tagged muon a probe is tagged
      int taggedProbesCounterTg=0;
      // loop over probe muons
      for(edm::View<pat::Muon>::const_iterator probe=probes->begin(); probe!=probes->end(); ++probe){
	// check and skip overlaps
	if( probe->originalObjectRef() != tag->originalObjectRef() ){
	  // select as probe muon if in resonance mass range and of opposite charge
	  if((tag->charge()*probe->charge()) < 0){
	    if( fabs((tag->p4()+probe->p4()).mass()-mass_)<deltaM_){
	      taggedProbes->push_back(*probe);
	      taggedProbesCounterEv++;
	      taggedProbesCounterTg++;
	    }
	    hists_.find("lepLepMass")->second->Fill((tag->p4()+probe->p4()).mass()); //histo with di-muon mass
	  }
	  //histo with multiplied charge to check the ratio between same and opposite charge leptons
	  if( fabs((tag->p4()+probe->p4()).mass()-mass_)<deltaM_) hists_.find("lepLepCharge")->second->Fill(tag->charge()*probe->charge());
	}
      }
      hists_.find("taggedProbesCounterTg")->second->Fill(taggedProbesCounterTg);
    }
    hists_.find("taggedProbesCounterEv")->second->Fill(taggedProbesCounterEv);
    hists_.find("MultTag")->second->Fill(tags->size()); // multiplicity of tagged muon per event
    hists_.find("MultGlobal")->second->Fill(probes->size()); // multiplicity of global (i.e. probe cand) muon per event
    evt.put(taggedProbes);
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ResonanceTagProbeProducer );
