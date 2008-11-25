#include "TopAnalysis/TopUtils/interface/CompHist.h"
#include "TopAnalysis/TopUtils/interface/ConfigFile.h"
#include "TopAnalysis/TopUtils/interface/RootPostScript.h"

using namespace std;

void CompHist::readLabels(std::string s, std::vector<std::string>& vec) {
  //-----------------------------------------------
  // fill vector of std::string's from a single
  // std::string s; the process starts as soon as
  // leading " are found and starts a new substr
  // as soon as a "; is encountered
  //-----------------------------------------------
  std::stringstream stream(s);
  std::string buffer, label;
  while (!stream.eof()) {
    stream >> buffer;
    if (buffer.find("\"", 0)==0) {
      //start new label if leading " are found
      label=buffer;
    } else {
      //concatenate buffer to label else
      label+=" ";
      label+=buffer;
    }
    
    //push_back label if it starts with " and ends with ";
    if (buffer.find("\"", buffer.size()-2)==buffer.size()-2 && buffer.find(
									   ";", buffer.size()-2)==buffer.size()-1) {
      vec.push_back(label.substr(1, label.size()-3) );
    }
  }
}

void CompHist::configBlockIO(ConfigFile& cfg) {
  //-----------------------------------------------
  // read all configurables defined in CompHisto-
  // grams from config file. Throw human readable
  // exception when misspellings occure
  //-----------------------------------------------

  //-----------------------------------------------
  // input/output files
  //-----------------------------------------------
  histFile_  = cfg.read<std::string>( "histInput",    Config::kEmpty    );
  readVector ( cfg.read<std::string>( "rootInput"  ), fileNameList_     );
  readVector ( cfg.read<std::string>( "inputDirs"  ), dirNameList_      );
  readVector ( cfg.read<std::string>( "histFilter" ), histFilterList_   );
  readVector ( cfg.read<std::string>( "plotFilter" ), plotFilterList_   );
  filterOpt_ = cfg.read<std::string>( "filterOption", Config::kEmpty    );
  output_    = cfg.read<std::string>( "rootOutput",   Config::kEmpty    );
  rootOutDir_= cfg.read<std::string>( "outputDir",    Config::kEmpty    );
  readVector ( cfg.read<std::string>( "outputLabels" ), outputLabelList_);
  writeTo_   = cfg.read<std::string>( "writePlotsTo", "." );
  writeAs_   = cfg.read<std::string>( "writePlotsAs", "ps" );
  outputFileName_ = cfg.read<std::string>( "outputFileName", "inspect"  );

  if(histFile_.empty() && histFilterList_.empty() ){
    cerr << "ERROR[Config/configBlockIO]              \n"
	 << " * variable 'histInput'  is empty        \n"
	 << " * variable 'histFilter' is empty        \n"
	 << " * at least one variable needs to be set \n"
	 << "sorry this will quit your program here...\n";
    exit(1);
  }
}

void CompHist::configBlockHist(ConfigFile& cfg) {
  //-----------------------------------------------
  // read all configurables defined in CompHisto-
  // grams from config file. Throw human readable
  // exception when misspellings occure
  //-----------------------------------------------

  //-----------------------------------------------
  // canvas steering
  //-----------------------------------------------
  readVector( cfg.read<std::string>( "xLog" ), logX_ );
  readVector( cfg.read<std::string>( "yLog" ), logY_ );
  readVector( cfg.read<std::string>( "xGrid" ), gridX_);
  readVector( cfg.read<std::string>( "yGrid" ), gridY_);
  
  //-----------------------------------------------
  // histogram steering
  //-----------------------------------------------
  readVector( cfg.read<std::string>( "histScale" ), scale_ );
  readVector( cfg.read<std::string>( "histMinimum"), min_ );
  readVector( cfg.read<std::string>( "histMaximum"), max_ );
  readVector( cfg.read<std::string>( "histErrors" ), errors_ );
  readVector( cfg.read<std::string>( "histType" ), histStyle_ );
  readVector( cfg.read<std::string>( "histStyle" ), commonStyle_ );
  readVector( cfg.read<std::string>( "histColor" ), histColor_ );
  readVector( cfg.read<std::string>( "lineWidth" ), commonWidth_ );
  readVector( cfg.read<std::string>( "markerStyle"), markerStyle_ );
  readVector( cfg.read<std::string>( "markerSize" ), markerSize_ );
  readLabels( cfg.read<std::string>( "xAxes" ), xAxes_ );
  readLabels( cfg.read<std::string>( "yAxes" ), yAxes_ );
  
  //-----------------------------------------------
  // legend steering
  //-----------------------------------------------
  readLabels( cfg.read<std::string>( "legEntries" ),legendEntries_);
  legXLeft_ = cfg.read<double>( "legXLeft",  0.25 );
  legXRight_= cfg.read<double>( "legXRight", 0.95 );
  legYLower_= cfg.read<double>( "legYLower", 0.70 );
  legYUpper_= cfg.read<double>( "legYUpper", 0.95 );
}

