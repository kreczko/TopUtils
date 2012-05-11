#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TFile.h"
#include "TH2F.h"
#include "TChain.h"
#include "TLatex.h"
#include "TF2.h"
#include "TGraphErrors.h"
#include "TMath.h"

using namespace std;
Double_t rounderror(Double_t value, Double_t error, Int_t k, bool highUncert);

int main(int argc, char* argv[])
{
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  Int_t ParticleID = 0; //0 = non b jets, 1 = b jets, 2 = electrons, 3 = muons, 4 = MET
  if(argc>=2)ParticleID = atoi(argv[1]);
  TString identifier;
  if(ParticleID == 0)identifier = "nonb";
  else if(ParticleID == 1)identifier = "b";
  else if(ParticleID == 2)identifier = "elec";
  else if(ParticleID == 3)identifier = "muon";
  else if(ParticleID == 4)identifier = "MET";

  TString puString = "";
  if(argc>=3){
    if(atoi(argv[2])==1)puString="_puWeight";
    if(atoi(argv[2])==2)puString="_puWeightDown";
    if(atoi(argv[2])==3)puString="_puWeightUp";
  }

  TFile* file = new TFile("resol"+identifier+"histos"+puString+".root");

  TString useErf = "";
  if(argc>=5){
    std::cout<<"using erf function for MET resolution"<<std::endl;
    useErf=argv[4];
  }

  Int_t etbinNum;
  if(ParticleID==0 || ParticleID==1)etbinNum = 25;
  else if(ParticleID==4)etbinNum = 41;
  else etbinNum = 10;
  Int_t etstartbin = 0;
  if(argc>=4)etstartbin = atoi(argv[3]);
  else etstartbin = 2;
  Int_t etabinNum;
  if(ParticleID == 2 || ParticleID == 3)etabinNum = 24;
  else if(ParticleID == 4)etabinNum = 1;
  else etabinNum = 26;
  TString etahname[26];
  Double_t METetabinning[27] = {-3., 3.};
  Double_t Towerbinning[27] = {0.0, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5}; // HCAL tower
  Double_t Electronetabinning[25] = {0.0, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5}; // HCAL tower (crack modified)
  Double_t Muonetabinning[25] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4};
  vector<Double_t> etabinning;
  if(ParticleID == 2)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Electronetabinning[ibin]);
  if(ParticleID == 3)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Muonetabinning[ibin]);
  else if(ParticleID == 4)
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(METetabinning[ibin]);
  else
    for(Int_t ibin = 0; ibin <= etabinNum; ibin++)etabinning.push_back(Towerbinning[ibin]);

  vector<Double_t> etabinning2;
  for(Int_t ibin = 0; ibin <= etabinNum; ibin++)
    for(Int_t ibinsmall = 0; ibinsmall < 10; ibinsmall++)etabinning2.push_back(etabinning[ibin]+(etabinning[ibin+1]-etabinning[ibin])*ibinsmall/10.);

  Double_t Jetbinning[26] = {0.,20.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,110.,120.,130.,150.,180.,230.,300.,500.};
  Double_t Electronbinning[11] = {0.,4.,10.,18.,26.,34.,42.,50.,65.,85.,500.};
  Double_t Muonbinning[11] = {0.,4.,6.,9.,13.,19.,28.,40.,60.,85.,500.};
  Double_t METbinning[42] = {0., 2., 5., 8., 11., 13., 16., 19., 22., 25., 28., 32., 36., 40., 44., 48., 52., 56., 60., 65., 70., 75., 80., 86., 92., 98., 104., 110., 116., 122., 128., 135., 142., 150., 160., 170., 180., 200., 240., 300., 400., 500.};

  vector<Double_t> etbinning;
  if(ParticleID==0 || ParticleID==1)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Jetbinning[ibin]);
  else if(ParticleID==4)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(METbinning[ibin]);
  else if(ParticleID==2)
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Electronbinning[ibin]);
  else
    for(Int_t ibin = 0; ibin <= etbinNum; ibin++)etbinning.push_back(Muonbinning[ibin]);
  vector<Double_t> etbinning2;
  for(Int_t ibin = 0; ibin <= etbinNum; ibin++)
    for(Int_t ibinsmall = 0; ibinsmall < 100; ibinsmall++)etbinning2.push_back(etbinning[ibin]+(etbinning[ibin+1]-etbinning[ibin])*ibinsmall/100.);

  TGraphErrors* bGraph[26];

  TH1D* axeshisto = new TH1D("axeshisto","axeshisto",etbinNum,&(etbinning.front()));
    
  TCanvas* bet = new TCanvas("bet","bet",700,900);
  TCanvas* etetacanvas = new TCanvas("etetacanvas","etetacanvas",700,900);
  etetacanvas->Divide(1,2);
  TCanvas* chi2canvas = new TCanvas("chi2canvas","chi2canvas",500,900);
  chi2canvas->Divide(1,3);

  TCanvas* beteta = new TCanvas("beteta","beteta",700,900);
  TCanvas* etetacanvaseta = new TCanvas("etetacanvaseta","etetacanvaseta",700,900);
  etetacanvaseta->Divide(1,2);
  TCanvas* chi2canvaseta = new TCanvas("chi2canvaseta","chi2canvaseta",500,900);
  chi2canvaseta->Divide(1,3);

  TCanvas* betphi = new TCanvas("betphi","betphi",700,900);
  TCanvas* etetacanvasphi = new TCanvas("etetacanvasphi","etetacanvasphi",700,900);
  etetacanvasphi->Divide(1,2);
  TCanvas* chi2canvasphi = new TCanvas("chi2canvasphi","chi2canvasphi",500,900);
  chi2canvasphi->Divide(1,3);

  TString etstrg;
  TString etastrg;
  TString phistrg;
  vector<TString> etvec;
  vector<TString> etavec;
  vector<TString> phivec;

  for(Int_t i_etetaphi = 0; i_etetaphi < 3; i_etetaphi++){
  TF1* myetfunction = 0;

  // define 1D functions in E_T here!
  if(ParticleID == 3 && i_etetaphi==0)myetfunction = new TF1("linear","[0]+[1]*x",etbinning[etstartbin],500.);//muon ET resolution
  else if(((ParticleID==0 || ParticleID==1)&& i_etetaphi!=0))// || (ParticleID == 3 && i_etetaphi==1) || (ParticleID == 2 && i_etetaphi==1))
    myetfunction = new TF1("reduced","sqrt([0]*[0]+[1]*[1]/(x*x))",etbinning[etstartbin],500.);
  else if(ParticleID==4 && i_etetaphi==0 && useErf=="erf")
    myetfunction = new TF1("erf","sqrt([0]*[0]+[1]*[1]/(x*x))*TMath::Erf([2]*x)",etbinning[etstartbin],500.);
  else myetfunction = new TF1("complete","sqrt([0]*[0]+[1]*[1]/(x)+[2]*[2]/(x*x))",etbinning[etstartbin],500.);
  // not using b-jets above 230 GeV as they are affected by reco-jet merging
  // could possibly be recovered by reducing Gaussian-fit range for these bins
  if(ParticleID == 1)myetfunction->SetRange(etbinning[etstartbin],230.);

  file->cd();
  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    TString loweretastrg;
    TString upperetastrg;
    Char_t etachar[10];
    sprintf(etachar,"%1.3f",etabinning[ieta]);
    loweretastrg = etachar;
    sprintf(etachar,"%1.3f",etabinning[ieta+1]);
    upperetastrg = etachar;
    if(i_etetaphi==0)etahname[ieta] = "h"+loweretastrg+"_"+upperetastrg;
    else if(i_etetaphi==1)etahname[ieta] = "h_eta"+loweretastrg+"_"+upperetastrg;
    else if(i_etetaphi==2)etahname[ieta] = "h_phi"+loweretastrg+"_"+upperetastrg;
    else continue;

    bGraph[ieta] = (TGraphErrors*) (file->Get(etahname[ieta]))->Clone();
  }

  if(ParticleID == 0 || ParticleID == 1){
    if(i_etetaphi==0) axeshisto->SetMaximum(0.45);
    else axeshisto->SetMaximum(0.1);
  }
  else if(ParticleID == 2 || ParticleID == 3){
    if(i_etetaphi==0)axeshisto->SetMaximum(0.1);
    else axeshisto->SetMaximum(0.003);
  }
  else axeshisto->SetMaximum(2.5);

  TH2D* eteta = new TH2D("eteta","eteta",etbinNum,&(etbinning.front()),etabinNum,&(etabinning.front()));
  TH2D* etetadraw = new TH2D("etetadraw","etetadraw",etbinNum*100,&(etbinning2.front()),etabinNum,&(etabinning.front()));
  TH2D* eteta2 = new TH2D("eteta2","eteta2",etbinNum*100,&(etbinning2.front()),etabinNum*10,&(etabinning2.front()));

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    if(i_etetaphi==0){
      if(ieta%2==0){
	bet->Divide(1,2);
	bet->cd(1);
      }
      else bet->cd(2);
    }
    else if(i_etetaphi==1){
      if(ieta%2==0){
	beteta->Divide(1,2);
	beteta->cd(1);
      }
      else beteta->cd(2);
    }
    else if(i_etetaphi==2){
      if(ieta%2==0){
	betphi->Divide(1,2);
	betphi->cd(1);
      }
      else betphi->cd(2);
    }
    if(ParticleID == 0)axeshisto->GetXaxis()->SetTitle("light jet E_{T} [GeV]");
    if(ParticleID == 1)axeshisto->GetXaxis()->SetTitle("b jet E_{T} [GeV]");
    if(ParticleID == 2)axeshisto->GetXaxis()->SetTitle("electron E_{T} [GeV]");
    if(ParticleID == 3)axeshisto->GetXaxis()->SetTitle("muon E_{T} [GeV]");
    if(ParticleID == 4)axeshisto->GetXaxis()->SetTitle("MET [GeV]");
    axeshisto->GetYaxis()->SetTitle("#sigma(E_{T}^{rec}/E_{T}^{gen})");
    if(i_etetaphi==1)axeshisto->GetYaxis()->SetTitle("#sigma(#eta^{rec})");
    if(i_etetaphi==2)axeshisto->GetYaxis()->SetTitle("#sigma(#phi^{rec})");
    axeshisto->Draw("axis");
  
    bGraph[ieta]->SetMarkerStyle(20);
    bGraph[ieta]->SetMarkerSize(0.8);

    myetfunction->SetLineWidth(1);
    myetfunction->SetLineColor(kBlue);

    double functionLowEdge = 0.;
    double functionUppEdge = 0.;
    myetfunction->GetRange(functionLowEdge,functionUppEdge);

    // need initial values to avoid fit problems
    if(ParticleID != 4 || i_etetaphi!=1)
      for(int i = 0; i < myetfunction->GetNpar(); i++)
	myetfunction->SetParameter(i,0.0001);

    bGraph[ieta]->Fit(myetfunction->GetName(),"+QR");
    bGraph[ieta]->DrawClone("pe1same");
    myetfunction->SetRange(0,500);
    myetfunction->DrawCopy("same");
    myetfunction->SetLineColor(kRed);
    myetfunction->SetRange(functionLowEdge,functionUppEdge);
    myetfunction->DrawCopy("same");

    cout << "myetfunction: " << myetfunction->GetExpFormula("p") << ", Prob: " << myetfunction->GetProb() << endl;
    TLatex l;
    l.SetTextAlign(11);
    l.SetTextSize(0.055);

    TString namestring;
    Char_t namechar[10];
    sprintf(namechar,"%i",(ieta+1)/2);
    namestring = namechar;
    TString loweretastring;
    TString upperetastring;
    TString etastring;
    TString parastring;
    Char_t parachar[10];
    sprintf(parachar,"%1.3f",etabinning[ieta]);
    loweretastring = parachar;
    etastring = loweretastring;
    etastring += "<|#eta|<";
    sprintf(parachar,"%1.3f",etabinning[ieta+1]);
    upperetastring = parachar;
    etastring += upperetastring;
    etastring += ": ";

    if((TString)myetfunction->GetName()=="linear")
      sprintf(parachar,"%1.5f",myetfunction->GetParameter(0));
    else
      sprintf(parachar,"%1.5f",TMath::Abs(myetfunction->GetParameter(0)));
    parastring = parachar;

    if((TString)myetfunction->GetName()=="complete"){
      sprintf(parachar,"^{2}+%1.4f^{2}/E_{T}+",TMath::Abs(myetfunction->GetParameter(1)));
      parastring += parachar;
      sprintf(parachar,"%1.4f",TMath::Abs(myetfunction->GetParameter(2)));
      parastring += parachar;
      if(ParticleID == 0 || ParticleID == 1)l.DrawLatex(50.,0.4,etastring +"#sqrt{"+ parastring +"^{2}/E_{T}^{2}}");
      else if(ParticleID == 2){
	if(i_etetaphi==0)l.DrawLatex(50.,0.09,etastring +"#sqrt{"+ parastring +"^{2}/E_{T}^{2}}");
	else l.DrawLatex(50.,0.0027,etastring +"#sqrt{"+ parastring +"^{2}/E_{T}^{2}}");
      }
      else if(ParticleID == 3)l.DrawLatex(50.,0.0027,etastring +"#sqrt{"+ parastring +"^{2}/E_{T}^{2}}");
      else if(ParticleID == 4 && i_etetaphi!=1)l.DrawLatex(50.,2.2,"#sqrt{"+ parastring +"^{2}/E_{T}^{2}}");
    }
    else if((TString)myetfunction->GetName()=="reduced"){
      sprintf(parachar,"^{2}+%1.4f^{2}/E_{T}^{2}}",TMath::Abs(myetfunction->GetParameter(1)));
      parastring += parachar;
      if(ParticleID == 0 || ParticleID == 1)l.DrawLatex(50.,0.09,etastring +"#sqrt{"+ parastring);
      if(ParticleID == 2 || ParticleID == 3)l.DrawLatex(50.,0.0027,etastring +"#sqrt{"+ parastring);
    }
    else if((TString)myetfunction->GetName()=="erf"){
      sprintf(parachar,"^{2}+%1.4f^{2}/E_{T}^{2}}#upointerf(",TMath::Abs(myetfunction->GetParameter(1)));
      parastring += parachar;
      sprintf(parachar,"%1.4f#upointE_{T})",TMath::Abs(myetfunction->GetParameter(2)));
      parastring += parachar;
      l.DrawLatex(50.,2.2,etastring +"#sqrt{"+ parastring);
    }
    else if((TString)myetfunction->GetName()=="linear"){
      sprintf(parachar,"+%1.6f#upointE_{T}",myetfunction->GetParameter(1));
      parastring += parachar;
      l.DrawLatex(50.,0.09,etastring +parastring);
    }
    else{
      l.DrawLatex(50.,0.09,"unknown function");
    }
    
    double par0 = rounderror(myetfunction->GetParameter(0), myetfunction->GetParameter(0), 3, false);
    double par1 = rounderror(myetfunction->GetParameter(1), myetfunction->GetParameter(1), 3, false);
    double par2 = rounderror(myetfunction->GetParameter(2), myetfunction->GetParameter(2), 3, false);

