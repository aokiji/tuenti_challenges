#!/usr/bin/perl
use strict;
use warnings;
use Net::Telnet ();
use Path::Tiny;
use List::AllUtils qw(min first max);
my $host = "52.49.91.111";
my $port = "9988";

my @words = path('words.txt')->lines( { 'chomp' => 1 } );


playgame( telnet() );

sub telnet() {
	my $telnet = new Net::Telnet(
		Timeout => 0.3,
		Prompt  => '/> $/',
		Host    => $host,
		Port    => $port,
		Dump_Log   => 'dump.log',
		Input_log => 'input.log',
	);
	
	$telnet->binmode(1);
	if ( $telnet->open() != 1 ) {
		die "Couldn't open connection\n";
	}
	
	$telnet->waitfor(Match => '/Press enter to continue.../i', Timeout => 10);
	
	my @lines = $telnet->cmd(' ');
	print @lines;
	my $count = $lines[-2] =~ tr/_//;
	print $count, "\n";
	
	return ($telnet, $count);
}

sub choose_letter {
	my ($word_filter, $tried, $word_length, $tries_left) = @_;
	my %letter_count = ();
	my $total_letters = $word_length * @$word_filter;
	for my $word_filter_id (@$word_filter) {
		my $word = $words[$word_filter_id];
		my %letters = map { substr( $word, $_, 1 ) => 1 } ( 0 .. length($word) - 1 );
		for my $letter (keys %letters) {
			++$letter_count{$letter};
		}
	}
	
	my $max = 0;
	my $letter = undef;
	my $sorted_keys = [reverse sort {$letter_count{$a} <=> $letter_count{$b}} grep {!$tried->{$_}} keys %letter_count];
	
	# with less tries, less risky
	my $risk = $tries_left / 6;
	my $index;
	if ($word_length < 8) {
		$index = 0;
	}
	else {
		$index = int($tries_left * (min(6, @$sorted_keys - 1)) / 6);
	}

	print $index, " ", $word_length, " ", $tries_left, "\n";
	return $sorted_keys->[$index];
}

sub playgame() {
	my ( $telnet, $word_length ) = @_;
	my (%tried, $count, @word_filter, $tries_left);
	
	my $reset = sub {
		$count = $word_length = $_[0];
		@word_filter = grep { length( $words[$_] ) == $word_length } ( 0 .. $#words );
		%tried = ();
		$tries_left = 6;
	};
	
	$reset->($word_length);

	while ( 1 ) {
		my $letter = choose_letter(\@word_filter, \%tried, $word_length, $tries_left);
		
		if (!defined $letter) {
			return;
		}
		
		$tried{$letter} = 1;
		my @lines = eval {
			my @lines = $telnet->cmd($letter);
		};
		if (! @lines) {
			@lines = $telnet->cmd(' ');
			if (first {$_ =~ /Congratulations|cleared/} @lines) {
				print "WE WON, to the next level!!\n";
				print @lines;
				$word_length = $lines[-2] =~ tr/_//;
				$reset->($word_length);
				next;
			}
			else {
				print @lines;
				return;
			}
		}
		print @lines[-10 .. -1];
		
		my $new_count = $lines[-2] =~ tr/_//;
		print $new_count, " ", $count, "\n"; 
		if ($new_count == $count) {
			# failed
			@word_filter = grep {$words[$_] !~ /$letter/} @word_filter;
			$tries_left--;
		}
		else {
			# succeded
			my $found = join('', split (/\W+/, $lines[-2]));
			$found =~ s/_/./g;
			
			@word_filter = grep {$words[$_] =~ m/$found/} @word_filter;
		}
		
		$count = $new_count;
		
		for my $line (@lines) {
			if ($line =~ /GAME OVER$/) {
				# reset connection and restart
				($telnet, $count) = telnet();
				$reset->($count);
				next;
			}
		}

	}
}
