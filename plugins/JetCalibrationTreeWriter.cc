// -*- C++ -*-
//
// Package:    JetCalibrationTreeWriter
// Class:      JetCalibrationTreeWriter
// 
/**\class JetCalibrationTreeWriter JetCalibrationTreeWriter.cc TOPunbinnedFit/JetCalibrationTreeWriter/src/JetCalibrationTreeWriter.cc
*/
//
// Original Author:  Holger Enderle,68/111,4719,
//         Created:  Mon Jan 26 16:29:19 CET 2009
// $Id: JetCalibrationTreeWriter.cc,v 1.2 2009/08/13 08:54:10 snaumann Exp $
//
//

#include "JetCalibrationTreeWriter.h"

//
// class declaration
//

class JetCalibrationTreeWriter : public edm::EDAnalyzer {
   public:
      explicit JetCalibrationTreeWriter(const edm::ParameterSet&);
      ~JetCalibrationTreeWriter();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------

  edm::InputTag jetLabel_;
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
JetCalibrationTreeWriter::JetCalibrationTreeWriter(const edm::ParameterSet& iConfig):
  jetLabel_(iConfig.getUntrackedParameter<edm::InputTag>("jetTag"))
{
   //now do what ever initialization is needed

   tree = fs->make<TTree>("JetCalibrationtree","JetCalibrationtree");
   treeMemPtr = new JetCalibrationVariables;

   tree->Branch("JetCalibrationVariables","JetCalibrationVariables", &treeMemPtr); // address of pointer!
}


JetCalibrationTreeWriter::~JetCalibrationTreeWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
JetCalibrationTreeWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle<edm::View<pat::Jet> > jetHandle;
   iEvent.getByLabel(jetLabel_, jetHandle);
   edm::View<pat::Jet> jets = *jetHandle;
   TRandom *sRandom(new TRandom3(0));
   double toyrecenergy = 0.;

   for(edm::View<pat::Jet>::const_iterator jet_iter = jets.begin(); jet_iter!=jets.end(); ++jet_iter){
     if(jet_iter->genJet())
     if(jet_iter->genJet()->et()>=10. && TMath::Abs(jet_iter->eta())<3.0){

       if(TMath::Abs(jet_iter->genJet()->eta())<=1.4){
	 toyrecenergy = sRandom->Gaus(jet_iter->genJet()->et(),0.05*jet_iter->genJet()->et()+sqrt(jet_iter->genJet()->et())+0.5*jet_iter->eta()*jet_iter->eta());
       }
       else if(TMath::Abs(jet_iter->genJet()->eta())>1.4){
	 toyrecenergy = sRandom->Gaus(jet_iter->genJet()->et(),0.01*jet_iter->genJet()->et()+2.*sqrt(jet_iter->genJet()->et())-0.5*jet_iter->eta()*sqrt(jet_iter->genJet()->et()));
       }
       treeMemPtr->fillVariables(jet_iter->genJet()->energy(), jet_iter->genJet()->et(), jet_iter->genJet()->pt(), jet_iter->genJet()->eta(), jet_iter->genJet()->phi(), jet_iter->energy(), jet_iter->et(), jet_iter->pt(), jet_iter->eta(), jet_iter->phi(), jet_iter->pt()/jet_iter->genJet()->pt(), jet_iter->partonFlavour(), toyrecenergy, jet_iter->emEnergyFraction());

       tree->Fill();
     }
   }

// #ifdef THIS_IS_AN_EVENT_EXAMPLE
//    Handle<ExampleData> pIn;
//    iEvent.getByLabel("example",pIn);
// #endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif

}


// ------------ method called once each job just before starting event loop  ------------
void 
JetCalibrationTreeWriter::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
JetCalibrationTreeWriter::endJob() {

  delete treeMemPtr; treeMemPtr = 0;

}

//define this as a plug-in
DEFINE_FWK_MODULE(JetCalibrationTreeWriter);
