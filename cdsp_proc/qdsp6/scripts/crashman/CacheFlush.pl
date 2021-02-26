 	
$inputfileName=$ARGV[0];	
open(FLUSHFILE, ">$inputfileName\\CacheFlushFlag\.txt") or die "not able to open file \"CacheFlushFlag.txt\" in write mode";#Writing to a file "CacheFlushFlag.txt" which is output.
	if (open(CALIST,"$inputfileName\\flush\_cache\.txt")){
	$inputline=<CALIST>;
	while ( $inputline ne "" ) {
	if($inputline =~ /.+\*(\w+).+/)
	{
	$cacheflushvalue = $1;
	$cacheflushvalue=hex($cacheflushvalue);
	print FLUSHFILE ("$cacheflushvalue");
	#print"\n$cacheflushvalue\n";
	goto done;
	}
	$inputline=<CALIST>;
	}
done:	
	}	
	else
	{
	print "No Such file name flush_cache\.txt\n";
	}