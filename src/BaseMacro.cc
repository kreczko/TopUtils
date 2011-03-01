#include "TopAnalysis/TopUtils/interface/BaseMacro.h"

BaseMacro::BaseMacro(const edm::ParameterSet& cfg) : lumi_(cfg.getParameter<double>("lumi")), verbose_(cfg.getParameter<bool>("verbose"))
{
  // load input files
  if(verbose_) std::cout << " prepare InputCollection..." << std::endl;
  std::vector<edm::ParameterSet> inputs = cfg.getParameter<std::vector<edm::ParameterSet> >("inputs");
  for(std::vector<edm::ParameterSet>::const_iterator input=inputs.begin(); input!=inputs.end(); ++input){
    // check if file exists
    if(TFile(input->getParameter<std::string>("file").c_str()).IsZombie()){
      std::cout << "file: " << input->getParameter<std::string>("file").c_str() << "does not exist." << std::endl; exit(-1); 
    }
    inputs_.push_back(std::make_pair(input->getParameter<std::string>("label"), std::make_pair(new TFile(input->getParameter<std::string>("file").c_str()), input->getParameter<double>("scale"))));
  }
  
  //load histograms of interest
  if(verbose_) std::cout << " prepare HistMap..." << std::endl;
  std::vector<std::string> hists = cfg.getParameter<std::vector<std::string> >("hists");
  for(std::vector<std::string>::const_iterator hist = hists.begin(); hist!=hists.end(); ++hist){
    std::map<std::string, TH1*> mapBuffer;
    for(InputCollection::const_iterator input = inputs_.begin(); input!=inputs_.end(); ++input){
      TH1F* histBuffer=0;
      input->second.first->GetObject(hist->c_str(), histBuffer);
      if(histBuffer){
	// the full path within the file needs to be given here
	mapBuffer[input->first] = histBuffer;
	// apply normalization (expected to be in pb)
	mapBuffer[input->first]->Scale(input->second.second);
      }
      else{
	std::cout << " WARNING: histogram " << (*hist) << "not in file " << input->second.first->GetName() << std::endl;
      }
    }
    // add to histogram map
    hists_[*hist]=mapBuffer;
  }
}

void BaseMacro::save(const std::vector<TH1*>& hists, const std::string& fileName) const
{
  // open output file
  TFile* outputFile = new TFile(fileName.c_str());
  for(std::vector<TH1*>::const_iterator hist=hists.begin(); hist!=hists.end(); ++hist){
    (*hist)->Write();
  }
  outputFile->Close();
}

void BaseMacro::save(const std::vector<TCanvas*>& canvs, std::string fileName) const
{
  // open output file
  TCanvas* output = new TCanvas("output");
  output->Print(fileName.append(".ps[").c_str());
  for(std::vector<TCanvas*>::const_iterator canv=canvs.begin(); canv!=canvs.end(); ++canv){
    std::cout << "canvas: " << (*canv)->GetName() << std::endl;
    output = *canv; output->Print(fileName.append(".ps").c_str());
  }
  // close ps file
  output->Print(fileName.append(".ps]").c_str());
  delete output; 
}

void BaseMacro::save(const TCanvas& canv, const std::string& fileType) const 
{
  TString fileName(canv.GetName()); fileName.ReplaceAll("/","_");
  canv.Print(fileName+fileType);
}
