#!perl.exe 

#$f3_input = $ARGV[0];
$oppath=$ARGV[0];
$opfullpath=$oppath."\/Compare.txt";
$UniqueName=$ARGV[1];
open (OUTFILE, ">$opfullpath") or die " cant open output file: $!\n";

if($UniqueName eq "NotGiven")
{
print OUTFILE "0";
}
elsif(($UniqueName eq "search") || ($UniqueName eq "SEARCH") || ($UniqueName eq "Search"))
{
print OUTFILE "1";
}
else
{
print OUTFILE "2";
}

close(OUTFILE);