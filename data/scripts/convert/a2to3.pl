#!/usr/bin/perl

use Cwd qw/abs_path/;

#$basedir="";

		$basedir=&abs_path($0);
        $basedir=~s/\\/\//g;
        $basedir=~ qw{^(.*)[/][^/]*$};
        $basedir=$1 . "/";
        

if ($#ARGV != 1) {
    &showusage;
    exit(1);
}

my $ifile = $ARGV[0];
my $ofile = $ARGV[1];

(my $ext) = $ifile =~ /\.([a-zA-Z]{3})$/;
$ext =~ lc($ext);

	
local(@_)  = ($ifile,$ofile);
	
	if ($ext eq "opc") {
		do "${basedir}opc2xml.pl";
	} elsif ($ext eq "aid") {
		do "${basedir}aid2xml.pl";
	} elsif ($ext eq "idn") {
		do "${basedir}idn2xml.pl";
	} elsif ($ext eq "adp") {
		do "${basedir}adp2xml.pl"; 
	} else {
		print "Error: I don't recognize the extension $ext\n";
	}