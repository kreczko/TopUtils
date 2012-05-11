#include <TStyle.h>
#include <TCanvas.h>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TH2F.h"
#include "TChain.h"
#include "TGraphErrors.h"
#include "TMath.h"

using namespace std;

#include "TopAnalysis/TopUtils/interface/extract_sigma.h"
#include "TopAnalysis/TopUtils/interface/ResolutionVariables.h"

int main(int argc, char* argv[])
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TString inputfilename = "ResolutionTree.root";
  if(argc>=3) inputfilename = argv[2];
  cout<<"input file: "+inputfilename<<endl;
  TFile* inputfile = new TFile(inputfilename,"READ");
  TTree* tree = (TTree*)inputfile->Get("resolutionTreeWriter/ResolutionTree");
  ResolutionVariables* resobject = new ResolutionVariables();
  tree->SetMakeClass(1);
  tree->SetBranchAddress("ResolutionVariables",&resobject);
  tree->SetBranchAddress("Gen_energy",&resobject->Gen_energy);
  tree->SetBranchAddress("Gen_et",&resobject->Gen_et);
  tree->SetBranchAddress("Gen_pt",&resobject->Gen_pt);
  tree->SetBranchAddress("Gen_eta",&resobject->Gen_eta);
  tree->SetBranchAddress("Gen_phi",&resobject->Gen_phi);
  tree->SetBranchAddress("Reco_energy",&resobject->Reco_energy);
  tree->SetBranchAddress("Reco_et",&resobject->Reco_et);
  tree->SetBranchAddress("Reco_pt",&resobject->Reco_pt);
  tree->SetBranchAddress("Reco_eta",&resobject->Reco_eta);
  tree->SetBranchAddress("Reco_phi",&resobject->Reco_phi);
  tree->SetBranchAddress("pt_reco_over_gen",&resobject->pt_reco_over_gen);
  tree->SetBranchAddress("Reco_emFraction",&resobject->Reco_emFraction);
  tree->SetBranchAddress("Gen_sumEt",&resobject->Gen_sumEt);
  tree->SetBranchAddress("Reco_sumEt",&resobject->Reco_sumEt);
  tree->SetBranchAddress("DeltaPhi",&resobject->DeltaPhi);
  tree->SetBranchAddress("DeltaR",&resobject->DeltaR);
  tree->SetBranchAddress("nextDeltaR",&resobject->nextDeltaR);
  tree->SetBranchAddress("pTinnerTracker",&resobject->pTinnerTracker);
  tree->SetBranchAddress("Particle_ID",&resobject->Particle_ID);
  tree->SetBranchAddress("whichMuon",&resobject->whichMuon);
  tree->SetBranchAddress("PUweight",&resobject->PUweight);
  tree->SetBranchAddress("PUweightUp",&resobject->PUweightUp);
  tree->SetBranchAddress("PUweightDown",&resobject->PUweightDown);

  Int_t ParticleID = 0; //0 = non b jets, 1 = b jets, 2 = electrons, 3 = muons, 4 = MET
  if(argc>=2)ParticleID = atoi(argv[1]);
  TString identifier;
  if(ParticleID == 0){
    identifier = "nonb";
    cout<<"resolutions for udsc jets"<<endl;
  }
  else if(ParticleID == 1){
    identifier = "b";
    cout<<"resolutions for b jets"<<endl;
  }
  else if(ParticleID == 2){
    identifier = "elec";
    cout<<"resolutions for electrons"<<endl;
  }
  else if(ParticleID == 3){
    identifier = "muon";
    cout<<"resolutions for muons"<<endl;
  }
  else if(ParticleID == 4){
    identifier = "MET";
    cout<<"resolutions for MET"<<endl;
  }

  TString puString = "";
  if(argc>=4){
    if(atoi(argv[3])==1)puString="_puWeight";
    if(atoi(argv[3])==2)puString="_puWeightDown";
    if(atoi(argv[3])==3)puString="_puWeightUp";
  }
  TFile* file = new TFile("resol"+identifier+"histos"+puString+".root","RECREATE");

  Int_t etbinNum;
  if(ParticleID==0 || ParticleID==1)etbinNum = 25;
  else if(ParticleID == 4)etbinNum = 41;
  else etbinNum = 10;
  Int_t etabinNum;
  if(ParticleID == 2 || ParticleID == 3)etabinNum = 24;
  else if(ParticleID == 4)etabinNum = 1;
  else etabinNum = 26;
  TString etselection[41];
  TString etaselection[26];
  TString ethname[41];
  TString ethnameeta[41];
  TString ethnamephi[41];
  TString etahname[26];
  TString etahnameeta[26];
  TString etahnamephi[26];
  Double_t METetabinning[27] = {-3., 3.};
  Double_t Towerbinning[27] = {0.0, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5}; // HCAL tower
  Double_t Electronetabinning[25] = {0.0, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5}; // HCAL tower (crack modified)
  Double_t Muonetabinning[25] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4};
  vector<Double_t> etabinning;
  if(ParticleID == 2)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Electronetabinning[ibin]);
  else if(ParticleID == 4)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(METetabinning[ibin]);
  else if(ParticleID == 3)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Muonetabinning[ibin]);
  else
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Towerbinning[ibin]);

  Double_t Jetbinning[26] = {0.,20.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,110.,120.,130.,150.,180.,230.,300.,1800.};
  Double_t Electronbinning[11] = {0.,4.,10.,18.,26.,34.,42.,50.,65.,85.,1800.};
  Double_t Muonbinning[11] = {0.,4.,6.,9.,13.,19.,28.,40.,60.,85.,1800.};
  Double_t METbinning[42] = {0., 2., 5., 8., 11., 13., 16., 19., 22., 25., 28., 32., 36., 40., 44., 48., 52., 56., 60., 65., 70., 75., 80., 86., 92., 98., 104., 110., 116., 122., 128., 135., 142., 150., 160., 170., 180., 200., 240., 300., 400., 2000.};

  vector<Double_t> etbinning;
  if(ParticleID==0 || ParticleID==1)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Jetbinning[ibin]);
  else if(ParticleID==4)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(METbinning[ibin]);
  else if(ParticleID==2)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Electronbinning[ibin]);
  else 
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Muonbinning[ibin]);
  TString nonbselection = " && Particle_ID!=0 && TMath::Abs(Particle_ID)<5"; // jets from udsc quarks
  TString bselection = " && TMath::Abs(Particle_ID)==5"; // jets from b quarks
  TString electronselection = " && TMath::Abs(Particle_ID)==11 && nextDeltaR>0.4"; // electrons well seperated from the next electron
  TString muonselection = " && TMath::Abs(Particle_ID)==13 && nextDeltaR>0.01 && whichMuon==1"; // global muons well seperated from the next muon
  TString METselection = " && Particle_ID==-21"; // MET (ID from TreeWriter)

  TGraphErrors* bGraph[26];
  TGraphErrors* bGrapheta[26];
  TGraphErrors* bGraphphi[26];

  TH1D* bincenterhisto[1066];

  TH1* reshisto[1066];
  TH1* reshistoeta[1066];
  TH1* reshistoeta2[1066];
  TH1* reshistoeta3[1066];
  TH1* reshistophi[1066];
  TH1* reshistophi2[1066];
  TH1* reshistophi3[1066];

  TCanvas* controlcan[26];
  TCanvas* controlcaneta[26];
  TCanvas* controlcanphi[26];
  TCanvas* probcan = new TCanvas("probcan","probcan",700,450);

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){

    TString myetastrg;
    Char_t myetachar[10];
    sprintf(myetachar,"%i",ieta);
    myetastrg = myetachar;

    controlcan[ieta] = new TCanvas("controlcan"+myetastrg,"controlcan"+myetastrg,700,900);
    if(ParticleID==0 || ParticleID==1)controlcan[ieta]->Divide(4,6);
    else if(ParticleID == 4)controlcan[ieta]->Divide(5,8);
    else controlcan[ieta]->Divide(3,3);
    controlcaneta[ieta] = new TCanvas("controlcaneta"+myetastrg,"controlcaneta"+myetastrg,700,900);
    if(ParticleID==0 || ParticleID==1)controlcaneta[ieta]->Divide(4,6);
    else if(ParticleID == 4)controlcaneta[ieta]->Divide(5,8);
    else controlcaneta[ieta]->Divide(3,3);
    controlcanphi[ieta] = new TCanvas("controlcanphi"+myetastrg,"controlcanphi"+myetastrg,700,900);
    if(ParticleID==0 || ParticleID==1)controlcanphi[ieta]->Divide(4,6);
    else if(ParticleID == 4)controlcanphi[ieta]->Divide(5,8);
    else controlcanphi[ieta]->Divide(3,3);

    TString loweretastrg;
    TString upperetastrg;
    Char_t etachar[10];
    sprintf(etachar,"%1.3f",etabinning[ieta]);
    loweretastrg = etachar;
    sprintf(etachar,"%1.3f",etabinning[ieta+1]);
    upperetastrg = etachar;

    if(ParticleID != 4)etaselection[ieta] = " && TMath::Abs(Gen_eta)>="+loweretastrg+" && TMath::Abs(Gen_eta)<"+upperetastrg;
    else etaselection[ieta] = " && Gen_eta>="+loweretastrg+" && Gen_eta<"+upperetastrg;
    etahname[ieta] = "h"+loweretastrg+"_"+upperetastrg;
    etahnameeta[ieta] = "h_eta"+loweretastrg+"_"+upperetastrg;
    etahnamephi[ieta] = "h_phi"+loweretastrg+"_"+upperetastrg;
 
    bGraph[ieta] = new TGraphErrors(41);
    bGraph[ieta]->SetTitle(etahname[ieta]);

    bGrapheta[ieta] = new TGraphErrors(41); 
    bGrapheta[ieta]->SetTitle(etahnameeta[ieta]);

    bGraphphi[ieta] = new TGraphErrors(41); 
    bGraphphi[ieta]->SetTitle(etahnamephi[ieta]);

    for(Int_t isel = 0; isel < etbinNum; isel++){

      TString etaetnumber;
      Char_t etaetchar[10];
      sprintf(etaetchar,"%i",(ieta*etbinNum)+isel);
      etaetnumber = etaetchar;

      bincenterhisto[(ieta*etbinNum)+isel] = new TH1D("bincenterhisto"+etaetnumber,"bincenterhisto"+etaetnumber,150000,etbinning[isel],etbinning[isel+1]);

      if(ParticleID==0 || ParticleID==1){
	reshisto[(ieta*etbinNum)+isel] = new TH1F("reshisto"+etaetnumber,"reshisto"+etaetnumber,200,-1.,1.);

	reshistoeta[(ieta*etbinNum)+isel] = new TH1F("reshistoeta"+etaetnumber,"reshistoeta"+etaetnumber,600,-0.3,0.3);
	reshistoeta2[(ieta*etbinNum)+isel] = new TH1F("reshistoeta2_"+etaetnumber,"reshistoeta2_"+etaetnumber,200,-0.1,0.1);
	reshistoeta3[(ieta*etbinNum)+isel] = new TH1F("reshistoeta3_"+etaetnumber,"reshistoeta3_"+etaetnumber,100,-0.05,0.05);

	reshistophi[(ieta*etbinNum)+isel] = new TH1F("reshistophi"+etaetnumber,"reshistophi"+etaetnumber,800,-0.4,0.4);
	reshistophi2[(ieta*etbinNum)+isel] = new TH1F("reshistophi2_"+etaetnumber,"reshistophi2_"+etaetnumber,400,-0.2,0.2);
	reshistophi3[(ieta*etbinNum)+isel] = new TH1F("reshistophi3_"+etaetnumber,"reshistophi3_"+etaetnumber,200,-0.1,0.1);
      }

      else if(ParticleID==2 || ParticleID==3){
	reshisto[(ieta*etbinNum)+isel] = new TH1F("reshisto"+etaetnumber,"reshisto"+etaetnumber,200,-0.1,0.1);

	reshistoeta[(ieta*etbinNum)+isel] = new TH1F("reshistoeta_"+etaetnumber,"reshistoeta_"+etaetnumber,900,-0.009,0.009);
	reshistoeta2[(ieta*etbinNum)+isel] = new TH1F("reshistoeta2_"+etaetnumber,"reshistoeta2_"+etaetnumber,300,-0.003,0.003);
	reshistoeta3[(ieta*etbinNum)+isel] = new TH1F("reshistoeta3_"+etaetnumber,"reshistoeta3_"+etaetnumber,100,-0.001,0.001);

	reshistophi[(ieta*etbinNum)+isel] = new TH1F("reshistophi_"+etaetnumber,"reshistophi_"+etaetnumber,900,-0.009,0.009);
	reshistophi2[(ieta*etbinNum)+isel] = new TH1F("reshistophi2_"+etaetnumber,"reshistophi2_"+etaetnumber,300,-0.003,0.003);
	reshistophi3[(ieta*etbinNum)+isel] = new TH1F("reshistophi3_"+etaetnumber,"reshistophi3_"+etaetnumber,100,-0.001,0.001);
      }
      else{
	reshisto[(ieta*etbinNum)+isel] = new TH1F("reshisto"+etaetnumber,"reshisto"+etaetnumber,300,-1.,5.);
	reshistoeta[(ieta*etbinNum)+isel] = new TH1F("reshistoeta_"+etaetnumber,"reshistoeta_"+etaetnumber,300,-3.,3.);
	reshistophi[(ieta*etbinNum)+isel] = new TH1F("reshistophi_"+etaetnumber,"reshistophi_"+etaetnumber,300,-TMath::Pi(),TMath::Pi());
      }
      
    }
  }
  for(Int_t isel = 0; isel < etbinNum; isel++){
    TString lower_et_cut;
    TString upper_et_cut;
    Char_t tmpchar[10];
    sprintf(tmpchar,"%1.0f",etbinning[isel]);
    lower_et_cut = tmpchar;
    sprintf(tmpchar,"%1.0f",etbinning[isel+1]);
    upper_et_cut = tmpchar;

    etselection[isel] = "Gen_et>"+lower_et_cut+" && Gen_et<="+upper_et_cut;
    ethname[isel] = "h"+lower_et_cut+"_"+upper_et_cut;
    ethnameeta[isel] = "h_eta"+lower_et_cut+"_"+upper_et_cut;
    ethnamephi[isel] = "h_phi"+lower_et_cut+"_"+upper_et_cut;
  }

  double weight = 1.;

  Int_t nevent = (Int_t)tree->GetEntries();
  for(Int_t ientry = 0; ientry < nevent; ++ientry){
    tree->GetEntry(ientry);

    if(argc>=4){
      if(atoi(argv[3])==1)weight = resobject->PUweight;
      if(atoi(argv[3])==2)weight = resobject->PUweightDown;
      if(atoi(argv[3])==3)weight = resobject->PUweightUp;
    }

    for(Int_t ieta = 0; ieta < etabinNum; ieta++){

      for(Int_t isel = 0; isel < etbinNum; isel++){
	if(ParticleID == 0){
	  if(resobject->Particle_ID!=0
	     && TMath::Abs(resobject->Particle_ID)<5
	     && TMath::Abs(resobject->Gen_eta)>=etabinning[ieta]
	     && TMath::Abs(resobject->Gen_eta)<etabinning[ieta+1]
	     && resobject->Gen_et>etbinning[isel]
	     && resobject->Gen_et<=etbinning[isel+1]){
 	    bincenterhisto[(ieta*etbinNum)+isel]->Fill(resobject->Gen_et, weight);
	    reshisto[(ieta*etbinNum)+isel]->Fill(resobject->Reco_et/resobject->Gen_et-1, weight);
	    if(isel>10){
	      reshistoeta3[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi3[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else if(isel>3){
	      reshistoeta2[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi2[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else{
	      reshistoeta[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	  }
	}
	else if(ParticleID == 1){
	  if(TMath::Abs(resobject->Particle_ID)==5
	     && TMath::Abs(resobject->Gen_eta)>=etabinning[ieta]
	     && TMath::Abs(resobject->Gen_eta)<etabinning[ieta+1]
	     && resobject->Gen_et>etbinning[isel]
	     && resobject->Gen_et<=etbinning[isel+1]){
	    bincenterhisto[(ieta*etbinNum)+isel]->Fill(resobject->Gen_et, weight);
	    reshisto[(ieta*etbinNum)+isel]->Fill(resobject->Reco_et/resobject->Gen_et-1, weight);
	    if(isel>10){
	      reshistoeta3[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi3[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else if(isel>3){
	      reshistoeta2[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi2[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else{
	      reshistoeta[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	  }
	}
	else if(ParticleID == 2){
	  if(TMath::Abs(resobject->Particle_ID)==11
	     && resobject->nextDeltaR>0.4
	     && TMath::Abs(resobject->Gen_eta)>=etabinning[ieta]
	     && TMath::Abs(resobject->Gen_eta)<etabinning[ieta+1]
	     && resobject->Gen_et>etbinning[isel]
	     && resobject->Gen_et<=etbinning[isel+1]){
	    bincenterhisto[(ieta*etbinNum)+isel]->Fill(resobject->Gen_et, weight);
	    reshisto[(ieta*etbinNum)+isel]->Fill(resobject->Reco_et/resobject->Gen_et-1, weight);
	    if(isel>5){
	      reshistoeta3[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi3[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else if(isel>2){
	      reshistoeta2[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi2[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else {
	      reshistoeta[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	  }
	}
	else if(ParticleID == 3){
	  if(TMath::Abs(resobject->Particle_ID)==13
	     && resobject->nextDeltaR>0.01
	     && resobject->whichMuon==1
	     && TMath::Abs(resobject->Gen_eta)>=etabinning[ieta]
	     && TMath::Abs(resobject->Gen_eta)<etabinning[ieta+1]
	     && resobject->Gen_et>etbinning[isel]
	     && resobject->Gen_et<=etbinning[isel+1]){
	    bincenterhisto[(ieta*etbinNum)+isel]->Fill(resobject->Gen_et, weight);
	    reshisto[(ieta*etbinNum)+isel]->Fill(resobject->Reco_pt/resobject->Gen_pt-1, weight);
	    if(isel>5){
	      reshistoeta3[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi3[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else if(isel>2){
	      reshistoeta2[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi2[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	    else {
	      reshistoeta[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	      reshistophi[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	    }
	  }
	}
	else if(ParticleID == 4){
	  if(resobject->Particle_ID==-21
	     && resobject->Gen_et>etbinning[isel]
	     && resobject->Gen_et<=etbinning[isel+1]){
	    bincenterhisto[(ieta*etbinNum)+isel]->Fill(resobject->Gen_et, weight);
	    reshisto[(ieta*etbinNum)+isel]->Fill(resobject->Reco_et/resobject->Gen_et-1, weight);
	    reshistoeta[(ieta*etbinNum)+isel]->Fill(resobject->Reco_eta-resobject->Gen_eta, weight);
	    reshistophi[(ieta*etbinNum)+isel]->Fill(resobject->DeltaPhi, weight);
	  }
	}
      }
    }
  }

  Double_t sigma[41];
  Double_t sigmaerr[41];
  Double_t sigmaeta[41];
  Double_t sigmaerreta[41];
  Double_t sigmaphi[41];
  Double_t sigmaerrphi[41];

  TH1D* controlhisto[41];
  TH1D* controlhistoeta[41];
  TH1D* controlhistophi[41];
  TH1F* probabilityeta = new TH1F("probabilityeta","probabilityeta",100,0.,1.);

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    for(Int_t isel = 0; isel < etbinNum; isel++){

      TString lower_et_cut;
      TString upper_et_cut;
      Char_t tmpchar[10];
      sprintf(tmpchar,"%1.0f",etbinning[isel]);
      lower_et_cut = tmpchar;
      sprintf(tmpchar,"%1.0f",etbinning[isel+1]);
      upper_et_cut = tmpchar;
      
      TString myselection;
      if(ParticleID == 0)myselection = etselection[isel]+etaselection[ieta]+nonbselection;
      else if(ParticleID == 1)myselection = etselection[isel]+etaselection[ieta]+bselection;
      else if(ParticleID == 2)myselection = etselection[isel]+etaselection[ieta]+electronselection;
      else if(ParticleID == 3)myselection = etselection[isel]+etaselection[ieta]+muonselection;
      else if(ParticleID == 4)myselection = etselection[isel]+etaselection[ieta]+METselection;

      controlhisto[isel] = (TH1D*)reshisto[(ieta*etbinNum)+isel]->Clone(ethname[isel]);
      if((ParticleID==2 || ParticleID==3) && isel>5){
	controlhistoeta[isel] = (TH1D*)reshistoeta3[(ieta*etbinNum)+isel]->Clone(ethnameeta[isel]);
	controlhistophi[isel] = (TH1D*)reshistophi3[(ieta*etbinNum)+isel]->Clone(ethnamephi[isel]);
      }
      else if((ParticleID==2 || ParticleID==3) && isel>2){
	controlhistoeta[isel] = (TH1D*)reshistoeta2[(ieta*etbinNum)+isel]->Clone(ethnameeta[isel]);
	controlhistophi[isel] = (TH1D*)reshistophi2[(ieta*etbinNum)+isel]->Clone(ethnamephi[isel]);
      }
      else if((ParticleID==0 || ParticleID==1) && isel>10){
	controlhistoeta[isel] = (TH1D*)reshistoeta3[(ieta*etbinNum)+isel]->Clone(ethnameeta[isel]);
	controlhistophi[isel] = (TH1D*)reshistophi3[(ieta*etbinNum)+isel]->Clone(ethnamephi[isel]);
      }
      else if((ParticleID==0 || ParticleID==1) && isel>3){
	controlhistoeta[isel] = (TH1D*)reshistoeta2[(ieta*etbinNum)+isel]->Clone(ethnameeta[isel]);
	controlhistophi[isel] = (TH1D*)reshistophi2[(ieta*etbinNum)+isel]->Clone(ethnamephi[isel]);
      }
      else{
	controlhistoeta[isel] = (TH1D*)reshistoeta[(ieta*etbinNum)+isel]->Clone(ethnameeta[isel]);
	controlhistophi[isel] = (TH1D*)reshistophi[(ieta*etbinNum)+isel]->Clone(ethnamephi[isel]);
      }

      if(isel==0)controlcan[ieta]->cd(isel+1);
      else controlcan[ieta]->cd(isel);
      controlhisto[isel]->Draw();
      if(isel==0)controlcaneta[ieta]->cd(isel+1);
      else controlcaneta[ieta]->cd(isel);
      controlhistoeta[isel]->Draw();
      if(isel==0)controlcanphi[ieta]->cd(isel+1);
      else controlcanphi[ieta]->cd(isel);
      controlhistophi[isel]->Draw();

      cout << "selection criteria is: " << myselection << endl;
      vector<double> sigmatemp = extract_sigma(controlhisto[isel]);
      sigma[isel] = sigmatemp[0];
      sigmaerr[isel] = sigmatemp[1];
      bGraph[ieta]->SetPoint(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMean(),sigma[isel]);
      if(sigmaerr[isel]!=0.)bGraph[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),sigmaerr[isel]);
      else bGraph[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),9999);
      controlcan[ieta]->Update();
      
      if(ParticleID != 4){
	sigmatemp = extract_sigma(controlhistoeta[isel]);
	sigmaeta[isel] = sigmatemp[0];
	sigmaerreta[isel] = sigmatemp[1];
	probabilityeta->Fill(sigmatemp[2]);
	bGrapheta[ieta]->SetPoint(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMean(),sigmaeta[isel]);
	if(sigmaerreta[isel]!=0.)bGrapheta[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),sigmaerreta[isel]);
	else bGrapheta[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),9999);
	controlcaneta[ieta]->Update();
      }
      sigmatemp = extract_sigma(controlhistophi[isel]);
      sigmaphi[isel] = sigmatemp[0];
      sigmaerrphi[isel] = sigmatemp[1];
      bGraphphi[ieta]->SetPoint(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMean(),sigmaphi[isel]);
      if(sigmaerrphi[isel]!=0.)bGraphphi[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),sigmaerrphi[isel]);
      else bGraphphi[ieta]->SetPointError(isel,bincenterhisto[(ieta*etbinNum)+isel]->GetMeanError(),9999);
      controlcanphi[ieta]->Update();

      delete bincenterhisto[(ieta*etbinNum)+isel];
      delete reshisto[(ieta*etbinNum)+isel]; 
      delete reshistoeta[(ieta*etbinNum)+isel]; 
      delete reshistophi[(ieta*etbinNum)+isel]; 
      if(ParticleID==2 || ParticleID==3){
      delete reshistoeta2[(ieta*etbinNum)+isel]; 
      delete reshistophi2[(ieta*etbinNum)+isel]; 
      delete reshistoeta3[(ieta*etbinNum)+isel]; 
      delete reshistophi3[(ieta*etbinNum)+isel];
      } 
    }
    file->WriteTObject(bGraph[ieta],etahname[ieta]);
    file->WriteTObject(bGrapheta[ieta],etahnameeta[ieta]);
    file->WriteTObject(bGraphphi[ieta],etahnamephi[ieta]);
    TString etastrg;
    Char_t tmpchar2[10];
    sprintf(tmpchar2,"%i",ieta+1);
    etastrg = tmpchar2;
    if(argc>=5 && atoi(argv[4])){
      if(ParticleID == 0){
	controlcan[ieta]->Print("gauscontrol/gauscontrolnonb_"+etastrg+".ps");
	controlcaneta[ieta]->Print("gauscontrol/gauscontrolnonb_eta_"+etastrg+".ps");
	controlcanphi[ieta]->Print("gauscontrol/gauscontrolnonb_phi_"+etastrg+".ps");
      }
      else if(ParticleID == 1){
	controlcan[ieta]->Print("gauscontrol/gauscontrolb_"+etastrg+".ps");
	controlcaneta[ieta]->Print("gauscontrol/gauscontrolb_eta_"+etastrg+".ps");
	controlcanphi[ieta]->Print("gauscontrol/gauscontrolb_phi_"+etastrg+".ps");
      }
      else if(ParticleID == 2){
	controlcan[ieta]->Print("gauscontrol/gauscontrolele_"+etastrg+".ps");
	controlcaneta[ieta]->Print("gauscontrol/gauscontrolele_eta_"+etastrg+".ps");
	controlcanphi[ieta]->Print("gauscontrol/gauscontrolele_phi_"+etastrg+".ps");
      }
      else if(ParticleID == 3){
	controlcan[ieta]->Print("gauscontrol/gauscontrolmuon_"+etastrg+".ps");
	controlcaneta[ieta]->Print("gauscontrol/gauscontrolmuon_eta_"+etastrg+".ps");
	controlcanphi[ieta]->Print("gauscontrol/gauscontrolmuon_phi_"+etastrg+".ps");
      }
      else if(ParticleID == 4){
	controlcan[ieta]->Print("gauscontrol/gauscontrolMET_"+etastrg+".ps");
	controlcanphi[ieta]->Print("gauscontrol/gauscontrolMET_phi_"+etastrg+".ps");
      }
    }
    delete controlcan[ieta];
    delete controlcaneta[ieta];
    delete controlcanphi[ieta];
  }
  probcan->cd();
  probabilityeta->Draw();
  probcan->Print("gauscontrol/gausprobability_eta.ps");
  delete file;
  return 0;
}
