#include <TStyle.h>
#include <TCanvas.h>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TH2F.h"
#include "TChain.h"
#include "TGraphErrors.h"

using namespace std;

#include "TopAnalysis/TopUtils/interface/extract_sigma.h"

int main(int argc, char* argv[])
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);

  Bool_t bJets = 0;
  if(argc>=2)bJets = argv[1];
  TFile* file = 0;
  if(bJets)file = new TFile("resolbhistos.root","RECREATE");
  else file = new TFile("resolnonbhistos.root","RECREATE");

  // my chain
  TString inputfile = "allJetCalibrationTree.root";
  if(argc>=3) inputfile = argv[2];
  TChain mychain("jetCalibrationTreeWriter/JetCalibrationtree");
  mychain.Add(inputfile);

  const Int_t etbinNum = 25;
  const Int_t etabinNum = 26;
  TString etselection[etbinNum];
  TString etaselection[etabinNum];
  TString ethname[etbinNum];
  TString etahname[etabinNum];
  Double_t etabinning[27] = {0.0, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5}; // HCAL tower
  Double_t etbinning[26] = {0.,20.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,110.,120.,130.,150.,180.,230.,300.,1800.};

  TString bselection = " && TMath::Abs(Jet_flavour)==5";
  TString nonbselection = " && TMath::Abs(Jet_flavour)!=5";
 
  TGraphErrors* bGraph[etabinNum];
  TH1D* controlhisto[etbinNum];
  TH1* bincenterhisto;

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){

    TCanvas* controlcan = new TCanvas("controlcan","controlcan",700,900);
    controlcan->Divide(4,6);

    TString loweretastrg;
    TString upperetastrg;
    Char_t etachar[10];
    sprintf(etachar,"%1.3f",etabinning[ieta]);
    loweretastrg = etachar;
    sprintf(etachar,"%1.3f",etabinning[ieta+1]);
    upperetastrg = etachar;

    etaselection[ieta] = " && TMath::Abs(GenJet_eta)>="+loweretastrg+" && TMath::Abs(GenJet_eta)<"+upperetastrg;
    etahname[ieta] = "h"+loweretastrg+"_"+upperetastrg;

    Double_t sigma[etbinNum];
    Double_t sigmaerr[etbinNum];
    TH1* reshisto = new TH1F("reshisto","reshisto",100,-1.,1.);
    TGraphErrors* tmpGraph = new TGraphErrors(25);
    tmpGraph->SetTitle(etahname[ieta]);
    bGraph[ieta] = (TGraphErrors*)tmpGraph->Clone(etahname[ieta]); 
    delete tmpGraph;

    for(Int_t isel = 0; isel < etbinNum; isel++){
      TString lower_et_cut;
      TString upper_et_cut;
      Char_t tmpchar[10];
      sprintf(tmpchar,"%1.0f",etbinning[isel]);
      lower_et_cut = tmpchar;
      sprintf(tmpchar,"%1.0f",etbinning[isel+1]);
      upper_et_cut = tmpchar;
      
      etselection[isel] = "GenJet_et>"+lower_et_cut+" && GenJet_et<="+upper_et_cut+" && RecoJet_emFraction<0.95";
      ethname[isel] = "h"+lower_et_cut+"_"+upper_et_cut;
      
      TString myselection;
      if(bJets)myselection = etselection[isel]+etaselection[ieta]+bselection;
      else myselection = etselection[isel]+etaselection[ieta]+nonbselection;

      mychain.Draw("GenJet_pt>>bincenterhisto(150000,"+lower_et_cut+","+upper_et_cut+")",myselection,"goff");
      bincenterhisto = mychain.GetHistogram();
      //cout<<"bincenterhisto->GetMean(): "<<bincenterhisto->GetMean()<<endl;
      
      mychain.Draw("RecoJet_pt/GenJet_pt-1>>reshisto",myselection,"goff");
      reshisto = mychain.GetHistogram();
      reshisto->SetTitle(ethname[isel]);
      controlhisto[isel] = (TH1D*)reshisto->Clone(ethname[isel]);

      if(isel==0)controlcan->cd(isel+1);
      else controlcan->cd(isel);
      controlhisto[isel]->Draw();

      cout << "selection criteria is: " << myselection << endl;
      vector<double> sigmatemp = extract_sigma(controlhisto[isel]);
      sigma[isel] = sigmatemp[0];
      sigmaerr[isel] = sigmatemp[1];
      bGraph[ieta]->SetPoint(isel,bincenterhisto->GetMean(),sigma[isel]);
      if(sigmaerr[isel]!=0.)bGraph[ieta]->SetPointError(isel,bincenterhisto->GetMeanError(),sigmaerr[isel]);
      else bGraph[ieta]->SetPointError(isel,bincenterhisto->GetMeanError(),9999);
      controlcan->Update();
    }
    delete bincenterhisto;
    delete reshisto; 
    file->WriteTObject(bGraph[ieta],etahname[ieta]);
    TString etastrg;
    Char_t tmpchar2[10];
    sprintf(tmpchar2,"%i",ieta+1);
    etastrg = tmpchar2;
    controlcan->Print("gauscontrol_"+etastrg+".ps");
    delete controlcan;
  }
  delete file;
  return 0;
}
