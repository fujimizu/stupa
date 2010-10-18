#!/usr/bin/perl

package StupaClientLwp;

use strict;
use warnings;
use base qw(LWP::UserAgent);
use URI::Escape;

sub new {
    my ($class, %argv) = @_;
    my $self = $class->SUPER::new(
        keep_alive => ['localhost'],
        timeout    => 5,
    );
    return $self;
}

sub _make_request_url {
    my ($self, $method, $args) = @_;
    my $url = sprintf "http://%s:%d/%s", 'localhost', 8080, $method;
    if ($args) {
        $url .= '?' . join('&', map {
            sprintf "%s=%s", $_, uri_escape $args->{$_}
        } keys %{ $args });
    }
    return $url;
}

sub _send_request {
    my ($self, $url) = @_;
    my $res = $self->post($url, {});
    return $res->content if $res->is_success();
}

sub search_by_document {
    my ($self, $document_ids, $max) = @_;
    return if !$document_ids;
    my %option;
    $option{query} = join "\t", @{ $document_ids };
    $option{max} = $max if $max > 0;
    my $url = $self->_make_request_url('dsearch', \%option);
    my $response = $self->_send_request($url);
    return if !$response;
    my %results;
    foreach my $line (split /\n/, $response) {
        next if !$line;
        my ($document_id, $point) = split /\t/, $line;
        $results{$document_id} = $point;
    }
    return \%results;
}

package main;

my $max = 10;
my $client = StupaClientLwp->new;
my @query = ('トヨタ自動車', '日産');
for (my $i = 0; $i < 10000; $i++) {
    print "$i\n" if $i % 1000 == 0;
    my $results = $client->search_by_document(\@query, $max);
}
