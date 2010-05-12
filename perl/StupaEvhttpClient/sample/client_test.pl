#!/usr/bin/perl

use strict;
use warnings;
use FindBin;
use lib "$FindBin::Bin/../lib";
use StupaEvhttpClient;

my $client = StupaEvhttpClient->new(
    host    => 'localhost',
    port    => 8080,
    timeout => 5,
);

## size
my $size = $client->size();
print "Size: $size\n\n";

## search by document ids
my $results;
my $max = 10;
my @query = ('トヨタ自動車', '日産');
printf "Search_by_document (max: $max, query: %s):\n",
    join(', ', @query);
$results = $client->search_by_document(\@query, $max);
if ($results) {
    my @document_ids = sort { $results->{$b} <=> $results->{$a} }
        keys %{ $results };
    for (my $i = 0; $i < scalar @document_ids; $i++) {
        printf "%d\t%s\t%.3f\n",
            $i+1, $document_ids[$i], $results->{$document_ids[$i]};
    }
}
print "\n";

## search by feature ids
@query = ('トヨタ', '日産');
printf "Search_by_feature (max: $max, query: %s):\n",
    join(', ', @query);
$results = $client->search_by_feature(\@query, $max);
if ($results) {
    my @document_ids = sort { $results->{$b} <=> $results->{$a} }
        keys %{ $results };
    for (my $i = 0; $i < scalar @document_ids; $i++) {
        printf "%d\t%s\t%.3f\n",
            $i+1, $document_ids[$i], $results->{$document_ids[$i]};
    }
}

## delete a document
my ($document_id, $ret, $message);
$document_id = 'トヨタ自動車';
printf "Delete (id: $document_id): ";
$ret = $client->delete_document($document_id);
$message = $ret ? "succeeded" : "failed";
print "$message\n";
printf "Size: %d\n\n", $client->size();

## add a document
$document_id = 'トヨタ自動車';
my @feature_ids = ('自動車', 'ヴィッツ', 'レクサス', 'パッソ');
printf "Add (id: $document_id): ";
$ret = $client->add_document($document_id, \@feature_ids);
$message = $ret ? "succeeded" : "failed";
print "$message\n";
printf "Size: %d\n\n", $client->size();

## search by document
@query = ('トヨタ自動車');
printf "Search_by_document (max: $max, query: %s):\n",
    join(', ', @query);
$results = $client->search_by_document(\@query, $max);
if ($results) {
    my @document_ids = sort { $results->{$b} <=> $results->{$a} }
        keys %{ $results };
    for (my $i = 0; $i < scalar @document_ids; $i++) {
        printf "%d\t%s\t%.3f\n",
            $i+1, $document_ids[$i], $results->{$document_ids[$i]};
    }
}
print "\n";

## save
my $filename = '/tmp/stupa_evhttpd_tmp';
printf "Save: (file: $filename): ";
$ret = $client->save($filename);
$message = $ret ? "succeeded" : "failed";
print "$message\n";

## load
printf "Load (file: $filename): ";
$ret = $client->load($filename);
$message = $ret ? "succeeded" : "failed";
print "$message\n";
