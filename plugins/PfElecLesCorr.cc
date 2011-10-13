// -*- C++ -*-
//
// Package:    PfElecLesCorr
// Class:      PfElecLesCorr
// 
/**\class PfElecLesCorr PfElecLesCorr.cc TopAnalysis/PfElecLesCorr/src/PfElecLesCorr.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Fri Sep 30 11:19:47 CEST 2011
// $Id: PfElecLesCorr.cc,v 1.1 2011/10/13 09:28:08 jkiesele Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

//
// class declaration
//

class PfElecLesCorr : public edm::EDProducer {
   public:
      explicit PfElecLesCorr(const edm::ParameterSet&);
      ~PfElecLesCorr();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual void beginRun(edm::Run&, edm::EventSetup const&);
      virtual void endRun(edm::Run&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

  // ----------member data ---------------------------
  edm::ParameterSet conf_;
  edm::InputTag src_;
  double SFBarrel_, SFEC_, eta_;

};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
PfElecLesCorr::PfElecLesCorr(const edm::ParameterSet& iConfig):
  conf_(iConfig)
{

  src_           =iConfig.getParameter<edm::InputTag>    ( "src" );
  SFBarrel_    =iConfig.getParameter<double>             ( "SFBarrel" );
  SFEC_        =iConfig.getParameter<double>             ( "SFEC" );
  eta_           =iConfig.getParameter<double>           ( "eta" );

  produces<std::vector<reco::PFCandidate> >();
  
}


PfElecLesCorr::~PfElecLesCorr()
{

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PfElecLesCorr::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;


  Handle<std::vector<reco::PFCandidate> > cands;
  iEvent.getByLabel(src_,cands);



  int i=0;

  std::auto_ptr<std::vector<reco::PFCandidate> > outCands (new std::vector<reco::PFCandidate> );

  for (std::vector<reco::PFCandidate>::const_iterator cand=cands->begin(); cand<cands->end(); cand++){

    double rescaler=1;
    outCands->push_back((reco::PFCandidate)*cand);

    if(cand->eta()<eta_) rescaler=SFBarrel_;
    else                 rescaler=SFEC_;

    (outCands->begin()+i)->rescaleMomentum(rescaler);
    i++;


  }
 
  iEvent.put(outCands);

}

// ------------ method called once each job just before starting event loop  ------------
void 
PfElecLesCorr::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PfElecLesCorr::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
PfElecLesCorr::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
PfElecLesCorr::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
PfElecLesCorr::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
PfElecLesCorr::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PfElecLesCorr::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PfElecLesCorr);
