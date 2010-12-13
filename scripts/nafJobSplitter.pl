#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use File::Path;
use Data::Dumper;


use constant MOTD => <<MOTD;
***
  New: to prevent afs hangs, output is now written to the worker node first.
       Only at the end of the job it is copied to the current directory.
       
  New: jobs now have names depending on the config file: jJOBNUMBER_CONFIG

MOTD


sub syntax {
    print <<END_USAGE_INFO;
*****************************************************************
* nafJobSplitter.pl                                             *
* A very simple script to split jobs and submit them to the NAF *
*****************************************************************

Consider using the config file MyAna.py. It may contain a long
list of input files.

Instead of running "cmsRun MyAna.py", run "nafJobSplitter.pl NumberOfJobs MyAna.py [maxEvents=-1]"

This will create a directory "naf_MyAna" with all files needed to submit 
many jobs. Then it will submit all jobs to the NAF batch system.
The optional parameter maxEvents is used for each job, i.e. 2 jobs and maxEvents=100 
will process 200 events in total.

Please cd to the directory containing the config file first:

DO: "cd PATH_TO_CONFIG ; PATH_TO_SUBMIT/nafJobSplitter.pl 5 configfile.py"
DO NOT: "./nafJobSplitter.pl 5 PATH_TO_CONFIG/configfile.py".

Run "qstat -u your_username" to check if your jobs are running.
If needed, use "qsub naf_MyAna/njs_MyAna_jobNUMBER.sh" to resubmit a certain job.

Finally, add the histograms: hadd output.root naf_MyAna/*.root

The jobs will be split on a per file basis, i.e. if you run over 10 files, you cannot 
use more than 10 jobs. If you run over 3 files using 2 jobs, then one job will run
over 2 files and one job will run over 1 file (ignoring file sizes).

******************************************************************
* What to do after submitting - if jobs crash / to monitor jobs  *
******************************************************************
nafJobSplitter check naf_DIRECTORY [timeInMinutes]
-> will automatically resubmit crashed jobs
Automatically joins output files and trigger report if all jobs are done

If timeInMinutes is given, it will check every given minutes. Program
exits if all jobs are done.


END_USAGE_INFO
    print MOTD;
    exit 1;
}

sub getRunningJobIDs {
    my $all = `qstat`;
    die "qstat has returned something unexpected:\n$all" unless $all =~ /^job-ID\s+prior/;
    map { ($_ => 1) } $all =~ m/^(\d+)\s+\S+\s+\S+\s+$ENV{USER}\s+(?:r|qw)\s+/mg;
}

sub getIDtoData {
    
# returns a list of this:
#           '2979237' => {
#                          '-script' => 'j7_Run2010A_Nov04ReReco_cff.sh',
#                          '-id' => '7'
#                        }

    my $file = shift;
    open my $FH, '<', $file or die "$file: $!";
    chomp(my @all = <$FH>);
    my %id;
    my @del;
    my %result = map { 
        my @row = split "\t";
	push @del, $id{$row[0]} if exists $id{$row[0]};
	$id{$row[0]} = $row[1];

	($row[1] => {-id => $row[0], -script => $row[2]}) 
	} @all;
    delete $result{$_} for @del;
    %result;
}

sub checkJob {
    my $dir = shift;
    my %running = getRunningJobIDs();#print Dumper \%running; 
    my %jobids = getIDtoData("$dir/jobids.txt"); #die Dumper \%jobids;
    my $runningJobs = grep {exists $running{$_}} keys %jobids;
    my $doneJobs = @{[glob "$dir/out*.txt"]};
    printf " -->  %d%%  --  %d jobs, %d running, %d done.\n", 
        100*$doneJobs / keys %jobids,
        scalar keys %jobids, 
        $runningJobs,
        $doneJobs;
    for my $batchid (sort grep { !exists $running{$_} } keys %jobids) {
        if (!-e "$dir/out$jobids{$batchid}{-id}.txt") {
            print "job $batchid --> $jobids{$batchid}{-script} seems to have died, resubmitting...\n";
            resubmitJob($dir, $jobids{$batchid}{-id}, $jobids{$batchid}{-script});
        }
    }
#    exit;
    
#    print Dumper \%jobids;
}

sub submitJob {
    my ($dir, $N, $script) = @_;
    my $line = `qsub $dir/$script`;
    if ($line =~ /Your job (\d+) \(".+"\) has been submitted/) {
        return $1;
    } else {
        die "Cannot submit job!\n$line";
    }
}

sub resubmitJob {
    my ($dir, $N, $script) = @_;
    #print "would resubmit\n";exit;
    my $newJid = submitJob($dir, $N, $script);
    if ($newJid) {
        my $success;
        do {
            $success = open my $FH, '>>', "$dir/jobids.txt";
            $success &&= print $FH "$N\t$newJid\t$script\n";
            if (!$success) {
                print "$!\nCan't write to $dir/jobids.txt, trying again in 5sec\n";
                sleep 5;
            }
        } while (!$success);
        print "Job $script --> $newJid has been resubmitted\n";
    } else {
        die "Could not resubmit";
    }
}

my ($numberOfJobs, $config, $maxEvents) = @ARGV;
syntax() unless $config;
if ($numberOfJobs eq 'check') {
    my $done;
    do {
        { local $|=1;
          print "Looking for jobs in $ARGV[1]...";
        }
        $done = checkJob($ARGV[1]);
        if (!$done && defined $ARGV[2]) {
            print "Waiting for next check, cancel with Ctrl-C...\n;";
            sleep $ARGV[2]*60-10;
            print "Only 10 seconds left, don't cancel me once the check starts!\n";
        }
    } while (!$done && defined $ARGV[2]);
    exit;
} 
$maxEvents ||= -1;
print MOTD;
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
    open my $BATCH, '>', "naf_$config/j${job}_${config}.sh" or die $!;
    print $BATCH $cfg;
}

print "Precompiling python files...\n";
for my $job (0..$numberOfJobs-1) {
    system("python -mcompileall naf_$config");
}

$_ = 3;
while (--$_) {
    print "Submitting in $_ seconds, press Ctrl-C to cancel\n";
    sleep 1;
}

open my $JOBIDS, '>>', "naf_$config/jobids.txt" or die $!;
for my $job (0..$numberOfJobs-1) {
    print "Submitting job $job...\n";
    my $jid = submitJob("naf_$config", $job, "j${job}_${config}.sh");
    if ($jid) {
        print $JOBIDS "$job\t$jid\tj${job}_${config}.sh\n";
    } else {
        die "Cannot submit job!\n";
    }
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

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32($maxEvents)
)

if jobNumber == 0:
    fh = open('OUTPUTPATH/joined.txt', 'w')
    fh.write(process.TFileService.fileName.pythonValue() + "\\n")
    fh.close

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
#$ -o /dev/null
###### -o naf_DIRECTORY/outNUMBER.txt
exec > $TMPDIR/stdout.txt 2>&1
#exec > $TMPDIR/stdout.txt 2>$TMPDIR/stderr.txt

# change to scratch directory

current=`pwd`

perl -pe 's/OUTPUTPATH/$ENV{TMPDIR}/g' < $current/naf_DIRECTORY/CONFIGFILE.py > $TMPDIR/run.py
cmsRun $TMPDIR/run.py

if [[ -e $TMPDIR/joined.txt ]] ; then mv $TMPDIR/joined.txt $current/naf_DIRECTORY/ ; fi
mv $TMPDIR/OUTPUTFILE $current/naf_DIRECTORY/
mv $TMPDIR/stdout.txt $current/naf_DIRECTORY/outNUMBER.txt
#mv $TMPDIR/stderr.txt $current/naf_DIRECTORY/

END_OF_TEMPLATE
}
