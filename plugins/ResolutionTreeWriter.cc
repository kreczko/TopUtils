// -*- C++ -*-
//
// Package:    ResolutionTreeWriter
// Class:      ResolutionTreeWriter
// 
/**\class ResolutionTreeWriter ResolutionTreeWriter.cc TOPunbinnedFit/ResolutionTreeWriter/src/ResolutionTreeWriter.cc
*/
//
// Original Author:  Holger Enderle,68/111,4719,
//         Created:  Mon Jan 26 16:29:19 CET 2009
// $Id: ResolutionTreeWriter.cc,v 1.7 2012/05/11 15:41:47 henderle Exp $
//
//

#include "ResolutionTreeWriter.h"

//
// class decleration
//

class ResolutionTreeWriter : public edm::EDAnalyzer {
   public:
      explicit ResolutionTreeWriter(const edm::ParameterSet&);
      ~ResolutionTreeWriter();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------

  edm::InputTag jetLabel_;
  edm::InputTag electronLabel_;
  edm::InputTag muonLabel_;
  edm::InputTag METLabel_;
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
ResolutionTreeWriter::ResolutionTreeWriter(const edm::ParameterSet& iConfig):
  jetLabel_(iConfig.getUntrackedParameter<edm::InputTag>("jetTag")),
  electronLabel_(iConfig.getUntrackedParameter<edm::InputTag>("electronTag")),
  muonLabel_(iConfig.getUntrackedParameter<edm::InputTag>("muonTag")),
  METLabel_(iConfig.getUntrackedParameter<edm::InputTag>("METTag"))
{
  //now do what ever initialization is needed
  if(iConfig.exists("weightTags" )) weightTags_= iConfig.getParameter< std::vector<edm::InputTag> >("weightTags" );
  
  tree = fs->make<TTree>("ResolutionTree","ResolutionTree");
  treeMemPtr = new ResolutionVariables;
  
  tree->Branch("ResolutionVariables","ResolutionVariables", &treeMemPtr); // address of pointer!
}


ResolutionTreeWriter::~ResolutionTreeWriter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
ResolutionTreeWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle<edm::View<pat::Jet> > jetHandle;
   iEvent.getByLabel(jetLabel_, jetHandle);
   edm::View<pat::Jet> jets = *jetHandle;

   edm::Handle<edm::View<pat::Electron> > eleHandle;
   iEvent.getByLabel(electronLabel_, eleHandle);
   edm::View<pat::Electron> electrons = *eleHandle;

   edm::Handle<edm::View<pat::Muon> > muoHandle;
   iEvent.getByLabel(muonLabel_, muoHandle);
   edm::View<pat::Muon> muons = *muoHandle;

   edm::Handle<edm::View<pat::MET> > METHandle;
   iEvent.getByLabel(METLabel_, METHandle);
   edm::View<pat::MET> METs = *METHandle;

   // prepare the event weights
   std::vector<double> weights;
   // get weight from the CMSSW event
   // loop over all eventWeightTags (only 3 can be used at the moment)
   //for(unsigned iWeight=0; iWeight < weightTags_.size(); iWeight++){
   for(unsigned iWeight=0; iWeight < 3; iWeight++){
     if(iWeight < weightTags_.size()){
       // get weight by label
       edm::Handle<double> wgt;
       iEvent.getByLabel(weightTags_[iWeight], wgt);
       // check weights
       if(wgt.isValid())weights.push_back(*wgt);
       else{
	 weights.push_back(0.);
	 std::cout<< "eventWeight " << iWeight << " not found"<<std::endl;
	 edm::LogInfo("weightNotFound") << "eventWeight " << iWeight << " not found";//should probably be an error
       }
     }
     else{
       if(iWeight==0){
	 // if no weight is given, simply set it to 1.
	 weights.push_back(1.);
	 edm::LogInfo("weightNotGiven") << "no eventWeight given, setting to 1.";
       }
       else{
	 // if no systematic weights are given, simply set them to 0.
	 weights.push_back(0.);
	 edm::LogInfo("weightNotGiven") << "no systematic eventWeight given, setting to 0.";
       }
     }
   }

