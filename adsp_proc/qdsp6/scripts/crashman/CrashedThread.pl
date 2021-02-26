#$inputfile="C\:\\Dropbox\\1260766\\DumpTimeStamp\.txt";
$inputfileName=$ARGV[0];
$TCBCrash=0;
$inputfile="$inputfileName\\Area_IO\.txt";
#$inputfile="Area_IO\.txt";
open(DUMPFILE, ">$inputfileName\\GetCrashedTCB\.txt") or die "not able to open file \"GetCrashedTCB.txt\" in write mode";#Writing to a file "system.xls" which is output.
if (open(TRACEFILE,"$inputfile")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
#print"\n$inputline\n";
if($inputline =~ /^0x(\w+)\s+/)
{
$CrashedTCB = $1;
$CrashedTCB="0x$CrashedTCB";
$TCBCrash=1;
print"\n$CrashedTCB \n";
print DUMPFILE ("1\n");
print DUMPFILE ("$CrashedTCB");
goto labelout;
}
if($inputline =~ /\s+error TCB 0x(\w+)/)
{
$CrashmanTCB = $1;
print"\n$CrashmanTCB \n";
}
$inputline=<TRACEFILE>;
}
#print DUMPFILE ("0\n");
labelout:
}
else
{
die"out\:No Such file name Area_IO\.txt"
}
close TRACEFILE;

if($TCBCrash==0)
{
if (open(TRACEFILE,"$inputfile")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
if($inputline =~ /error TCB 0x(\w+)/)
{
$CrashedTCB = $1;
$TCBCrash=1;
$CrashedTCB="0x$CrashedTCB";
print"\n$CrashedTCB \n";
print DUMPFILE ("2\n");
print DUMPFILE ("$CrashedTCB");
}
$inputline=<TRACEFILE>;
#print"\n$inputline\n";
}
}
else
{
die"out\:No Such file name Area_IO\.txt"
}

}
if($TCBCrash==0)
{
print DUMPFILE ("0\n");
}
close DUMPFILE;