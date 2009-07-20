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

  gStyle->SetOptStat(0);

  Bool_t bJets = 0;
  if(argc>=2)bJets = argv[1];
  TFile* file = 0;
  if(bJets)file = new TFile("resolbhistos.root");
  else file = new TFile("resolnonbhistos.root");

  const Int_t etbinNum = 25;
  Int_t etstartbin = 0;
  if(argc>=3)etstartbin = atoi(argv[2]);
  else etstartbin = 3;
  const Int_t etabinNum = 26;
  TString etahname[etabinNum];
  Double_t etabinning[27] = {0.0, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.87, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5};
  Double_t etabinning2[261] = {0, 0.0087, 0.0174, 0.0261, 0.0348, 0.0435, 0.0522, 0.0609, 0.0696, 0.0783, 0.087, 0.0957, 0.1044, 0.1131, 0.1218, 0.1305, 0.1392, 0.1479, 0.1566, 0.1653, 0.174, 0.1827, 0.1914, 0.2001, 0.2088, 0.2175, 0.2262, 0.2349, 0.2436, 0.2523, 0.261, 0.2697, 0.2784, 0.2871, 0.2958, 0.3045, 0.3132, 0.3219, 0.3306, 0.3393, 0.348, 0.3567, 0.3654, 0.3741, 0.3828, 0.3915, 0.4002, 0.4089, 0.4176, 0.4263, 0.435, 0.4437, 0.4524, 0.4611, 0.4698, 0.4785, 0.4872, 0.4959, 0.5046, 0.5133, 0.522, 0.5307, 0.5394, 0.5481, 0.5568, 0.5655, 0.5742, 0.5829, 0.5916, 0.6003, 0.609, 0.6177, 0.6264, 0.6351, 0.6438, 0.6525, 0.6612, 0.6699, 0.6786, 0.6873, 0.696, 0.7047, 0.7134, 0.7221, 0.7308, 0.7395, 0.7482, 0.7569, 0.7656, 0.7743, 0.783, 0.7917, 0.8004, 0.8091, 0.8178, 0.8265, 0.8352, 0.8439, 0.8526, 0.8613, 0.87, 0.8787, 0.8874, 0.8961, 0.9048, 0.9135, 0.9222, 0.9309, 0.9396, 0.9483, 0.957, 0.9657, 0.9744, 0.9831, 0.9918, 1.0005, 1.0092, 1.0179, 1.0266, 1.0353, 1.044, 1.0527, 1.0614, 1.0701, 1.0788, 1.0875, 1.0962, 1.1049, 1.1136, 1.1223, 1.131, 1.1397, 1.1484, 1.1571, 1.1658, 1.1745, 1.1832, 1.1919, 1.2006, 1.2093, 1.218, 1.2267, 1.2354, 1.2441, 1.2528, 1.2615, 1.2702, 1.2789, 1.2876, 1.2963, 1.305, 1.3137, 1.3224, 1.3311, 1.3398, 1.3485, 1.3572, 1.3659, 1.3746, 1.3833, 1.392, 1.4007, 1.4094, 1.4181, 1.4268, 1.4355, 1.4442, 1.4529, 1.4616, 1.4703, 1.479, 1.4877, 1.4964, 1.5051, 1.5138, 1.5225, 1.5312, 1.5399, 1.5486, 1.5573, 1.566, 1.5747, 1.5834, 1.5921, 1.6008, 1.6095, 1.6182, 1.6269, 1.6356, 1.6443, 1.653, 1.6617, 1.6704, 1.6791, 1.6878, 1.6965, 1.7052, 1.7139, 1.7226, 1.7313, 1.74, 1.749, 1.758, 1.767, 1.776, 1.785, 1.794, 1.803, 1.812, 1.821, 1.83, 1.84, 1.85, 1.86, 1.87, 1.88, 1.89, 1.9, 1.91, 1.92, 1.93, 1.9413, 1.9526, 1.9639, 1.9752, 1.9865, 1.9978, 2.0091, 2.0204, 2.0317, 2.043, 2.0559, 2.0688, 2.0817, 2.0946, 2.1075, 2.1204, 2.1333, 2.1462, 2.1591, 2.172, 2.187, 2.202, 2.217, 2.232, 2.247, 2.262, 2.277, 2.292, 2.307, 2.322, 2.3398, 2.3576, 2.3754, 2.3932, 2.411, 2.4288, 2.4466, 2.4644, 2.4822, 2.5};
  Double_t etbinning[26] = {0.,20.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75.,80.,85.,90.,95.,100.,110.,120.,130.,150.,180.,230.,300.,500.};

  TGraphErrors* bGraph[etabinNum];

  TH1D* axeshisto = new TH1D("axeshisto","axeshisto",25,etbinning);
    
  TF1* myetfunction = new TF1("myetfunction","[0]+[1]/sqrt(x)",etbinning[etstartbin],1800.);

  file->cd();
  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    TString loweretastrg;
    TString upperetastrg;
    Char_t etachar[10];
    sprintf(etachar,"%1.3f",etabinning[ieta]);
    loweretastrg = etachar;
    sprintf(etachar,"%1.3f",etabinning[ieta+1]);
    upperetastrg = etachar;
    etahname[ieta] = "h"+loweretastrg+"_"+upperetastrg;

    bGraph[ieta] = (TGraphErrors*) (file->Get(etahname[ieta]))->Clone();
  }

  TCanvas* bet = new TCanvas("bet","bet",700,900);
  TCanvas* justanothercanvas = new TCanvas("justanothercanvas","justanothercanvas",400,500);

  axeshisto->SetMaximum(0.45);

  TCanvas* etetacanvas = new TCanvas("etetacanvas","etetacanvas",900,500);
  TCanvas* etetacanvas2 = new TCanvas("etetacanvas2","etetacanvas2",900,500);

  TH2D* eteta = new TH2D("eteta","eteta",etbinNum,etbinning,etabinNum,etabinning);
  TH2D* eteta2 = new TH2D("eteta2","eteta2",etbinNum,etbinning,etabinNum*10,etabinning2);

  for(Int_t ieta = 0; ieta < etabinNum; ieta++){
    if(ieta%2==0){
      bet->Divide(1,2);
      bet->cd(1);
    }
    else bet->cd(2);
    axeshisto->Draw("axis");
  
    bGraph[ieta]->SetMarkerStyle(20);
    bGraph[ieta]->SetMarkerSize(0.8);

    myetfunction->SetLineWidth(1);

    bGraph[ieta]->Fit("myetfunction","+RQ");
    bGraph[ieta]->DrawClone("pe1same");
    myetfunction->DrawCopy("same");

    TLatex l;
    l.SetTextAlign(11);
    l.SetTextSize(0.07);

    TString namestring;
    Char_t namechar[10];
    sprintf(namechar,"%i",(ieta+1)/2);
    namestring = namechar;
    TString parastring;
    Char_t parachar[10];
    sprintf(parachar,"%1.3f",etabinning[ieta]);
    parastring = parachar;
    parastring += "<|#eta|<";
    sprintf(parachar,"%1.3f",etabinning[ieta+1]);
    parastring += parachar;
    parastring += ": ";
    sprintf(parachar,"%1.4f",myetfunction->GetParameter(0));
    parastring += parachar;

    parastring += "+";
    sprintf(parachar,"%1.4f",myetfunction->GetParameter(1));
    parastring += parachar;
    parastring += "/#sqrt{E_{T}}";
    l.DrawLatex(50.,0.4,parastring);

    if(ieta%2==1){
      bet->Print("binnedetbincentercorr"+namestring+".eps");
      bet->Clear();
    }
    etetacanvas->cd();
    for(Int_t iet = 0; iet < etbinNum; iet++){
      eteta->Fill((etbinning[iet]+etbinning[iet+1])/2,(etabinning[ieta]+etabinning[ieta+1])/2,myetfunction->GetParameter(0)+myetfunction->GetParameter(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2));
      Double_t sigmaError = bGraph[ieta]->GetErrorY(iet);
      eteta->SetBinError(iet+1,ieta+1,sigmaError);
      //eteta->SetBinError(iet+1,ieta+1,sqrt(TMath::Power(myetfunction->GetParError(0),2)+TMath::Power(myetfunction->GetParError(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2),2)));
      }
  }
  TF2* mybarrelfunction = new TF2("mybarrelfunction","[0]+[1]/sqrt(x)+[2]*y*y/(x)",etbinning[etstartbin],500.,0.,1.479);
  TF2* myendcapfunction = new TF2("myendcapfunction","[0]+[1]/sqrt(x)+[2]*1.479*1.479/(x)+[3]*(y-1.479)/sqrt(x)+[4]*(y*y-1.479*1.479)/x",etbinning[etstartbin],500.,1.479,2.5);
  //TF2* myendcapfunction = new TF2("myendcapfunction","[0]+[1]/sqrt(x)+[2]*y/sqrt(x)",etbinning[etstartbin],500.,1.479,2.5);

  eteta->Fit("mybarrelfunction","+R0");
  myendcapfunction->FixParameter(0,mybarrelfunction->GetParameter(0));
  myendcapfunction->FixParameter(1,mybarrelfunction->GetParameter(1));
  myendcapfunction->FixParameter(2,mybarrelfunction->GetParameter(2));
  eteta->Fit("myendcapfunction","+R0");

  eteta->GetXaxis()->SetRangeUser(etbinning[etstartbin],500);
  eteta->Draw("colz");
  etetacanvas->Print("binnedfit2D.eps");

  TCanvas* chi2canvas = new TCanvas("chi2canvas","chi2canvas",500,900);
  chi2canvas->Divide(1,3);
  TCanvas* chi2distributioncan = new TCanvas("chi2distributioncan","chi2distributioncan",900,500);

  TH1D* chi2probhisto = new TH1D("chi2probhisto","chi2probhisto",50,0.,1.);
  TH1D* barrelchi2probhisto = new TH1D("barrelchi2probhisto","barrelchi2probhisto",50,0.,1.);
  TH1D* endcapchi2probhisto = new TH1D("endcapchi2probhisto","endcapchi2probhisto",50,0.,1.);
  TH2D* chi2distribution = new TH2D("chi2distribution","chi2distribution",25,etbinning,etabinNum,etabinning);
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
      Double_t mybarrel_sigma = mybarrelfunction->GetParameter(0)+mybarrelfunction->GetParameter(1)/sqrt(et)+mybarrelfunction->GetParameter(2)*((etabinning[ieta]+etabinning[ieta+1])/2)*((etabinning[ieta]+etabinning[ieta+1])/2)/(et);
      Double_t myendcap_sigma = myendcapfunction->GetParameter(0)+myendcapfunction->GetParameter(1)/sqrt(et)+myendcapfunction->GetParameter(2)*1.479*1.479/(et)+myendcapfunction->GetParameter(3)*((etabinning[ieta]+etabinning[ieta+1])/2-1.479)/sqrt(et)+myendcapfunction->GetParameter(4)*(((etabinning[ieta]+etabinning[ieta+1])/2)*((etabinning[ieta]+etabinning[ieta+1])/2)-1.479*1.479)/(et);

      if(ieta<17){
	if(iet==etbinNum-1)barreletabins++;
	chi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-mybarrel_sigma)/sigmaError,2),1));
	barrelchi2probhisto->Fill(TMath::Prob(TMath::Power((sigma-mybarrel_sigma)/sigmaError,2),1));
	chi2 += TMath::Power((sigma-mybarrel_sigma)/sigmaError,2);
	barrelchi2 += TMath::Power((sigma-mybarrel_sigma)/sigmaError,2);
	chi2distribution->SetBinContent(iet+1,ieta+1,(sigma-mybarrel_sigma)/sigmaError);
	justanotherhisto->Fill((sigma-mybarrel_sigma)/sigmaError);
	for(Int_t iieta = 0; iieta < 10; iieta++){
	  Double_t mybarrel_sigma2 = mybarrelfunction->GetParameter(0)+mybarrelfunction->GetParameter(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2)+mybarrelfunction->GetParameter(2)*(etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.)*(etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.)/((etbinning[iet]+etbinning[iet+1])/2);
	  eteta2->SetBinContent(iet+1,(10*ieta)+iieta+1,mybarrel_sigma2);
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
	  Double_t myendcap_sigma2 = myendcapfunction->GetParameter(0)+myendcapfunction->GetParameter(1)/sqrt((etbinning[iet]+etbinning[iet+1])/2)+myendcapfunction->GetParameter(2)*1.479*1.479/((etbinning[iet]+etbinning[iet+1])/2)+myendcapfunction->GetParameter(3)*(etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.-1.479)/sqrt((etbinning[iet]+etbinning[iet+1])/2)+myendcapfunction->GetParameter(4)*((etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.)*(etabinning[ieta]+(etabinning[ieta+1]-etabinning[ieta])*iieta/10.)-1.479*1.479)/((etbinning[iet]+etbinning[iet+1])/2);
	  eteta2->SetBinContent(iet+1,(10*ieta)+iieta+1,myendcap_sigma2);
	}
      }
    }
  }
  justanothercanvas->cd();
  justanotherhisto->Draw("e");
  etetacanvas2->cd();
  eteta2->GetXaxis()->SetRangeUser(etbinning[etstartbin],500);
  eteta2->Draw("colz");
  chi2canvas->cd(1);
  chi2probhisto->Draw("e");
  chi2canvas->cd(2);
  barrelchi2probhisto->Draw("e");
  chi2canvas->cd(3);
  endcapchi2probhisto->Draw("e");

  chi2distributioncan->cd();
  chi2distribution->Draw("colz");

  cout << "barrelfunction: " << mybarrelfunction->GetParameter(0) << " + " << mybarrelfunction->GetParameter(1) << "/sqrt(et) + " << mybarrelfunction->GetParameter(2) << "*eta*eta/et" << endl;
  cout << "endcapfunction: " << myendcapfunction->GetParameter(0) << " + " << myendcapfunction->GetParameter(1) << "/sqrt(et) + " << myendcapfunction->GetParameter(2) << "*1.479*1.479/et + " << myendcapfunction->GetParameter(3) << "*(eta-1.479)/sqrt(et) + " << myendcapfunction->GetParameter(4) << "*(eta*eta-1.479*1.479)/(et)" << endl;

  Double_t barrelchi2perNdof = barrelchi2/((etbinNum-etstartbin)*barreletabins-3);//3 is the number of parameter
  cout << "barrel: chi^2/Ndof = " << barrelchi2 << "/" << ((etbinNum-etstartbin)*barreletabins-3) << " = " << barrelchi2perNdof << endl;
  Double_t endcapchi2perNdof = endcapchi2/((etbinNum-etstartbin)*endcapetabins-2);//2 is the number of parameter
  cout << "endcap: chi^2/Ndof = " << endcapchi2 << "/" << ((etbinNum-etstartbin)*endcapetabins-2) << " = " << endcapchi2perNdof << endl;
  Double_t chi2perNdof = chi2/((etbinNum-etstartbin)*(etabinNum)-5);//5 is the number of parameter
  cout << "chi^2/Ndof = " << chi2 << "/" << ((etbinNum-etstartbin)*(etabinNum)-5) << " = " << chi2perNdof << endl;
  return 0;
}