   float deltaR = 0.;
   float deltaPhi = 0.;
   float deltaEta = 0.;
   float nextdeltaR = 0.;
   float nextdeltaPhi = 0.;
   float nextdeltaEta = 0.;
   float tmpnextdeltaR = 0.;
   for(edm::View<pat::Jet>::const_iterator jet_iter = jets.begin(); jet_iter!=jets.end(); ++jet_iter){
     if(jet_iter->genParton())
       if(jet_iter->genParton()->et()>=10. && TMath::Abs(jet_iter->genParton()->eta())<3.0 && jet_iter->genParton()->pdgId()==jet_iter->partonFlavour()){
	 nextdeltaR = 99;
	 for(edm::View<pat::Jet>::const_iterator jet_iter2 = jets.begin(); jet_iter2!=jets.end(); ++jet_iter2){
	   if(jet_iter==jet_iter2)continue;
	   nextdeltaPhi = jet_iter->genParton()->phi() - jet_iter2->phi();
	   while (nextdeltaPhi > TMath::Pi()) nextdeltaPhi -= 2*TMath::Pi();
	   while (nextdeltaPhi <= -TMath::Pi()) nextdeltaPhi += 2*TMath::Pi();
	   nextdeltaEta = jet_iter->genParton()->eta() - jet_iter2->eta();
	   tmpnextdeltaR=sqrt(nextdeltaPhi*nextdeltaPhi + nextdeltaEta*nextdeltaEta);
	   if(tmpnextdeltaR<nextdeltaR)nextdeltaR=tmpnextdeltaR;
	 }
	 deltaPhi = jet_iter->genParton()->phi() - jet_iter->phi();
	 while (deltaPhi > TMath::Pi()) deltaPhi -= 2*TMath::Pi();
	 while (deltaPhi <= -TMath::Pi()) deltaPhi += 2*TMath::Pi();
	 deltaEta = jet_iter->genParton()->eta() - jet_iter->eta();
	 deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	 if(jet_iter->isCaloJet())
	   treeMemPtr->fillVariables(jet_iter->genParton()->energy(), jet_iter->genParton()->et(), jet_iter->genParton()->pt(), 
				     jet_iter->genParton()->eta(), jet_iter->genParton()->phi(), 
				     jet_iter->energy(), jet_iter->et(), jet_iter->pt(), jet_iter->eta(), jet_iter->phi(), 
				     jet_iter->pt()/jet_iter->genParton()->pt(), jet_iter->partonFlavour(), jet_iter->emEnergyFraction(),
				     METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, 0, 0.,
				     weights[0], weights[1], weights[2]);
	 else if(jet_iter->isPFJet())
	   treeMemPtr->fillVariables(jet_iter->genParton()->energy(), jet_iter->genParton()->et(), jet_iter->genParton()->pt(), 
				     jet_iter->genParton()->eta(), jet_iter->genParton()->phi(), 
				     jet_iter->energy(), jet_iter->et(), jet_iter->pt(), jet_iter->eta(), jet_iter->phi(), 
				     jet_iter->pt()/jet_iter->genParton()->pt(), jet_iter->partonFlavour(), jet_iter->chargedEmEnergyFraction(), 
				     METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, jet_iter->genParton()->pdgId(), 0.,
				     weights[0], weights[1], weights[2]);
	 else
	   treeMemPtr->fillVariables(0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.);

	 tree->Fill();
       }
   }

