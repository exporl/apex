#!/usr/bin/perl

use strict;
use Tk;
use Tk::Dialog;
use Cwd qw/abs_path/;


my $delim;
 if ($^O eq "linux") {
        $delim='/';
 } elsif ($^O eq "MSWin32") {
        $delim="\\";
} else {
        die("OS not supported: $^O");
}

my $basedir = &getbasedir;

my $mw= MainWindow->new;

my $label1 = $mw->Label(-text => 'Input file (apex2):');
my $inputfile = $mw->Entry(-width => "50",
						-textvariable => "inputfile");
my $selectibutton = $mw->Button(-text => 'select', 
               -command => \&getifile_callback, 
               -state => 'normal');
my $label2 = $mw->Label(-text => 'Output file (apex3):');
my $outputfile = $mw->Entry(-width => "50",
						-textvariable => "outputfile");
my $selectobutton = $mw->Button(-text => 'select', 
               -command => \&getofile_callback, 
               -state => 'normal');

my $gobutton = $mw->Button(-text => 'Convert', 
               -command => \&convertbutton_callback, 
               -state => 'normal');

my $exitbutton = $mw->Button(-text => 'Exit', 
               -command => \&exit, 
               -state => 'normal');

my $statuslabel =  $mw->Label(-text => 'Status: waiting for user input');



$label1->pack;
$inputfile->pack;
$selectibutton->pack;
$label2->pack;
$outputfile->pack;
$selectobutton->pack;
$gobutton->pack;
$exitbutton->pack;
$statuslabel->pack;

MainLoop;

exit;


sub convertbutton_callback {
	my $ifile = $inputfile->get;
	my $ofile = $outputfile->get;
	
	unless (-f $ifile) {
		my $dialog=$mw->Dialog( -title => "Error",
                            -text => "Input file not found",
                            -default_button => "OK",
                            -buttons => [qw/OK/] );
    	my $answer=$dialog->Show(-global);
    	return;
	}
	
	unless (open(TEST, ">$ofile")) {
		my $dialog=$mw->Dialog( -title => "Error",
                            -text => "Can't open output file for writing",
                            -default_button => "OK",
                            -buttons => [qw/OK/] );
    	my $answer=$dialog->Show(-global);
    	return;
	} else {
		close (TEST);
	}
	
	(my $ext) = $ifile =~ /\.([a-zA-Z]{3})$/;
	$ext =~ lc($ext);
	
	$statuslabel->configure(-text => "Converting...");
	
	if ($ext eq "opc") {
		system "${basedir}opc2xml.pl \"$ifile\" \"$ofile\"";
	} elsif ($ext eq "aid") {
		system "${basedir}aid2xml.pl \"$ifile\" \"$ofile\"";
	} elsif ($ext eq "idn") {
		system "${basedir}idn2xml.pl \"$ifile\" \"$ofile\"";
	} else {
		my $dialog=$mw->Dialog( -title => "Error",
                            -text => "Extention $ext not supported",
                            -default_button => "OK",
                            -buttons => [qw/OK/] );
    	my $answer=$dialog->Show(-global);
    	$statuslabel->configure(-text => "Failed");
    	return;
	}
	
	$statuslabel->configure(-text => "Done.");
	
}


sub getifile_callback {
	my @types = (
	   ["All Apex Files", ".idn"],
	   ["All Apex Files", ".opc"],
	   ["identification", '.idn'],
	   ["identification", '.aid'],
	   ["opc", '.opc'],
       ["All Files", "*"] );
       
    my $name =    $mw->getOpenFile(-filetypes => \@types,
      -initialfile=> 'Default' );
	$inputfile->insert( 0,  $name );
	my $keepname = $name;
	$name =~ s/\.[a-zA-Z]{3}$/\.xml/;
	if ($keepname ne $name) {
		$outputfile->insert(0, $name);
	}
}

sub getofile_callback {
	my @types=(["Apex 3 XML files", ".xml"],
				["All Files", "*"]);
	$outputfile->insert( 0, $mw->getSaveFile(-filetypes => \@types,
      -initialfile=> 'Default',
      -defaultextension => '.xml')  );
}


sub getbasedir {
 		my $basedir=&abs_path($0);
        $basedir=~s/\\/\//g;
        $basedir=~ qw{^(.*)[/][^/]*$};
        $basedir=$1;
#        $basedir=&getcwd . $delim . $basedir;
        if (!defined($basedir) or $basedir eq '') {
            $basedir='.';
        }
        $basedir=&makedirend($basedir);
        return $basedir;
}

sub makedirend {
    my $dir = $_[0];

    if (substr($dir, -1) ne $delim) {
    $dir=$dir.$delim;
    }
    return $dir;

}

