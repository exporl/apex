#!/usr/bin/perl

use strict;
my $basedir=$0;				# add path of this script to searchpath for apexapi.pl
$basedir=~s/\\/\//g;
$basedir=~ qw{^(.*)[/][^/]*$};
unshift (@INC, $1);

require 'apexapi.pl';

# convert apex2 opc file to apex3.0 xml file
# usage:
#	opc2xml.pl <input file> <output file>

if ($#ARGV != 1) {
    &showusage;
    exit(1);
}

my $input = $ARGV[0];
my $output = $ARGV[1];

my $type="idn";

if ( open(INP, $input) == 0) {
    print "Error: can't open input file $input";
    exit(2);
}

if (open(OUTP, "> $output")==0) {
    print "Error: can't open output file $output";
    exit(3);
}

&parseshow;

&cleanup($output);

print "Converted file successfully\n";

exit;


sub parseshow {

	my %globals;			# path

# data structures
	my $method = &parsemethod;
	my @stimuli;
	($globals{path}, @stimuli)= &parsestimuli;

	&showheader;
	&showconstantprocedure(\@stimuli,$method);
	&showequalcorrector;
	&parseshowbuttonscreen;
	&showdatablocks(\%globals, \@stimuli);
	&parseshowdevice;
	print OUTP "<filters/>\n\n";
	my @standards;		# dummy
	&showstimuli(\@stimuli,\@standards,\%globals);
	print OUTP "<connections/>\n\n";
	&showresults("idn.xsl");
	&showfooter;

}



sub showusage {
print <<"VERDER";
Usage:
    idn2xml.pl <infile> <outfile>
VERDER

}
