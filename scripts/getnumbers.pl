#!/usr/sue/bin/perl

# -----------------------------------------------------------------------------------
# Script to add numbers from a given input file _mynumbers.txt_; the expected file 
# structure is:
#
# text0 d0  d1  d2  d3  d4  d5 text1
#
# In common use (i.e. if the input fil is derived from cmsRun summaries) you will
# have the following association:
#
# text1 : TrigReport (fixed),
# d0    : Trig (1),
# d1    : Bit# (0),
# d2    : Visited,
# d3    : Passed,
# d4    : Failed,
# d5    : Error,
# text1 : module/filter name of interest
#
# This script is expected to be used with _getnumbers.sh_.
# -----------------------------------------------------------------------------------

$File = "mynumbers.txt";

$nvisited=0;
$npassed=0;
$nfailed=0;
$nerror=0;

open(IN,"$File");
while($line=<IN>)
{
    chop($line);
    ($TrigReport,$Trig,$Bit,$Visited,$Passed,$Failed,$Error,$Name) = split(" ",$line);
    $nvisited=$nvisited+$Visited;
    $npassed=$npassed+$Passed;
    $nfailed=$nfailed+$Failed;
    $nerror=$nerror+$Error;
}
print "Visited: $nvisited\n";
print "Passed: $npassed\n";
print "Failed: $nfailed\n";
print "Error: $nerror\n";
close(IN);
