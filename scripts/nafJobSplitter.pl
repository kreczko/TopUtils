#!/usr/bin/perl

use strict;
use warnings;
use POSIX;
use File::Basename;
use File::Path;
use Data::Dumper;
use Term::ANSIColor qw(colored);
use Getopt::Std;

use constant C_OK => 'green bold';
use constant C_FILE => 'bold';
use constant C_ERROR => 'red bold';
use constant C_RESUBMIT => 'magenta';

use constant MOTD => <<MOTD;
MOTD


sub syntax {
    print <<'END_USAGE_INFO';
*****************************************************************
* nafJobSplitter.pl                                             *
* A very simple script to split jobs and submit them to the NAF *
*****************************************************************

Instead of running "cmsRun MyAna.py", run "nafJobSplitter.pl [parameters] NumberOfJobs MyAna.py"

This will create a directory "naf_MyAna" with all files needed to submit 
many jobs. Then it will submit all jobs to the NAF batch system.

Please cd to the directory containing the config file first:

DO: "cd PATH_TO_CONFIG ; PATH_TO_SUBMIT/nafJobSplitter.pl 5 configfile.py"
DO NOT: "./nafJobSplitter.pl 5 PATH_TO_CONFIG/configfile.py".

The jobs will be split on a per file basis, i.e. if you run over 10 files, you cannot 
use more than 10 jobs. If you run over 3 files using 2 jobs, then one job will run
over 2 files and one job will run over 1 file (ignoring file sizes).

Parameters  
  -q: choose queue, h_cpu in hours
        default: -q 48
        to modify the default, use the environt variable NJS_QUEUE, e.g. "export NJS_QUEUE=12"
  -o: output directory 
        default: `pwd`
      Note: the output will always be stored in the current directory. If you specify
      the ouput directory, NJS will create a symlink to it. E.g. it might be useful
      to specify -o /scratch/hh/current/cms/user/$USER/njs
      Use NJS_OUTPUT environment variable to set a default
  -m: maximum number of events per job
        default: -1 (i.e. no limit)
  -n: don't join output root files, don't sum the TrigReports automatically
  -p jobid: peek into job, i.e. show the current stdout

******************************************************************
* What to do after submitting - if jobs crash / to monitor jobs  *
******************************************************************
nafJobSplitter [-n] check naf_DIRECTORY [timeInMinutes]
-> will automatically resubmit crashed jobs
Automatically joins output files and trigger report if all jobs are done
unless -n is passed. If timeInMinutes is given, it will check every 
given minutes. Program exits if all jobs are done.


END_USAGE_INFO
    print MOTD;
    exit 1;
}

sub peekIntoJob {
    my $jid = shift;
    my @jobs = grep { $_->[0] eq $jid} map {[split /\s+/]} `qstat -u $ENV{USER}`;
    #die Dumper \@jobs;
    die "Did not find the job in the batch system.\n" if @jobs != 1;
    if ($jobs[0][7] =~ /\@(.+)/) {
        print "Please wait...\n";
        system("qrsh -l h_cpu=00:01:00 -l h=$1 -l h_vmem=400M -now n 'cat /tmp/$jid.*/stdout.txt'");
    } else {
        die "Didn't find hostname\n";
    }
}

