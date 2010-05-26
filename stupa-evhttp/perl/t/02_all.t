use strict;
use warnings;
use Stupa::Evhttp::Client;
use Test::More qw(no_plan);
#use Test::More tests => 10;

use constant {
    HOST         => 'localhost',
    PORT         => 22122,
    NUM_DOCUMENT => 20,
    NUM_FEATURE  => 10,
    LEN_FEATURE  => 5,
    MAX_RESULT   => 10,
};

sub random_features {
    my @characters = split //, 'abcdefghijklmnopqrstuvwxyz1234567890';
    my @features;
    for (my $i = 0; $i < NUM_FEATURE; $i++) {
        my $feature = '';
        for (my $j = 0; $j < LEN_FEATURE; $j++) {
            my $index = int(rand(scalar @characters));
            $feature .= $characters[$index];
        }
        push @features, $feature;
    }
    return \@features;
}

my $client = Stupa::Evhttp::Client->new(
    host => HOST,
    port => PORT,
);
$client->clear();

is($client->size(), 0);

# add documents
my $feature_ids;
for (my $i = 0; $i < NUM_DOCUMENT; $i++) {
    my $document_id = $i;
    $feature_ids = random_features();
    $client->add_document($document_id, $feature_ids);
}
is($client->size(), NUM_DOCUMENT);

# search by document ids
for (my $i = 0; $i < NUM_DOCUMENT; $i++) {
    my $document_id = $i;
    my $results = $client->search_by_document([$document_id], MAX_RESULT);
    ok($results);
    map { ok(0 <= $_ && $_ < NUM_DOCUMENT) } keys %{ $results };
}

# search by feature ids
my $results = $client->search_by_feature($feature_ids, MAX_RESULT);
ok($results);
map { ok(0 <= $_ && $_ < NUM_DOCUMENT) } keys %{ $results };

# delete documents
my $document_id = 0;
$client->delete_document($document_id);
$results = $client->search_by_document($document_id, MAX_RESULT);
is($client->size(), NUM_DOCUMENT-1);
ok(!$results);

# save
my $filename = '/tmp/stupa_perl_test';
$client->save($filename);

my $prev_size = $client->size();

# clear
$client->clear();
is($client->size(), 0);

# load
$client->load($filename);
is($client->size(), $prev_size);
