sub max ($$) { $_[$_[0] < $_[1]] }


open NAMEFILE,"<","tool_names.txt";

$mode = 1;
$mode = $ARGV[0] if scalar(@ARGV) > 0;

@names = ();
@descriptions = ();

while(<NAMEFILE>) {
    $line = $_;
    $line =~ s/\n//;
    if ( $line =~ /\A\Z/ ) {
	next;
    }

    ($name,$desc) = split /\|/, $line;
    if ($mode == 1) {
	$name = "**".$name."**";
    }

    push @names, $name;
    push @descriptions, $desc;
}

$max_name_len = 0;
$max_desc_len = 0;
for ($i = 0; $i < scalar @names; $i++) {
    $max_name_len = max($max_name_len, length($names[$i]));
    $max_desc_len = max($max_desc_len, length($descriptions[$i]));
}

print '=' x $max_name_len . " " . '=' x $max_desc_len . "\n";
$fmt = "%-$max_name_len" . "s %-$max_desc_len" . "s\n";
for ($i = 0; $i < scalar @names; $i++) {
    printf( $fmt, $names[$i], $descriptions[$i] );
}
print '=' x $max_name_len . " " . '=' x $max_desc_len . "\n";
