use strict;
use warnings;
use StupaEvhttpClient;
use Test::More tests => 9;

my $host = 'localhost';
my $port = 8080;
my $client = StupaEvhttpClient->new(
    host => $host,
    port => $port,
);
can_ok($client, 'new');
can_ok($client, 'add_document');
can_ok($client, 'delete_document');
can_ok($client, 'size');
can_ok($client, 'clear');
can_ok($client, 'search_by_document');
can_ok($client, 'search_by_feature');
can_ok($client, 'save');
can_ok($client, 'load');
