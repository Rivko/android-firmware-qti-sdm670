#$inputfile="C\:\\Dropbox\\1260766\\DumpTimeStamp\.txt";
$inputfileName=$ARGV[0];
$inputfile="$inputfileName\\Crashed_Thread_RegSet\.txt";
#$inputfile="Area_IO\.txt";
open(DUMPFILE, ">$inputfileName\\GetCrashedELR\.txt") or die "not able to open file \"GetCrashedELR.txt\" in write mode";#Writing to a file "system.xls" which is output.
if (open(TRACEFILE,"$inputfile")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
#if($inputline =~ /\s+elr = (\w+)\,/)
if(($inputline =~ /STID\s+ELR\s+(\w+).+/) || ($inputline =~ /STID\s+\w+\s+ELR\s+(\w+).+/))
{
$ELRValue="0x$1";
print DUMPFILE ("$ELRValue");
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out\:No Such file name Crashed_Thread_RegSet\.txt"
}

