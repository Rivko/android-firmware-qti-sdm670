#!perl.exe
# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
#
#             HASH ADSP AND INCLUDE IN MODEM BIN TO LOAD ON SURF
#
# GENERAL DESCRIPTION
#   A Utility to create hashed ADSP image and generatin Modem.bin to load on SURF
#   Usage: #useage :Perl hashDSP_Generate_Modem_Bin.pl <Modem_Root> <DSP_ROOT> <FLAVOUR> <Output_folder>";
#
#  Copyright(c) 2011 by QUALCOMM, Incorporated. All Rights Reserved.
# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====

# ===========================================================================
#
#                       EDIT HISTORY FOR FILE
#
#   This section contains comments describing changes made to this file.
#   Notice that changes are listed in reverse chronological order.
#
#  when       who     what, where, why
# --------   ---     ---------------------------------------------
#  02/25/11  achintal First version
#  04/19/11  achintal Second version
#  06/14/11  achintal Third version
# ===========================================================================
#
#                      INCLUDE FILES FOR MODULE
#
# ===========================================================================


use File::Path;
use File::Copy;

$fn=0;
$ERROR=0;
$MODEM_BUILD=0;
$fw=0;
$NumArgs=$#ARGV;
sub usage
{
print "Usage: Perl $0 <Modem_Root> <DSP_ROOT> <FLAVOUR> <Output_folder>\n
Eg: Perl $0 \\\\char\\wormhole-sandiego\\M8660AAABQNLGM1080 Y:\\achintal\\achintal_HXGN_M8660.06.00.22.14 AAABQNLG C:\\Dropbox \n";
exit;
}
usage() if($NumArgs!=3);
@warnings;
chdir($ARGV[0]) or error("Cannot access folder $ARGV[0]");
$DSP_ROOT = $ARGV[1];
$FLAVOUR= $ARGV[2];

if(-d "modem_proc") {
$MODEM_BUILD =1;
}
if(-d "modem_proc/build/ms/bin/PIL_IMAGES/SPLITBINS_".$FLAVOUR."/SPLITBINS_".$FLAVOUR."/MODEM")
{
$SPLIT_MODEM_PATH = "modem_proc\/build\/ms\/bin\/PIL_IMAGES\/SPLITBINS_".$FLAVOUR."\/SPLITBINS_".$FLAVOUR."\/MODEM";
}
elsif(-d "modem_proc/build/ms/bin/PIL_IMAGES/SPLITBINS_".$FLAVOUR."/MODEM")
{
$SPLIT_MODEM_PATH = "modem_proc\/build\/ms\/bin\/PIL_IMAGES\/SPLITBINS_".$FLAVOUR."\/MODEM";
}

#======================================================List of Files To be copied================================================================
@dirarray0= ($MODEM_BUILD,"modem_proc\/build\/ms","fileonly","h");
@dirarray1= ($MODEM_BUILD,"modem_proc\/build\/ms","fileonly","py");
@dirarray2= ($MODEM_BUILD,"modem_proc\/core\/bsp\/build","fileonly");
@dirarray3= ($MODEM_BUILD,"modem_proc\/core\/bsp\/amsslibs\/build\/qdsp6fw","fileonly");
@dirarray4= ($MODEM_BUILD,"modem_proc\/core\/bsp\/amsslibs\/build","fileonly");
if(-d "modem_proc/core/bsp/build/data")
{
@dirarray5= ($MODEM_BUILD,"modem_proc\/core\/bsp\/build\/data", "fileonly");
}
@dirarray6= ($MODEM_BUILD,"modem_proc\/core\/bsp\/build\/scripts", "fileonly");
@dirarray7= ($MODEM_BUILD,"modem_proc\/core\/bsp\/build\/tools", "all");
@dirarray8= ($MODEM_BUILD,"modem_proc\/core\/bsp\/tools\/SCons", "all");
@dirarray9= ($MODEM_BUILD,"modem_proc\/core\/bsp\/tools\/build", "all");
@dirarray10= ($MODEM_BUILD,"modem_proc\/core\/storage\/tools\/fattool", "fileonly","exe");
@dirarray11= ($MODEM_BUILD,"modem_proc\/core\/storage\/tools\/fattool", "fileonly","bin");
@dirarray12= ($MODEM_BUILD,"modem_proc\/tools\/build\/", "all");
@dirarray13= ($MODEM_BUILD,$SPLIT_MODEM_PATH, "all");



