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

exit;


sub parseshow {
	
	my %globals;			# path

# data structures
	my %sentences = &parsesentences;;			# key=key in apex file, value=sentence
	&parsemethod;
	my @stimuli;
	($globals{path}, @stimuli)= &parsestimuli;
	
	
	&showheader;
	&showprocedure(\@stimuli,\%sentences);
	&showcorrector;
	&parseshowscreen;
	&showdatablocks(\%globals, \@stimuli);
	&parseshowdevice;
	print OUTP "<filters/>\n\n";
	&showstimuli(@stimuli);
	print OUTP "<connections/>\n\n";
	&showfooter;

}




sub showusage {
print <<"VERDER";
Usage: 
    opc2xml.pl <infile> <outfile>
VERDER

}






sub parsestimuli {

}




sub parsesentences {
	my %sentences;

	&seekheader('[sentences]') or die("Can't find [sentences] header");
	
	my ($line, $key, $value);
	
	while ( $line = <INP>) {
		chomp ($line);
		($key,$value)= $line=~ /^sentence=([^,]+),(.*)$/;
		last if ($key eq '');
		$sentences{$key}=$value;
	}
	
	return %sentences;

}

sub showprocedure {
	my ($stimuli,$sentences) = @_;

print OUTP<<"VERDER";
  <procedure xsi:type="apex:constantProcedureType">
VERDER
&showreinforcement;
print OUTP<<"VERDER";
    <parameters>
      <presentations>2</presentations>
      <skip>0</skip>
      <order>sequential</order>
     </parameters>
VERDER

	&showtrials($stimuli,$sentences);	 
	
	print OUTP<<"VERDER";
   </procedure>	
   
   
VERDER
	
}


sub showtrials {
	my ($stimuli,$sentences) = @_;

	print OUTP "<trials>\n";
	
	my $stimulus;
	my $count;
	
	for ($count=0; $count< scalar @$stimuli; ++$count) {
		print OUTP<<"VERDER";
			<trial id="trial$count">
				<answer>$sentences->{$$stimuli[$count]->{sentence}}</answer>
				<screen id="screen1" />
        		<stimulus id="stimulus$count" />
			</trial>
VERDER
		
	}
	
	print OUTP "</trials>\n\n";
}


sub parseshowscreen {

# get help text
my $helptext;
if (seekheader('[help]')) {
	my $line = <INP>;
	chomp ($line);
	$helptext= $line =~ /^task=(.*)$/;
} else {
	$helptext= "Type what you hear";
}


print OUTP<<"VERDER";
 <screens>
    <screen id="screen1" >
      <gridLayout height="2" width="1" id="main_layout">
	  	<label x="1" y="1" id="helptext">
			<text>$helptext</text>
		</label>
        <textEdit x="1" y="2" id="text" />
      </gridLayout>
	  <default_answer_element>text</default_answer_element>
    </screen>
  </screens>
  
  
VERDER

}



sub showcorrector {
	print OUTP <<"VERDER";
	<corrector>
    	<type>isequal</type>
  	</corrector>
	
	
VERDER
	
}

