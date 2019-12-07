#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Socket;

my $sgn = -1;
my $SOCK_PATH = "/tmp/server.sock";
my $start_time = time();
my %info = (
    time  => 0,
    pid   => $$,
    uid   => $<,
    gid   => (split(/ /, $())[0],
    load1 => 0,
    load2 => 0,
    load3 => 0
);

sub update {
    $info{time} = time() - $start_time;
    my @load = split(/load average: /, qx(uptime));
    @load = split(/, /, $load[1]);
    $info{load1} = $load[0];
    $info{load2} = $load[1];
    $info{load3} = $load[2];
}

$SIG{HUP} = sub {
    $sgn = 0;
    signal_handler();
};
$SIG{INT} = sub {
    $sgn = 1;
    signal_handler();
};
$SIG{TERM} = sub {
    $sgn = 2;
    signal_handler();
};
$SIG{USR1} = sub {
    $sgn = 3;
    signal_handler();
};
$SIG{USR2} = sub {
    $sgn = 4;
    signal_handler();
};

sub signal_handler {
    update();
    print "\n";
    if ($sgn == 0) {
        print("PID: ", $info{pid});
    }
    elsif ($sgn == 1) {
        print("UID: ", $info{uid});
    }
    elsif ($sgn == 2) {
        print("GID: ", $info{gid});
    }
    elsif ($sgn == 3) {
        print("Time: ", $info{time});
    }
    else {
        printf("Avg load system time for 1 minute: %s \n\t 5 minutes: %s \n\t 15 minutes: %s ",
            $info{load1}, $info{load2}, $info{load3});
    }
    print("\n");
}

sub get_info {
    return(
        sprintf("PID: %li, GID: %li, UID: %li\n", $info{pid}, $info{gid}, $info{uid})
            . sprintf("Server working %li seconds\n ", $info{time})
            . sprintf("Average load system time for 1 minute: %.2s%%,\n\t 5 minutes: %.2s%%,\n\t 15 minutes: %.2s%%\n",
            $info{load1},
            $info{load2},
            $info{load3})
    );
}

unlink $SOCK_PATH;
socket(my $server, PF_UNIX, SOCK_STREAM, 0)
    or die "Error in socket: $!";
bind($server, sockaddr_un($SOCK_PATH))
    or die "Can't create server: $!";
listen($server, 10)
    or die "Error in listen: $!";
while (1) {
    update();
    my $client;
    while (!accept($client, $server)) {
        if (!$!{EINTR}) {
            die "Error in accept: $!";
        }
    }
    print $client get_info();
}