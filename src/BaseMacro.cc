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
    inputs_.push_back(make_pair(new TFile(input->getParameter<std::string>("file").c_str()), make_pair(input->getParameter<std::string>("label"), input->getParameter<double>("scale"))));
  }
  
  //load histograms of interest
  if(verbose_) std::cout << " prepare HistMap..." << std::endl;
  std::vector<std::string> hists = cfg.getParameter<std::vector<std::string> >("hists");
  for(std::vector<std::string>::const_iterator hist = hists.begin(); hist!=hists.end(); ++hist){
    std::vector<TH1*> buffer;
    for(InputCollection::const_iterator input = inputs_.begin(); input!=inputs_.end(); ++input){
      // full path within the file need to be given
      buffer.push_back((TH1*)input->first->Get(hist->c_str()));
      // apply normalization (expected to be to 1pb)
      buffer.back()->Scale(input->second.second);
    }
    // add to histogram map
    hists_[*hist]=buffer;
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
  // open ps file for first canvas to be written to file
  (*canvs.begin())->Print((fileName.append(".ps(")).c_str());
  std::cout << "canvas: " << (*canvs.begin())->GetName() << std::endl;
  for(std::vector<TCanvas*>::const_iterator canv=canvs.begin()+1; canv!=canvs.end(); ++canv){
    std::cout << "canvas: " << (*canv)->GetName() << std::endl;
    // fill canvases; close file for last element
    canv==canvs.end() ? (*canv)->Print((fileName.append(".ps)")).c_str()) : (*canv)->Print((fileName.append(".ps")).c_str());
  }
}

void BaseMacro::save(const TCanvas& canv, const std::string& fileType) const 
{
  TString fileName(canv.GetName()); fileName.ReplaceAll("/","_");
  canv.Print(fileName+fileType);
}