@dirarray = (\@dirarray0,\@dirarray1,\@dirarray2,\@dirarray3,\@dirarray4,\@dirarray5,\@dirarray6,\@dirarray7,\@dirarray8,\@dirarray9,\@dirarray10,\@dirarray11,\@dirarray12,\@dirarray13);

@dirarraysize = ($#dirarray0,$#dirarray1,$#dirarray2,$#dirarray3,$#dirarray4,$#dirarray5,$#dirarray6,$#dirarray7,$#dirarray8,$#dirarray9,$#dirarray10,$#dirarray11,$#dirarray12,$#dirarray13);


#================================================================================================================================================

$maxindex=$#dirarray;

$OriginalBuildPath = $ARGV[0];
@path = split(/\\/,$OriginalBuildPath);
$new = $path[$#path];
$Hash_Modem_Build  = $ARGV[3]."\\hash_".$new;

$cbitop="$ARGV[3]/output_";
print "mking directory $Hash_Modem_Build";
mkdir "$Hash_Modem_Build";

for(my $i=0;$i<= $maxindex;$i=$i+1) {

#open(OUTFILE,">$cbitoppath")||die("  can't create output_$i.txt");
	if($dirarray[$i][0] == 1){
			mkpath($Hash_Modem_Build."\/".$dirarray[$i][1]);
			if($dirarraysize[$i] == 2) {
			print "original path $OriginalBuildPath $dirarray[$i][1] $Hash_Modem_Build $dirarray[$i][1] $dirarray[$i][2]";
			dircopy($OriginalBuildPath."\/".$dirarray[$i][1],$Hash_Modem_Build."\/".$dirarray[$i][1],$dirarray[$i][2]);
			#print OUTFILE ($Hash_Modem_Build."\/".$dirarray[$i][1]);
			if (-d $OriginalBuildPath."\/".$dirarray[$i][1])
			{
			$cbitoppath=$cbitop.$fn.".txt";
			open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
			$fn++;
			print OUTFILE ($Hash_Modem_Build."\/".$dirarray[$i][1]);
			print  OUTFILE ("   copied\n");
			close(OUTFILE);
			}
			} elsif ($dirarraysize[$i] == 3) {

			dircopy($OriginalBuildPath."\/".$dirarray[$i][1],$Hash_Modem_Build."\/".$dirarray[$i][1],$dirarray[$i][2],$dirarray[$i][3]);
			if (-d $OriginalBuildPath."\/".$dirarray[$i][1])
			{
			$cbitoppath=$cbitop.$fn.".txt";
			open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
			$fn++;
			print OUTFILE ($Hash_Modem_Build."\/".$dirarray[$i][1]);
			print  OUTFILE ("   copied\n");
			close(OUTFILE);
			}
			} elsif ($dirarraysize[$i] == 4) {

			dircopy($OriginalBuildPath."\/".$dirarray[$i][1],$Hash_Modem_Build."\/".$dirarray[$i][1],$dirarray[$i][2],$dirarray[$i][3],$dirarray[$i][4]);
			if (-d $OriginalBuildPath."\/".$dirarray[$i][1])
			{
			$cbitoppath=$cbitop.$fn.".txt";
			open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
			$fn++;
			print OUTFILE ($Hash_Modem_Build."\/".$dirarray[$i][1]);
			print  OUTFILE ("   copied\n");
			close(OUTFILE);
			}
			}
		}



    }
$cpfatfs_path = $Hash_Modem_Build."\\modem_proc\\core\\storage\\tools\\fattool\\";
chdir ($DSP_ROOT);
mkpath($Hash_Modem_Build."\/"."\/modem_proc\/build\/ms\/Q6");
dircopy($DSP_ROOT."\/"."\/obj\/qdsp6v3_ReleaseG",$Hash_Modem_Build."\/"."\/modem_proc\/build\/ms\/","fileonly","mbn");
system("copy $Hash_Modem_Build\\modem_proc\\build\\ms\\dsp1.mbn $Hash_Modem_Build\\modem_proc\\build\\ms\\dsp.elf");
chdir ("$Hash_Modem_Build"."\/modem_proc\/core\/bsp\/build\/");
system("build.cmd qdsp6fw BUILD_ID=$FLAVOUR");
chdir ("$Hash_Modem_Build"."\/modem_proc\/build\/ms\/Q6\/");
system("python ..\\pil-splitter.py ..\\qdsp6_fw.mbn q6");
system("copy $cpfatfs_path\\fat.bin $Hash_Modem_Build\\modem_proc\\build\\ms\\modem.bin");

$Modem_split_images_path ="$Hash_Modem_Build\\$SPLIT_MODEM_PATH";
$Modem_split_images_path =~s/\\/\//g;
$Modem_split_list = `ls $Modem_split_images_path`;
@array = split(/\s+/, $Modem_split_list);
for $a (@array)
{
system "$cpfatfs_path\\cpfatfs.exe $Hash_Modem_Build\\modem_proc\\build\\ms\\modem.bin image $Hash_Modem_Build\\$SPLIT_MODEM_PATH\\$a";
}
$Q6_split_images_path ="$Hash_Modem_Build\\modem_proc\\build\\ms\\Q6\\";
$Q6_split_images_path =~s/\\/\//g;
$Q6_split_list = `ls $Q6_split_images_path`;
@Q6_SPLIT_IMAGES = split(/\s+/, $Q6_split_list);
for $q6_split_image (@Q6_SPLIT_IMAGES)
{
system "$cpfatfs_path\\cpfatfs.exe $Hash_Modem_Build\\modem_proc\\build\\ms\\modem.bin image $Hash_Modem_Build\\modem_proc\\build\\ms\\Q6\\$q6_split_image";
}
system("copy $cpfatfs_path\\fat.bin $Hash_Modem_Build\\modem_proc\\build\\ms\\Q6\\APQ.bin");
for $q6_split_image (@Q6_SPLIT_IMAGES)
{
system "$cpfatfs_path\\cpfatfs.exe $Hash_Modem_Build\\modem_proc\\build\\ms\\Q6\\APQ.bin image $Hash_Modem_Build\\modem_proc\\build\\ms\\Q6\\$q6_split_image";
}
print_error();

#=============================================================================================
#                                SUB FUNCTION DEFINITIONS
#=============================================================================================
sub error
{
	my ($message) = @_;
	$ERROR =1;
	$cbitoppath=$cbitop.$fn.".txt";
			open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
			$fn++;
	print "\n\U$message";
	print OUTFILE "\n\U$message";

	close(OUTFILE);
	push(@warnings,"\n$message");
	print_error();
	die("");
}

sub print_error
{
$cbitoppath=$cbitop.$fn.".txt";
	open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
	$fn++;
	if($ERROR==0){
	print "\n\n---------------------- COPY OF RELEVANT FILES SUCCESFUL-------------------------------------------- \n";
	print OUTFILE "CHECK THE SIZE OF THE MODEM.BIN FILE GENERATED TO NON-HLOS.BIN AT MODEM BUILD LOCATION\n";
	}elsif($ERROR==1){
	print "\n\n\U---------------------COPY MAY NOT BE PROPER AS IT IS FACING ONE OR MORE FOLDER ACCESS ERROR-----------------------\n";
	print  OUTFILE "CHECK THE SIZE OF THE MODEM.BIN FILE GENERATED TO NON-HLOS.BIN AT MODEM BUILD LOCATION\n";
	for ($i=0;$i<$#warnings+1;$i++)
      		{
			print $warnings[$i];
			print OUTFILE $warnings[$i];
			}
	}
	close(OUTFILE);
}

sub dircopy
{
my ($pathfrom , $pathto , $comment ,$fileextn ,$nametype) = @_;
my (@filename,$k,$j,$i);
 print "pathto $pathto\n";
$valid=1;
opendir (DIR, $pathfrom ) or $valid=0;
if($valid==1)
{
@filename = readdir(DIR);
if($comment eq "fileonly" ){
	if($#_ eq 2) {
		my $k=2;
		#print "\nCopying Dir $pathfrom ...... \n";
		while($k <= $#filename) {
			if(opendir(DIRA, $pathfrom."\/".$filename[$k])) {
			closedir(DIRA);
			}
			else {
			#copy($pathfrom."\/".$filename[$k],$pathto."\/".$filename[$k]);

			$cmd1="xcopy"." ".$pathfrom."\/".$filename[$k]." ".$pathto;

			$cmd1=~ s/\/\//\\/g;
			$cmd1=~ s/\//\\/g;
			$cmd1=$cmd1." /A /I  /Y /Z /Q";
   system($cmd1);

			#print "Copy file $pathfrom $filename[$k] Complete\n";
			#print "Copy file $pathfrom $filename[$k] Complete\n";
			}
		$k=$k+1;
		}
	}

	if($#_ eq 3){
		my $j=2;
		#print "\nCopying Dir $pathfrom ...... \n";
		while($j <= $#filename) {
		    if(opendir(DIRA, $pathfrom."\/".$filename[$j])) {
			closedir(DIRA);
			}
			else {
		    $filename[$j] =~ /(.+)\.(\w+)/;
			my $temp = $2;
			if($temp eq $fileextn){
				#print "Copy file $pathfrom $filename[$j] started\n";
				#copy($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j]);
				if  (-d $pathto)
				{
				}
				else
				{
				mkdir($pathto);
				}
				$cmd1="xcopy"." ".$pathfrom."\/".$filename[$j]." ".$pathto;
				$cmd1=~ s/\/\//\\/g;
				$cmd1=~ s/\//\\/g;
				$cmd1=$cmd1." /A /I  /Y /Z /Q";
    system($cmd1);
				#print "$cmd1\n";
				#print "Copy file $pathfrom $filename[$j] Complete\n";
				#print "Copy file $pathfrom $filename[$j] Complete\n";
				}
			}
		$j=$j+1;
		}
	}
	if($#_ eq 4){
		my $i=2;
		#print "\nCopying Dir $pathfrom ...... \n";
		while($i <= $#filename) {
		    if(opendir(DIRA, $pathfrom."\/".$filename[$i])) {
			closedir(DIRA);
			}
			else {
		    $filename[$i] =~ /(.+)\.(\w+)/;
			my $temp = $2;
			if($temp eq $fileextn && ($filename[$i]=~ /$nametype/)){
				#print "Copy file $pathfrom $filename[$i] started\n";
				#copy($pathfrom."\/".$filename[$i],$pathto."\/".$filename[$i]);
				$cmd1="xcopy"." ".$pathfrom."\/".$filename[$i]." ".$pathto;

				$cmd1=~ s/\/\//\\/g;
				$cmd1=~ s/\//\\/g;
                                $cmd1=$cmd1." /A /I  /Y /Z /Q";
                                system($cmd1);

				#print "Copy file $pathfrom $filename[$i] Complete\n";
				#print "Copy file $pathfrom $filename[$i] Complete\n";
				}
			}
		$i=$i+1;
		}
	}
}

 if($comment eq "folderonly" ){
	if($#_ eq 2) {
		my $j=2;
		#print "\nCopying Dir $pathfrom ...... \n";
		while($j <= $#filename) {
		if(opendir(DIRB, $pathfrom."\/".$filename[$j])) {
		mkpath($pathto."\/".$filename[$j]);
		dircopyfolder($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j]);
		closedir(DIRB);
		}
		$j=$j+1;
		}
	}
	if($#_ eq 4){
		my $i=2;
		#print "\nCopying Dir $pathfrom ...... \n";
		while($i <= $#filename) {
		    if(opendir(DIRB, $pathfrom."\/".$filename[$i]) && ($filename[$i]=~ /$nametype/) ) {
				mkpath($pathto."\/".$filename[$i]);
				dircopyfolder($pathfrom."\/".$filename[$i],$pathto."\/".$filename[$i]);
				closedir(DIRB);
				}
		    $i=$i+1;
			}
		}
	}
 if($comment eq "all" ) {
 if($#_ eq 2) {
    dircopyfolder($pathfrom,$pathto);
	}
 elsif($#_ eq 3){
    dircopyfolder($pathfrom,$pathto,$fileextn);
	}
	}
closedir(DIR);
}
else  {
	push(@warnings,"\nUnable to access folder $pathfrom");
       	$ERROR=1 ;
	}
}
sub dircopyfolder
{
my ($pathfrom,$pathto,$ext) = @_;
my (@filename,$k,$j);
$valid=1;
opendir (DIRNEW, $pathfrom ) or $valid=0;
if($valid==1)
{
@filename = readdir(DIRNEW);

	my $j=2;
	#print "\nCopying Dir $pathfrom ...... \n";
	while($j <= $#filename) {
	if(opendir(DIRSUB, $pathfrom."\/".$filename[$j])) {
		mkpath($pathto."\/".$filename[$j]);
		if($#_ == 1){
			dircopyfolder($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j]);
			}
		elsif($#_ == 2){
			dircopyfolder($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j],$ext);
			}
	closedir(DIRSUB);
	}
	else {
		if($#_ == 1){
		#print "Copy file $pathfrom $filename[$j] started\n";
		#copy($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j]);

			$cmd1="xcopy"." ".$pathfrom."\/".$filename[$j]." ".$pathto;

			$cmd1=~ s/\/\//\\/g;
			$cmd1=~ s/\//\\/g;
			$cmd1=$cmd1." /A /I  /Y /Z /Q";
                        system($cmd1);

			#print "Copy file $pathfrom $filename[$j] Complete\n";
			#print "Copy file $pathfrom $filename[$j] Complete\n";
			}
		elsif($#_ == 2){
		    $filename[$j] =~ /(.+)\.(\w+)/;
			my $temp = $2;
			if($ext eq $temp){
			#print "Copy file $pathfrom $filename[$j] started\n";
			#copy($pathfrom."\/".$filename[$j],$pathto."\/".$filename[$j]);


				$cmd1="xcopy"." ".$pathfrom."\/".$filename[$j]." ".$pathto;
				#print "$cmd1\n";
				$cmd1=~ s/\/\//\\/g;
				$cmd1=~ s/\//\\/g;
$cmd1=$cmd1." /A /I  /Y /Z /Q";
                                system($cmd1);

				#print "Copy file $pathfrom $filename[$j] Complete\n";
				#print "Copy file $pathfrom $filename[$j] Complete\n";
				}
			}
	}
	$j=$j+1;
	}
closedir(DIRNEW);
}
else  {
	push(@warnings,"\nUnable to access folder $pathfrom");
       	$ERROR=1 ;
	}
}

$cbitoppath=$cbitop.$fn.".txt";
			open(OUTFILE,">$cbitoppath")or die "cant open $cbitoppath";
			$fn++;


print OUTFILE ("Copy Over");

close(OUTFILE);


