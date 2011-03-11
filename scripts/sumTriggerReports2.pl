#! /usr/bin/env perl

# sum up triggerReport crab_0_xxxxxx_yyyyyy/res/CMSSW_*.stdout
# rewritten by Wolf Behrenhoff
# fully automatic sum everything

use strict;
use warnings;
use Data::Dumper;
use Scalar::Util qw(looks_like_number);

die "Please give job output files as parameter(s)!\n" unless my @files = @ARGV;

my @accu;

for my $filename (@files) {
    open my $fh, '<', $filename or die "Cannot open file $filename: $!\n";
    my @lines = <$fh>;
    # start of interestiong information is: TrigReport ---------- Event  Summary ------------
    shift @lines while @lines && $lines[0] !~ /^TrigReport\s+\-+\s+Event\s+Summary/;
    die "Cannot find TrigReport in $filename\n" unless @lines;
    for (0..@lines-1) {
        last if $lines[$_] =~ /TimeReport/;
        push @{$accu[$_]}, $lines[$_];
    }
}

for my $lines (@accu) {
    #split on whitespace and also store the whitespace, also allow zero whitespace between = and digit
    my $splitRE = qr /(\s+|(?<==)(?=\d))/;
    my @original = split $splitRE, @{$lines}[0];
    my @names = @original;
    for (1..@{$lines}-1) {
        my @add =  split $splitRE, @{$lines}[$_];
        for my $fieldnr (0..@add-1) {
            if (looks_like_number($add[$fieldnr])) {
                $names[$fieldnr] += $add[$fieldnr];
                $names[$fieldnr-1] = ' ' x (length($original[$fieldnr-1]) + length($original[$fieldnr]) - length($names[$fieldnr]));
            }
        }
    }
    print join '', @names;
}
