#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../lib";
use StupaEvhttpClient;
use Data::Dumper;

my $client = StupaEvhttpClient->new(
    host    => 'localhost',
    port    => 8080,
    timeout => 1,
);

my $max = 10;
my @query = ('トヨタ自動車', '日産');
for (my $i = 0; $i < 10000; $i++) {
    my $results = $client->search_by_document(\@query, $max);
#    my $results = $client->size();
#    print "$i\t$results\n";
#    print "$i\n" if $i % 1000 == 0;
#    print Dumper $results if $i % 100 == 0;
    print "$i: ", Dumper $results;
}
