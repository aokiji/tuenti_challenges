use List::AllUtils qw(first_index);
use Path::Tiny;

my @lines = path('labyrinth')->lines({'chomp' => 1});
my $key = '';
my $first_line = $lines[1];
$first_line =~ s/\W*//g;
$key .= $first_line;

my @vertical_keys = map { 
	my $line = $_;
	my @key = ();
	while ($line =~ m/(\w)/g) {
		push @key, $1;	
	} 
	[@key];
} @lines[1..$#lines - 1];

# remove repeated character
chop($key);
$key .= join('', map {$_->[-1]} @vertical_keys);

my $last_line = $lines[-2];
$last_line =~ s/\W*//g;
# remove repeated character
chop($key);
$key .= reverse($last_line);

# remove repeated character
chop($key);
$key .= reverse(join('', map {$_->[0]} @vertical_keys[2..$#vertical_keys]));

print $key, "\n";