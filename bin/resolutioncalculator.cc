#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TH2F.h"
#include "TChain.h"
#include "TLatex.h"
#include "TF2.h"
#include "TGraphErrors.h"
#include "TMath.h"

using namespace std;

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
  TFile* file = new TFile("resol"+identifier+"histos.root");

  Int_t etbinNum;
  if(ParticleID==0 || ParticleID==1)etbinNum = 25;
  else if(ParticleID==4)etbinNum = 41;
  else etbinNum = 10;
  Int_t etstartbin = 0;
  if(argc>=3)etstartbin = atoi(argv[2]);
  else etstartbin = 3;
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
  //Double_t Leptonbinning[10] = {0.,4.,10.,18.,28.,40.,54.,70.,100.,500.};
  Double_t Electronbinning[11] = {0.,4.,10.,18.,26.,34.,42.,50.,65.,85.,500.};
  Double_t Muonbinning[11] = {0.,4.,6.,9.,13.,19.,28.,40.,60.,85.,500.};
  Double_t METbinning[42] = {0., 2., 5., 8., 11., 13., 16., 19., 22., 25., 28., 32., 36., 40., 44., 48., 52., 56., 60., 65., 70., 75., 80., 86., 92., 98., 104., 110., 116., 122., 128., 135., 142., 150., 160., 170., 180., 200., 240., 300., 400., 500.};
