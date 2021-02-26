#!perl.exe 

#$f3_input = $ARGV[0];
$oppath=$ARGV[0];
open(DYNFINFILE, ">$oppath\\DynamicLoadInfoFin\.txt") or die "not able to open file \"DynamicLoadInfoFin.txt\" in write mode";#Writing to a file "system.xls" which is output.
open(DYNOBJFINFILE, ">>$oppath\\DynamicLoadObjInfo\.txt") or die "not able to open file \"DSPAnalysis.txt\" in write mode";#Writing to a file "system.xls" which is output.


if (open(DYNLIST,"$oppath\\DynamicLoadInfo.txt")){
$inputline=<DYNLIST>;
	while ( $inputline ne "" ) {
	#if($inputline =~ /.+\.l_addr \= (\w+)/)
	if($inputline =~ /.+\.l_addr \= (.+)/)
	{
	 $addr=$1;
     $addr1=hex($addr);
     if($addr1 == 0)
     {
       print DYNFINFILE ("0\n");
       goto end;	   
	 }
	 else
	 {
	    print DYNFINFILE ("1\n");
		print DYNOBJFINFILE ("The Dynamic Object is loaded at the address \= $addr\n");
	 } 
	}
	if($inputline =~ /.+\.l_name \= .+ \"(.+)\"/)
	{
	$name=$1;
	print DYNOBJFINFILE ("The name of the Dynamic Object loaded \= $name\n");
	}
	#if($inputline =~ /.+\.l_next \= (\w+)/)
	if($inputline =~ /.+\.l_next \= (.+)/)
	{
	 $next=$1;
	 $next1=hex($next);
	 if($next1 == 0)
	 {
    	print DYNFINFILE ("0\n");
	  }
	 else
	 {
	    print DYNFINFILE ("1\n");
	 }
	}
	$inputline=<DYNLIST>;
	}
}
else
{
die"out\:No Such file name DynamicLoadInfo\.txt"
}

end:
close(DYNFINFILE);
close(DYNOBJFINFILE);
close(DYNLIST);