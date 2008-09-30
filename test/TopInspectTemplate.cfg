$ -- configBlockIO -- $
$=====================================================================
$
$ input/output files
$
$=====================================================================

  histInput   = {$histInput}      $ hist file, which contains the names of the 
                                  $ histograms to be plottet 
                                  $                               
  rootInput    = {$rootInput}     $ root input file(s) corresponding to samples
                                  $ (separated by blanks)
                                  $
  inputDirs    = {$inputDirs}     $ directory in which the histograms are kept
                 			      $ within the root file; for edm::Analyzers 
                                  $ this  corresponds to the module name of the 
                                  $ Analyzer in the cfg file (separated by blanks)
                                  $
  histFilter   = {$histFilter}    $ list of filter strings; only histograms con-
                                  $ taining these stings as substrings are loaded
                                  $ (separated by blanks); empty means everything
                                  $ is plotted 
                                  $
  plotFilter   = {$plotFilter}    $ list of filter strings; only histograms con-
                                  $ taining these stings as substrings are plotted
                                  $ (separated by blanks); empty means everything
                                  $ is plotted
                                  $
  filterOption = {$filterOption}  $ histogram filter option ('begins', 'ends' & 
                                  $ 'contains' are supported
                                  $
  rootOutput   = {$rootOutput}    $ root output file to write histograms to
                                  $
  outputDir    = {$outputDir}     $ output directory in root output file 
                                  $
  outputLabels = {$outputLabels}  $ output labels for multiple ps/eps files 
                                  $ (separated by blanks)
                                  $
  writePlotsTo = {$writePlotsTo}  $ directory to save the plottet histograms to;
                                  $ '.' is the working directory
                                  $
  writePlotsAs = {$writePlotsAs}  $ decide wether to write histograms to [ps] or 
                                  $ [eps] files
  outputFileName = {$outputFileName} $ gives the filename of the ps-output
  								  $ the filename is 'inspect' in the case of left blank

$ -- configBlockHist -- $
$=====================================================================
$
$ canvas and histogram steering
$
$=====================================================================

  xLog         = {$xLog}    	  $ logs can be declared for each histogram 
                                  $ individually; per default they are  
  yLog         = {$yLog}          $ switched off
                                  $                                  
  xGrid        = {$xGrid}		  $ grids can be declared for each histogram
								  $ individually; per default they are
  yGrid        = {$yGrid} 		  $ switched off
                            	  $
  histScale    = {$histScale}     $ histogram scale; can be steered for each 
                                  $ histogram individually; empty means scale
                                  $ is _1_
                                  $
  histMinimum  = {$histMinimum}   $ histogram minimum; can be steered for 
                                  $ each histogram individually; empty means 
                                  $ set to _0_
                                  $
  histMaximum  = {$histMaximum}   $ histogram maximum; can be steered for 
                                  $ each histogram individually; empty means
                                  $ set automatically
                                  $
  histErrors   = {$histErrors}    $ draw histogram errors; can be steered for 
                                  $ each sample; empty means no errors
                                  $
  histType     = {$histType}      $ defines wether histogram should be plotted
                                  $ as line[0], with poly markers[1] or filled 
                                  $ [2]; default is line [0]; can be steered 
                                  $ for each sample individually
                                  $
  histStyle    = {$histStyle}     $ defines line or fill style for each sample
                                  $
  histColor    = {$histColor}     $ defines line/marker/fill color for each 
                                  $ sample
                                  $
  lineWidth    = {$lineWidth}     $ defines line width for each sample
                                  $
  markerStyle  = {$markerStyle}   $ defines marker style for each sample
                                  $
  markerSize   = {$markerSize}    $ defines marker size for each sample
                                  $
                                  $ set axes titles for each histograms;
                                  $ has to be givin in '"' and end with ';'
  xAxes        = {$xAxes}
                 

  yAxes        = {$yAxes}
                 



                                                                      $
                                                                      $ should contain a legend entry for each 
                                                                      $ sample in '"' and separated by ';'
  legEntries   = {$legEntries}                             
                 
                                                                      $
  legXLeft     = {$legXLeft}                                                 $ so far a steering of the legend coord's 
                                                                      $ is only supported globally
  legXRight    = {$legXRight}                                                 $
                                                                      $
  legYLower    = {$legYLower}                                                 $
                                                                      $
  legYUpper    = {$legYUpper}                                                 $
  
  $ -- configBlockSum -- $
$=====================================================================
$
$ histogram addition
$
$=====================================================================

  histWeights = {$histWeights}                                                $ define weights which should be applied
                                                                      $ to each sample before adding them up 
