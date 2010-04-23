package StupaEvhttpClient;

use strict;
use warnings;
use base qw(Class::Accessor::Fast);
use Carp qw(croak);
use URI::Escape;
use WWW::Curl::Easy;

our $VERSION = '0.01';

__PACKAGE__->mk_accessors(qw(host port timeout));

use constant {
    TIMEOUT => 5,
};

sub new {
    my ($self, %argv) = @_;
    map { exists $argv{$_} or croak $_ } qw(host port);
    $self->SUPER::new({
        host       => $argv{host},
        port       => $argv{port},
        timeout    => $argv{timeout} || TIMEOUT,
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

sub add_document {
}

sub delete_document {
}

sub size {
    my $self = shift;
    my $curl = WWW::Curl::Easy->new;
    my $url = $self->_make_request_url('size');
    $curl->setopt(CURLOPT_URL, $url);
    $curl->setopt(CURLOPT_TIMEOUT, $self->timeout);
    $curl->setopt(CURLOPT_HTTPHEADER, ['Connection: Keep-Alive']);

    open my $content, '>', \my $content_data;
    $curl->setopt(CURLOPT_WRITEDATA, $content);
    my $ret = $curl->perform();
    return if $ret != 0;
    my $size = (split /\n/, $content_data)[0];
    return $size;
}

sub search_by_document {
    my ($self, $document_ids, $max) = @_;
    return if !$document_ids;

    my $curl = WWW::Curl::Easy->new;
    my %option;
    $option{query} = join "\t", @{ $document_ids };
    $option{max} = $max if $max > 0;
    my $url = $self->_make_request_url('dsearch', \%option);
    $curl->setopt(CURLOPT_URL, $url);
    $curl->setopt(CURLOPT_TIMEOUT, $self->timeout);
    $curl->setopt(CURLOPT_HTTPHEADER, ['Connection: Keep-Alive']);

    open my $content, '>', \my $content_data;
    $curl->setopt(CURLOPT_WRITEDATA, $content);
    my $ret = $curl->perform();
    return if $ret != 0;

    my %results;
    foreach my $line (split /\n/, $content_data) {
        next if !$line;
        my ($document_id, $point) = split /\t/, $line;
        $results{$document_id} = $point;
    }
    return \%results;
}

sub search_by_feature {
    my ($self, $feature_ids, $max) = @_;
    return if !$feature_ids;

    my $curl = WWW::Curl::Easy->new;
    my %option;
    $option{query} = join "\t", @{ $feature_ids };
    $option{max} = $max if $max > 0;
    my $url = $self->_make_request_url('dsearch', \%option);
    $curl->setopt(CURLOPT_URL, $url);
    $curl->setopt(CURLOPT_TIMEOUT, $self->timeout);
    $curl->setopt(CURLOPT_HTTPHEADER, ['Connection: Keep-Alive']);

    open my $content, '>', \my $content_data;
    $curl->setopt(CURLOPT_WRITEDATA, $content);
    my $ret = $curl->perform();
    return if $ret != 0;

    my %results;
    foreach my $line (split /\n/, $content_data) {
        next if !$line;
        my ($document_id, $point) = split /\t/, $line;
        $results{$document_id} = $point;
    }
    return \%results;
}

sub save {
}

sub load {
}

1;

__END__

=head1 NAME

StupaEvhttpClient -

=head1 SYNOPSIS

  use StupaEvhttpClient;

=head1 DESCRIPTION

StupaEvhttpClient is

=head1 AUTHOR

Mizuki Fujisawa E<lt>fujisawa@bayon.ccE<gt>

=head1 SEE ALSO

Stupa - an Asociative Search Engine <http://code.google.com/p/stupa/>

=head1 LICENSE

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

=cut
