#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use File::Path;

sub syntax {
    print <<END_USAGE_INFO;
****************************************************************
nafJobSplitter.pl
A very simple script to split jobs and submit them to the NAF
****************************************************************

Consider using the config file MyAna.py. It may contain a long
list of input files.

Instead of running "cmsRun MyAna.py", run "nafJobSplitter.pl NumberOfJobs MyAna.py"

This will create a directory "naf_MyAna" with all files needed to submit 
many jobs. Then it will submit all jobs to the NAF batch system.

Please cd to the directory containing the config file first:

DO: "cd PATH_TO_CONFIG ; PATH_TO_SUBMIT/nafJobSplitter.pl 5 configfile.py"
DO NOT: "./nafJobSplitter.pl 5 PATH_TO_CONFIG/configfile.py".

Run "qstat -u your_username" to check if your jobs are running.
If needed, use "qsub naf_MyAna/jobNUMBER.sh" to resubmit a certain job.

Finally, add the histograms: hadd output.root naf_MyAna/*.root

The jobs will be split on a per file basis, i.e. if you run over 10 files, you cannot 
use more than 10 jobs. If you run over 3 files using 2 jobs, then one job will run
over 2 files and one job will run over 1 file (ignoring file sizes).

END_USAGE_INFO
    exit 1;
}

my ($numberOfJobs, $config) = @ARGV;
syntax() unless $config;
$config =~ s/\.py$//;

mkpath "naf_$config";
for my $job (0..$numberOfJobs-1) {
    my $cfg = getConfigTemplate();
    $cfg =~ s/CONFIGFILE/$config/g;
    $cfg =~ s/OUTPUTFILE/$config-$job.root/g;
    $cfg =~ s/NUMBER_OF_JOBS/$numberOfJobs/g;
    $cfg =~ s/JOB_NUMBER/$job/g;

    open my $JOB, '>', "naf_$config/$config$job.py" or die $!;
    print $JOB $cfg;
    
    $cfg = getBatchsystemTemplate();
    $cfg =~ s/CONFIGFILE/$config$job/g;
    $cfg =~ s/OUTPUTFILE/$config-$job.root/g;
    $cfg =~ s/NUMBER/$job/g;
    $cfg =~ s/DIRECTORY/$config/g;
    open my $BATCH, '>', "naf_$config/job$job.sh" or die $!;
    print $BATCH $cfg;
}

$_ = 5;
while (--$_) {
    print "Submitting in $_ seconds, press Ctrl-C to cancel\n";
    sleep 1;
}

for my $job (0..$numberOfJobs-1) {
    print "Submitting job $job...\n";
    system("qsub naf_$config/job$job.sh");
}



sub getConfigTemplate {
    return <<END_OF_TEMPLATE;

from CONFIGFILE import *

numberOfFiles = len(process.source.fileNames)
numberOfJobs = NUMBER_OF_JOBS
jobNumber = JOB_NUMBER

process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
print "running over these files:"
print process.source.fileNames

## overwrite TFileService
process.TFileService = cms.Service("TFileService",
    fileName = cms.string('OUTPUTPATH/OUTPUTFILE')
)

END_OF_TEMPLATE
}

sub getBatchsystemTemplate {
    return <<'END_OF_TEMPLATE';
#!/bin/zsh
#
#(make sure the right shell will be used)
#$ -S /bin/zsh
#
#(the cpu time for this job)
#$ -l h_cpu=11:00:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=2000M
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#
# use current dir and current environment
#$ -cwd
#$ -V
#
#$ -o naf_DIRECTORY/outNUMBER.txt

# change to scratch directory (local, not lustre in this example)

current=`pwd`

perl -pe 's/OUTPUTPATH/$ENV{TMPDIR}/g' < $current/naf_DIRECTORY/CONFIGFILE.py > $TMPDIR/run.py
cmsRun $TMPDIR/run.py

mv $TMPDIR/OUTPUTFILE $current/naf_DIRECTORY/

END_OF_TEMPLATE
}
