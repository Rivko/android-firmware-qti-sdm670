#$inputfile="C\:\\Dropbox\\1260766\\DumpTimeStamp\.txt";
$inputfileName=$ARGV[0];
$uniquenum=$ARGV[1];
$flag=$ARGV[2];
$TragetName=$ARGV[3];
$count=0;
$inputfile="$inputfileName\\Crashed_ThreadCallStack\.txt";
#$inputfile="$inputfileName\\StackWalkerCallStack\.txt";
$inputfile1="$inputfileName\\DSPAnalysis\.txt";
$inputfile2="$inputfileName\\Error_Cause\.txt";
$inputfile3="$inputfileName\\Crashed_Thread_RegSet\.txt";
#$inputfile="Area_IO\.txt";
#open(SIGFILE, ">\\\\harv-pramodp\\Builds\\CrashDatabase\\$uniquenum\.txt") or die "not able to open file \"GetCrashedTCB.txt\" in write mode";#Writing to a file "system.xls" which is output.
open(SIGFILE, ">$inputfileName\\$TragetName\_$uniquenum\.txt") or die "not able to open file \"$uniquenum\.txt\" in write mode";#Writing to a file "system.xls" which is output.
if (open(TRACEFILE,"$inputfile1")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
if($inputline =~ /The crashed SW Thraed is : (\w+)/)
{
$CrashedThr=$1;
print SIGFILE ("$CrashedThr\n");
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out\:No Such file name DSPAnalysis\.txt"
}
close TRACEFILE;

if (open(TRACEFILE,"$inputfile2")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
if($inputline =~ /.+\= 0x(\w+)/)
{
$hexval=$1;
chomp($hexval);
print SIGFILE ("0x$hexval");
}
elsif($inputline =~ /.+\=(.+)/)
{
$decval=$1;
#print"\n$decval\n";
$hexval = sprintf("%x", $decval);
#print"\n$hexval\n";
#printf("%x",$decval);
print SIGFILE ("0x$hexval");
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out\:No Such file name Error_Cause\.txt"
}
close TRACEFILE;

if (open(TRACEFILE,"$inputfile3")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
#if($inputline =~ /.+\=(.+)/)
if(($inputline =~ /HTID\s+\w+\s+SSR\s+(\w+)/) || ($inputline =~ /HTID\s+SSR\s+(\w+)/))
{
$decval=$1;
@decval1=split('', $decval);
print SIGFILE ("$decval1[6]$decval1[7]\n");
#$val=$decval&0x000000FF;
#print"\n$val\n";
#$hexval = sprintf("%x", $decval);
#printf("%x",$decval);
#print SIGFILE ("0x$hexval\n");
#print "\n$hexval\n";
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out\:No Such file name Error_Cause\.txt"
}
close TRACEFILE;


#if($flag==2)
#{
if (open(TRACEFILE,"$inputfile")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
##print"\n$inputline\n";
if($count != 9)
{
##if($inputline =~ /.+\*(.+)\(.+/)
#if($inputline =~ /^(\w+)\s+\(/)
if(($inputline =~ /.+\*(.+)\(.+/) || ($inputline =~ /^(\w+)\s+\(/) || ($inputline =~ /^\.(\w+)\s+\(/))
{
$callstack=$1;
#print "$callstack";
if($temp ne $callstack)
{
#print"\n$callstack\n";
print SIGFILE ("$callstack\n");
$count++;
}
$temp=$callstack;
##print SIGFILE ("$callstack\n");
}
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out\:No Such file name Crashed_ThreadCallStack\.txt"
}
close TRACEFILE;
#}
#else
#{
#if (open(TRACEFILE,"$inputfile")){
#$inputline=<TRACEFILE>;
#while ( $inputline ne "" ) {
#if($count != 9)
#{
#if($inputline =~ /.+\*(.+)\(.+/)
#{
#$callstack=$1;
#print SIGFILE ("$callstack\n");
#}
#$count++;
#}
#$inputline=<TRACEFILE>;
#}
#}
#else
#{
#die"out\:No Such file name Crashed_ThreadCallStack\.txt"
#}
#close TRACEFILE;
#}