void CompHist::readHistogramList() {
	//-----------------------------------------------
	// fill the list histList_ with all requested
	// histogram names; the names are recieved from
	// a .hist file; jump out if the reading takes
	// too long
	//-----------------------------------------------
	ifstream histFile(histFile_.c_str() );

	int count=0;
	while ( !histFile.eof() ) {
		std::string buffer;
		histFile >> buffer;
		if (buffer.size()>0) {
			TString cmp(buffer);
			if (histFilter(cmp, histFilterList_) )
				histList_.push_back(buffer);
			else {
				if (verbose_) {
					cout << " histogram is filtered out according"
							<< " to settings in cfg file; filterOpt:  "
							<< filterOpt_ << endl;
				}
			}
		}
		if (count>999) {
			cerr << "ERROR caught in slope for histogram" << endl;
			cerr << "      names. Misspelled file name ?" << endl;
			cerr << histFile_.c_str() << endl;
			break;
		}
		++count;
	}
}

void CompHist::loadHistograms() {
	//-----------------------------------------------
	// fill histograms listed in histList_ into a
	// TObjectArray for each sample given in the
	// list fileNameList_ and its directories; Each
	// TObjectArray is then pushed into the vector
	// sampleList_ which keeps all TObjectArrays of
	// histograms for each sample
	//-----------------------------------------------

	//fill vector of root files
	std::vector<std::string>::const_iterator name;
	for (name = fileNameList_.begin(); name!=fileNameList_.end(); ++name) {
		fileList_.push_back(new TFile( name->c_str() ));
	}

	//fill histogram list
	if ( !histFile_.empty() )
		readHistogramList();
	else {
		if (histFilterList_.size()>0) {
			if ( !filterOpt_.compare("contains") )
				cout
						<< "WARNING: missing hist file in cfg file; will use list of histogram"
						<< endl
						<< "         names given in histFilter instead, requires filterOption "
						<< endl
						<< "         'optains'; will move filterOption to 'optain' to go on..."
						<< endl;
			filterOpt_="contains";
			histList_ =histFilterList_;
		}
	}

	//fill vector of histogram arrays
	std::vector<TFile*>::const_iterator file = fileList_.begin();
	for (int jdx=0; file!=fileList_.end(); ++file, ++jdx) {
		std::vector<std::string>::const_iterator rootDir;
		for (rootDir = dirNameList_.begin(); rootDir!=dirNameList_.end(); ++rootDir) {
			TH1F *dummy;
			TObjArray hist;
			for (int idx=0; idx<(int)histList_.size(); ++idx) {
				TString directory( *rootDir);
				directory+="/";
				directory+=histList_[idx];

				if (verbose_) {
					cout << "load from: " << (*file)->GetName() << " " << "directory: "
							<< directory << endl;
				}
				dummy = (TH1F*)(*file)->Get(directory);
				if ( !dummy) {
					cerr << "WARNING:" << " Didn't find indicated hist" << " ["
							<< directory << "]" << " in file" << " [" << (*file)->GetName() << "]" << endl;
					return;
				}

				// prepare unambiguous histogram name
				TString outname(fileNameList_[jdx]);
				outname.Remove(outname.Last('.'), outname.Length());
				outname += ":";
				outname += *rootDir;
				outname +="/";
				outname +=dummy->GetName();
				dummy->SetName(outname);

				// add histogram to list
				hist.Add(dummy);
				if (verbose_) {
					cout << "added to list as: " << dummy->GetName() << endl;
				}
			} // end of histogram name list
			sampleList_.push_back(hist);
		} // end of root directory list
	} // end of root file list
}

