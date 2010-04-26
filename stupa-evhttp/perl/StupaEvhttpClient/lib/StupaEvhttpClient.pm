package StupaEvhttpClient;

use strict;
use warnings;
use base qw(Class::Accessor::Fast);
use Carp qw(croak);
use URI::Escape;
use WWW::Curl::Easy;

our $VERSION = '0.01';

__PACKAGE__->mk_accessors(qw(curl host port timeout));

use constant {
    TIMEOUT => 5,
};

sub new {
    my ($self, %argv) = @_;
    map { exists $argv{$_} or croak $_ } qw(host port);

    my $timeout = $argv{timeout} || TIMEOUT;
    my $curl = WWW::Curl::Easy->new;
    $curl->setopt(CURLOPT_TIMEOUT, $timeout);
    $curl->setopt(CURLOPT_HTTPHEADER, ['Connection: Keep-Alive']);
    $self->SUPER::new({
        host => $argv{host},
        port => $argv{port},
        curl => $curl,
    });
}

sub _make_request_url {
    my ($self, $method, $args) = @_;
    my $url = sprintf "http://%s:%d/%s", $self->host, $self->port, $method;
    if ($args) {
        $url .= '?' . join('&', map {
            sprintf "%s=%s", $_, uri_escape $args->{$_}
        } keys %{ $args });
    }
    return $url;
}

sub _send_request {
    my ($self, $url) = @_;
    return if !$url;
    $self->curl->setopt(CURLOPT_URL, $url);
    open my $content, '>', \my $content_data;
    $self->curl->setopt(CURLOPT_WRITEDATA, $content);
    my $ret = $self->curl->perform();
#    close $content;
    return ($ret, $content_data);
}

sub add_document {
    my ($self, $document_id, $feature_ids) = @_;
    return if !defined $document_id && !$feature_ids;
    my %option = (
        id      => $document_id,
        feature => join "\t", @{ $feature_ids },
    );
    my $url = $self->_make_request_url('add', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return $ret == 0 ? 1 : 0;
}

sub delete_document {
    my ($self, $document_id) = @_;
    return if !defined $document_id;
    my %option = (
        id => $document_id
    );
    my $url = $self->_make_request_url('delete', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return $ret == 0 ? 1 : 0;
}

sub size {
    my $self = shift;
    my $url = $self->_make_request_url('size');
    my ($ret, $response) = $self->_send_request($url);
    return if $ret != 0 || !$response;
    my $size = (split /\n/, $response)[0];
    return $size;
}

sub search_by_document {
    my ($self, $document_ids, $max) = @_;
    return if !$document_ids;
    my %option;
    $option{query} = join "\t", @{ $document_ids };
    $option{max} = $max if $max > 0;
    my $url = $self->_make_request_url('dsearch', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return if $ret != 0 || !$response;
    my %results;
    foreach my $line (split /\n/, $response) {
        next if !$line;
        my ($document_id, $point) = split /\t/, $line;
        $results{$document_id} = $point;
    }
    return \%results;
}

sub search_by_feature {
    my ($self, $feature_ids, $max) = @_;
    return if !$feature_ids;
    my %option;
    $option{query} = join "\t", @{ $feature_ids };
    $option{max} = $max if $max > 0;
    my $url = $self->_make_request_url('dsearch', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return if $ret != 0 || !$response;
    my %results;
    foreach my $line (split /\n/, $response) {
        next if !$line;
        my ($document_id, $point) = split /\t/, $line;
        $results{$document_id} = $point;
    }
    return \%results;
}

sub save {
    my ($self, $filename) = @_;
    return if !$filename;
    my %option = (
        file => $filename,
    );
    my $url = $self->_make_request_url('save', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return $ret == 0 ? 1 : 0;
}

sub load {
    my ($self, $filename) = @_;
    return if !$filename;
    my %option = (
        file => $filename,
    );
    my $url = $self->_make_request_url('load', \%option);
    my ($ret, $response) = $self->_send_request($url);
    return $ret == 0 ? 1 : 0;
}

1;

__END__

=head1 NAME

StupaEvhttpClient - client library of Stupa search engine

=head1 SYNOPSIS

  use StupaEvhttpClient;
  my $client = StupaEvhttpClient->new(
      host    => 'localhost',
      port    => 8080,
      timeout => 5,
  );
  ## get size
  my $size = $client->size();
  
  ## search by document ids
  my $max = 10;
  my @query = ('apple', 'banana');
  my $results = $client->search_by_document(\@query, $max);
  if ($results) {
      my @document_ids = sort { $results->{$b} <=> $results->{$a} }
          keys %{ $results };
      for (my $i = 0; $i < scalar @document_ids; $i++) {
          printf "%d\t%s\t%.3f\n",
              $i+1, $document_ids[$i], $results->{$document_ids[$i]};
      }
  }
  
  ## delete a document
  my $document_id = 'apple';
  my $ret = $client->delete_document($document_id);

=head1 DESCRIPTION

StupaEvhttpClient is a client library of Stupa search engine.

=head1 METHODS

=head2 new(%arg)

Create a new instance.

'host' option is the host name of Stupa server, and must be specified.

'port' option is the port number of Stupa server, and must be specified.

'timeout' option is the timeout seconds of the connection to Stupa server, and default value is 5.

=head2 add_document($document_id, $feature_ids)

Add an input document to Stupa server.

$document_id parameter is the identifier of a document. $feature_ids parameter is the identifiers of the features of a document, and must be a array reference.

=head2 delete_document($document_id)

Delete a documen from Stupa server.

$document_id parameter is the identifier of a document.

=head2 size()

Get the number of stored documents in Stupa server.

=head2 search_by_document($document_ids, $max)

Search documents by a query of document ids.

$document_ids parameter is the identifiers of documents which are used as a search query, and must be array reference. $max parameter is the maximum number of search results.

=head2 search_by_feature($feature_ids, $max)

Search documents by queries of document ids.

$feature_ids parameter is the identifiers of features which are used as a search query, and must be array reference. $max parameter is the maximum number of search results.

=head2 save($filename)

Save current state (documents, indexes) to a file.

$filename parameter is the path of the saved file.

=head2 load($filename)

Load documents and indexes from a file.

$filename parameter is the path of the loaded file.

=head1 AUTHOR

Mizuki Fujisawa E<lt>fujisawa@bayon.ccE<gt>

=head1 SEE ALSO

Stupa - an Asociative Search Engine <http://code.google.com/p/stupa/>

=head1 LICENSE

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

=cut
