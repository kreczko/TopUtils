#include "FWCore/Utilities/interface/EDMException.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "TopAnalysis/TopUtils/plugins/ResonanceTagProbeProducer.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

ResonanceTagProbeProducer::ResonanceTagProbeProducer(const edm::ParameterSet& cfg):
  tags_  ( cfg.getParameter< edm::InputTag >("tags"    ) ),
  probes_( cfg.getParameter< edm::InputTag >( "probes" ) ),
  mass_  ( cfg.getParameter< double >( "mass"   ) ),
  deltaM_( cfg.getParameter< double >( "deltaM" ) )
{
  produces<std::vector<pat::Muon> >();
}

void
ResonanceTagProbeProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // tag muon collection
  edm::Handle<edm::View<pat::Muon> > tags; 
  evt.getByLabel(tags_, tags);
  // probe muon collection
  edm::Handle<edm::View<pat::Muon> > probes; 
  evt.getByLabel(probes_, probes);
 
  // prepare vector of output vector
  std::auto_ptr<std::vector<pat::Muon> > taggedProbes(new std::vector<pat::Muon>);

  // loop over muon references for the tag muon
  for(edm::View<pat::Muon>::const_iterator tag=tags->begin(); tag!=tags->end(); ++tag){
    // loop over muon references for the probe muon
    for(edm::View<pat::Muon>::const_iterator probe=probes->begin(); probe!=probes->end(); ++probe){
      // check and skip overlaps
      if( probe->originalObjectRef() != tag->originalObjectRef() ){
	if( ((tag->p4()+probe->p4()).mass()-mass_)<deltaM_){
	  taggedProbes->push_back(*probe);
	}
      }
    }
  }
  evt.put(taggedProbes);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ResonanceTagProbeProducer );