bool CompHist::histFilter(TString& cmp, std::vector<std::string>& ref) {
	bool contained=true;
	if (!filterOpt_.empty()) {
		contained=false;
		if (!filterOpt_.compare("begins")) {
			contained = histFilter(cmp, ref, kBeginsWith);
		} else if (!filterOpt_.compare("ends")) {
			contained = histFilter(cmp, ref, kEndsWith);
		} else if (!filterOpt_.compare("contains")) {
			contained = histFilter(cmp, ref, kContains);
		} else {
			cerr << "ERROR during histogram filtering   " << endl;
			cerr << "      filter option is not properly" << endl;
			cerr << "      specified : " << filterOpt_ << endl;
		}
	}
	return contained;
}

bool CompHist::histFilter(TString& cmp, std::vector<std::string>& ref,
		CompHist::HistFilter option) {
	bool contained=true;
	if (ref.size()>0) {
		contained=false;
		for (unsigned int idx=0; idx<ref.size(); ++idx) {
			TString buffer(ref[idx]);
			if ( !option==kContains)
				buffer+="_";
			if (option==kBeginsWith && cmp.BeginsWith(buffer) ) {
				contained=true;
				break;
			}
			if (option==kEndsWith && cmp.EndsWith(buffer) ) {
				contained=true;
				break;
			}
			if (option==kContains && cmp.Contains(buffer) ) {
				contained=true;
				break;
			}
		}
	}
	return contained;
}

void CompHist::draw(TCanvas& canv, TLegend& leg, int& idx, int& jdx) {
	//-----------------------------------------------
	// loop all samples via the list sampleList_, which
	// containas the histograms of each sample as
	// TObjects in TObjectArrays
	//-----------------------------------------------
	TH1* hfirst = 0; //draw first histogram on top of others
	//after all histograms have been drawn
	bool isTH2 = false;
	std::vector<TObjArray>::const_iterator hist = sampleList_.begin();
	for (int kdx=0; hist!=sampleList_.end(); ++hist, ++kdx) {
		TH1& hcmp = *((TH1*)(*hist)[idx]); //recieve histogram
		isTH2 = hcmp.InheritsFrom("TH2");
		setCanvLog(canv, jdx);
		setCanvGrid(canv, jdx);
		setHistStyles(hcmp, idx, jdx, kdx);
		// for the first histogram just draw
		// for the following ones draw same
		if (kdx==0) {
			hfirst = (TH1*)hcmp.Clone(); // buffer first histogram to redraw it after all
			if (!errors_.empty() && errors_[kdx])
				hcmp.Draw("e");
			else
				hcmp.Draw();
		} else {
			if (!errors_.empty() && errors_[kdx])
				hcmp.Draw("samee");
			else
				hcmp.Draw("same");
		}
		if (isTH2)
			break; // only first histogram and no legend in case of TH2
		// add legend entry in appropriate format
		switch (histStyle_[kdx]) {
		case HistStyle::Line:
			leg.AddEntry( &hcmp, legend(kdx).c_str(), "L");
			break;

		case HistStyle::Marker:
			leg.AddEntry( &hcmp, legend(kdx).c_str(), "PL");
			break;

		case HistStyle::Filled:
			leg.AddEntry( &hcmp, legend(kdx).c_str(), "FL");
			break;
		}
	}
	if (!isTH2) {
		if (!errors_.empty() && errors_[0]) {
			hfirst->Draw("esame");
		} else {
			hfirst->Draw("same");
		}
		leg.Draw("same");
	}
	canv.RedrawAxis();
	canv.Update();
}

