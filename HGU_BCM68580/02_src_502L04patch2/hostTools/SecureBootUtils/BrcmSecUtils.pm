#!/usr/bin/env perl
package BrcmSecUtils;
use strict;
use Exporter;
use File::stat;
#use warning;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);
$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = ();
@EXPORT_OK = qw(run_shell compress_lzma f2hex fdump fsplit set_val_at sign_sha256 encrypt_aes_128_cbc fappend);
%EXPORT_TAGS = (DEFALUT=> [qw(&run_shell) ],
		both=> [qw(&run_shell &compress_lzma &f2hex &fdump &fsplit &set_val_at &sign_sha256 &encrypt_aes_128_cbc &fappend)]);



my $verbose = 1;
#Dumps file to hexamdecimal chars
sub f2hex {
	my $fname = shift;
	my $fin = undef;
	my $data;
	my $var;
	open ($fin,"<","$fname") or die "$0: Failed to open $fname $!";
	binmode $fin;
	local $/ = undef;
	$data = <$fin>;
	$var = unpack('H*',$data);
	close($fin) or die "$0: Failed to open $fname $!";
	return $var;
}

sub fappend {
        my $in = shift;
        my $data = shift;
        open (my $fhndl,">","$in") or die "$0: Failed to open $in $!";
        #binmode $fhndl;
        #printf "Length of the data:0x%x\n", length $data;
        print $fhndl $data;
        close($fhndl) or die "$0: Failed to close $!";
}

sub fdump {
        my $in = shift;
        my $data = shift;
        open (my $fhndl,"+>","$in") or die "$0: Failed to create $in $!";
        binmode $fhndl;
        #printf "Length of the data:0x%x\n", length $data;
        print $fhndl $data;
        close($fhndl) or die "$0: Failed to close $!";
}

sub fsplit {
        my ($split_size, $in, $split_struct) = @_;
        open (my $fin,"<","$in") or die "$0: Failed to open $in $!";
        binmode $fin;
        local $/ = undef;
        my $data = <$fin>;
        close($fin) or die "$0: Failed to close $!";
	$split_struct->{"left"} = substr $data, 0, $split_size;	
	$split_struct->{"right"} = substr $data, $split_size, length $data ;
}

# set value in a byte array

sub set_val_at {
	my  ($bytes, $offs, $val, $endian) = @_;
	$endian = $endian eq 'little'?'V':'N';
	if ($verbose > 1) {
		printf "$0 old data %s | new size 0x%x \n",unpack('H*' ,$$bytes),$val;
	}
	substr($$bytes, $offs) = pack($endian, $val);
}

sub encrypt_aes_128_cbc {
	my $key_file = shift;
	my $iv_file = shift;
	my $if = shift;
	my $of = shift;
	my $bek = f2hex("$key_file");
	my $biv = f2hex("$iv_file");
	run_shell("openssl enc -aes-128-cbc -K $bek -iv $biv -in $if -out $of");
}

sub sign_sha256 {
	my $pem = shift;
	my $if = shift;
	my $of = shift;
	#print "Signing $if with $pem res $of\n";
	run_shell("openssl dgst -sign $pem -keyform pem -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-1 -out $of $if");
}

sub compress_lzma {
	my ($if, $of, $tools) = @_;
	run_shell("$tools/lzmacmd e $if $of -d22 -lp2 -lc1");
	my $fi = stat($of);
	return $fi->size; 
}

sub run_shell {
	my $cmd = shift;
	if ($verbose > 1) {
		print "$0: $cmd\n";
	}
	system($cmd) == 0 or die "ERROR: $cmd had failed"; 
}
