#!/usr/bin/perl

use strict;
my $basedir=$0;				# add path of this script to searchpath for apexapi.pl
$basedir=~s/\\/\//g;
$basedir=~ qw{^(.*)[/][^/]*$};
unshift (@INC, $1);

require 'apexapi.pl';

# convert apex2 adp file to apex3.0 xml file
# usage:
#	adp2xml.pl <input file> <output file>

if ($#ARGV != 1) {
    &showusage;
    exit(1);
}

my $input = $ARGV[0];
my $output = $ARGV[1];

my $type="opc";

if ( open(INP, $input) == 0) {
    print "Error: can't open input file '$input'";
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
	$globals{adaptparam}="param";		# apex 2 selects a stimulus by order

	if ( $#standards > -1) {
		$globals{hasstandard}=1;
	} else {
		$globals{hasstandard}=0;
	}
	
	&showheader;
	&showadaptiveprocedure(\@stimuli, \@standards, \%globals, $method);
	&showalternativescorrector($method);
	&parseshowscreen($method);
	&showdatablocks(\%globals, \@stimuli,\@standards);
	&parseshowdevice;
	print OUTP "<filters/>\n\n";
	&showstimuli(\@stimuli,\@standards,\%globals);
	print OUTP "<connections/>\n\n";
	&showfooter;

}



sub showusage {
print <<"VERDER";
Usage: 
    adp2xml.pl <infile> <outfile>
VERDER

}






sub parseshowscreen {
	my ($method) = @_;

# get help text
my $helptext;
if (seekheader('[help]')) {
	my $line = <INP>;
	chomp ($line);
	$helptext= $line =~ /^task=(.*)$/;
} 
# else {
# 	$helptext= ""Type what you hear";
# }

if (!defined($method->{intervals})) {
	die("Can't find number of intervals in parseshowscreen");
}

my $intervals = $method->{intervals};



print OUTP<<"VERDER";
 <screens>
VERDER
  &showreinforcement;
  print OUTP<<"VERDER";
    <screen id="screen1" >
VERDER
if ($helptext ne '') {	
print OUTP<<"VERDER";
      <gridLayout height="2" width="1" id="main_layout">
	  	<label x="1" y="1" id="helptext">
			<text>$helptext</text>
		</label>
VERDER
}

print OUTP<<"VERDER";
		<gridLayout height="1" width="$intervals" id="button_layout" x="1" y="2">
VERDER


	
for (my $i=1; $i<=$intervals; ++$i) {
	
	print OUTP<<"VERDER";
		<button x="$i" y="1" id="button$i">
			<text>$i</text>
		</button>
VERDER

}
	

print OUTP<<"VERDER";
	</gridLayout>
VERDER

if ($helptext ne '') {	
print OUTP<<"VERDER";
      </gridLayout>
VERDER
}	  

print OUTP "		<buttongroup id=\"buttongroup\">\n";
for (my $i=1; $i<=$intervals; ++$i) {
        print OUTP "			<button id=\"button$i\"/>\n";
}
print OUTP "		</buttongroup>\n";

print OUTP<<"VERDER";
		<default_answer_element>buttongroup</default_answer_element>
    </screen>
  </screens>
VERDER

}





