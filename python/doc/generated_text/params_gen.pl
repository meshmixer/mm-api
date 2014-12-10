sub max ($$) { $_[$_[0] < $_[1]] }

open NAMEFILE,"<","tool_params.txt";

$mode = 1;
$mode = $ARGV[0] if scalar(@ARGV) > 0;

@tool_names = ();
@tool_descs = ();
$active_tool = "";

@params = ();

print ".. _tool-params-label:\n\n";
print "Tool Parameters\n";
print "===============\n";
print "* **vector3f** is a 3-float list\n";
print "* **matrix3f** is a 9-float3x3 row-major matrix\n";
print "\n";

while(<NAMEFILE>) {
    $line = $_;
    $line =~ s/^\s+//;
    $line =~ s/\s+$//;
    if ( $line =~ /\A\Z/ ) {
	next;
    }
    if ($line =~ /\A\[/ ) {
	($name, $middle, $desc) = split /\s+/,$line,3;
	$name =~ s/\]|\[//g;
	push @tool_names, $name;
	push @tool_descs, $desc;

	if ( length($active_tool) > 0 ) {
	    print_param_table($active_tool, @params);
	    @params = ();
	}
	$active_tool = $name;

	next;
    }
    if ($line =~ /\A\"/) {
	($name, $sep, $type, $rest) = split /\s+/,$line,4;
	$name =~ s/\"//g;
	$rec = {
	    NAME => $name,
	    TYPE => $type,
	    DESC => $rest
	};
	push @params, $rec;

	next;
    }
}

exit;

sub print_param_table {
    $tool_name = shift;
    @params = @_;
    
    @maxlens = (0,0,0);
    foreach $rec (@params) {
	$maxlens[0] = max($maxlens[0], length($rec->{NAME}));
	$maxlens[1] = max($maxlens[1], length($rec->{TYPE}));
	$maxlens[2] = max($maxlens[2], length($rec->{DESC}));
    }

    print "\n$tool_name\n";
    print '-' x length($tool_name) . "\n";
    print '=' x $maxlens[0] . " " . '=' x $maxlens[1] . " " . '=' x $maxlens[2] . "\n";
    $fmt = "%-$maxlens[0]s %-$maxlens[1]s %-$maxlens[2]s\n";
    foreach $rec (@params) {
	printf($fmt, $rec->{NAME}, $rec->{TYPE}, $rec->{DESC});
    }
    print '=' x $maxlens[0] . " " . '=' x $maxlens[1] . " " . '=' x $maxlens[2] . "\n";			  

}
