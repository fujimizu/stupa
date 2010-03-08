#
# Autogenerated by Thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#
require 5.6.0;
use strict;
use warnings;
use Thrift;

use Types;

# HELPER FUNCTIONS AND STRUCTURES

package StupaThrift_add_document_args;
use base qw(Class::Accessor);
StupaThrift_add_document_args->mk_accessors( qw( document_id features ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{document_id} = undef;
  $self->{features} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{document_id}) {
      $self->{document_id} = $vals->{document_id};
    }
    if (defined $vals->{features}) {
      $self->{features} = $vals->{features};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_add_document_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::STRING) {
        $xfer += $input->readString(\$self->{document_id});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
      /^2$/ && do{      if ($ftype == TType::LIST) {
        {
          my $_size0 = 0;
          $self->{features} = [];
          my $_etype3 = 0;
          $xfer += $input->readListBegin(\$_etype3, \$_size0);
          for (my $_i4 = 0; $_i4 < $_size0; ++$_i4)
          {
            my $elem5 = undef;
            $xfer += $input->readString(\$elem5);
            push(@{$self->{features}},$elem5);
          }
          $xfer += $input->readListEnd();
        }
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_add_document_args');
  if (defined $self->{document_id}) {
    $xfer += $output->writeFieldBegin('document_id', TType::STRING, 1);
    $xfer += $output->writeString($self->{document_id});
    $xfer += $output->writeFieldEnd();
  }
  if (defined $self->{features}) {
    $xfer += $output->writeFieldBegin('features', TType::LIST, 2);
    {
      $output->writeListBegin(TType::STRING, scalar(@{$self->{features}}));
      {
        foreach my $iter6 (@{$self->{features}}) 
        {
          $xfer += $output->writeString($iter6);
        }
      }
      $output->writeListEnd();
    }
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_add_document_result;
use base qw(Class::Accessor);

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_add_document_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_add_document_result');
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_delete_document_args;
use base qw(Class::Accessor);
StupaThrift_delete_document_args->mk_accessors( qw( document_id ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{document_id} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{document_id}) {
      $self->{document_id} = $vals->{document_id};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_delete_document_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::STRING) {
        $xfer += $input->readString(\$self->{document_id});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_delete_document_args');
  if (defined $self->{document_id}) {
    $xfer += $output->writeFieldBegin('document_id', TType::STRING, 1);
    $xfer += $output->writeString($self->{document_id});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_delete_document_result;
use base qw(Class::Accessor);

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_delete_document_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_delete_document_result');
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_size_args;
use base qw(Class::Accessor);

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_size_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_size_args');
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_size_result;
use base qw(Class::Accessor);
StupaThrift_size_result->mk_accessors( qw( success ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{success} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{success}) {
      $self->{success} = $vals->{success};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_size_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^0$/ && do{      if ($ftype == TType::I64) {
        $xfer += $input->readI64(\$self->{success});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_size_result');
  if (defined $self->{success}) {
    $xfer += $output->writeFieldBegin('success', TType::I64, 0);
    $xfer += $output->writeI64($self->{success});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_search_by_document_args;
use base qw(Class::Accessor);
StupaThrift_search_by_document_args->mk_accessors( qw( max query ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{max} = undef;
  $self->{query} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{max}) {
      $self->{max} = $vals->{max};
    }
    if (defined $vals->{query}) {
      $self->{query} = $vals->{query};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_search_by_document_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::I64) {
        $xfer += $input->readI64(\$self->{max});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
      /^2$/ && do{      if ($ftype == TType::LIST) {
        {
          my $_size7 = 0;
          $self->{query} = [];
          my $_etype10 = 0;
          $xfer += $input->readListBegin(\$_etype10, \$_size7);
          for (my $_i11 = 0; $_i11 < $_size7; ++$_i11)
          {
            my $elem12 = undef;
            $xfer += $input->readString(\$elem12);
            push(@{$self->{query}},$elem12);
          }
          $xfer += $input->readListEnd();
        }
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_search_by_document_args');
  if (defined $self->{max}) {
    $xfer += $output->writeFieldBegin('max', TType::I64, 1);
    $xfer += $output->writeI64($self->{max});
    $xfer += $output->writeFieldEnd();
  }
  if (defined $self->{query}) {
    $xfer += $output->writeFieldBegin('query', TType::LIST, 2);
    {
      $output->writeListBegin(TType::STRING, scalar(@{$self->{query}}));
      {
        foreach my $iter13 (@{$self->{query}}) 
        {
          $xfer += $output->writeString($iter13);
        }
      }
      $output->writeListEnd();
    }
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_search_by_document_result;
use base qw(Class::Accessor);
StupaThrift_search_by_document_result->mk_accessors( qw( success ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{success} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{success}) {
      $self->{success} = $vals->{success};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_search_by_document_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^0$/ && do{      if ($ftype == TType::LIST) {
        {
          my $_size14 = 0;
          $self->{success} = [];
          my $_etype17 = 0;
          $xfer += $input->readListBegin(\$_etype17, \$_size14);
          for (my $_i18 = 0; $_i18 < $_size14; ++$_i18)
          {
            my $elem19 = undef;
            $elem19 = new SearchResult();
            $xfer += $elem19->read($input);
            push(@{$self->{success}},$elem19);
          }
          $xfer += $input->readListEnd();
        }
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_search_by_document_result');
  if (defined $self->{success}) {
    $xfer += $output->writeFieldBegin('success', TType::LIST, 0);
    {
      $output->writeListBegin(TType::STRUCT, scalar(@{$self->{success}}));
      {
        foreach my $iter20 (@{$self->{success}}) 
        {
          $xfer += ${iter20}->write($output);
        }
      }
      $output->writeListEnd();
    }
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_search_by_feature_args;
use base qw(Class::Accessor);
StupaThrift_search_by_feature_args->mk_accessors( qw( max query ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{max} = undef;
  $self->{query} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{max}) {
      $self->{max} = $vals->{max};
    }
    if (defined $vals->{query}) {
      $self->{query} = $vals->{query};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_search_by_feature_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::I64) {
        $xfer += $input->readI64(\$self->{max});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
      /^2$/ && do{      if ($ftype == TType::LIST) {
        {
          my $_size21 = 0;
          $self->{query} = [];
          my $_etype24 = 0;
          $xfer += $input->readListBegin(\$_etype24, \$_size21);
          for (my $_i25 = 0; $_i25 < $_size21; ++$_i25)
          {
            my $elem26 = undef;
            $xfer += $input->readString(\$elem26);
            push(@{$self->{query}},$elem26);
          }
          $xfer += $input->readListEnd();
        }
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_search_by_feature_args');
  if (defined $self->{max}) {
    $xfer += $output->writeFieldBegin('max', TType::I64, 1);
    $xfer += $output->writeI64($self->{max});
    $xfer += $output->writeFieldEnd();
  }
  if (defined $self->{query}) {
    $xfer += $output->writeFieldBegin('query', TType::LIST, 2);
    {
      $output->writeListBegin(TType::STRING, scalar(@{$self->{query}}));
      {
        foreach my $iter27 (@{$self->{query}}) 
        {
          $xfer += $output->writeString($iter27);
        }
      }
      $output->writeListEnd();
    }
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_search_by_feature_result;
use base qw(Class::Accessor);
StupaThrift_search_by_feature_result->mk_accessors( qw( success ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{success} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{success}) {
      $self->{success} = $vals->{success};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_search_by_feature_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^0$/ && do{      if ($ftype == TType::LIST) {
        {
          my $_size28 = 0;
          $self->{success} = [];
          my $_etype31 = 0;
          $xfer += $input->readListBegin(\$_etype31, \$_size28);
          for (my $_i32 = 0; $_i32 < $_size28; ++$_i32)
          {
            my $elem33 = undef;
            $elem33 = new SearchResult();
            $xfer += $elem33->read($input);
            push(@{$self->{success}},$elem33);
          }
          $xfer += $input->readListEnd();
        }
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_search_by_feature_result');
  if (defined $self->{success}) {
    $xfer += $output->writeFieldBegin('success', TType::LIST, 0);
    {
      $output->writeListBegin(TType::STRUCT, scalar(@{$self->{success}}));
      {
        foreach my $iter34 (@{$self->{success}}) 
        {
          $xfer += ${iter34}->write($output);
        }
      }
      $output->writeListEnd();
    }
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_save_args;
use base qw(Class::Accessor);
StupaThrift_save_args->mk_accessors( qw( filename ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{filename} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{filename}) {
      $self->{filename} = $vals->{filename};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_save_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::STRING) {
        $xfer += $input->readString(\$self->{filename});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_save_args');
  if (defined $self->{filename}) {
    $xfer += $output->writeFieldBegin('filename', TType::STRING, 1);
    $xfer += $output->writeString($self->{filename});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_save_result;
use base qw(Class::Accessor);
StupaThrift_save_result->mk_accessors( qw( success ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{success} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{success}) {
      $self->{success} = $vals->{success};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_save_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^0$/ && do{      if ($ftype == TType::BOOL) {
        $xfer += $input->readBool(\$self->{success});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_save_result');
  if (defined $self->{success}) {
    $xfer += $output->writeFieldBegin('success', TType::BOOL, 0);
    $xfer += $output->writeBool($self->{success});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_load_args;
use base qw(Class::Accessor);
StupaThrift_load_args->mk_accessors( qw( filename ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{filename} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{filename}) {
      $self->{filename} = $vals->{filename};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_load_args';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^1$/ && do{      if ($ftype == TType::STRING) {
        $xfer += $input->readString(\$self->{filename});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_load_args');
  if (defined $self->{filename}) {
    $xfer += $output->writeFieldBegin('filename', TType::STRING, 1);
    $xfer += $output->writeString($self->{filename});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThrift_load_result;
use base qw(Class::Accessor);
StupaThrift_load_result->mk_accessors( qw( success ) );

sub new {
  my $classname = shift;
  my $self      = {};
  my $vals      = shift || {};
  $self->{success} = undef;
  if (UNIVERSAL::isa($vals,'HASH')) {
    if (defined $vals->{success}) {
      $self->{success} = $vals->{success};
    }
  }
  return bless ($self, $classname);
}

sub getName {
  return 'StupaThrift_load_result';
}

sub read {
  my ($self, $input) = @_;
  my $xfer  = 0;
  my $fname;
  my $ftype = 0;
  my $fid   = 0;
  $xfer += $input->readStructBegin(\$fname);
  while (1) 
  {
    $xfer += $input->readFieldBegin(\$fname, \$ftype, \$fid);
    if ($ftype == TType::STOP) {
      last;
    }
    SWITCH: for($fid)
    {
      /^0$/ && do{      if ($ftype == TType::BOOL) {
        $xfer += $input->readBool(\$self->{success});
      } else {
        $xfer += $input->skip($ftype);
      }
      last; };
        $xfer += $input->skip($ftype);
    }
    $xfer += $input->readFieldEnd();
  }
  $xfer += $input->readStructEnd();
  return $xfer;
}

sub write {
  my ($self, $output) = @_;
  my $xfer   = 0;
  $xfer += $output->writeStructBegin('StupaThrift_load_result');
  if (defined $self->{success}) {
    $xfer += $output->writeFieldBegin('success', TType::BOOL, 0);
    $xfer += $output->writeBool($self->{success});
    $xfer += $output->writeFieldEnd();
  }
  $xfer += $output->writeFieldStop();
  $xfer += $output->writeStructEnd();
  return $xfer;
}

package StupaThriftIf;

use strict;


sub add_document{
  my $self = shift;
  my $document_id = shift;
  my $features = shift;

  die 'implement interface';
}

sub delete_document{
  my $self = shift;
  my $document_id = shift;

  die 'implement interface';
}

sub size{
  my $self = shift;

  die 'implement interface';
}

sub search_by_document{
  my $self = shift;
  my $max = shift;
  my $query = shift;

  die 'implement interface';
}

sub search_by_feature{
  my $self = shift;
  my $max = shift;
  my $query = shift;

  die 'implement interface';
}

sub save{
  my $self = shift;
  my $filename = shift;

  die 'implement interface';
}

sub load{
  my $self = shift;
  my $filename = shift;

  die 'implement interface';
}

package StupaThriftRest;

use strict;


sub new {
  my ($classname, $impl) = @_;
  my $self     ={ impl => $impl };

  return bless($self,$classname);
}

sub add_document{
  my ($self, $request) = @_;

  my $document_id = ($request->{'document_id'}) ? $request->{'document_id'} : undef;
  my $features = ($request->{'features'}) ? $request->{'features'} : undef;
  return $self->{impl}->add_document($document_id, $features);
}

sub delete_document{
  my ($self, $request) = @_;

  my $document_id = ($request->{'document_id'}) ? $request->{'document_id'} : undef;
  return $self->{impl}->delete_document($document_id);
}

sub size{
  my ($self, $request) = @_;

  return $self->{impl}->size();
}

sub search_by_document{
  my ($self, $request) = @_;

  my $max = ($request->{'max'}) ? $request->{'max'} : undef;
  my $query = ($request->{'query'}) ? $request->{'query'} : undef;
  return $self->{impl}->search_by_document($max, $query);
}

sub search_by_feature{
  my ($self, $request) = @_;

  my $max = ($request->{'max'}) ? $request->{'max'} : undef;
  my $query = ($request->{'query'}) ? $request->{'query'} : undef;
  return $self->{impl}->search_by_feature($max, $query);
}

sub save{
  my ($self, $request) = @_;

  my $filename = ($request->{'filename'}) ? $request->{'filename'} : undef;
  return $self->{impl}->save($filename);
}

sub load{
  my ($self, $request) = @_;

  my $filename = ($request->{'filename'}) ? $request->{'filename'} : undef;
  return $self->{impl}->load($filename);
}

package StupaThriftClient;


use base qw(StupaThriftIf);
sub new {
  my ($classname, $input, $output) = @_;
  my $self      = {};
  $self->{input}  = $input;
  $self->{output} = defined $output ? $output : $input;
  $self->{seqid}  = 0;
  return bless($self,$classname);
}

sub add_document{
  my $self = shift;
  my $document_id = shift;
  my $features = shift;

    $self->send_add_document($document_id, $features);
  $self->recv_add_document();
}

sub send_add_document{
  my $self = shift;
  my $document_id = shift;
  my $features = shift;

  $self->{output}->writeMessageBegin('add_document', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_add_document_args();
  $args->{document_id} = $document_id;
  $args->{features} = $features;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_add_document{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_add_document_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  return;
}
sub delete_document{
  my $self = shift;
  my $document_id = shift;

    $self->send_delete_document($document_id);
  $self->recv_delete_document();
}

sub send_delete_document{
  my $self = shift;
  my $document_id = shift;

  $self->{output}->writeMessageBegin('delete_document', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_delete_document_args();
  $args->{document_id} = $document_id;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_delete_document{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_delete_document_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  return;
}
sub size{
  my $self = shift;

    $self->send_size();
  return $self->recv_size();
}

sub send_size{
  my $self = shift;

  $self->{output}->writeMessageBegin('size', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_size_args();
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_size{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_size_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  if (defined $result->{success} ) {
    return $result->{success};
  }
  die "size failed: unknown result";
}
sub search_by_document{
  my $self = shift;
  my $max = shift;
  my $query = shift;

    $self->send_search_by_document($max, $query);
  return $self->recv_search_by_document();
}

sub send_search_by_document{
  my $self = shift;
  my $max = shift;
  my $query = shift;

  $self->{output}->writeMessageBegin('search_by_document', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_search_by_document_args();
  $args->{max} = $max;
  $args->{query} = $query;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_search_by_document{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_search_by_document_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  if (defined $result->{success} ) {
    return $result->{success};
  }
  die "search_by_document failed: unknown result";
}
sub search_by_feature{
  my $self = shift;
  my $max = shift;
  my $query = shift;

    $self->send_search_by_feature($max, $query);
  return $self->recv_search_by_feature();
}

sub send_search_by_feature{
  my $self = shift;
  my $max = shift;
  my $query = shift;

  $self->{output}->writeMessageBegin('search_by_feature', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_search_by_feature_args();
  $args->{max} = $max;
  $args->{query} = $query;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_search_by_feature{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_search_by_feature_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  if (defined $result->{success} ) {
    return $result->{success};
  }
  die "search_by_feature failed: unknown result";
}
sub save{
  my $self = shift;
  my $filename = shift;

    $self->send_save($filename);
  return $self->recv_save();
}

sub send_save{
  my $self = shift;
  my $filename = shift;

  $self->{output}->writeMessageBegin('save', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_save_args();
  $args->{filename} = $filename;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_save{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_save_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  if (defined $result->{success} ) {
    return $result->{success};
  }
  die "save failed: unknown result";
}
sub load{
  my $self = shift;
  my $filename = shift;

    $self->send_load($filename);
  return $self->recv_load();
}

sub send_load{
  my $self = shift;
  my $filename = shift;

  $self->{output}->writeMessageBegin('load', TMessageType::CALL, $self->{seqid});
  my $args = new StupaThrift_load_args();
  $args->{filename} = $filename;
  $args->write($self->{output});
  $self->{output}->writeMessageEnd();
  $self->{output}->getTransport()->flush();
}

sub recv_load{
  my $self = shift;

  my $rseqid = 0;
  my $fname;
  my $mtype = 0;

  $self->{input}->readMessageBegin(\$fname, \$mtype, \$rseqid);
  if ($mtype == TMessageType::EXCEPTION) {
    my $x = new TApplicationException();
    $x->read($self->{input});
    $self->{input}->readMessageEnd();
    die $x;
  }
  my $result = new StupaThrift_load_result();
  $result->read($self->{input});
  $self->{input}->readMessageEnd();

  if (defined $result->{success} ) {
    return $result->{success};
  }
  die "load failed: unknown result";
}
package StupaThriftProcessor;

use strict;


sub new {
    my ($classname, $handler) = @_;
    my $self      = {};
    $self->{handler} = $handler;
    return bless ($self, $classname);
}

sub process {
    my ($self, $input, $output) = @_;
    my $rseqid = 0;
    my $fname  = undef;
    my $mtype  = 0;

    $input->readMessageBegin(\$fname, \$mtype, \$rseqid);
    my $methodname = 'process_'.$fname;
    if (!$self->can($methodname)) {
      $input->skip(TType::STRUCT);
      $input->readMessageEnd();
      my $x = new TApplicationException('Function '.$fname.' not implemented.', TApplicationException::UNKNOWN_METHOD);
      $output->writeMessageBegin($fname, TMessageType::EXCEPTION, $rseqid);
      $x->write($output);
      $output->writeMessageEnd();
      $output->getTransport()->flush();
      return;
    }
    $self->$methodname($rseqid, $input, $output);
    return 1;
}

sub process_add_document {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_add_document_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_add_document_result();
    $self->{handler}->add_document($args->document_id, $args->features);
    $output->writeMessageBegin('add_document', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_delete_document {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_delete_document_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_delete_document_result();
    $self->{handler}->delete_document($args->document_id);
    $output->writeMessageBegin('delete_document', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_size {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_size_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_size_result();
    $result->{success} = $self->{handler}->size();
    $output->writeMessageBegin('size', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_search_by_document {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_search_by_document_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_search_by_document_result();
    $result->{success} = $self->{handler}->search_by_document($args->max, $args->query);
    $output->writeMessageBegin('search_by_document', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_search_by_feature {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_search_by_feature_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_search_by_feature_result();
    $result->{success} = $self->{handler}->search_by_feature($args->max, $args->query);
    $output->writeMessageBegin('search_by_feature', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_save {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_save_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_save_result();
    $result->{success} = $self->{handler}->save($args->filename);
    $output->writeMessageBegin('save', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

sub process_load {
    my ($self, $seqid, $input, $output) = @_;
    my $args = new StupaThrift_load_args();
    $args->read($input);
    $input->readMessageEnd();
    my $result = new StupaThrift_load_result();
    $result->{success} = $self->{handler}->load($args->filename);
    $output->writeMessageBegin('load', TMessageType::REPLY, $seqid);
    $result->write($output);
    $output->writeMessageEnd();
    $output->getTransport()->flush();
}

1;