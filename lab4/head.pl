#!/usr/bin/perl -T 

use strict;
use warnings qw(FATAL all);
use Getopt::Long qw(GetOptions);

sub print_chars {
    my $fd = $_[0];
    my $n = $_[1];

    my $chars;
    read $fd, $chars, $n;
    print $chars;
}

sub print_lines {
    my $fd = $_[0];
    my $left = $_[1];
    
    my $line = <$fd>;
    while ($line && $left > 0) {
        print $line;
        $line = <$fd>;
        $left--;
    }
}

my $bytes;
my $lines = 10;
GetOptions(
    'bytes|c=i' => \$bytes,
    'lines|n=i' => \$lines,
) or die "Usage: $0 [-c, --bytes=NUM] [-n, --lines=NUM] [FILE]...";

if (scalar @ARGV >= 1) {
    foreach my $file (@ARGV) {
        my $fd;
        if ($file eq "-") {
            $fd = *STDIN;
            $file = "standart input";
        } elsif(not open($fd, '<', $file)) {
            warn "cannot open '$file' for reading";
            next;
        }

        if (scalar @ARGV > 1) {
            print "==> $file <==\n";
        }
        
        if (defined $bytes) {
            print_chars $fd, $bytes;
        } else {
            print_lines $fd, $lines;
        }
        close $fd;
    }
} else {
    if (defined $bytes) {
        print_chars *STDIN, $bytes;
    } else {
        print_lines *STDIN, $lines;
    }
}