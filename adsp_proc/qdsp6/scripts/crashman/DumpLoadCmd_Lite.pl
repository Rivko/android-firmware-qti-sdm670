#!perl.exe 

$oppath=$ARGV[0];
print"\n$oppath\n";
$target=$ARGV[1];
$ramdump=$ARGV[2];
$crmbuild=$ARGV[3];
$customerelf=$ARGV[4];
$BinaryLength=$ARGV[5];
$adspstartadd=$ARGV[6];
$choice=$ARGV[7];
$DumpCheckFlag=$ARGV[8];
$Check=$ARGV[9];
$slpi=$ARGV[10];
$guest_os=$ARGV[11];
$align=$ARGV[12];
$cdsp_flag=$ARGV[13];

open (OUTFILE, ">$oppath\\..\\adspcrashman_click\.bat") or die " cant open output file: $!\n";
print OUTFILE ("\@echo off\n");
print OUTFILE ("if not exist \\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\Crashman\\Latest\\adspcrashman_dumps.bat (\n");
print OUTFILE ("if not exist $crmbuild\\qdsp6\\scripts\\Crashman\\adspcrashman_dumps.bat (\n");
print OUTFILE ("    echo \"Please update crashman scripts\"\n");
print OUTFILE ("    pause\n");
print OUTFILE (") else (\n");
print OUTFILE ("    pushd $crmbuild\\qdsp6\\scripts\\Crashman\n");
if($Check=="0")
{
print OUTFILE ("    adspcrashman_dumps.bat $target $ramdump C\:\\Crashman_Temp $crmbuild $crmbuild\\build\\ms $BinaryLength $adspstartadd $choice $DumpCheckFlag $slpi $guest_os $align $cdsp_flag\n");
}
else
{
print OUTFILE ("    adspcrashman_dumps.bat $target $ramdump C\:\\Crashman_Temp $crmbuild $customerelf $BinaryLength $adspstartadd $choice $DumpCheckFlag $slpi $guest_os $align $cdsp_flag\n");
}
print OUTFILE ("    pause\n");
print OUTFILE (")\n");
print OUTFILE (") else (\n");
print OUTFILE ("    pushd \\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\Crashman\\Latest\n");
if($Check=="0")
{
print OUTFILE ("    adspcrashman_dumps.bat $target $ramdump C\:\\Crashman_Temp $crmbuild $crmbuild\\build\\ms $BinaryLength $adspstartadd $choice $DumpCheckFlag $slpi $guest_os $align $cdsp_flag\n");
}
else
{
print OUTFILE ("    adspcrashman_dumps.bat $target $ramdump C\:\\Crashman_Temp $crmbuild $customerelf $BinaryLength $adspstartadd $choice $DumpCheckFlag $slpi $guest_os $align $cdsp_flag\n");
}
print OUTFILE ("    pause\n");
print OUTFILE (")\n");
close(OUTFILE);