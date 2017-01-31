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


if (length(@ARGV) != 2) {
     print <<"BLA";
Syntax: generatefiles.pl <path> <prefix>
BLA

}

my $targetpath=$ARGV[0];
my $pathprefix=$ARGV[1];


print "Files in $targetpath:\n";
print "------------------------\n";


processpath($targetpath);

sub processpath {
     my $path = $_[0];
     my $prefix = $_[1];
     my $component = $_[2];

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
#             if ( $file =~ /(.*)\.(xml|apx)$/) {
                 push(@files, $file);
#             }
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

#     my $dirmade;
#     if ($path!=".") {
#     	 $dirmade=1;
#         print "<Directory Id=\"DirID$lastdir\" Name=\"" .  genshortname($lastdir) . "\" LongName=\"$lastdir\">\n";
#     }
#
     if (length(@files)) {
#         my $newuuid=`uuid`;
#         chomp($newuuid);
#         $newuuid=lc($newuuid);
#         my $componentid="$component$lastdir";
#         push(@components,$componentid);
#         print <<"FFF";
#    <Component Id="$componentid" DiskId="1"
#               Guid="$newuuid">
#FFF
#
         foreach my $file (@files) {
             my $fullpath = $path . "/" . $file;

             $file =~ /(.*)\.([^.])*$/;
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
                      Source="\$(var.InstallPath)/$pathprefix/$file"
                      />
FFF

         }

#         print "</Component>\n";

     }

#     if (length(@directories)) {
#         foreach my $file (@directories) {
#             my $fullpath = $path . "/" . $file;
#             processpath($fullpath,$prefix,$component);
#         }
#
#     }
#     if ($dirmade) {
#         print "</Directory>\n";
#     }
}



sub genshortname {
     my $name = $_[0];

     my $base;
     my $ext;

     ($base,$ext) = $name =~ /(.*)\.([^.]+)$/;

     my $shortname = substr($base,0,4) . int(rand(9999)) . "." . substr($ext,0,3);

     return $shortname;
}
