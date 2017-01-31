#!/usr/bin/perl

use strict;
my $basedir=$0;				# add path of this script to searchpath for apexapi.pl
$basedir=~s/\\/\//g;
$basedir=~ qw{^(.*)[/][^/]*$};
unshift (@INC, $1);

require('confusionmatrix.pl');

# automatically analyse apex 3 results file

my $xsltproc = "xalan";
my $apexprefix = "/home/tom/data/apex/apex/";
my $xsltprefix = $apexprefix . "/scripts/xslt/";

if ($#ARGV != 0) {
    &showusage;
    exit(1);
}

my $ifile = $ARGV[0];

# get xslt script filename
open(F,$ifile) or die ("Can't open input file: $ifile");
my $found=0;
my $xsltscript;
while ($_=<F>) {
	chomp;
	if (/<xsltscript>\s*([^<]+)<\/xsltscript>/) {
		$found=1;
		$xsltscript=$1;
		last;
	}
}

unless ($found) {
	die "Can't find xslt script to use\n";
} else {
#	print "Using script $xsltscript";
}
close(F);

my $xsltpath = $xsltprefix . $xsltscript;
unless (-e $xsltpath ) {
	die("Can't open script: $xsltpath");
}

open (R, "$xsltproc -text -nh -xsl file:\"$xsltpath\" -in file:\"$ifile\"|");
my $stimconfmat;
my $buttonconfmat;
my @stimulus;
my @respons;
my $stimgo=0;
my $buttongo=0;
while (my $l=<R>) {
	print $l;
	chomp ($l);
	if ($l =~ /Stimulus\/answerstimulus pairs/i ) {
		$stimgo=1;
	} elsif ($stimgo==1) {
		@stimulus = split(/\t/, $l);
		++$stimgo;
	} elsif ($stimgo==2) {
		@respons = split(/\t/, $l);
		++$stimgo;
	} elsif ($stimgo==3) {
		if ($#stimulus == $#respons) {
			$stimconfmat = confusionmatrix(\@stimulus,\@respons);
		}
		++$stimgo;
	}

	if ($l =~ /Stimulus\/correct answer/i ) {
		$buttongo=1;
	} elsif ($buttongo==1) {
		++$buttongo;		# skip line
	} elsif ($buttongo==2) {
		@stimulus = split(/\t/, $l);
		++$buttongo;
	} elsif ($buttongo==3) {
		@respons = split(/\t/, $l);
		++$buttongo;
	} elsif ($buttongo==4) {
		if ($#stimulus == $#respons) {
			$buttonconfmat = confusionmatrix(\@stimulus,\@respons);
		}
		++$buttongo;
	}

}
close (R);


print "\n";

if ($stimconfmat) {
	print "\n[Calculated stimulus confusion matrix] \n";
	print $stimconfmat;
	print "\n";
}

if ($buttonconfmat) {
	print "\n[Calculated button confusion matrix] \n";
	print $buttonconfmat;
	print "\n";
}


#showconfusionmatrix(\@result);

sub showusage {
	print "Usage:\n\na3a.pl <inputfile.xml> [>outputfile]\n";
}


sub showconfusionmatrix {
	my $result = @_;
	foreach my $line (@$result) {
		#if ($line =~ 
	}
}