void CompHist::drawPs() {
	//-----------------------------------------------
	// define canvas
	//-----------------------------------------------
	TCanvas *canv = new TCanvas("canv", "histograms", 600, 600);
	setCanvasStyle( *canv);

	//-----------------------------------------------
	// open output file
	//-----------------------------------------------
	TString output(writeTo_.c_str() );
	output += "/";
	output += outputFileName_;
	output += ".";
	output += writeAs_;
	TPostScript psFile(output, 111); //112 for portrait

	//-----------------------------------------------
	// loop histograms via the list of histogram
	// names stored in histList_, open a new page
	// for each histogram & plot each sample in
	// the same canvas
	//-----------------------------------------------
	for (int idx=0, jdx=0; idx<(int)histList_.size(); ++idx) {
		// prepare compare string for plot filtering
		TString cmp(histList_[idx]);
		if ( !histFilter(cmp, plotFilterList_) ) {
			if (verbose_) {
				cout << " event is filtered out according to"
						<< " settings in cfg file; filterOpt:  " << filterOpt_
						<< " hist: " << cmp << endl;
			}
			continue;
		}
		psFile.NewPage();
		//-----------------------------------------------
		// on each page the legend needs to be redeclared
		//-----------------------------------------------
		TLegend* leg = new TLegend(legXLeft_,legYLower_,legXRight_,legYUpper_);
		setLegendStyle( *leg);
		draw(*canv, *leg, idx, jdx);
		if (idx == (int)histList_.size()-1) {
			psFile.Close();
		}
		++jdx;
		delete leg;
	}
	canv->Close();
	delete canv;
}

void CompHist::drawEps() {
	//-----------------------------------------------
	// define canvas
	//-----------------------------------------------
	TCanvas *canv = new TCanvas("canv", "histograms", 600, 600);
	setCanvasStyle( *canv);

	//-----------------------------------------------
	// loop histograms via the list of histogram
	// names stored in histList_, open a new page
	// for each histogram & plot each sample in
	// the same canvas
	//-----------------------------------------------
	for (int idx=0, jdx=0; idx<(int)histList_.size(); ++idx) {
		//-----------------------------------------------
		// open output files
		//-----------------------------------------------
		TString output(writeTo_.c_str() );
		output += "/";
		output += histList_[ idx ];
		output += ".";
		output += writeAs_;
		TPostScript psFile(output, 113);

		// prepare compare string for plot filtering
		TString cmp(histList_[idx]);
		if ( !histFilter(cmp, plotFilterList_) ) {
			if (verbose_) {
				cout << " event is filtered out according to"
						<< " settings in cfg file; filterOpt:  " << filterOpt_
						<< " hist: " << cmp << endl;
			}
			continue;
		}
		psFile.NewPage();
		//-----------------------------------------------
		// on each page the legend needs to be redeclared
		//-----------------------------------------------
		TLegend* leg = new TLegend(legXLeft_,legYLower_,legXRight_,legYUpper_);
		setLegendStyle( *leg);
		draw(*canv, *leg, idx, jdx);
		psFile.Close();
		++jdx;
		delete leg;
	}
	canv->Close();
	delete canv;
}

std::string CompHist::legend(int idx) {
	char buffer[100];
	sprintf(buffer, "undefined sample %i", idx);

	if (idx<=(int)legendEntries_.size() ) {
		return legendEntries_[idx];
	}
	return buffer;
}

double CompHist::findMaximum(int idx) {
	double max=-1.;
	for (std::vector<TObjArray>::const_iterator hist = sampleList_.begin(); hist
			!=sampleList_.end(); ++hist) {
		TH1& hcmp = *((TH1*)(*hist)[idx]); //recieve histogram
		if ( max<0 || (hcmp.GetMaximum()+hcmp.GetBinError(hcmp.GetMaximumBin())) > max ) 
		  max = hcmp.GetMaximum() + hcmp.GetBinError(hcmp.GetMaximumBin());
	}
	return max;
}

