#!/usr/bin/perl

use strict;
use utf8;
use feature 'unicode_strings';
binmode(STDOUT, ":utf8");

if ($ARGV[0] =~ /about/i){
    print "Interrogates arduino weather station, parses results and pushes to influxdb.\n";
    exit 0;
}

my $url = "http://n.n.n.n";
open(HTTP, '-|', "curl -s '$url' | grep \"|\" | sed 's/ //g' | sed 's/<.*//g'");

binmode(HTTP, ":utf8");

while ( my $line = <HTTP>) {

    chomp;
    my ($temp, $humid, $barom, $dewpt) = split /\|/, $line;
    chomp $dewpt;
    my $push = `curl -s -i XPOST \"http://localhost:8086/write?db=weather\" --data-binary \"conditions temp=$temp,humid=$humid,barometer=$barom,dewpt=$dewpt\"\n`;

    print $push;

}
close(HTTP);

exit 0;
