#!/usr/bin/perl

use strict;

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

my $type="opc";

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
	my @standards;
	($globals{path}, @stimuli)= &parsestimuli;
	($globals{defaultstandard}, @standards) = &parsestandards;
	$globals{adaptparam}="label";		# apex 2 selects a stimulus by order

	if ( $#standards > -1) {
		$globals{hasstandard}=1;
	} else {
		$globals{hasstandard}=0;
	}
	
	&showheader;
	&showadaptiveprocedure(\@stimuli, \@standards, \%globals, $method);
	&showequalcorrector;
	&parseshowbuttonscreen;
	&showdatablocks(\%globals, \@stimuli);
	&parseshowdevice;
	print OUTP "<filters/>\n\n";
	&showstimuli(\@stimuli,\@standards,\%globals);
	print OUTP "<connections/>\n\n";
	&showfooter;

}



sub showusage {
print <<"VERDER";
Usage: 
    aid2xml.pl <infile> <outfile>
VERDER

}