double CompHist::findMinimum(int idx) {
	double min=1.;
	for (std::vector<TObjArray>::const_iterator hist = sampleList_.begin(); hist
			!=sampleList_.end(); ++hist) {
		TH1& hcmp = *((TH1*)(*hist)[idx]); //recieve histogram
		if ( min>0 || (hcmp.GetMinimum()-hcmp.GetBinError(hcmp.GetMinimumBin())) < min ) 
		  min = hcmp.GetMinimum() - hcmp.GetBinError(hcmp.GetMinimumBin());
	}
	return min;
}

void CompHist::setLegendStyle(TLegend& leg) {
	leg.SetFillStyle( 0);
	leg.SetFillColor( 0);
	leg.SetBorderSize( 0);
}

void CompHist::setCanvasStyle(TCanvas& canv) {
	canv.SetFillStyle( 4000);
	canv.SetLeftMargin( 0.20);
	canv.SetRightMargin( 0.05);
	canv.SetBottomMargin( 0.15);
	canv.SetTopMargin( 0.05);
}

void CompHist::setCanvLog(TCanvas& canv, int idx) {
	if (idx<(int)logX_.size() && idx<(int)logY_.size()) {
		canv.SetLogx(logX_[idx]);
		canv.SetLogy(logY_[idx]);
	} else if (idx<(int)logY_.size()) {
		canv.SetLogx( 0);
		canv.SetLogy(logY_[idx]);
	} else {
		canv.SetLogx( 0);
		canv.SetLogy( 0);
	}
}

void CompHist::setCanvGrid(TCanvas& canv, int idx) {
	if (idx<(int)gridX_.size() && idx<(int)gridY_.size()) {
		canv.SetGridx(gridX_[idx]);
		canv.SetGridy(gridY_[idx]);
	} else if (idx<(int)gridY_.size()) {
		canv.SetGridx( 0);
		canv.SetGridy(gridY_[idx]);
	} else {
		canv.SetGridx( 0);
		canv.SetGridy( 0);
	}
}

void CompHist::setAxesStyle(TH1& hist, const char* titleX, const char* titleY) {
	hist.SetTitle("");

	hist.GetXaxis()->SetTitle(titleX);
	hist.GetXaxis()->SetTitleSize( 0.06);
	hist.GetXaxis()->SetTitleColor( 1);
	hist.GetXaxis()->SetTitleOffset( 1.0);
	hist.GetXaxis()->SetTitleFont( 62);
	hist.GetXaxis()->SetLabelSize( 0.05);
	hist.GetXaxis()->SetLabelFont( 62);
	hist.GetXaxis()->CenterTitle();
	hist.GetXaxis()->SetNdivisions( 505);

	hist.GetYaxis()->SetTitle(titleY);
	hist.GetYaxis()->SetTitleSize( 0.07);
	hist.GetYaxis()->SetTitleColor( 1);
	hist.GetYaxis()->SetTitleOffset( 1.3);
	hist.GetYaxis()->SetTitleFont( 62);
	hist.GetYaxis()->SetLabelSize( 0.05);
	hist.GetYaxis()->SetLabelFont( 62);
}

