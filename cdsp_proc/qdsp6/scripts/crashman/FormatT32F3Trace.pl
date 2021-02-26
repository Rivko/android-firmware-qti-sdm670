use Math::BigInt;

# Perl script used with recover_f3.cmm to reconstruct F3 messages
# Must be used with recover_f3.cmm that supports Qshrink output
#
# Usage:
#   perl FormatT32F3Trace <token_input.txt> <hash.txt>
#      token_input.txt is the output file from recover_f3.cmm
#      hash.txt (optional) is the QShrink hash file
#   The output will be printed to stdout.
#
# $Header: //components/rel/dspcore.adsp/4.0/scripts/crashman/FormatT32F3Trace.pl#3 $
#
# Copyright (c) 2009 by QUALCOMM, Inc

my $debug_flag=0;

if (defined($ARGV[0])) 
{
    $f3in = $ARGV[0];
} else 
{ 
    die "F3 trace input file not specified.\n";
}

if (defined($ARGV[1])) 
{
    $hashfile = $ARGV[1];
} else 
{ 
    $hashfile = "";
}

my %item = ();
my %qhash = ();

open (INFILE, "$f3in") or die "Can't open file $f3in for reading";

InitHash();

while(<INFILE>)
{

    $line = $_;

    if (/^(\w*) (.*)/)
    {
	$payload = $2;
	$token = $1;
	$payload =~ s/\r$//;

	if ($token eq "ITEM")
	{
	    # ITEM keyword found
	    # Begin processing new item from input file 
	    # print "ITEM found ($payload)\n"; 
	    ProcessItem($payload);
	}
	elsif ($token eq "TIME")
	{
	    ProcessTime($payload);
	}
	elsif ($token eq "MSG_FORMAT")
	{
	    $item{mformat} = $payload;
	}
	elsif ($token eq "MSG_FILE")
	{
	    $item{mfile} = $payload;
	}
	elsif ($token eq "MSG_HASH")
	{
	    ProcessHash($payload);
	}
	elsif ($token eq "MSG_LINE")
	{
	    $item{mline} = $payload;
	}
	elsif ($token eq "MSG_ARGS")
	{
	    ProcessArguments($payload);
	}
	elsif ($token eq "CORRUPT_DATA")
        {
	    CompletePreviousItem();
	    print "-----  (Possible missing MSGs at this location...corrupt buffer data found)\n";
	}
	else
	{
	    # Ignore EVENT data
            # If EVENTs are needed in the final output,
            # add logic to handle them here.
	    # print "Other cases\n";
	}
    }
}

CompletePreviousItem();

close INFILE;

sub ProcessItem 
{
    my $payload = shift;
    # print "Item rountine.\n";

    CompletePreviousItem();

    if ($payload =~ /(\w*) 0x(\d*)/)
    {

	$item{type} = $1;
	$item{header} = $2;
    }
    else
    {
	print "Unexpected ITEM payload: $payload\n";
    }
}

sub ProcessTime
{
    my $payload = shift;
    #print "Time routine.\n";

    if ($payload =~ /.*0x([ABCDEFabcdef\d]{8}) 0x([ABCDEFabcdef\d]{4})/)
    {
	#print "Time is $1 $2\n";
        $item{time_low32} = $1;
        $item{time_high16} = $2;

        ConvertTime();
    }
    else
    {
	print "Unexpected TIME payload: $payload\n";
    }
}

sub ConvertTime
{
    # constants for use by Time conversion
    $max32bit = hex("FFFFFFFF");
    my $cfactor = Math::BigInt->new($max32bit);
    my $cfactor = $cfactor+1;

    # Compute starting time from timestamp.
    # Need to use BigInt as this is a 48bit number
    $time = (hex($item{time_high16}) * $cfactor) + hex($item{time_low32});

    # Timestamp is in 1.25ms units, so convert to actual ms
    $time = ($time*5)/4;			
    
    # time starting in ms units
    # extract miliseconds value
    $item{time_ms} = $time % 1000;
    $time = ($time - $item{time_ms})/1000;

    # time now in seconds
    # extract seconds value
    $item{time_s} = $time % 60;
    $time = ($time - $item{time_s})/60;

    # time now in minutes
    # extract minutes value
    $item{time_m} = $time % 60;
    $time = ($time - $item{time_m})/60;

    # time now in hours
    # extract hours value
    $item{time_h} = $time % 24;

    # No use currently for anything above hours
    # But could be computed here.

    if ($debug_flag==1)
    {
	#print "Time is $item{time_h} : $item{time_m} : $item{time_s} . $item{time_ms}\n";
    }
}

sub ProcessArguments
{
    my $payload = shift;
    @args = split / /, $payload;
    foreach (@args)
    {
	if ($_ =~ /0x([ABCDEFabcdef\d]{2,8})/)
	{
	    #print "Argument: $_ $1\n";
	    push (@{$item{margs}},hex($1));
	}
    }
}

sub CompletePreviousItem
{
    if ($item{type} eq "MSG")
    {
	$final_time = sprintf("%02d:%02d:%02d.%03d",
			      $item{time_h},
			      $item{time_m},
			      $item{time_s},
			      $item{time_ms});
	$final_format = sprintf($item{mformat},@{$item{margs}});

	$final_msg = sprintf("%s    %5d   %s   %s\n",
			     $final_time,
			     $item{mline},
			     $item{mfile},
			     $final_format);

	print $final_msg;
    }
    else
    {
	#print "Skipping event\n";
    }

    # Print out item
    if ($debug_flag==1)
    {
	print "\n------------------------\n";
	while (($key, $value) = each(%item))
	{
	    print $key.", ".$value."\n";
	}
	foreach (@{$item{margs}})
	{
	    print "Arg: $_\n";
	}
	
	print "\n------------------------\n";
    }

    # Clear ITEM data
    %item = ();
}

sub InitHash
{
    if ($hashfile ne "")
    {
	unless (open (HASHFILE, $hashfile))
	{
	    print "Can't open hashfile $hashfile\n";
	    return;
	}

	print "Reading in hash file...";

	while(<HASHFILE>)
	{
	    if(/^(\d*):([\w\.]+):(.*)/)
	    {
		$qhash{$1} = [$2,$3];
	    }
	    else
	    {
		# print "Ignoring hash line: $_\n";
	    }
	}
	
	close HASHFILE;
	
	print "done\n";
    }
}

sub ProcessHash
{
    $payload = shift;
    $item{mhash} = hex($payload);

    if (exists $qhash{$item{mhash}})
    {
	$item{mfile} = $qhash{$item{mhash}}[0];
	$item{mformat} = $qhash{$item{mhash}}[1];
    }
    else
    {
	$item{mformat} = "`F3 QSR RECOVERY ERROR: Hash $payload not found";
	$item{mfile} = "-----";
    }

}