//   Double_t METbinning[40] = {0., 270., 310., 330., 350., 370., 390., 410., 420., 430., 440., 450., 460., 470., 480., 490., 500., 510., 520., 530., 540., 550., 560., 570., 580., 590., 610., 630., 650., 670., 690., 710., 730., 760., 800., 850., 910., 1010., 1300., 2000.};
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
  TCanvas* justanothercanvas = new TCanvas("justanothercanvas","justanothercanvas",400,500);
  TCanvas* etetacanvas = new TCanvas("etetacanvas","etetacanvas",700,900);
  etetacanvas->Divide(1,2);
  TCanvas* etetacanvas2 = new TCanvas("etetacanvas2","etetacanvas2",900,500);
  TCanvas* chi2canvas = new TCanvas("chi2canvas","chi2canvas",500,900);
  chi2canvas->Divide(1,3);
  TCanvas* chi2distributioncan = new TCanvas("chi2distributioncan","chi2distributioncan",900,500);

  TCanvas* beteta = new TCanvas("beteta","beteta",700,900);
  TCanvas* justanothercanvaseta = new TCanvas("justanothercanvaseta","justanothercanvaseta",400,500);
  TCanvas* etetacanvaseta = new TCanvas("etetacanvaseta","etetacanvaseta",700,900);
  etetacanvaseta->Divide(1,2);
  TCanvas* etetacanvas2eta = new TCanvas("etetacanvas2eta","etetacanvas2eta",900,500);
  TCanvas* chi2canvaseta = new TCanvas("chi2canvaseta","chi2canvaseta",500,900);
  chi2canvaseta->Divide(1,3);
  TCanvas* chi2distributioncaneta = new TCanvas("chi2distributioncaneta","chi2distributioncaneta",900,500);

  TCanvas* betphi = new TCanvas("betphi","betphi",700,900);
  TCanvas* justanothercanvasphi = new TCanvas("justanothercanvasphi","justanothercanvasphi",400,500);
  TCanvas* etetacanvasphi = new TCanvas("etetacanvasphi","etetacanvasphi",700,900);
  etetacanvasphi->Divide(1,2);
  TCanvas* etetacanvas2phi = new TCanvas("etetacanvas2phi","etetacanvas2phi",900,500);
  TCanvas* chi2canvasphi = new TCanvas("chi2canvasphi","chi2canvasphi",500,900);
  chi2canvasphi->Divide(1,3);
  TCanvas* chi2distributioncanphi = new TCanvas("chi2distributioncanphi","chi2distributioncanphi",900,500);

  for(Int_t i_etetaphi = 0; i_etetaphi < 3; i_etetaphi++){
  TF1* myetfunction = 0;

  // define 1D functions in E_T here!
  if(ParticleID == 3 && i_etetaphi==0)myetfunction = new TF1("myetfunction","[0]+[1]*(x)",etbinning[etstartbin],1800.);//muon ET resolution
  else myetfunction = new TF1("myetfunction","sqrt([0]*[0]+[1]*[1]/(x)+[2]*[2]/(x*x))",etbinning[etstartbin],1800.);

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
    axeshisto->GetXaxis()->SetTitle("E_{T} [GeV]");
    axeshisto->GetYaxis()->SetTitle("#sigma(E_{T}^{rec}/E_{T}^{gen})");
    if(i_etetaphi==1)axeshisto->GetYaxis()->SetTitle("#sigma(#eta^{rec})");
    if(i_etetaphi==2)axeshisto->GetYaxis()->SetTitle("#sigma(#phi^{rec})");
    axeshisto->Draw("axis");
  
    bGraph[ieta]->SetMarkerStyle(20);
    bGraph[ieta]->SetMarkerSize(0.8);

    myetfunction->SetLineWidth(1);
    myetfunction->SetLineColor(kRed);

//     TF1* myref;
//     if(ieta == 0 || ieta == 1){
//       if(ParticleID == 0){
// 	if(i_etetaphi==0)myref = new TF1("myref","8.04345/x+0.0705188",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==1)myref = new TF1("myref","0.038575+0.129026*exp(-(0.0427157*x))",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==2)myref = new TF1("myref","0.025945+0.16529*exp(-(0.027385*x))",etbinning[etstartbin],1800.);
//       }
//       if(ParticleID == 1){
// 	if(i_etetaphi==0)myref = new TF1("myref","10.1445/x+0.0445196",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==1)myref = new TF1("myref","0.0354411+0.130211*exp(-(0.0423321*x))",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==2)myref = new TF1("myref","0.0217379+0.157228*exp(-(0.0237049*x))",etbinning[etstartbin],1800.);
//       }
//       if(ParticleID == 2){
// 	if(i_etetaphi==0)myref = new TF1("myref","0.326238/x+0.00760789",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==1)myref = new TF1("myref","0.000266154+0.000104322*exp(-(0.0140464*x))",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==2)myref = new TF1("myref","0.000175676+0.000471783*exp(-(0.0383161*x))",etbinning[etstartbin],1800.);
//       }
//       if(ParticleID == 3){
// 	if(i_etetaphi==0)myref = new TF1("myref","-0.0552605/x+0.0115814",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==1)myref = new TF1("myref","0.000322451+0.000107167*exp(-(0.0156347*x))",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==2)myref = new TF1("myref","7.21523e-05+0.000293781*exp(-(0.0518546*x))",etbinning[etstartbin],1800.);
//       }
//       if(ParticleID == 4){
// 	if(i_etetaphi==0)myref = new TF1("myref","11.7801/x+0.145218",etbinning[etstartbin],1800.);
// 	if(i_etetaphi==2)myref = new TF1("myref","0.201336+1.53501*exp(-(0.0216707*x))",etbinning[etstartbin],1800.);
//       }
//     }

    bGraph[ieta]->Fit("myetfunction","+QR");
    bGraph[ieta]->DrawClone("pe1same");
    myetfunction->SetRange(0,500);
    myetfunction->DrawCopy("same");
    //if(ieta == 0 || ieta == 1)myref->DrawCopy("same");
    cout << "myetfunction: " << myetfunction->GetExpFormula("p") << ", Prob: " << myetfunction->GetProb() << endl;
    TLatex l;
    l.SetTextAlign(11);
    l.SetTextSize(0.055);

    TString namestring;
    Char_t namechar[10];
    sprintf(namechar,"%i",(ieta+1)/2);
    namestring = namechar;
    TString etastring;
    TString parastring;
    Char_t parachar[10];
    sprintf(parachar,"%1.3f",etabinning[ieta]);
    etastring = parachar;
    etastring += "<|#eta|<";
    sprintf(parachar,"%1.3f",etabinning[ieta+1]);
    etastring += parachar;
    etastring += ": ";

    sprintf(parachar,"%1.5f",myetfunction->GetParameter(0));
    parastring = parachar;

    if(ParticleID == 3 && i_etetaphi==0)sprintf(parachar,"+%1.6f#upointE_{T}",myetfunction->GetParameter(1));
    else{
      sprintf(parachar,"^{2}+%1.4f^{2}/E_{T}+",myetfunction->GetParameter(1));
      parastring += parachar;
      sprintf(parachar,"%1.4f",myetfunction->GetParameter(2));
      parastring += parachar;
    }
    if(ParticleID == 3&& i_etetaphi==0)l.DrawLatex(50.,0.09,etastring +parastring);
    else if(ParticleID == 0 || ParticleID == 1){
      if(i_etetaphi==0)l.DrawLatex(50.,0.4,etastring + "#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
      else l.DrawLatex(50.,0.09,etastring + "#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
    }
    else if(ParticleID == 2){
      if(i_etetaphi==0)l.DrawLatex(50.,0.09,etastring + "#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
      else l.DrawLatex(50.,0.0027,etastring + "#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
    }
    else if(ParticleID == 3)l.DrawLatex(50.,0.0027,etastring + "#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
    else{
      if(i_etetaphi!=1)l.DrawLatex(50.,1.7,"#sqrt{" + parastring + "^{2}/E_{T}^{2}}");
    }

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
    if(ParticleID != 4){
      Double_t sigmaError = 1000.;
      if(i_etetaphi==0)etetacanvas->cd(1);
      else if(i_etetaphi==1)etetacanvaseta->cd(1);
      else if(i_etetaphi==2)etetacanvasphi->cd(1);
      for(Int_t iet = 0; iet < etbinNum; iet++){
	eteta->Fill((etbinning[iet]+etbinning[iet+1])/2,(etabinning[ieta]+etabinning[ieta+1])/2,myetfunction->Eval((etbinning[iet]+etbinning[iet+1])/2));
	sigmaError = bGraph[ieta]->GetErrorY(iet);
	eteta->SetBinError(iet+1,ieta+1,sigmaError);
	//eteta->SetBinError(iet+1,ieta+1,sqrt(TMath::Power(myetfunction->GetParError(0),2)+TMath::Power(myetfunction->GetParError(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2),2)));
      }
      for(Int_t iet = 0; iet < etbinNum; iet++){
	sigmaError = bGraph[ieta]->GetErrorY(iet);
	for(Int_t iiet = 0; iiet < 100; iiet++){
	  etetadraw->Fill(etbinning[iet]+(etbinning[iet+1]-etbinning[iet])*iiet/100.,(etabinning[ieta]+etabinning[ieta+1])/2,myetfunction->Eval(etbinning[iet]+(etbinning[iet+1]-etbinning[iet])*iiet/100.));
	  etetadraw->SetBinError((iet*10)+1+iiet,ieta+1,sigmaError);
	  //eteta->SetBinError(iet+1,ieta+1,sqrt(TMath::Power(myetfunction->GetParError(0),2)+TMath::Power(myetfunction->GetParError(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2),2)));
	}
      }
    }
  }
  TF2* mybarrelfunction = 0;
  TF2* myendcapfunction = 0;

  // define 2D functions in E_T and eta for barrel and endcap here!

  Double_t crack = 0.;
  if(ParticleID == 3 && i_etetaphi==0)crack = 1.1;
  else if(ParticleID == 2 || ParticleID == 3)crack = 0.9;
  else crack = 1.479;
  Char_t barrelfktchar[60];
  Char_t endcapfktchar[100];
  if(ParticleID == 3 && i_etetaphi==0){
    sprintf(barrelfktchar,"[0]+[1]*x+[2]*y*y");
    sprintf(endcapfktchar,"[0]+[1]*x+[2]*%1.3f*%1.3f+[3]*x*(y-%1.3f)+[4]*(y-%1.3f)",crack,crack,crack,crack);
  }
  else if(i_etetaphi==0){
    sprintf(barrelfktchar,"sqrt([0]+[1]/(x)+[2]/(x*x))+[3]*y*y");
    sprintf(endcapfktchar,"(sqrt([0]+[1]/(x)+[2]/(x*x))+[3]*%1.3f*%1.3f)+[4]*(y*y-%1.3f*%1.3f)+[5]*(y-%1.3f)+[6]*(y-%1.3f)/sqrt(x)",crack,crack,crack,crack,crack,crack);
  }
  else{
    sprintf(barrelfktchar,"[0]*[0]+[1]/(x)+[2]*y*y");
    sprintf(endcapfktchar,"[0]*[0]+[1]/(x)+[2]*%1.3f*%1.3f+[3]*(y*y-%1.3f*%1.3f)+[4]*(y-%1.3f)+[5]*(y-%1.3f)/sqrt(x)",crack,crack,crack,crack,crack,crack);
  }
  //TString crackstrg = crackchar;

//   if(ParticleID == 3 && i_etetaphi==0)mybarrelfunction = new TF2("mybarrelfunction","[0]+[1]*x+[2]*y",etbinning[etstartbin],500.,0.,crack);
//   else mybarrelfunction = new TF2("mybarrelfunction","sqrt([0]+[1]/(x)+[2]/(x*x))+[3]*y*y",etbinning[etstartbin],500.,0.,crack);//"[0]+[1]/sqrt(x)+[2]*y*y/(x)"
  mybarrelfunction = new TF2("mybarrelfunction",barrelfktchar,etbinning[etstartbin],500.,0.,crack);
  myendcapfunction = new TF2("myendcapfunction",endcapfktchar,etbinning[etstartbin],500.,crack,2.5);


//   if(ParticleID == 3 && i_etetaphi==0)myendcapfunction = new TF2("myendcapfunction","[0]+[1]*x+[2]*"+crackstrg+"+[3]*x*(y-"+crackstrg+")+[4]*(y-"+crackstrg+")",etbinning[etstartbin],500.,crack,2.5);
//   else if((ParticleID == 2 || ParticleID == 3) && i_etetaphi!=0)myendcapfunction = new TF2("myendcapfunction","(sqrt([0]+[1]/(x)+[2]/(x*x))+[3]*"+crackstrg+"*"+crackstrg+")+[4]*(y*y-"+crackstrg+"*"+crackstrg+")+[5]*(y-"+crackstrg+")+[6]*(y-"+crackstrg+")/sqrt(x)",etbinning[etstartbin],500.,crack,2.5);
//   else myendcapfunction = new TF2("myendcapfunction","(sqrt([0]+[1]/(x)+[2]/(x*x))+[3]*1.479*1.479)+[4]*(y*y-1.479*1.479)+[5]*(y-1.479)+[6]*(y-1.479)/sqrt(x)",etbinning[etstartbin],500.,crack,2.5);//"sqrt([0]+[1]/(x)+[2]*1.479*1.479*1.479*1.479/(x*x)+[3]*(y*y-1.479*1.479)/(x)+[4]*(y*y*y*y-1.479*1.479*1.479*1.479)/(x*x))"    ,    "[0]+[1]/sqrt(x)+[2]*y/sqrt(x)"    ,    "[0]+[1]/sqrt(x)+[2]*1.479*1.479/(x)+[3]*(y-1.479)/sqrt(x)+[4]*(y*y-1.479*1.479)/x"

  if(ParticleID != 4){
    eteta->Fit("mybarrelfunction","+R0m");
    myendcapfunction->FixParameter(0,mybarrelfunction->GetParameter(0));
    myendcapfunction->FixParameter(1,mybarrelfunction->GetParameter(1));
    myendcapfunction->FixParameter(2,mybarrelfunction->GetParameter(2));
    if(i_etetaphi==0 && ParticleID != 3){
      myendcapfunction->FixParameter(3,mybarrelfunction->GetParameter(3));
    }
    eteta->Fit("myendcapfunction","+R0m");
    
    etetadraw->GetXaxis()->SetRangeUser(etbinning[etstartbin],500);
    if(i_etetaphi==0)etetadraw->SetTitle("E_{T} resolution from 1D fits");
    if(i_etetaphi==1)etetadraw->SetTitle("#eta resolution from 1D fits");
    if(i_etetaphi==2)etetadraw->SetTitle("#phi resolution from 1D fits");
    etetadraw->GetXaxis()->SetTitle("E_{T} [GeV]");
    etetadraw->GetYaxis()->SetTitle("#eta");
    gPad->SetRightMargin(0.14);
    etetadraw->Draw("colz");
    //etetacanvas->Print("binned1Dfit_eteta.eps");
  }

  TH1D* chi2probhisto = new TH1D("chi2probhisto","chi2probhisto",50,0.,1.);
  TH1D* barrelchi2probhisto = new TH1D("barrelchi2probhisto","barrelchi2probhisto",50,0.,1.);
  TH1D* endcapchi2probhisto = new TH1D("endcapchi2probhisto","endcapchi2probhisto",50,0.,1.);
  TH2D* chi2distribution = new TH2D("chi2distribution","chi2distribution",etbinNum,&(etbinning.front()),etabinNum,&(etabinning.front()));
  TH1D* justanotherhisto = new TH1D("justanotherhisto","justanotherhisto",50,-5.,5.);
  Double_t chi2 = 0.;
  Double_t barrelchi2 = 0.;
  Double_t endcapchi2 = 0.;
  Int_t barreletabins = 0;
  Int_t endcapetabins = 0;

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    for(Int_t iet = etstartbin; iet < etbinNum; iet++){
      Double_t et = 0.;
      Double_t sigma = 0.;
      bGraph[ieta]->GetPoint(iet,et,sigma);
      Double_t sigmaError = bGraph[ieta]->GetErrorY(iet);
      Double_t myMET_sigma = myetfunction->Eval(et);
      Double_t mybarrel_sigma = mybarrelfunction->Eval(et,(etabinning[ieta]+etabinning[ieta+1])/2);
      Double_t myendcap_sigma = myendcapfunction->Eval(et,(etabinning[ieta]+etabinning[ieta+1])/2);
      if(ParticleID != 4){
      if(etabinning[ieta]<crack){
	if(iet==etbinNum-1)barreletabins++;
	chi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-mybarrel_sigma)/sigmaError,2),1));
	barrelchi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-mybarrel_sigma)/sigmaError,2),1));
	chi2 += TMath::Power((sigma-mybarrel_sigma)/sigmaError,2);
	barrelchi2 += TMath::Power((sigma-mybarrel_sigma)/sigmaError,2);
	chi2distribution->SetBinContent(iet+1,ieta+1,(sigma-mybarrel_sigma)/sigmaError);
	justanotherhisto->Fill((sigma-mybarrel_sigma)/sigmaError);
	for(Int_t iieta = 0; iieta < 10; iieta++){
	  for(Int_t iiieta = 0; iiieta < 100; iiieta++){
	    Double_t mybarrel_sigma2 = mybarrelfunction->Eval(etbinning[iet]+(etbinning[iet+1]-etbinning[iet])*iiieta/100.,etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.);
	    eteta2->SetBinContent((100*iet)+iiieta+1,(10*ieta)+iieta+1,mybarrel_sigma2);
	  }
	}
      }
      else{
	if(iet==etbinNum-1)endcapetabins++;
	chi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-myendcap_sigma)/sigmaError,2),1));
	endcapchi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-myendcap_sigma)/sigmaError,2),1));
	chi2 += TMath::Power((sigma-myendcap_sigma)/sigmaError,2);
	endcapchi2 += TMath::Power((sigma-myendcap_sigma)/sigmaError,2);
	chi2distribution->SetBinContent(iet+1,ieta+1,(sigma-myendcap_sigma)/sigmaError);
	justanotherhisto->Fill((sigma-myendcap_sigma)/sigmaError);
	for(Int_t iieta = 0; iieta < 10; iieta++){
	  for(Int_t iiieta = 0; iiieta < 100; iiieta++){
	    Double_t myendcap_sigma2 = myendcapfunction->Eval(etbinning[iet]+(etbinning[iet+1]-etbinning[iet])*iiieta/100.,etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.);
	    eteta2->SetBinContent((100*iet)+iiieta+1,(10*ieta)+iieta+1,myendcap_sigma2);
	  }
	}
      }
      }
      else{
	chi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-myMET_sigma)/sigmaError,2),1));
	chi2 += TMath::Power((sigma-myMET_sigma)/sigmaError,2);
	justanotherhisto->Fill((sigma-myMET_sigma)/sigmaError);
      }
    }
  }
  if(i_etetaphi==0)justanothercanvas->cd();
  else if(i_etetaphi==1)justanothercanvaseta->cd();
  else if(i_etetaphi==2)justanothercanvasphi->cd();
  justanotherhisto->Draw("e");
  if(i_etetaphi==0)chi2canvas->cd(1);
  else if(i_etetaphi==1)chi2canvaseta->cd(1);
  else if(i_etetaphi==2)chi2canvasphi->cd(1);
  chi2probhisto->Draw("e");
  if(i_etetaphi==0)chi2canvas->cd(2);
  else if(i_etetaphi==1)chi2canvaseta->cd(2);
  else if(i_etetaphi==2)chi2canvasphi->cd(2);
  barrelchi2probhisto->Draw("e");
  if(i_etetaphi==0)chi2canvas->cd(3);
  else if(i_etetaphi==1)chi2canvaseta->cd(3);
  else if(i_etetaphi==2)chi2canvasphi->cd(3);
  endcapchi2probhisto->Draw("e");

  if(ParticleID != 4){
  if(i_etetaphi==0)etetacanvas->cd(2);
  else if(i_etetaphi==1)etetacanvaseta->cd(2);
  else if(i_etetaphi==2)etetacanvasphi->cd(2);
  eteta2->GetXaxis()->SetRangeUser(etbinning[etstartbin],500);
  if(i_etetaphi==0)eteta2->SetTitle("E_{T} resolution from 2D fit");
  if(i_etetaphi==1)eteta2->SetTitle("#eta resolution from 2D fit");
  if(i_etetaphi==2)eteta2->SetTitle("#phi resolution from 2D fit");
  eteta2->GetXaxis()->SetTitle("E_{T} [GeV]");
  eteta2->GetYaxis()->SetTitle("#eta");
  gPad->SetRightMargin(0.14);
  eteta2->Draw("colz");
  if(i_etetaphi==0){
  etetacanvas->Print("binnedfit"+identifier+"_eteta.eps");
  etetacanvas->Print("binnedfit"+identifier+"_eteta.root");
  chi2distributioncan->cd();
  }
  else if(i_etetaphi==1){
  etetacanvaseta->Print("binnedfiteta"+identifier+"_eteta.eps");
  etetacanvaseta->Print("binnedfiteta"+identifier+"_eteta.root");
  chi2distributioncaneta->cd();
  }
  else if(i_etetaphi==2){
  etetacanvasphi->Print("binnedfitphi"+identifier+"_eteta.eps");
  etetacanvasphi->Print("binnedfitphi"+identifier+"_eteta.root");
  chi2distributioncanphi->cd();
  }
  chi2distribution->GetXaxis()->SetRangeUser(etbinning[etstartbin],500);
  chi2distribution->SetTitle("normalized residuals");
  chi2distribution->GetXaxis()->SetTitle("E_{T} [GeV]");
  chi2distribution->GetYaxis()->SetTitle("#eta");
  gPad->SetRightMargin(0.14);
  chi2distribution->Draw("colz");
  if(i_etetaphi==0){
    chi2distributioncan->Print("normresdistribution"+identifier+".eps");
    cout << "ET resolution:" << endl;
  }
  else if(i_etetaphi==1){
    chi2distributioncaneta->Print("normresdistributioneta"+identifier+".eps");
    cout << "eta resolution:" << endl;
  }
  else if(i_etetaphi==2){
    chi2distributioncanphi->Print("normresdistributionphi"+identifier+".eps");
    cout << "phi resolution:" << endl;
  }

  cout << "barrelfunction: " << mybarrelfunction->GetExpFormula("p") << endl;
  cout << "endcapfunction: " << myendcapfunction->GetExpFormula("p") << endl;

  cout << "barrel: chi^2/Ndof = " << barrelchi2 << "/" << mybarrelfunction->GetNDF() << " = " << barrelchi2/mybarrelfunction->GetNDF() << endl;
  cout << "endcap: chi^2/Ndof = " << endcapchi2 << "/" << myendcapfunction->GetNDF() << " = " << endcapchi2/myendcapfunction->GetNDF() << endl;
  Double_t allNdof = mybarrelfunction->GetNDF()+myendcapfunction->GetNDF();
  cout << "chi^2/Ndof = " << chi2 << "/" << allNdof << " = " << chi2/allNdof << endl;
  }
  else if(i_etetaphi!=1){
    if(i_etetaphi==0)cout << "ET resolution:" << endl;
    else if(i_etetaphi==2)cout << "phi resolution:" << endl;
    cout << "function: " << myetfunction->GetExpFormula("p") << endl;

    Double_t chi2perNdof = chi2/((etbinNum-etstartbin)-myetfunction->GetNumberFreeParameters());
    cout << "chi^2/Ndof = " << chi2 << "/" << ((etbinNum-etstartbin)-myetfunction->GetNumberFreeParameters()) << " = " << chi2perNdof << endl;
  }
  delete eteta;
  delete etetadraw;
  delete eteta2;
  delete chi2probhisto;
  delete barrelchi2probhisto;
  delete endcapchi2probhisto;
  delete chi2distribution;
  delete justanotherhisto;
  }
  return 0;
}
