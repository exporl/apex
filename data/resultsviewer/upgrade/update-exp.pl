#!/usr/bin/perl

# upgrade experiment file from pre revision 695 to 695
#
# change order of <channels> and <gain> for wavdevice

$filename=$ARGV[0];

open(F, "<$filename") or die ("Can't open $filename\n");
@input = <F>;
close(F);


$gainfound = 0;
$difffound=0;
my @result;

foreach (@input) {
	if ( /<gain/ ) {
		$keepgain=$_;
		$gainfound=1;
		next;
	}
	if ($gainfound) {
		if ( /<channels>/) {
			print "Swapping <gain> and <channels>\n";
			$difffound=1;
			push(@result, $_);
			push(@result,$keepgain);
			$gainfound=0;
			next;
		} else {
			push(@result,$keepgain);
		}
		$gainfound=0;
	}
	push(@result,$_);

}


if ($difffound) {
	open(F, ">$filename") or die ("Can't open $filename for writing\n");
	print F @result;
	close(F);
} else {
	print "$filename: No old version detected\n";
}
