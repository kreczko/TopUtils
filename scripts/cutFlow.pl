#!/usr/bin/perl -w

# script to generate cutflow table in HTML format
# from TrigReports


# integrated luminosity of data in /pb
my $lumi = 0.7;

# input files (last file is for data)
my @files = ("ttbarmumu.txt",
             "ttbartaumu.txt",
	     "ttbarbg.txt",
	     "singletops.txt",
	     "singletopt.txt",
	     "singletoptw.txt",
	     "zz.txt",
	     "wz.txt",
	     "zz.txt",
	     "ztautau.txt",
	     "zmumu.txt",
	     "wmunu.txt",
	     "qcd.txt",
	     "dataAug13.txt"
	    );

# cross section over number of events	    
my @weights = ($lumi*165/632010.,
               $lumi*165/632010.,
	       $lumi*165/632010.,
	       $lumi*4.6/412055.,
	       $lumi*63.2/528593.,
	       $lumi*10.6/466437.,
	       $lumi*5.9/145000.,
	       $lumi*18.2/118120.,
	       $lumi*43/122980.,
	       $lumi*1495/2160000.,
	       $lumi*1495/2051268.,
	       $lumi*28049/3./2047693.,
	       $lumi*296900*0.2684/4377187.
	      ); #for data no weight is needed so 
	         #weight array is 1 shorter then files array	    

# modules which you want to appear in cutflow
my @modules = ("analyzePrimaryVertex",
               "filterTrigger",
	       "tightMuonSelection",
	       "globalMuonSelection",
	       "hardMuonSelection",
	       "goodTrackMuonSelection",
	       "goodD0MuonSelection",
	       "goodFitMuonSelection",
	       "ecalMipMuonSelection",
	       "hcalMipMuonSelection",
	       "isolatedMuonSelection",
	       "hardJetSelection",
	       "tightJetSelection",
	       "metSelection",
	       "filterDiMuonMassQCDveto",
	       "filterDiMuonMassZveto",
	       "filterHypoValidity",
	       "filterBtag"
              );
	    
# counter for summed stats	     
my @countSum;
my @countSumErrorSquare;
# initiate all couters with zero
foreach (@modules){
  push(@countSum,0);
  push(@countSumErrorSquare,0);  
}
	     
#print header for html
print "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"";
print "       \"http://www.w3.org/TR/html4/strict.dtd\">\n";
print "<html>\n";
print "<head>\n";
print "<title>Cutflow Table</title>\n";
print "</head>\n";
print "<body>\n";
print "<h1>Cutflow</h1>\n";
print "<table border=\"1\" style=\"text-align:right;\">\n";
print "<tr>\n  <th style=\"width:250px\";></th>";  

# print table header
foreach my $module (@modules) {
  print "  <th style=\"width:250px\";>$module</th>";
}; # end loop over modules    
print "</tr>\n";

# loop over all files
for( my $i=0; $i<@files-1; $i++) { #files-1 because it is not looped over datafile 

  my $file = $files[$i];  
  #skip if file does not exist
  if(!-e $file) {next;}  
  open(IN,$file);
    
  # get event weight for file i from array
  my $weight = $weights[$i];
  
  # print filelabel into first column  
  my $filelabel = substr($file,0,index($file,"."));
  print "<tr>\n  <td>$filelabel</td>";  
  
  # loop over all modules  
  for( my $j=0; $j<@modules; $j++) {

    my $module = $modules[$j];    
  
    # find lines in summary table
    my @modline = `grep TrigReport $file | grep $module`;
    # extract number from line
    if($modline[0] =~ /TrigReport\s+\d\s+\d\s+\d+\s+(\d+).*/) {
      my $passed = $1;
      if($passed>0){    
        #statistical error is just squareroot        
        my $staterror = sqrt($passed);
        #weight eventnumbers and statistic errors
        $passed *= $weight;
        $staterror *= $weight;
	
	$countSum[$j] += $passed;
	$countSumErrorSquare[$j] += $weight*$passed;
      
        print "  <td style=\"white-space:nowrap;\">";
        printf("%.3f", $passed);
        print " +- ";
        printf("%.3f", $staterror);
        print "</td>";
      }
      else{ # if event number is zero
        print "  <td>-</td>";       
      }
    } else {
      print "  <td>ERROR: didn't find module $module from $file</td>";
    };         
  }; # end loop over modules    
  close IN;
  print "</tr>\n";
}; # end loop over files


# table line for added stats
print "<tr>\n  <td>Sum</td>";
for( my $i=0; $i<@modules; $i++) {
  print "  <td style=\"white-space:nowrap;\">";
  printf("%.3f", $countSum[$i]);
  print " +- ";
  printf("%.3f", sqrt($countSumErrorSquare[$i]));
  print "</td>";
}; # end loop over modules    
print "</tr>\n";

# table line for real data
print "<tr>\n  <td>Data</td>";

if(-e $files[-1]){
  my $file = $files[-1];
  open(IN,$file);
  
  for( my $i=0; $i<@modules; $i++) {
    my $module = $modules[$i];      
    # find lines in summary table
    my @modline = `grep TrigReport $file | grep $module`;
    # extract number from line
    if($modline[0] =~ /TrigReport\s+\d\s+\d\s+\d+\s+(\d+).*/) {
      my $passed = $1;
      if($passed>0){           
        my $staterror = sqrt($passed);       
        print "  <td style=\"white-space:nowrap;\">";
        printf("%.3f", $passed);
        print " +- ";
        printf("%.3f", $staterror);
        print "</td>";
      }
      else{ # if event number is zero
        print "  <td>-</td>";       
      }
    } else {
      print "  <td>ERROR: didn't find module $module from $file</td>";
    };  
  }; # end loop over modules    
print "</tr>\n";
}

# close table
print "</table>\n";
print "</html>\n";

exit;