void CompHist::setHistStyles(TH1& hist, int idx, int jdx, int kdx) {
	//-----------------------------------------------
	// check hist style; throw exception if style
	// is not competible with specifications; set
	// default line[0] if vector is too short
	//-----------------------------------------------
	int histType=0;
	if (kdx<((int)histStyle_.size())) {
		if (HistStyle::Line<=histStyle_[kdx] && histStyle_[kdx]
				<=HistStyle::Filled) {
			histType=histStyle_[kdx];
		} else {
			cerr << "ERROR histogram Type outof bound, value is: " << kdx
					<< endl;
			cerr << "      allowed range is: " << HistStyle::Line << " - "
					<< HistStyle::Filled << endl;
			cerr << "      Please correct parameter \"histType\"" << endl;
			cerr << "      in your cfg file" << endl;
			cerr << "      [--called in setHistStyles--]" << endl;
			throw "Histogram Type cannot be specified";
		}
	} else {
		cerr << "ERROR histogram has no Type specified" << endl;
		cerr << "      Please specify parameter \"histType\" " << endl;
		cerr << "      in your cfg file" << endl;
		cerr << "      [--called in setHistStyles--]" << endl;
		throw "Histogram has no Type";
	}

	//define histogram styles
	setHistLabels(hist, jdx);
	setHistScale(hist, jdx);
	setHistMax(hist, idx, jdx);
	setHistMin(hist, idx, jdx);

	switch (histType) {
	case HistStyle::Line:
		setLineWidth(hist, kdx);
		setLineColor(hist, kdx);
		setLineStyle(hist, kdx);
		hist.SetOption("box");
		break;

	case HistStyle::Marker:
		setLineWidth(hist, kdx);
		setLineColor(hist, kdx);
		setMarkerColor(hist, kdx);
		setMarkerStyle(hist, kdx);
		setMarkerSize(hist, kdx);
		hist.SetOption("scat");
		break;

	case HistStyle::Filled:
		setLineWidth(hist, kdx);
		setLineColor(hist, kdx);
		setFillColor(hist, kdx);
		setFillStyle(hist, kdx);
		hist.SetOption("col");
		break;
	}
}

void CompHist::setHistLabels(TH1& hist, int idx) {
	if (idx<((int)xAxes_.size()) && idx<((int)yAxes_.size())) {
		setAxesStyle(hist, xAxes_[idx].c_str(), yAxes_[idx].c_str() );
	} else if (idx<((int)xAxes_.size())) {
		setAxesStyle(hist, xAxes_[idx].c_str(), "events");
	} else {
		if (strcmp(hist.GetTitle(), "") == 0) {
			setAxesStyle(hist, hist.GetXaxis()->GetTitle(), "events");
		} else {
			if (hist.InheritsFrom("TH2") ) {
				// try to find labels for x and y axes
				// using the histogram title
				// and a substring as separator
				TString h2Title = hist.GetTitle();
				TString xySep = " vs. ";
				if (h2Title.Contains(xySep) ) {
					TString xTitle = h2Title( 0, h2Title.Index(xySep) );
					int yTitleStart = h2Title.Index(xySep)+xySep.Length();
					TString yTitle = h2Title(yTitleStart, h2Title.Length()
							-yTitleStart);
					setAxesStyle(hist, xTitle.Data(), yTitle.Data() );
				} else
					setAxesStyle(hist, "x", "y");
			} else
				setAxesStyle(hist, hist.GetTitle(), "events");
		}
	}
}

void CompHist::setHistScale(TH1& hist, int idx) {
	if (idx<((int)scale_.size())) {
		if (scale_[idx]>0)
			hist.Scale(scale_[idx]/hist.Integral());
	}
}

void CompHist::setHistMax(TH1& hist, int idx, int jdx) {
	if ( ((int)max_.size()>0) && (jdx<(int)max_.size())) {
		hist.SetMaximum(max_[jdx]);
	} else {
	        if( findMaximum(idx)>0 ) hist.SetMaximum(1.5*findMaximum(idx));
		  else hist.SetMaximum(0);
	}
}

void CompHist::setHistMin(TH1& hist, int idx, int jdx) {
	if ( ((int)min_.size()>0) && (jdx<(int)min_.size())) {
		hist.SetMinimum(min_[jdx]);
	} else {
		if( findMinimum(idx)<0 ) hist.SetMinimum(1.1*findMinimum(idx));
		  else hist.SetMinimum(0);
	}
}

void CompHist::setLineWidth(TH1& hist, int jdx) {
	hist.SetLineWidth( 5);
	if (jdx<((int)commonWidth_.size())) {
		hist.SetLineWidth(commonWidth_[jdx]);
	}
}

