#!/usr/bin/perl

use strict;
use Bio::SeqIO;

if ($#ARGV < 0) {
  print "Use:  $0 seq_fasta [-s] (-s for the total sum)\n";
  exit(0);
}

my $sum=0;
my $in = Bio::SeqIO->new('-file' => "$ARGV[0]") ;
if ($ARGV[1] =~ /-s/){
  while ( my $seqobj = $in->next_seq() ){
    $sum += $seqobj->length();
  }
  print "$sum\n";
}
else{
  while ( my $seqobj = $in->next_seq() ){
    print $seqobj->id()." ".$seqobj->length()."\n";
    $sum += $seqobj->length();
  }
}
