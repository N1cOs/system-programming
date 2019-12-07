#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Socket;

my $SOCK_PATH = "/tmp/server.sock";
my $sock_addr = sockaddr_un($SOCK_PATH);

socket(my $server, PF_UNIX, SOCK_STREAM, 0)
    or die "Error in socket: $!";
connect($server, $sock_addr)
    or die "Error in connect: $!";
print <$server>