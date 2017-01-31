#!/usr/bin/perl

use strict;


# my @answers = ( 's1', 's2', 's3',  's1', 's2', 's3', 's3' );
# my @input = ( 's1', 's2', 's3',  's1', 's2', 's3', 's1' );
#
# confusionmatrix( \@answers, \@input);

return 1;

sub confusionmatrix {
	my ($answers, $input) = @_;

	my %matrix;
	my $result = '';

	if ( $#$answers != $#$input) {
		die "invalid input";
	}

	foreach my $a (@$answers) {
		foreach my $b (@$answers) {
			$matrix{$a}{$b}=0;
		}
	}

	for (my $i=0; $i<=$#$answers; ++$i) {
		$a=$$answers[$i];
		$b=$$input[$i];
# 		if ( exists($matrix{$a}{$b})) {
			$matrix{$a}{$b} = $matrix{$a}{$b}+1;
# 		} else {
#			$matrix{$a}{$b} = 1;
#		}
	}


	foreach my $i (sort keys %{$matrix{$a}} ) {
		$result .= "\t$i";
	}
	$result .= "\n";

	foreach my $a (sort keys %matrix) {
		$result .= "$a\t";
		foreach my $i (sort keys %{$matrix{$a}}) {
			$result .= $matrix{$a}{$i};
			$result .= "\t";
		}
		$result .= "\n";
	}

	return $result;

}