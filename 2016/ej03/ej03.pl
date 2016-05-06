#!/usr/bin/perl
use YAML qw();
use IO::All;
{
	my $filedata < io('-') ;
	$filedata =~ s/^\.\.\.\n?$//m;
	my $config = YAML::Load($filedata);
	
	for my $tape_id (sort keys %{$config->{tapes}}) {
		my $tape = $config->{tapes}{$tape_id};
		my $result = processTape($config->{code}, $tape);
		print "Tape #", $tape_id, ": ", $result, "\n";
	}
}

sub processTape {
	my ($code, $tape) = @_;
	my @tape  = unpack("(A1)*", $tape);
	my $op = "start";
	my $curr = 0;
	while ($op ne "end") {
		my $ops = $code->{$op}{$tape[$curr] // ' '};
		if (exists $ops->{'write'}) {
			$tape[$curr] = $ops->{'write'};
		}
		if (exists $ops->{'move'}) {
			if($ops->{'move'} eq 'left') {
				$curr--;
			}
			else {
				$curr++;
			}
		}
		if (exists $ops->{'state'}) {
			$op = $ops->{'state'};
		}
	}
	return join('', @tape);
}
