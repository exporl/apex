#!/usr/bin/perl

use strict;
#use OSSP::uuid;
#
#tie my $uuid, 'OSSP::uuid::tie';
#$uuid = [ "v1" ];
#print "UUIDs: $uuid, $uuid, $uuid\n";
# use OSSP::uuid;
#         my $uuid    = new OSSP::uuid;
#         my $uuid_ns = new OSSP::uuid;
#         $uuid_ns->load("ns:URL");
#         $uuid->make("v1");
#         undef $uuid_ns;
#         my $str = $uuid->export("str");
#         undef $uuid;
#         print "$str\n";
#

my $script = $0;
$script =~ s/([^\/\\]*$)//;
$script = `pwd $script`;
chomp($script);
chdir($script);
chdir("../../../examples");

#open(FID, "find ../../../examples |");
#my @examplefiles=<FID>;
#close(FID);
#chomp(@examplefiles);

my @components;

processpath(".", "examples","ExamplesComp");

open(FF, ">${script}/examplesfeature.wxi");
print FF<<"BLA";
<?xml version="1.0" encoding="utf-8"?>
<Include xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://schemas.microsoft.com/wix/2003/01/wi ../../../../../temp/wix/doc/wix.xsd"
	xmlns="http://schemas.microsoft.com/wix/2003/01/wi">
              <Feature Id="Examples" Title="Examples" Absent="allow"
              AllowAdvertise="yes" Description="Example experiment files." Level="1">
BLA

foreach my $component (@components) {
    print FF<<"BLA";
                    <ComponentRef Id="$component"/>
BLA
}
print FF<<"BLA";
        </Feature>
</Include>
BLA

close(FF);

sub processpath {
     my $path = $_[0];
     my $prefix = $_[1];
     my $component = $_[2];
#     my $componentguid = $_[3];

     opendir(DIR, $path);
     my @thefiles= readdir(DIR);
     closedir(DIR);

     my $filefound = 0;
     my $componentopened = 0;
     my $lastdir;
     ($lastdir) = $path =~ /([^\/]*)$/;

     my @directories;
     my @files;


     foreach my $file (@thefiles) {
         my $fullpath = $path . "/" . $file;
         next if ($file eq "." or $file eq "..");

         if (-f $fullpath) {
             if ( $file =~ /(.*)\.(xml|apx|wav|js|html)$/) {
                 push(@files, $file);
             }
         } elsif (-d $fullpath) {
             unless ( $file =~ /^\./) {
                 push(@directories, $file);
             }
         } else {
              die("internal error");
         }
     }

     if (!length(@files) and !length(@directories)) {
         return;
     }

     my $dirmade;
     if ($path ne ".") {
     	 $dirmade=1;
         print "<Directory Id=\"DirID$lastdir\" Name=\"" .  genshortname($lastdir) . "\" LongName=\"$lastdir\">\n";
     }

     if (length(@files)) {
         my $newuuid=`uuid`;
         chomp($newuuid);
         $newuuid=lc($newuuid);
         my $componentid="$component$lastdir";
         push(@components,$componentid);
         print <<"FFF";
    <Component Id="$componentid" DiskId="1"
               Guid="$newuuid">
FFF

         foreach my $file (@files) {
             my $fullpath = $path . "/" . $file;

             $file =~ /(.*)\.(xml|apx|wav|jpg|js)$/;
             my $barename=$1;
             my $ext=$2;

             my $shortname = genshortname($file);
             my $ppath = $path;
             $ppath =~ s{^\./}{};

             my $fileid = "FileID$barename" . int(rand(100));
             $fileid =~ s/\W//g;
             print <<"FFF";
<File Id="$fileid" Name="$shortname"
                      LongName="$file"
                      Source="\$(var.InstallPath)/$prefix/$ppath/$file"
                      />
FFF

         }

         print "</Component>\n";

     }

     if (length(@directories)) {
         foreach my $file (@directories) {
             my $fullpath = $path . "/" . $file;
             processpath($fullpath,$prefix,$component);
         }

     }
     if ($dirmade) {
         print "</Directory>\n";
     }
 }



sub genshortname {
     my $name = $_[0];

     my $base;
     my $ext;

     ($base,$ext) = $name =~ /(.*)\.([^.]+)$/;

     my $shortname = substr($base,0,4) . int(rand(9999)) . "." . substr($ext,0,3);

     return $shortname;
}
