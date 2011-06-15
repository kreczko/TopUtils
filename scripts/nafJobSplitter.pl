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

########################
### PROGRAM BEGINS
########################

my %args;
getopts('SsbW:kJjp:q:o:m:t:', \%args);

if ($args{'p'}) {
    peekIntoJob($args{'p'});    
} else {
    my ($numberOfJobs, $config) = @ARGV;
    syntax() unless $config;
    if ($numberOfJobs eq 'check') {
        check(@ARGV[1..$#ARGV]);
        exit;
    } 
    submitNewJob($numberOfJobs, $config);
}

################################################################################################
## END MAIN
################################################################################################

sub syntax {
    print <<'END_USAGE_INFO';
nafJobSplitter.pl
 - A very simple script to split jobs and submit them to the NAF

******************************************************************
* Before submitting                                              *
******************************************************************

Make sure that the name of the variable containing your cms.Process() 
is "process". (process = cms.Process("whateverNameYouLike") )

Also make sure that you output your histograms using process.TFileService.

Please cd to the directory containing the config file.


******************************************************************
* Submitting to the NAF batch farm                               *
******************************************************************

The nafJobSplitter is easy to use: instead of running "cmsRun MyAna.py", 
execute "nafJobSplitter.pl [parameters] NumberOfJobs MyAna.py"

This will create a directory "naf_MyAna" with all files needed to submit 
many jobs. Then it will submit all jobs to the NAF batch system.

The jobs will be split on a per file basis, i.e. if you run over 10 files, you cannot 
use more than 10 jobs. If you run over 3 files using 2 jobs, then one job will run
over 2 files and one job will run over 1 file (ignoring file sizes).

Available Parameters  
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
  -J: Don't automatically join output files and sum up TrigReports
       If you forget to pass this parameter, touch 'naf_DIR/autojoin'
       to enable automatic joining. Remove the autojoin file to 
       disable auto joining.
  -k: keep all source root files after hadd'ing them
       touch/remove the file 'autoremove' to enable/disable the 
       feature at some later point in time
  -W secs: waiting time in secs before submission (default: 2 secs)

******************************************************************
* What to do after submitting - if jobs crash / to monitor jobs  *
******************************************************************

nafJobSplitter.pl [parameters] check naf_DIRECTORY [naf_DIRECTORY2 ...] 

The check command will automatically resubmit crashed jobs and/or
put jobs in Eqw state back to qw.

Available Parameters
  -j: join output root files, sum the TrigReports if all jobs are done
       You only need this if you have used -J to submit the jobs
  -t mins: perform the check every mins minutes
  -s show extended speed summary (for each job)
  -S do not show the read speed summary (faster), default is to show 
     Timing-tstoragefile-read-totalMegabytes and
     Timing-tstoragefile-read-totalMsecs
  -b show speed summary in bytes / bytes per second

To peek into running jobs, i.e. to show the current stdout:
  nafJobSplitter.pl -p jobid
where jobid is "jobid.arraynumber", e.g. "4491742.7".

END_USAGE_INFO
    exit 1;
}

sub submitNewJob {
    my ($numberOfJobs, $config) = @_;
    die "Error: $config is not a CMS configuration file!\n" unless -f $config && -s $config;
    $config =~ s/\.py$//;
    my $shellScript = "j_${config}.sh";

    createNJSDirectory("naf_$config");
    
    my $cfgPy = getConfigTemplate();
    my $cfgSh = getBatchsystemTemplate();
    
    for ($cfgPy, $cfgSh) {
        s/CONFIGFILE/$config/g;
        s/NUMBER_OF_JOBS/$numberOfJobs/g;
        s/DIRECTORY/$config/g;
    }

    open my $JOB, '>', "naf_$config/$config.py" or die $!;
    print $JOB $cfgPy;
    
    open my $BATCH, '>', "naf_$config/$shellScript" or die $!;
    print $BATCH $cfgSh;
    
    unless ($args{'J'}) {
        open my $BATCH, '>', "naf_$config/autojoin" or die $!;
    }
    unless ($args{'k'}) {
        open my $BATCH, '>', "naf_$config/autoremove" or die $!;
    }
    #mkdir "naf_$config/$_" or die $! for 1..$numberOfJobs);
    
    $_ = defined $args{'W'} ? $args{'W'} : 2;
    die "Invalid waiting time!\n" unless /^\d+$/ && $_>=0 && $_<1000;
    ++$_;
    while (--$_) {
        print "Submitting in $_ seconds, press Ctrl-C to cancel\n";
        sleep 1;
    }
    {open my $FH, '>', "naf_$config/exe" or die $!; print $FH $shellScript;}
    die "Cannot submit job!\n" unless submitJob("naf_$config", 1, $numberOfJobs, $shellScript);
}

sub submitJob {
    my ($dir, $from, $to, $script) = @_;
    $script ||= do { open my $FH, '<', "$dir/exe"; <$FH> };
    my $line = `qsub -t $from-$to:1 $dir/$script`;
    if ($line =~ /Your job-array (\d+)(?:\.\d+-\d+:1) \(".+"\) has been submitted/) {
        my $success;
        my $ids = ''; $ids .= "$_\t$1\n" for $from..$to;
        do {
            $success = open my $FH, '>>', "$dir/jobids.txt";
            $success &&= print $FH $ids;
            if (!$success) {
                print "$!\nCan't write to $dir/jobids.txt, trying again in 5sec\n";
                sleep 5;
            }
        } while (!$success);
        print $from == $to ? "Job $from submitted with job-id $1\n" : "Jobs $from to $to submitted with job-id $1\n";
        return $1;
    } else {
        die "Cannot submit job!\n$line";
    }
}

sub check {
    my @dirs = @_;
    my $alldone;
    do {
        $alldone = 1;
        my $qstat = QStat->new();
        for my $dir (@dirs) {
            { local $|=1; print "Looking for jobs in $dir"; }
            $alldone = checkJob($dir, $qstat) && $alldone;
        }
        if (!$alldone && defined $args{'t'}) {
            print "Waiting for next check, cancel with Ctrl-C...\n";
            sleep $args{'t'}*60-10;
            print "Only 10 seconds left, don't cancel me once the check starts!\n";
        }
    } while (!$alldone && defined $args{'t'});    
}

sub checkJob {
    my ($dir, $qstat) = @_;
    my %jobs = getJobs($qstat, "$dir/jobids.txt"); #get arrayid => job-object
    
    my ($NRunning, $NResubmitted, $NWaiting, $NDoneJobs, $NError) = (0) x 5;
    
    while (my ($arrId, $job) = each %jobs) {
        if ($job) {
            my $state = $job->state();
            if ($state =~ /E/) {
                print colored("\njob has error state:\n", C_RESUBMIT);
                print grep /error reason/, $job->statusInfo();
                print colored("clearing error state...\n", C_RESUBMIT);
                $job->clearError() == 256 or die "Cannot clear error state!\n";
                ++$NResubmitted;
            } elsif ($state =~ /r/) {
                ++$NRunning;
            } else {
                ++$NWaiting;
            }
        } else {
            #job is not there
            if (-e "$dir/out$arrId.txt") {
                ++$NDoneJobs;
            } elsif (-e "$dir/err$arrId.txt") {
                ++$NError;
                print colored("\ncmsRun didn't return success, see $dir/err$arrId.txt", C_ERROR);
            } else {
                print colored("\n -> job $arrId seems to have died, resubmitting...", C_RESUBMIT);
                submitJob($dir, $arrId, $arrId);
                ++$NResubmitted;
            }
        }
    }
    print "\n" if $NResubmitted || $NError;
    printf " -->  %d%%  --  %d jobs", 100*$NDoneJobs / keys %jobs, scalar keys %jobs, 
    my @N = (', %d queueing' => $NWaiting,
             ', %d running' => $NRunning,
             ', %d resubmitted' => $NResubmitted,
             ', %d error' => $NError,
             ', %d done' => $NDoneJobs);
    while (@N) {
        my $str = shift @N; my $val = shift @N;
        printf $str, $val if $val;
    }
    my @details;
    @details = showFJRsummary($dir) unless $args{'S'};
    print ".\n";
    showFJRdetails(@details) if $args{'s'} && !$args{'S'};
    
    if ($NDoneJobs == keys %jobs) {
        open my $JOINED, '<', "$dir/joined.txt" or die "Cannot open joined.txt: $!\n";
        my $joined = <$JOINED>;
        if (!-e "$dir/$joined") {
            my $config = $dir; $config =~ s/naf_//;
            if ($args{'j'}) {
                print "Joining output files...\n";
                system('hadd', '-f', "$dir/$joined", glob("$dir/$config-*.root"));
                my $str = fileparse($joined, '.root') . '.txt';
                system("sumTriggerReports2.pl $dir/out*.txt > $dir/$str");
                print colored("Joined output file is: ", C_OK),
                    colored("$dir/$joined\n", C_FILE),
                    colored("Joined TrigReport is ", C_OK),
                    colored("$dir/$str\n", C_FILE);
            } else {
                print " - Hint: pass the -j option to join files\n";
            }
        } else {
            print colored(" - results have already been joined\n", C_OK);
        }
    }
}

sub bytesToHuman {
    my $bytes = shift;
    return ($bytes, '') if $args{'b'};
    my @PREFIX = ('', qw(k M G T P E Z Y));
    my $index = 0;
    while ($bytes >= 1000) {
        $bytes /= 1000;
        ++$index;
    }
    return ($bytes, $PREFIX[$index]);
}

sub readFJRFileWithRE {
    my ($fileName, $sum) = @_;
    open my $fh, '<', $fileName or die "Cannot open $fileName: $!\n";
    my $file = do { local $/; <$fh> };
    my %result;
    my @files = $file =~ m!^.*/(.*\.root)</LFN>$!mg;
    for (keys %$sum) {
        $result{$_} = $1 if $file =~ m!^\s*<Metric Name="$_" Value="(.*?)"/>$!m;
    }
    return (\@files, %result);
}

sub showFJRsummary {
    my $dir = shift;
    my %sum;
    #Timing-dcap-read-totalMegabytes Timing-dcap-read-totalMsecs 
    @sum{qw(Timing-tstoragefile-read-totalMegabytes Timing-tstoragefile-read-totalMsecs)} = (0) x 10;
    my @files = glob("$dir/jobreport*.xml");
    my @details;
    for (@files) {
        my ($filesRef, %perf) = readFJRFileWithRE($_, \%sum);
        for (keys %sum) {
            $sum{$_} += $perf{$_};
        }
        push @details, [$filesRef, 
                        $perf{'Timing-tstoragefile-read-totalMegabytes'}*1e6,
                        $perf{'Timing-tstoragefile-read-totalMegabytes'}*1e6 / ($perf{'Timing-tstoragefile-read-totalMsecs'}/1e3)
                        ];
    }
    if (@files) {
        printf ", read %.2f %sB at %.2f %sB/s per job", 
            bytesToHuman($sum{'Timing-tstoragefile-read-totalMegabytes'}*1e6), 
            bytesToHuman($sum{'Timing-tstoragefile-read-totalMegabytes'}*1e6 / ($sum{'Timing-tstoragefile-read-totalMsecs'}/1e3));        
    }
    
    return @details;
}

sub showFJRdetails {
    my @details = @_;
    for (sort {$b->[2] <=> $a->[2]} @details) {
        printf "%.2f %sB/s (total %.2f %sB): ", bytesToHuman($_->[2]), bytesToHuman($_->[1]);
        print "@{$_->[0]}\n";
    }
}


# returns a list of this:
#       1 => blessed ref of job 444257.1
#       7 => undef
sub getJobs {
    my ($qstat, $file) = @_;
    open my $FH, '<', $file or die "$file: $!";
    my %result;
    while (<$FH>) {
        $result{$1} = $qstat->job("$2.$1") if /^(\d+)\t(\d+)$/;
    }
    %result;
}

sub createNJSDirectory {
    my $dir = shift;
    my $symlinkDir = $args{'o'} || $ENV{NJS_OUTPUT};
    if ($symlinkDir) {
        my $newDir = "$symlinkDir/" . strftime("%Y-%m-%dT%T-",localtime) . $dir;
        if (-l $dir) {
            warn "Old symlink $dir exists, removing it.\n";
            unlink $dir;
        }
        mkpath $newDir;
        symlink $newDir, $dir or die "Cannot create symlink $dir --> $newDir\n";
    } else {
        mkpath $dir;
    }
}

sub peekIntoJob {
    my $jid = shift;
    my $job = QStat->new()->job($jid);
    if ($job) {
        die "The job is not running\n" unless $job->state() =~ /r/;
        $job->peek();
    } else {
        die "Cannot find job. Please pass jobid.arraynumber (separated by .) of a running job\n";
    }
}

sub getConfigTemplate {
    my $maxEvents = $args{'m'} || -1;
    return <<END_OF_TEMPLATE;

import os
from CONFIGFILE import *

numberOfFiles = len(process.source.fileNames)
numberOfJobs = NUMBER_OF_JOBS
jobNumber = int(os.environ["SGE_TASK_ID"]) - 1

process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
print "running over these files:"
print process.source.fileNames

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32($maxEvents)
)

if jobNumber == 0:
    fh = open('OUTPUTPATH/joined.txt', 'w')
    fh.write(eval(process.TFileService.fileName.pythonValue()))
    fh.close

## overwrite TFileService
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("OUTPUTPATH/CONFIGFILE-" + str(jobNumber + 1) + ".root")
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
exec > $TMPDIR/stdout.txt 2>&1

# change to scratch directory

#fs flush
current=`pwd`

perl -pe 's/OUTPUTPATH/$ENV{TMPDIR}/g' < $current/naf_DIRECTORY/CONFIGFILE.py > $TMPDIR/run.py

# if [ "$SGE_TASK_ID" = "0" ] ; then
#     # do nothing
# else
#     sleeptime=$((20 + $RANDOM*NUMBER_OF_JOBS/32767))
#     echo "Sleeping for $sleeptime..."
#     sleep $sleeptime
# fi

PYTHONDONTWRITEBYTECODE=1 cmsRun -j $TMPDIR/jobreport.xml $TMPDIR/run.py

if [ "$?" = "0" ] ; then
    if [ -e $TMPDIR/joined.txt ] ; then mv $TMPDIR/joined.txt $current/naf_DIRECTORY/ ; fi
    mv $TMPDIR/jobreport.xml $current/naf_DIRECTORY/jobreport$SGE_TASK_ID.xml
    mv $TMPDIR/CONFIGFILE-$SGE_TASK_ID.root $current/naf_DIRECTORY/
    if [ -e $current/naf_DIRECTORY/autojoin ] ; then
        NDone=`ls $current/naf_DIRECTORY/out*.txt | wc -l`
        NDone=$(($NDone + 1))
        if [ "$NDone" = "NUMBER_OF_JOBS" ] ; then
            joined=`cat $current/naf_DIRECTORY/joined.txt`
            hadd -f $current/naf_DIRECTORY/$joined.$SGE_TASK_ID $current/naf_DIRECTORY/CONFIGFILE-*.root
            if [ "$?" = "0" ] ; then
                mv -f $current/naf_DIRECTORY/$joined.$SGE_TASK_ID $current/naf_DIRECTORY/$joined
                cp -f $TMPDIR/stdout.txt $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
                sumTriggerReports2.pl $current/naf_DIRECTORY/out*.txt > $current/naf_DIRECTORY/`basename $joined .root`.txt
                if [ -e $current/naf_DIRECTORY/autoremove ] ; then
                    rm -f $current/naf_DIRECTORY/CONFIGFILE-*.root
                fi
            fi
        fi
    fi
    mv -f $TMPDIR/stdout.txt $current/naf_DIRECTORY/out$SGE_TASK_ID.txt
else
    mv $TMPDIR/stdout.txt $current/naf_DIRECTORY/err$SGE_TASK_ID.txt
fi

END_OF_BATCH_TEMPLATE
    my $replace = $args{'q'} 
        ? ($args{'q'} . ':00:00')
        : $ENV{NJS_QUEUE} 
            ? $ENV{NJS_QUEUE}.':00:00' 
            : '48:00:00';
    $templ =~ s/__HCPU__/$replace/;
    return $templ;
}


################################################################################################
##  Classes to read qstat
################################################################################################

package Job;
sub new {
    my ($class, $qstatLine) = @_;
    my $self = \$qstatLine;
    bless $self, $class;
}

sub peek {
    my $self = shift;
    die "Job is not running, cannot peek\n" unless $self->state() eq 'r';
    if ($self->queue() =~ /\@(.+)/) {
        print "Please wait, this can take up to a few minutes...\n";
        my $jid = $self->fullId();
        system("qrsh -l h_cpu=00:01:00 -l h=$1 -l h_vmem=400M -now n 'cat /tmp/$jid.*/stdout.txt'");
    } else {
        die "Didn't find hostname\n";
    }
}

sub statusInfo {
    my $self = shift;
    my $command = 'qstat -j ' . $self->fullId();
    `$command`;
}

sub clearError {
    my $self = shift;
    system('qmod -cj ' . $self->fullId());
}

sub extract {
    my ($self, $tag) = @_;
    return unless $$self =~ m!<$tag>(.*?)</$tag>!;
    $1;
}

sub id {
    my $self = shift;
    $self->extract('JB_job_number');
}

sub priority {
    my $self = shift;
    $self->extract('JAT_prio');
}

sub name {
    my $self = shift;
    $self->extract('JB_name');
}

sub user {
    my $self = shift;
    $self->extract('JB_owner');
}

sub state {
    my $self = shift;
    $self->extract('state');
}

sub time {
    my $self = shift;
    $self->extract('JB_submission_time') || $self->extract('JAT_start_time');
}

sub queue {
    my $self = shift;
    $self->extract('queue_name');
}

sub slots {
    my $self = shift;
    $self->extract('slots');
}

sub arrayid {
    my $self = shift;
    $self->extract('tasks') || 0;
}

sub fullId {
    my $self = shift;
    if (my $arr = $self->arrayid()) {
        return $self->id() . ".$arr";
    } else {
        return $self->id();
    }
}

package QStat;
sub new {
    my $class = shift;
    my $self = {};
    my $all = `qstat -xml -g d -u $ENV{USER}`;
    die "qstat has returned something unexpected:\n$all" unless $all =~ m!</job_info>!;
    for ($all =~ m!<job_list.*?>(.*?)</job_list>!sg) {
        my $job = Job->new($_);
        $self->{$job->fullId()} = $job;
    }
    bless $self, $class;
}

sub jobs {
    my ($self) = @_;
    return values %$self;
}

sub job {
    my ($self, $fullId) = @_;
    my ($j) = grep {$fullId eq $_->fullId()} $self->jobs();
    return $j if $j;
    return;
}

1;

