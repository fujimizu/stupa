#!/usr/bin/perl
#
# Client sample in perl
#

use strict;
use warnings;
use FindBin;
use lib "$FindBin::Bin/lib";

use Thrift;
use Thrift::BinaryProtocol;
use Thrift::Socket;
use Thrift::BufferedTransport;
use Thrift::FramedTransport;

use StupaThrift;

my $socket = Thrift::Socket->new('localhost', 9090);
my $transport = $socket;
if (@ARGV) {
    my $option = shift @ARGV;
    if ($option eq '--buffered') {
        $transport = Thrift::BufferedTransport->new($socket, 1024, 1024);
    }
    elsif ($option eq '--framed') {
        # for NonblockingServer
        $transport = Thrift::FramedTransport->new($socket, 1024, 1024);
    }
    else {
        warn "Usage: $0 [--bufferd | --framed]\n";
        exit 1;
    }
}
my $client = StupaThriftClient->new(Thrift::BinaryProtocol->new($transport));

eval {
    $transport->open;
};
if ($@) {
    print "[ERROR] ", $@->{message}, "\n";
    exit 1;
}

# add documents
while (my $line = <DATA>) {
    chomp $line;
    my @data = split /\s/, $line;
    my $document_id = shift @data;
    print "Add: $document_id\t", join(', ', @data), "\n";
    eval {
        $client->add_document($document_id, \@data);
    };
    if ($@) {
        print "[ERROR] ", $@->{message}, "\n";
        exit 1;
    }
}

# get size
eval {
    my $size = $client->size();
    print "Size: $size\n";
};
if ($@) {
    print "[ERROR] ", $@->{message}, "\n";
    exit 1;
}

# search
my @queries = ('Alex');
my $max = 20;
eval {
    my $results = $client->search($max, \@queries);
    print "Search Result:\n";
    for (my $i = 0; $i < scalar @{ $results }; $i++) {
        printf " %d\t%s\t%f\n", $i+1, $results->[$i]->name, $results->[$i]->point;
    }
};
if ($@) {
    print "[ERROR] ", $@->{message}, "\n";
    exit 1;
}

# save, load
my $filename = '/tmp/stupa_saved.tmp';
eval {
    $client->save($filename);

    $client->load($filename);
};
if ($@) {
    print "[ERROR] ", $@->{message}, "\n";
    exit 1;
}

$transport->close;

__DATA__
Alex Pop R&B Rock
Bob Jazz Reggae
Dave Classic World
Ted Jazz Metal Reggae
Fred Pop Rock Hip-hop
Sam Classic Rock