   for(edm::View<pat::Electron>::const_iterator ele_iter = electrons.begin(); ele_iter!=electrons.end(); ++ele_iter){
     if(ele_iter->genLepton())
       if(ele_iter->genLepton()->pt()>0. && TMath::Abs(ele_iter->genLepton()->eta())<3.0){
	 nextdeltaR = 99;
	 for(edm::View<pat::Electron>::const_iterator ele_iter2 = electrons.begin(); ele_iter2!=electrons.end(); ++ele_iter2){
	   if(ele_iter==ele_iter2)continue;
	   nextdeltaPhi = ele_iter->genLepton()->phi() - ele_iter2->phi();
	   while (nextdeltaPhi > TMath::Pi()) nextdeltaPhi -= 2*TMath::Pi();
	   while (nextdeltaPhi <= -TMath::Pi()) nextdeltaPhi += 2*TMath::Pi();
	   nextdeltaEta = ele_iter->genLepton()->eta() - ele_iter2->eta();
	   tmpnextdeltaR=sqrt(nextdeltaPhi*nextdeltaPhi + nextdeltaEta*nextdeltaEta);
	   if(tmpnextdeltaR<nextdeltaR)nextdeltaR=tmpnextdeltaR;
	 }     
	 deltaPhi = ele_iter->genLepton()->phi() - ele_iter->phi();
	 while (deltaPhi > TMath::Pi()) deltaPhi -= 2*TMath::Pi();
	 while (deltaPhi <= -TMath::Pi()) deltaPhi += 2*TMath::Pi();
	 deltaEta = ele_iter->genLepton()->eta() - ele_iter->eta();
	 deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);

	 treeMemPtr->fillVariables(ele_iter->genLepton()->energy(), ele_iter->genLepton()->et(), ele_iter->genLepton()->pt(), 
				   ele_iter->genLepton()->eta(), ele_iter->genLepton()->phi(), 
				   ele_iter->energy(), ele_iter->et(), ele_iter->pt(), ele_iter->eta(), ele_iter->phi(), 
				   ele_iter->pt()/ele_iter->genLepton()->pt(), ele_iter->charge()*-11, 0., 
				   METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, 0, 0.,
				   weights[0], weights[1], weights[2]);
	 
	 tree->Fill();
       }
   }

   for(edm::View<pat::Muon>::const_iterator muon_iter = muons.begin(); muon_iter!=muons.end(); ++muon_iter){
     if(muon_iter->genLepton())
       if(muon_iter->genLepton()->pt()>0. && TMath::Abs(muon_iter->genLepton()->eta())<3.0){
	 nextdeltaR = 99;
	 for(edm::View<pat::Muon>::const_iterator muon_iter2 = muons.begin(); muon_iter2!=muons.end(); ++muon_iter2){
	   if(muon_iter==muon_iter2)continue;
	   nextdeltaPhi = muon_iter->genLepton()->phi() - muon_iter2->phi();
	   while (nextdeltaPhi > TMath::Pi()) nextdeltaPhi -= 2*TMath::Pi();
	   while (nextdeltaPhi <= -TMath::Pi()) nextdeltaPhi += 2*TMath::Pi();
	   nextdeltaEta = muon_iter->genLepton()->eta() - muon_iter2->eta();
	   tmpnextdeltaR=sqrt(nextdeltaPhi*nextdeltaPhi + nextdeltaEta*nextdeltaEta);
	   if(tmpnextdeltaR<nextdeltaR)nextdeltaR=tmpnextdeltaR;
	 }
	 deltaPhi = muon_iter->genLepton()->phi() - muon_iter->phi();
	 while (deltaPhi > TMath::Pi()) deltaPhi -= 2*TMath::Pi();
	 while (deltaPhi <= -TMath::Pi()) deltaPhi += 2*TMath::Pi();
	 deltaEta = muon_iter->genLepton()->eta() - muon_iter->eta();
	 deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	 float pTinnerTracker = 0.;
	 int whichMuon=0;
	 // Which type of muons in the collection?
	 if(muon_iter->isStandAloneMuon())
	   if(muon_iter->isGlobalMuon())
	     if(muon_iter->isTrackerMuon()){
	       whichMuon = 1; // STA + GLB + TM
	       pTinnerTracker = muon_iter->innerTrack()->pt();
	     }
	     else{
	       whichMuon = 2; // STA + GLB
	       pTinnerTracker = muon_iter->innerTrack()->pt();
	     }
	   else 
	     if(muon_iter->isTrackerMuon()){
	       whichMuon = 3;  // STA + TM
	       pTinnerTracker = muon_iter->innerTrack()->pt();
	     }
	     else whichMuon = 5; // STA
	 else
	   if(muon_iter->isTrackerMuon()){
	     whichMuon = 4; // TM
	     pTinnerTracker = muon_iter->innerTrack()->pt();
	   }

	 treeMemPtr->fillVariables(muon_iter->genLepton()->energy(), muon_iter->genLepton()->et(), muon_iter->genLepton()->pt(), 
				   muon_iter->genLepton()->eta(), muon_iter->genLepton()->phi(), 
				   muon_iter->energy(), muon_iter->et(), muon_iter->pt(), muon_iter->eta(), muon_iter->phi(), 
				   muon_iter->pt()/muon_iter->genLepton()->pt(), muon_iter->charge()*-13, 0., 
				   METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, whichMuon, pTinnerTracker,
				   weights[0], weights[1], weights[2]);
	 
	 tree->Fill();
       }
   }

   if(METs.begin()->genMET())
     if(METs.begin()->genMET()->pt()>0.){
       nextdeltaR = 99;
       deltaPhi = METs.begin()->genMET()->phi() - METs.begin()->phi();
       while (deltaPhi > TMath::Pi()) deltaPhi -= 2*TMath::Pi();
       while (deltaPhi <= -TMath::Pi()) deltaPhi += 2*TMath::Pi();
       deltaEta = METs.begin()->genMET()->eta() - METs.begin()->eta();
       deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
       if(METs.begin()->isCaloMET())
	 treeMemPtr->fillVariables(METs.begin()->genMET()->energy(), METs.begin()->genMET()->et(), METs.begin()->genMET()->pt(), 
				   METs.begin()->genMET()->eta(), METs.begin()->genMET()->phi(), 
				   METs.begin()->energy(), METs.begin()->et(), METs.begin()->pt(), METs.begin()->eta(), METs.begin()->phi(), 
				   METs.begin()->pt()/METs.begin()->genMET()->pt(), -21, METs.begin()->emEtFraction(), 
				   METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, 0, 0.,
				   weights[0], weights[1], weights[2]);
       else
	 treeMemPtr->fillVariables(METs.begin()->genMET()->energy(), METs.begin()->genMET()->et(), METs.begin()->genMET()->pt(), 
				   METs.begin()->genMET()->eta(), METs.begin()->genMET()->phi(), 
				   METs.begin()->energy(), METs.begin()->et(), METs.begin()->pt(), METs.begin()->eta(), METs.begin()->phi(), 
				   METs.begin()->pt()/METs.begin()->genMET()->pt(), -21, 0., 
				   METs.begin()->genMET()->sumEt(), METs.begin()->sumEt(), deltaPhi, deltaR, nextdeltaR, 0, 0.,
				   weights[0], weights[1], weights[2]);
       
       tree->Fill();
     }
  
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif

}


// ------------ method called once each job just before starting event loop  ------------
void 
ResolutionTreeWriter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ResolutionTreeWriter::endJob() {

  delete treeMemPtr; treeMemPtr = 0;

}

//define this as a plug-in
DEFINE_FWK_MODULE(ResolutionTreeWriter);
