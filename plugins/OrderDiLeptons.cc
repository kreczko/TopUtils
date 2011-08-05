// -*- C++ -*-
//
// Package:    OrderDiLeptons
// Class:      OrderDiLeptons
// 
/**\class OrderDiLeptons OrderDiLeptons.cc TopAnalysis/OrderDiLeptons/src/OrderDiLeptons.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler,,,DESY
//         Created:  Thu Jun 30 16:23:53 CEST 2011
// $Id: OrderDiLeptons.cc,v 1.1 2011/08/05 13:33:30 jkiesele Exp $
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

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "DataFormats/Candidate/interface/CompositeCandidate.h"
//
// class declaration
//

class OrderDiLeptons : public edm::EDProducer {
   public:
      explicit OrderDiLeptons(const edm::ParameterSet&);
      ~OrderDiLeptons();

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

  edm::InputTag firstsrc_, secondsrc_, thirdsrc_;

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
OrderDiLeptons::OrderDiLeptons(const edm::ParameterSet& iConfig):
  conf_(iConfig)
{

  firstsrc_       =iConfig.getParameter<edm::InputTag>    ( "firstsrc"            );
  secondsrc_      =iConfig.getParameter<edm::InputTag>    ( "secondsrc"           );
  thirdsrc_       =iConfig.getParameter<edm::InputTag>    ( "thirdsrc"            );

  produces<reco::CompositeCandidateCollection >();
  
}


OrderDiLeptons::~OrderDiLeptons()
{
 

}


// ------------ method called to produce the data  ------------
void
OrderDiLeptons::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

  edm::Handle<reco::CandidateView> firstDiLeptons;
  iEvent.getByLabel(firstsrc_, firstDiLeptons);

  edm::Handle<reco::CandidateView> secondDiLeptons;
  iEvent.getByLabel(secondsrc_, secondDiLeptons);

  edm::Handle<reco::CandidateView> thirdDiLeptons;
  iEvent.getByLabel(thirdsrc_, thirdDiLeptons);






  std::auto_ptr<std::vector<reco::CompositeCandidate> > outDiLeptons (new std::vector<reco::CompositeCandidate> );
  std::auto_ptr<std::vector<reco::CompositeCandidate> >  inDiLeptons (new std::vector<reco::CompositeCandidate> );



  /////////Fill all inputs in one vector

   for (reco::CandidateView::const_iterator adiLepton = firstDiLeptons->begin() ; adiLepton != firstDiLeptons->end(); adiLepton++)
    {
      inDiLeptons->push_back((reco::CompositeCandidate)*adiLepton);
    }

   for (reco::CandidateView::const_iterator adiLepton = secondDiLeptons->begin() ; adiLepton != secondDiLeptons->end(); adiLepton++)
    {
      inDiLeptons->push_back((reco::CompositeCandidate)*adiLepton);
    }

   for (reco::CandidateView::const_iterator adiLepton = thirdDiLeptons->begin() ; adiLepton != thirdDiLeptons->end(); adiLepton++)
    {
      inDiLeptons->push_back((reco::CompositeCandidate)*adiLepton);
    }



  bool higherpt;
  unsigned int size = inDiLeptons->size();
  int i;


   while(outDiLeptons->size() < size)
     {
       i=0;
       for (std::vector<reco::CompositeCandidate>::const_iterator adiLepton = inDiLeptons->begin() ; adiLepton != inDiLeptons->end(); adiLepton++)
 	{
	  for(std::vector<reco::CompositeCandidate>::const_iterator odiLepton = inDiLeptons->begin() ; odiLepton != inDiLeptons->end(); odiLepton++)
	    {
	      higherpt=true;
	      if(adiLepton->daughter(0)->pt() + adiLepton->daughter(1)->pt() < odiLepton->daughter(0)->pt() + odiLepton->daughter(1)->pt())
	    	{
		  higherpt=false;
		  break;
	       	}
	    }
	  if(higherpt)
	    {
	      outDiLeptons->push_back(*adiLepton);
	      inDiLeptons->erase(inDiLeptons->begin()+i);
	      break;
	    }
	  i++;
 	}
     }




   iEvent.put(outDiLeptons);

}

// ------------ method called once each job just before starting event loop  ------------
void 
OrderDiLeptons::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
OrderDiLeptons::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
OrderDiLeptons::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
OrderDiLeptons::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
OrderDiLeptons::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
OrderDiLeptons::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
OrderDiLeptons::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(OrderDiLeptons);
