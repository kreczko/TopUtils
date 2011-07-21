#!/usr/bin/perl

use warnings;
use strict;
use Data::Dumper;
use Getopt::Std;

sub tempDir {
    for (qw(TMPDIR TMP TEMP TEMPDIR)) {
        return $ENV{$_} if exists $ENV{$_} && -d $ENV{$_};
    }
    "/tmp";
}

my %args;
getopts('fF:j:TO', \%args) || die "Wrong parameters\n";

unless (@ARGV) {
    print << "ENDSYNTAX";
fasthadd [-j N] [-f] [-T] [-O] target.root source1.root source2.root [source...]

Use hadd to sum up the histograms.

Parameters:
-j number of parallel jobs, default is 8
-f / -T / -O  see hadd
   to pass a number with -f (e.g. -f0) use -F (e.g. -F0) instead (due to limitation of Getopt)

ENDSYNTAX
    exit(1);
}

my $jobs = $args{j} || 8;

if ($jobs > @ARGV/4) {
    print "Not enough files or to many jobs - reducing the number of jobs!\n";
    $jobs = int(@ARGV/4);
}
print "Using $jobs hadds in parallel to join the files\n";

my $output = shift @ARGV;

my @inputFiles;
my $job = 0;
while (@ARGV) {
    push @{$inputFiles[$job]}, shift @ARGV;
    $job = ($job+1)%$jobs;
}

my $t = time;
for (1..$jobs) { unshift(@{$inputFiles[$_-1]}, tempDir() . "/fasthadd-$ENV{USER}-$t-$_.root") };
my $scriptName = tempDir() . "/fasthadd-execute-$ENV{USER}-$t.sh";

my $command = '';
for my $ar (@inputFiles) {
    $command .= "hadd @$ar & \n";
}

$command .= "wait\nhadd ";
if (exists $args{F}) {
    die "parameter '$args{F}' needs to be exactly one digit\n" unless $args{F} =~ /^\d$/;
    $command .= "-f$args{F} " if $args{F};
} else {
    $command .= "-f " if $args{f};
}
$command .= '-T ' if $args{T};
$command .= '-O ' if $args{O};
#die $command;
$command .= $output;
for (0..$jobs-1) { $command .= " " .$inputFiles[$_]->[0] };
for (0..$jobs-1) { $command .= "\nrm " .$inputFiles[$_]->[0] };

open my $cmd, '>', $scriptName or die $!;
print $cmd "#!/bin/sh\n$command";
close $cmd;
chmod 0700, $scriptName;
system($scriptName);
unlink $scriptName;

# print "$command\n";
# print Dumper \@inputFiles;

