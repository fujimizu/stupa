#!/usr/bin/perl

use strict;
use warnings;
use StupaEvhttpClient;

my $client = StupaEvhttpClient->new(
    host    => 'localhost',
    port    => 8080,
    timeout => 5,
);
my $size = $client->size(), "\n";
print "size: $size\n";

my $results;
$results = $client->search_by_document(['トヨタ自動車', '日産'], 10);
if ($results) {
    my @document_ids = sort { $results->{$b} <=> $results->{$a} }
        keys %{ $results };
    for (my $i = 0; $i < scalar @document_ids; $i++) {
        printf "%d\t%s\t%.3f\n",
            $i+1, $document_ids[$i], $results->{$document_ids[$i]};
    }
}
print "\n";

$results = $client->search_by_feature(['トヨタ', '日産'], 10);
if ($results) {
    my @document_ids = sort { $results->{$b} <=> $results->{$a} }
        keys %{ $results };
    for (my $i = 0; $i < scalar @document_ids; $i++) {
        printf "%d\t%s\t%.3f\n",
            $i+1, $document_ids[$i], $results->{$document_ids[$i]};
    }
}
print "\n";