# returns a hash: jobid -> status
sub getRunningJobIDs {
    my $all = `qstat`;
    die "qstat has returned something unexpected:\n$all" unless $all =~ /^job-ID\s+prior/;
    #map { ($_ => 1) } $all =~ m/^(\d+)\s+\S+\s+\S+\s+$ENV{USER}\s+(?:r|qw)\s+/mg;
    $all =~ m/^(\d+)\s+\S+\s+\S+\s+$ENV{USER}\s+(\S+)\s+/mg;
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

########################
### PROGRAM BEGINS
########################

my %args;
getopts('np:q:o:m:', \%args);

if ($args{'p'}) {
    peekIntoJob($args{'p'});    
} else {

    my ($numberOfJobs, $config) = @ARGV;
    my $maxEvents = $args{'m'} || -1;
    syntax() unless $config;
    if ($numberOfJobs eq 'check') {
        my $done;
        do {
            { local $|=1;
            print "Looking for jobs in $ARGV[1]";
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

    createNJSDirectory("naf_$config");
    
    for my $job (0..$numberOfJobs-1) {
        my $cfg = getConfigTemplate($maxEvents);
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
}


sub createNJSDirectory {
    my $dir = shift;
    my $symlinkDir = $args{'o'} || $ENV{NJS_OUTPUT};
    if ($symlinkDir) {
        my $newDir = "$symlinkDir/" . strftime("%Y-%m-%dT%T-",localtime) . $dir;
        mkpath $newDir;
        symlink $newDir, $dir;
    } else {
        mkpath $dir;
    }
}

sub getConfigTemplate {
    my $maxEvents = shift;
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
    my $templ = <<'END_OF_BATCH_TEMPLATE';
#!/bin/zsh
#
#(make sure the right shell will be used)
#$ -S /bin/zsh
#
#(the cpu time for this job)
#$ -l h_cpu=__HCPU__
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

fs flush
current=`pwd`

perl -pe 's/OUTPUTPATH/$ENV{TMPDIR}/g' < $current/naf_DIRECTORY/CONFIGFILE.py > $TMPDIR/run.py
cmsRun $TMPDIR/run.py

if [[ $? == 0 ]] ; then
    if [[ -e $TMPDIR/joined.txt ]] ; then mv $TMPDIR/joined.txt $current/naf_DIRECTORY/ ; fi
    mv $TMPDIR/OUTPUTFILE $current/naf_DIRECTORY/
    mv $TMPDIR/stdout.txt $current/naf_DIRECTORY/outNUMBER.txt
else
    mv $TMPDIR/stdout.txt $current/naf_DIRECTORY/errNUMBER.txt
fi

#mv $TMPDIR/stderr.txt $current/naf_DIRECTORY/

END_OF_BATCH_TEMPLATE
    my $replace = $args{'q'} 
        ? ($args{'q'} . ':00:00')
        : $ENV{NJS_QUEUE} 
            ? $ENV{NJS_QUEUE}.':00:00' 
            : '48:00:00';
    $templ =~ s/__HCPU__/$replace/;
    return $templ;
}

sub checkJob {
    my $dir = shift;
    my %running = getRunningJobIDs();#print Dumper \%running; 
    my %jobids = getIDtoData("$dir/jobids.txt"); #die Dumper \%jobids;
    my $runningJobs = grep {exists $running{$_} && $running{$_} !~ /E/} keys %jobids;
    my $doneJobs = @{[glob "$dir/out*.txt"]};
    printf " -->  %d%%  --  %d jobs, %d queueing/running, %d done.\n", 
        100*$doneJobs / keys %jobids,
        scalar keys %jobids, 
        $runningJobs,
        $doneJobs;
    if ($doneJobs == keys %jobids) {
        open my $JOINED, '<', "$dir/joined.txt" or die "Cannot open joined.txt: $!\n";
        chomp(my $joined = <$JOINED>);
        $joined =~ s/^'|'$//g;
        if (!-e "$dir/$joined") {
            my $config = $dir; $config =~ s/naf_//;
            if ($args{'n'}) {
                print "-n given, so not joining output files\n";
            } else {
                print "Joining output files...\n";
                system('hadd', '-f', "$dir/$joined", glob("$dir/$config-*.root"));
                my $str = fileparse($joined, '.root') . '.txt';
                system("sumTriggerReports2.pl $dir/out*.txt > $dir/$str");
                print colored("Joined output file is: ", C_OK),
                    colored("$dir/$joined\n", C_FILE),
                    colored("Joined TrigReport is ", C_OK),
                    colored("$dir/$str\n", C_FILE);
            }
        } else {
            print colored(" - results have already been joined\n", C_OK);
        }
    } else {
        for my $batchid (sort grep { !exists $running{$_} || $running{$_} =~/E/} keys %jobids) {
            if (!-e "$dir/out$jobids{$batchid}{-id}.txt") {
                if (-e "$dir/err$jobids{$batchid}{-id}.txt") {
                    print "job $batchid --> $jobids{$batchid}{-script}: " .
                        colored("cmsRun didn't return success, see $dir/err$jobids{$batchid}{-id}.txt\n", C_ERROR);
                    print " *** remove the err file and run nafJobSplitter check again to resubmit the job.\n";
                } else {
                    if (!exists $running{$batchid}) {
                        print "job $batchid --> $jobids{$batchid}{-script} " .
                            colored("seems to have died, resubmitting...\n", C_RESUBMIT);
                        resubmitJob($dir, $jobids{$batchid}{-id}, $jobids{$batchid}{-script});
                    } else {
                        print colored("job $batchid has error state:\n", C_RESUBMIT);
                        print grep /error reason/, `qstat -j $batchid`;
                        print colored("clearing error state...\n", C_RESUBMIT);
                        system("qmod -c $batchid") == 0 or die "Cannot clear error state!\n";
                    }
                }
            }
        }
    }
}
