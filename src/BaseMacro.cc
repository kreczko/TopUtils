#include "TopAnalysis/TopUtils/interface/BaseMacro.h"

BaseMacro::BaseMacro(const edm::ParameterSet& cfg) : lumi_(cfg.getParameter<double>("lumi"))
{
  // load input files
  std::vector<edm::ParameterSet> samples = cfg.getParameter<std::vector<edm::ParameterSet> >("samples");
  for(std::vector<edm::ParameterSet>::const_iterator sample=samples.begin(); sample!=samples.end(); ++sample){
    samples_.push_back(make_pair(new TFile(sample->getParameter<std::string>("file").c_str()), make_pair(sample->getParameter<std::string>("label"), sample->getParameter<double>("scale"))));
  }

  //load histograms of interest
  std::vector<std::string> hists = cfg.getParameter<std::vector<std::string> >("hists");
  for(std::vector<std::string>::const_iterator hist = hists.begin(); hist!=hists.end(); ++hist){
    std::vector<TH1*> buffer;
    for(SampleCollection::const_iterator sample = samples_.begin(); sample!=samples_.end(); ++sample){
      // full path within the file need to be given
      buffer.push_back((TH1*)sample->first->Get(hist->c_str()));
      // apply normalization (expected to be to 1pb)
      buffer.back()->Scale(sample->second.second);
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

void BaseMacro::save(const std::vector<TCanvas*>& canvs, std::string& fileName) const 
{
  // open ps file for first canvas to be written to file
  (*canvs.begin())->Print(fileName.append(".ps(").c_str());
  for(std::vector<TCanvas*>::const_iterator canv=canvs.begin()+1; canv!=canvs.end(); ++canv){
    // fill canvases; close file for last element
    canv==canvs.end() ? (*canv)->Print(fileName.append(".ps").c_str()) : (*canv)->Print(fileName.append(".ps)").c_str());
  }
}

void BaseMacro::save(const TCanvas& canv, const std::string& fileType) const 
{
  TString fileName(canv.GetName()); fileName.ReplaceAll("/","_");
  canv.Print(fileName+fileType);
}