void CompHist::setLineStyle(TH1& hist, int jdx) {
	hist.SetLineStyle( 1);
	if (jdx<((int)commonStyle_.size())) {
		hist.SetLineStyle(commonStyle_[jdx]);
	}
}

void CompHist::setLineColor(TH1& hist, int jdx) {
	hist.SetLineColor( 1);
	if (jdx<((int)histColor_.size())) {
		hist.SetLineColor(histColor_[jdx]);
	}
}

void CompHist::setFillStyle(TH1& hist, int jdx) {
	hist.SetFillStyle( 3005);
	if (jdx<((int)commonStyle_.size())) {
		hist.SetFillStyle(commonStyle_[jdx]);
	}
}

void CompHist::setFillColor(TH1& hist, int jdx) {
	hist.SetFillColor( 1);
	if (jdx<((int)histColor_.size())) {
		hist.SetFillColor(histColor_[jdx]);
	}
}

void CompHist::setMarkerStyle(TH1& hist, int jdx) {
	hist.SetMarkerStyle( 23);
	if (jdx<((int)markerStyle_.size())) {
		hist.SetMarkerStyle(markerStyle_[jdx]);
	}
}

void CompHist::setMarkerColor(TH1& hist, int jdx) {
	hist.SetMarkerColor( 1);
	if (jdx<((int)histColor_.size())) {
		hist.SetMarkerColor(histColor_[jdx]);
	}
}

void CompHist::setMarkerSize(TH1& hist, int jdx) {
	hist.SetMarkerSize( 2.3);
	if (jdx<((int)markerSize_.size())) {
		hist.SetMarkerSize(markerSize_[jdx]);
	}
}

void CompHist::writeOutput(CompHist::RootOutput option) {
	//---------------------------------------------
	// write filled target histogram to output file
	// if specified
	//---------------------------------------------
	if (isOutputRequested() ) {
		//-----------------------------------------------
		// open hist file for book keeping of hist names
		//-----------------------------------------------
		TString name(output_);
		name.Remove(name.Last('.'), name.Length());
		name+=".hist"; //replace .root by .hist
		ofstream histFile(name, std::ios::out);

		//-----------------------------------------------
		// open root output file and create directory
		// if necessary
		//-----------------------------------------------
		TFile file(output_.c_str(), "update");
		if ( !file.GetDirectory(rootOutDir_.c_str()) )
			// create new directory if it does not yet exist
			file.mkdir(rootOutDir_.c_str(), rootOutDir_.c_str() );
		else
			// clean-up directory if it was existing already
			(file.GetDirectory(rootOutDir_.c_str()))->Delete("*;*");
		file.cd(rootOutDir_.c_str());

		//-----------------------------------------------
		// loop and write out histograms
		//-----------------------------------------------
		if (option==kAll) {
			std::vector<TObjArray>::const_iterator hist = sampleList_.begin();
			for (; hist!=sampleList_.end(); ++hist) {
				for (unsigned int idx=0; idx<histList_.size(); ++idx) {
					histFile << ((TH1*)(*hist)[idx])->GetName() << "\n";
					((TH1*)(*hist)[idx])->Write();
				}
			}
		} else {
			for (unsigned int idx=0; idx<histList_.size(); ++idx) {
				// write first/last histograms in the sample list only
				histFile << (TH1*)((sampleList_.back())[idx])->GetName() << "\n";
				switch (option) {
				case kFirstOnly:
					(TH1*)((*sampleList_.begin())[idx])->Write();
					break;
				case kLastOnly:
					(TH1*)((sampleList_.back())[idx])->Write();
					break;
				default:
					cerr << "WARNING:"
							<< " Wrong option indicated for writeOutput"
							<< " [" << option << "]" << " while only "
							<< kFirstOnly << " up to " << kAll
							<< " are available" << endl;
					break;
				}
			}
		}
		file.Close();
	}
}