//     if(par0 == 0. && par1 == 0. && par2 == 0.){
//       par0 = rounderror(myetfunction->GetParameter(0), myetfunction->GetParError(0), 2, true);
//       par1 = rounderror(myetfunction->GetParameter(1), myetfunction->GetParError(1), 2, true);
//       par2 = rounderror(myetfunction->GetParameter(2), myetfunction->GetParError(2), 2, true);
//     }

    Char_t tmpformulachar0[10];
    sprintf(tmpformulachar0,"%g",TMath::Abs(par0));
    Char_t tmpformulachar1[10];
    sprintf(tmpformulachar1,"%g",TMath::Abs(par1));
    Char_t tmpformulachar2[10];
    sprintf(tmpformulachar2,"%g",TMath::Abs(par2));

    if(i_etetaphi==0){
      etstrg  = "    cms.PSet(\n";
      if(ParticleID != 4)etstrg += "    bin = cms.string('"+loweretastring+"<=abs(eta) && abs(eta)<"+upperetastring+"'),\n";
      if((TString)myetfunction->GetName()=="complete")
	etstrg += "    et  = cms.string('et * (sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/sqrt(et))^2 + ("+TString(tmpformulachar2)+"/et)^2))'),\n";
      else if((TString)myetfunction->GetName()=="reduced")
	etstrg += "    et  = cms.string('et * (sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/et)^2))'),\n";
      else if((TString)myetfunction->GetName()=="linear")
	etstrg += "    et  = cms.string('et * ("+TString(tmpformulachar0)+" + "+TString(tmpformulachar1)+" * et)'),\n";
      else if((TString)myetfunction->GetName()=="erf")
	etstrg += "    et  = cms.string('et * (sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/et)^2)) * erf("+TString(tmpformulachar2)+"*et)'),\n";
      else 
	etstrg += "    et  = cms.string('unknown function'),\n";
    }
    if(i_etetaphi==1){
      if((TString)myetfunction->GetName()=="complete")
	etastrg = "    eta  = cms.string('sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/sqrt(et))^2 + ("+TString(tmpformulachar2)+"/et)^2)'),\n";
      else if((TString)myetfunction->GetName()=="reduced")
	etastrg = "    eta  = cms.string('sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/et)^2)'),\n";
      else 
	etastrg = "    eta  = cms.string('unknown function'),\n";
    }
    if(i_etetaphi==2){
      if((TString)myetfunction->GetName()=="complete")
	phistrg = "    phi  = cms.string('sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/sqrt(et))^2 + ("+TString(tmpformulachar2)+"/et)^2)'),\n";
      else if((TString)myetfunction->GetName()=="reduced")
	phistrg = "    phi  = cms.string('sqrt("+TString(tmpformulachar0)+"^2 + ("+TString(tmpformulachar1)+"/et)^2)'),\n";
      else 
	phistrg = "    phi  = cms.string('unknown function'),\n";

      phistrg += "    ),\n";
    }
    if(i_etetaphi==0)etvec.push_back(etstrg);
    if(i_etetaphi==1)etavec.push_back(etastrg);
    if(i_etetaphi==2)phivec.push_back(phistrg);

    if(i_etetaphi==0){
      if(ieta%2==1 || (etabinNum%2==1 && ieta==etabinNum-1)){
      bet->Print("binnedetbincentercorr"+identifier+namestring+".eps");
      bet->Clear();
    }
    }
    else if(i_etetaphi==1 && ParticleID!=4){
      if(ieta%2==1 || (etabinNum%2==1 && ieta==etabinNum-1)){
      beteta->Print("binnedetbincentercorr_eta"+identifier+namestring+".eps");
      beteta->Clear();
    }
    }
    else if(i_etetaphi==2)if(ieta%2==1 || (etabinNum%2==1 && ieta==etabinNum-1)){
      betphi->Print("binnedetbincentercorr_phi"+identifier+namestring+".eps");
      betphi->Clear();
    }
  }
  delete eteta;
  delete etetadraw;
  delete eteta2;
  }
  ofstream res_file;
  res_file.open ("resolution_"+identifier+"_cff.py");

  if(ParticleID==0)res_file<<"udscResolutionPF = stringResolution.clone(parametrization = 'EtEtaPhi',\n";
  if(ParticleID==1)res_file<<"bjetResolutionPF = stringResolution.clone(parametrization = 'EtEtaPhi',\n";
  if(ParticleID==2)res_file<<"elecResolution   = stringResolution.clone(parametrization = 'EtEtaPhi',\n";
  if(ParticleID==3)res_file<<"muonResolution   = stringResolution.clone(parametrization = 'EtEtaPhi',\n";
  if(ParticleID==4)res_file<<"metResolutionPF  = stringResolution.clone(parametrization = 'EtEtaPhi',\n";
  res_file<<"                                          functions = cms.VPSet(\n";

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    res_file<<etvec[ieta];
    res_file<<etavec[ieta];
    res_file<<phivec[ieta];
  }
  res_file<<"    ),\n";
  res_file<<"                                          constraints = cms.vdouble(0)\n";
  res_file<<"                                          )\n";
  res_file.close();
  return 0;
}

Double_t rounderror(Double_t value, Double_t error, Int_t k = 2, bool highUncert = false)
{
  Double_t tempo = 0.;
  Double_t start = 1e20;
  Double_t ex = 0.0;
  Double_t ex2 = 0.0;
  Double_t blaexp = pow(10.0,-(k-1));
  bool warning = false;

  for(Int_t i = 0; i < 40; i++)
    {
      ex = start*pow(10.0,-1*i);
      Int_t tmp = static_cast<Int_t>(error/ex);
      if(tmp != 0 || value==0.)break;
    }
  tempo = static_cast<Int_t>(value/(ex*blaexp)+0.5)*ex*blaexp;
  if(highUncert && value!=0.)
    for(Int_t i = 1; i < 40; i++){
      if(tempo!=0.)break;
      warning = true;
      ex2 = pow(10.0,-1*i);
      tempo = static_cast<Int_t>(value/(ex*blaexp*ex2)+0.5)*ex*blaexp*ex2;
    }

  if(warning)std::cout<<"Warning: uncertainty is larger than the value!"<<std::endl;
  return tempo;
}
