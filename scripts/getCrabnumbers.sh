#!/bin/sh

# -----------------------------------------------------------------------------------
# Script to add numbers from a given number of .err input files from _cmsRun_. The
# expected structure of the file names is of type: 
#
# filename_#_sample.err
#
# where 'filename' corresponds to a filename label, '#' corresponds to a file 
# number and 'sample' corresponds to a sample label. The file numbering may start 
# from '0' or '1'. The expected file structure is expected to originate from the 
# cmsRun Summary Report. This Report is expected to contain summary lines of type:
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
# The expected Arguments aree the following:
#
# $1    : module/filter name of interest
# $2    : directory where the .stdout files are located
# $3    : filename of the .stdout file 
# $4    : highest number of the .stdout files
#
# The script will first create a file _mynumberbuffer.txt_ to contain all lines 
# from the input files that contain the module/filter name of interest. It will 
# then create a second file _mynumbers.txt_ to contain all lines of the buffer 
# file that contain the key 'TrigReport[[:space:]]....1[[:space:]]'. The last 
# file will be the input tp the perl script _getnumbers.pl_ with which this script
# is to be used. Output will be the summed number of events that [Visited], 
# [Passed], [Failed] the module/filter and a [Error] count as provided by the 
# cms Summary Report. Finally the buffer files are removed.
#
# Example use: getCrabnumbers.sh nameOfModuleInstdoutFile folderOfstdoutFiles nameOfstdoutFiles 100
# -----------------------------------------------------------------------------------

#folder=$2
#filename=$3
#numberOfFiles=$4 # highest number of input file

numberOfFiles=$lastFile

filenr=1
while [ $filenr -le 1 ]
do
    less ${folder}/${filename}_${filenr}.stdout | grep -w $1 > mynumberbuffer.txt

    filenr=`expr $filenr + 1`
done
while [ $filenr -le $numberOfFiles ]
do
    less ${folder}/${filename}_${filenr}.stdout | grep -w $1 >> mynumberbuffer.txt
    filenr=`expr $filenr + 1`
done
less mynumberbuffer.txt | grep 'TrigReport[[:space:]]....1[[:space:]]' > mynumbers.txt
rm mynumberbuffer.txt
echo 'statistics for filter '$1':'
perl getnumbers.pl
rm mynumbers.txt
