use File::Copy;
use File::Path;
use Spreadsheet::WriteExcel;
use Spreadsheet::WriteExcel::Workbook;
use Parse::RecDescent;
use CGI ':standard';
use GD::Graph::pie;
$targetName=$ARGV[0];
$PlName=$ARGV[1];
$CompilerUsed=$ARGV[2];
$Outpath=$ARGV[3];
#$OverallImageSizeLatest=$ARGV[1];
#$OverallImageSizePrev=$ARGV[2];
$LatestBuildID=$ARGV[4];
$PreviousBuildID=$ARGV[5];
$arg1_len=@ARGV;
if($arg1_len != 6)
{
print"**********Usage of the script:**********\n";
print"perl <name of the script> <Target Name> <PL Info> <Compiler Used> <OutPath> <Latest ADSP Build> <Previous ADSP Build>\n";
print"Target Name should be any of below:\n";
print"				8994\n";
print"Compiler name should be any of below:\n";
print"				gcc\\llvm\n";
print"Example:\n";
print"perl Image_Break_Down.pl 8994 2.6 c:\Temp ADSP.BF.2.6-00323-M8994AAAAAAAZL-1 ADSP.BF.2.6-00309-M8994AAAAAAAZL-1\n";
print"****************************************\n";
die"Give the correct Arguments";
}
#print "compiler used: $CompilerUsed";
if (($CompilerUsed =~ /gcc/i) || ($CompilerUsed =~ /llvm/i))
{
}
else
{
print"Compiler name should be any of below:\n";
print"				gcc\\llvm\n";
die"";
}
if ($CompilerUsed =~ /gcc/i)
{
$compilerFlag=6;
}
if ($CompilerUsed =~ /llvm/i)
{
$compilerFlag=7;
}
#mkpath ("\\\\harv-ypatnana\\Builds\\Badger_Memory_Analysis\\$PlName\\$targetName",0,0777);
#$pathName="\\\\harv-ypatnana\\Builds\\Badger_Memory_Analysis\\$PlName\\$targetName";
#$Outpath
mkpath ("$Outpath\\$PlName\\$targetName",0,0777);
$pathName="$Outpath\\$PlName\\$targetName";
#$pathName = "../$PlName/$targetName";
#mkdir $pathName,0777;
#die"out";
my $workbook = Spreadsheet::WriteExcel->new("$pathName\\$targetName\_$PlName\_MemoryAnalysis.xls");
my $worksheet1 = $workbook->addworksheet('Summary');
	my $format9 = $workbook->add_format(border => 1);
	my $format9   = $workbook->add_format( bg_color => 'yellow' );
	my $format9 = $workbook->add_format(border => 1);
	
	$format9->set_bold();
	$format9->set_color('Red');
my $format1 = $workbook->add_format(border => 1);	
$format1->set_bold();
$NoDiffFlag=0;

$row_summary=0;
$col_summary = 0;
###$worksheet1->write("$row_summary", $col_summary, "Target", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "LatestADSPBuild", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Size Limt in MB", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Consumed by CommonPD in MB", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Consumed by SensorsPD in MB", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "PreviousADSPBuild", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Size Limt in MB", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Consumed by CommonPD in MB", $format9);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "Memory Consumed by SensorsPD in MB", $format9);
###$col_summary++;


$build;
$path1="";
$path2="";

$t_buildID_1[0]=M8974AAAAAAAZL20;
$t_buildID_1[1]=M9625AAAAANAZL20;
$t_buildID_1[2]=M8626AAAAAAAZL22;
$t_buildID_1[3]=M8610AAAAAAAZL2200;
$targetName_1[0]=8974;
$targetName_1[1]=9625;
$targetName_1[2]=8626;
$targetName_1[3]=8610;
$NumofTargets=@t_buildID_1;
#print"\n$NumofTargets\n";
#die"out";

BiuldIndoRead:
#print"\npramodp\n";
#print("$oldBuildInfo[0]\n");
#print"\npramodp\n";
#close(STATFILE);
#die("out");
##for($overallloop=0;$overallloop<$NumofTargets;$overallloop++)
##{
#print"\n@oldBuildInfo\n";
#print("In overloop:$oldBuildInfo[0]\n");
#print("In overloop:$oldBuildInfo[$overallloop]\n");
$t_buildID=$t_buildID_1[$overallloop];
#print"\nBuildID is:$t_buildID\n";
#$targetName=$targetName_1[$overallloop];
#print"\nTarget Name is:$targetName\n";
$t_flag_1=0;	# Flags to enable/Disable sync+build+testcase
$t_flag_2=0;	# Flags to enable/Disable sync+build+testcase

#while(1)
#{
undef @latestBuildName;
undef @StatusflagName;
undef @BuildPath;
	system("\\\\sun\\CRMTools\\Smart\\nt\\bin\\FindBuild\.exe $LatestBuildID > $pathName\\build_info_latest\.txt");
	system("\\\\sun\\CRMTools\\Smart\\nt\\bin\\FindBuild\.exe $PreviousBuildID > $pathName\\build_info_previous\.txt");
	#print"\nBuild ID File: build_info_$t_buildID\.txt\n";
	if (open(BUILDINFO,"$pathName\\build_info_latest\.txt")){
		$buildinfoline=<BUILDINFO>;
		while ( $buildinfoline ne "" ) {
			if($buildinfoline =~ /^Build:\s+(.+)/)
			{
			    $Latestbuildname=$1;
				push(@latestBuildName,$buildname);
			}
			if($buildinfoline =~ /^Status:\s+(.+)/)
			{
			    $statusflag=$1;
				push(@StatusflagName,$statusflag);
			}
			if($buildinfoline =~ /^Location:\s+(.+)/)
			{
			    $LatestLocation=$1;
				push(@BuildPath,$Location);
			}
			$buildinfoline=<BUILDINFO>;			
		}
	}
	else
	{
	die"Not able to open the file";
	}
	if (open(BUILDINFO,"$pathName\\build_info_previous\.txt")){
		$buildinfoline=<BUILDINFO>;
		while ( $buildinfoline ne "" ) {
			if($buildinfoline =~ /^Build:\s+(.+)/)
			{
			    $Previousbuildname=$1;
				push(@latestBuildName,$buildname);
			}
			if($buildinfoline =~ /^Status:\s+(.+)/)
			{
			    $statusflag=$1;
				push(@StatusflagName,$statusflag);
			}
			if($buildinfoline =~ /^Location:\s+(.+)/)
			{
			    $PreviousLocation=$1;
				push(@BuildPath,$Location);
			}
			$buildinfoline=<BUILDINFO>;			
		}
	}
	else
	{
	die"Not able to open the file";
	}	

system("python Image_Break_Down.py $targetName $LatestLocation\\adsp_proc\\build\\bsp\\adsp\_link\\build\\AAAAAAAA\\ADSP\_PROC\_IMG_AAAAAAAAQ\.elf\.map CommonPD $compilerFlag $LatestLocation Latest $pathName");
if(($targetName == 8974) || ($targetName == 8626) || ($targetName == 8994))
{
system("python Image_Break_Down.py $targetName $LatestLocation\\adsp_proc\\build\\bsp\\sensor\_img\\build\\AAAAAAAA\\SENSOR\_IMG_AAAAAAAAQ\.elf\.map SensorsPD $compilerFlag $LatestLocation Sensors_Latest $pathName");
}
system("python Image_Break_Down.py $targetName $PreviousLocation\\adsp_proc\\build\\bsp\\adsp\_link\\build\\AAAAAAAA\\ADSP\_PROC\_IMG_AAAAAAAAQ\.elf\.map CommonPD $compilerFlag $PreviousLocation Prev $pathName");	
if(($targetName == 8974) || ($targetName == 8626) || ($targetName == 8994))
{
system("python Image_Break_Down.py $targetName $PreviousLocation\\adsp_proc\\build\\bsp\\sensor\_img\\build\\AAAAAAAA\\SENSOR\_IMG_AAAAAAAAQ\.elf\.map SensorsPD $compilerFlag $PreviousLocation Sensors_Prev $pathName");
}

	if($targetName == 9625)
	{
		$BuildTargetName = "mdm9x25";
	}
	elsif($targetName == 8974)
	{
		$BuildTargetName = "msm8974";
	}
	elsif($targetName == 8626)
	{
		$BuildTargetName = "msm8x26";
	}
	elsif($targetName == 8610)
	{
		$BuildTargetName = "msm8x10";
	}
		elsif($targetName == 8994)
	{
		$BuildTargetName = "msm8994";
	}

	$src_file = "$LatestLocation\\adsp_proc\\build\\chipset\\$BuildTargetName\\qdsp6.xml";
   open src_file or die "Cannot open file $src_file\n";

   #print " 1. Trying to get the DEFAULT_PHYSPOOL start address and size from $src_file\n";
   $lineCnt=0;
   $def_phy_pool_line_offset=-1;

   while(<src_file>)
   {
      if ($_ =~ /DEFAULT_PHYSPOOL/){
      
         #print "default phy pool section line num :$lineCnt\n";
         $def_phy_pool_line_offset=0;
        
      }
      if($def_phy_pool_line_offset != -1){
         $def_phy_pool_line_offset++;
         if($def_phy_pool_line_offset ==3){
            
            #get the start address     
            #print "line:$_\n";
            $_ =~ s/ //g;
            @line_array = split('"',$_);
         #   print "linearray = @line_array \n";
            #@line_array1_1 =split('{',$line_array[0]);
            #@line_array1_2 =split(',',$line_array1_1[1]);
 # print "linearray0 = $line_array[0] \n";

  #print "linearray1 = $line_array[1] \n";
  #print "linearray2 = $line_array[2] \n";
  #print "linearray3 = $line_array[3] \n";
            $Hex_start_addr_from_file =$line_array[1];
            $Dec_start_addr_from_file=hex($Hex_start_addr_from_file);
            #get the size
            #@line_array2_1 =split('}',$line_array[1]);
            $Hex_img_size_from_file =$line_array[3];
            $Dec_img_size_from_file=hex($Hex_img_size_from_file);
            
         }
      }
      $lineCnt++;
   }
   if (!$Dec_img_size_from_file ) {
      #print "the image size not found in $src_file\n";
      exit;
   }else{
      #print " ,Image size defined = $Hex_img_size_from_file\n";
	  $dec_image_size_from_file_latest=hex($Hex_img_size_from_file);  
	  $dec_image_size_from_file_latest=$dec_image_size_from_file_latest/(1024*1024);
   }
   close $src_file;
   #print("\nnice:$oldBuildInfo[$overallloop]\n");
   #die"out";
   #print"\n$oldBuildPath\n";
   #$anotherpath="adsp_proc\\build\\bsp\\adsp\_link\\build\\AAAAAAAA\\ADSP\_PROC\_IMG_AAAAAAAAQ\.elf\.map";
   #$oldBuildPath=$oldBuildInfo[$overallloop]\\$anotherpath;
   $OldBuildPath=$oldBuildInfo[$overallloop];
   #print"\n$oldBuildPath\n";
	
	$src_file = "$PreviousLocation\\adsp_proc\\build\\chipset\\$BuildTargetName\\qdsp6.xml";
	#$src_file=$first_xml_file.$second_xml_file;
   open src_file or die "Cannot open file $src_file\n";

   #print " 1. Trying to get the DEFAULT_PHYSPOOL start address and size from $src_file\n";
   $lineCnt=0;
   $def_phy_pool_line_offset=-1;

   while(<src_file>)
   {
      if ($_ =~ /DEFAULT_PHYSPOOL/){
      
         #print "default phy pool section line num :$lineCnt\n";
         $def_phy_pool_line_offset=0;
        
      }
      if($def_phy_pool_line_offset != -1){
         $def_phy_pool_line_offset++;
         if($def_phy_pool_line_offset ==3){
            
            #get the start address     
            #print "line:$_\n";
            $_ =~ s/ //g;
            @line_array = split('"',$_);
         #   print "linearray = @line_array \n";
            #@line_array1_1 =split('{',$line_array[0]);
            #@line_array1_2 =split(',',$line_array1_1[1]);
 # print "linearray0 = $line_array[0] \n";

  #print "linearray1 = $line_array[1] \n";
  #print "linearray2 = $line_array[2] \n";
  #print "linearray3 = $line_array[3] \n";
            $Hex_start_addr_from_file =$line_array[1];
            $Dec_start_addr_from_file=hex($Hex_start_addr_from_file);
            #get the size
            #@line_array2_1 =split('}',$line_array[1]);
            $Hex_img_size_from_file =$line_array[3];
            $Dec_img_size_from_file=hex($Hex_img_size_from_file);
            
         }
      }
      $lineCnt++;
   }
   if (!$Dec_img_size_from_file ) {
      #print "the image size not found in $src_file\n";
      exit;
   }else{
      #print " ,Image size defined = $Hex_img_size_from_file\n";
	  $dec_image_size_from_file_prev=hex($Hex_img_size_from_file);   
	  $dec_image_size_from_file_prev=$dec_image_size_from_file_prev/(1024*1024);
   }
   close $src_file;

	#system("perl Mem_Cmp_fin.pl $targetName $dec_image_size_from_file_latest $dec_image_size_from_file_prev");
    #system("rename Memory\_Info\_8974\.xls Memory\_Info\_8974\_Latest\.xls");	
	#print"\n$buildInfoLen\n";
	#print"\n$statusflagLen\n";
	#print"\n@latestBuildName\n";
	#print"\n@StatusflagName\n";
#if($targetName == 8974)
#{
##################################################################################################################################################	use Spreadsheet::WriteExcel;
print MEMDIFF ("Target \t LatestADSPSizeLimt \t LatestADSPSizeConsumed \t PrevADSPSizeLimt \t PrevADSPSizeConsumed \t FreeSpace\n");
print MEMDIFF ("$targetName \t $OverallImageSizeLatest \t $overallpasssize_curr \t $OverallImageSizeLatest \t $overallpasssize_prev \t $overallpasssizediff\n");

my $worksheet2 = $workbook->addworksheet("Memory_Analysis_$targetName\_Common_PD");
	my $format3 = $workbook->add_format(border => 1);
	my $format3   = $workbook->add_format( bg_color => 'yellow' );
	$format3->set_bold();
	$format3->set_color('Red');
my $format4 = $workbook->add_format(border => 1);	
$format4->set_bold();
#my $workbook = Spreadsheet::WriteExcel->new("MemDiff.xls");



###################################################################################################################################################################
$accumText=0;
$accumRodata=0;
$accumData=0;
$accumSdata=0;
$accumSbss=0;
$accumBss=0;
#print"\npramodp\n";
$comp_entry_flag = 0;
$comp_entry2_flag = 0;

#open(MEMDIFF,">Memory_diff_$targetName\.xls");
#print"\npramodp\n";
#die"out";
undef @Funcs_in_Both;
undef @TextDiff;
undef @RodataDiff;
undef @DataDiff;
undef @SdataDiff;
undef @SbssDiff;
undef @BssDiff;
undef @FuncAadded;
undef @TextExtra;
undef @RodataExtra;
undef @DataExtra;
undef @SdataExtra;
undef @SbssExtra;
undef @BssExtra;
undef @FuncDel;
undef @TextDel;
undef @RodataDel;
undef @DataDel;
undef @SdataDel;
undef @SbssDel;
undef @BssDel;
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Latest.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
#die"out";
while ( $memgateline ne "" ) {
if(($memgateline =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry_flag == 1))
{
$comp_entry_flag = 1;
#print"\nentry to component allowed\n";
#die"out";
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName=$1;
#print"$funcName";
#die"out";
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$comp_entry2_flag = 0;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry2_flag == 1))
{
$comp_entry2_flag = 1;
#print"\nentry to component2 allowed\n";
#die"out";
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName1=$1;
#print"\n$funcName1\n";
#die"out";
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
if($funcName eq $funcName1)
{
#print"\nEqual:$funcName1\n";
$diffText=($textName-$textName1);
#print"\n$textName : $textName1\n";
$diffRodata=($rodataName-$rodataName1);
$diffData=($dataName-$dataName1);
$diffSdata=($sdataName-$sdataName1);
$diffSbss=($sbssName-$sbssName1);
$diffBss=($bssName-$bssName1);
if(($diffText != 0) || ($diffRodata != 0) || ($diffData != 0) ||($diffSdata != 0) ||($diffSbss != 0) ||($diffBss != 0))
{
$accumText+=$diffText;
$accumRodata+=$diffRodata;
$accumData+=$diffData;
$accumSdata+=$diffSdata;
$accumSbss+=$diffSbss;
$accumBss+=$diffBss;
push(@Funcs_in_Both,$funcName1);
push(@TextDiff,$diffText);
push(@ComponentNameFin,$funcName1);
push(@CompLatestTextSize,$textName);
push(@CompPrevTextSize,$textName1); 
$CompLatestOADataSize=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
$CompPrevOADataSize=$rodataName1+$dataName1+$sdataName1+$sbssName1+$bssName1;
push(@CompLatestDataSize,$CompLatestOADataSize);
push(@CompPrevDataSize,$CompPrevOADataSize);
push(@RodataDiff,$diffRodata);
push(@DataDiff,$diffData);
push(@SdataDiff,$diffSdata);
push(@SbssDiff,$diffSbss);
push(@BssDiff,$diffBss);
}
goto lab123;
}
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"Out:Previous File";
}
push(@FuncAadded,$funcName);
#print"\nAdded:$funcName\n";
push(@TextExtra,$textName);
push(@RodataExtra,$rodataName);
push(@DataExtra,$dataName);
push(@SdataExtra,$sdataName);
push(@SbssExtra,$sbssName);
push(@BssExtra,$bssName);
push(@ComponentNameFin,$funcName);
push(@CompLatestTextSize,$textName);
$textName1=0;
push(@CompPrevTextSize,$textName1); 
$CompLatestOADataSize=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
push(@CompLatestDataSize,$CompLatestOADataSize);
$CompPrevOADataSize=0;
push(@CompPrevDataSize,$CompPrevOADataSize);
lab123:
#####################################################################
}
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:No latest file";
}
close(MEMFILE);
close(MEMFILE1);
#die"out";
$comp_entry_flag = 0;
$comp_entry2_flag = 0;
#################################################################################################
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
#die"out";
while ( $memgateline ne "" ) {
if(($memgateline =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry_flag == 1))
{
$comp_entry_flag = 1;
#print"\nentry to component allowed\n";
#die"out";
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Latest.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry2_flag == 1))
{
$comp_entry2_flag = 1;
#print"\nentry to component2 allowed\n";
#die"out";
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName1=$1;
#print"\n$funcName1\n";
#die"out";
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
if($funcName eq $funcName1)
{

goto lab231;
}
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"Out:Previous File";
}
push(@FuncDel,$funcName);
push(@TextDel,$textName);
push(@RodataDel,$rodataName);
push(@DataDel,$dataName);
push(@SdataDel,$sdataName);
push(@SbssDel,$sbssName);
push(@BssDel,$bssName);
push(@ComponentNameFin,$funcName);
push(@CompPrevTextSize,$textName);
$textName1=0;
push(@CompLatestTextSize,$textName1); 
$CompPrevOADataSize=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
push(@CompPrevDataSize,$CompPrevOADataSize);
$CompLatestOADataSize=0;
push(@CompLatestDataSize,$CompLatestOADataSize);

lab231:
#####################################################################
}
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:No latest file";
}
close(MEMFILE);
close(MEMFILE1);
############################################################################################3

print MEMDIFF ("Componenets that differ in Memory:\n");
$equalLen=@Funcs_in_Both;
print MEMDIFF ("Component Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
#################################################3
$col_target=0;
$row_target=0;
$worksheet2->write("$row_target", $col_target, "Components that differ in Memory", $format3);
$col_target++;
$row_target++;
$col_target=0;
$worksheet2->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
#die"out";
#print"\n$row_target\n";
for($i=0;$i<$equalLen;$i++)
{
$row_target++;
$col_target=0;
$BothFunName=$Funcs_in_Both[$i];
$worksheet2->write("$row_target", $col_target, "$BothFunName", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssDiff[$i]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDiff[$i]+$RodataDiff[$i]+$DataDiff[$i]+$SdataDiff[$i]+$SbssDiff[$i]+$BssDiff[$i];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\t$CompDiff[$i]\n");
}
#print"\n$row_target\n";
#print"\npramod\n";
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet2->write("$row_target", 0, "Components that are added:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet2->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
for($j=0;$j<$addLen;$j++)
{
$row_target++;
$col_target=0;
$worksheet2->write("$row_target", $col_target, "$FuncAadded[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssExtra[$j]", $format4);
$col_target++;
$Total_Comp_Mem=$TextExtra[$j]+$RodataExtra[$j]+$DataExtra[$j]+$SdataExtra[$j]+$SbssExtra[$j]+$BssExtra[$j];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\t$CompExtra[$j]\n");
}
$row_target++;
$row_target++;
$row_target++;
$worksheet2->write("$row_target", 0, "Components that are Deleted:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet2->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
for($k=0;$k<$delLen;$k++)
{
$row_target++;
$col_target=0;
$worksheet2->write("$row_target", $col_target, "$FuncDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssDel[$k]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDel[$k]+$RodataDel[$k]+$DataDel[$k]+$SdataDel[$k]+$SbssDel[$k]+$BssDel[$k];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\t$CompDel[$k]\n");
}
$row_target++;
$row_target++;
$row_target++;

####################################################3
for($i=0;$i<$equalLen;$i++)
{
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\n");
}
print MEMDIFF ("\nComponents that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
for($j=0;$j<$addLen;$j++)
{
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\n");
}
print MEMDIFF ("\nComponents that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
for($k=0;$k<$delLen;$k++)
{
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\n");
}

#die"end";


####################################################################################################################################################################


$accumText=0;
$accumRodata=0;
$accumData=0;
$accumSdata=0;
$accumSbss=0;
$accumBss=0;
#$targetName=$ARGV[0];
undef @Funcs_in_Both;
undef @TextDiff;
undef @RodataDiff;
undef @DataDiff;
undef @SdataDiff;
undef @SbssDiff;
undef @BssDiff;
undef @CompDiff;
undef @FuncAadded;
undef @TextExtra;
undef @RodataExtra;
undef @DataExtra;
undef @SdataExtra;
undef @SbssExtra;
undef @BssExtra;
undef @CompExtra;
undef @FuncDel;
undef @TextDel;
undef @RodataDel;
undef @DataDel;
undef @SdataDel;
undef @SbssDel;
undef @BssDel;
undef @CompDel;
$OverallImageSizeLatest=$ARGV[1];
$OverallImageSizePrev=$ARGV[2];




#open(MEMDIFF,">Memory_diff_$targetName\.xls");
$Latest_Mem_File="$pathName\\Memory_Info\_$targetName\_Latest.csv";
#print"\n$Latest_Mem_File\n";
if (open(MEMDIFF12,"$Latest_Mem_File")){
$memgateline12=<MEMDIFF12>;
$memgateline12=<MEMDIFF12>;
while ( $memgateline12 ne "" ) {
#Holes btw the sections in KB:,35.47
if($memgateline12 =~ /Holes btw the sections in KB\:,(.+)/)
{
$holename="Hole";
$holeSize=$1;
#print"\n$holename\:$holeSize\n";
push(@SecNameLatest,$holename);
push(@SecSizeLatest,$holeSize);
}
#The Size of .ukernel.island in KB:,25.85
if($memgateline12 =~ /The Size of (.+) in KB\:\,(.+)/)
{
$SectionNam=$1;
$SecionSiz=$2;
if (($SectionNam eq ".text") || ($SectionNam eq ".rodata") || ($SectionNam eq ".data") || ($SectionNam eq ".bss") || ($SectionNam eq ".sdata"))
{
push(@FinSecname, $SectionNam);
push(@FinSecsize, $SecionSiz);
}
#print"\n$SectionNam\:$SecionSiz\n";
push(@SecNameLatest,$SectionNam);
push(@SecSizeLatest,$SecionSiz);
#$startSize=$1;
}
$memgateline12=<MEMDIFF12>;
}
}
else
{
die"Not able to open the file";
}
close(MEMDIFF12);

if (open(MEMDIFF22,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline22=<MEMDIFF22>;
$memgateline22=<MEMDIFF22>;
while ( $memgateline22 ne "" ) {
if($memgateline22 =~ /Holes btw the sections in KB\:,(.+)/)
{
$holename="Hole";
$holeSize=$1;
#print"\n$holename\:$holeSize\n";
push(@SecNamePrev,$holename);
push(@SecSizePrev,$holeSize);
}
#The Size of .ukernel.island in KB:,25.85
if($memgateline22 =~ /The Size of (.+) in KB\:\,(.+)/)
{
$SectionNam=$1;
$SecionSiz=$2;
if (($SectionNam eq ".text") || ($SectionNam eq ".rodata") || ($SectionNam eq ".data") || ($SectionNam eq ".bss") || ($SectionNam eq ".sdata"))
{
push(@PrevFinSecname, $SectionNam);
push(@PrevFinSecsize, $SecionSiz);
}
#print"\n$SectionNam\:$SecionSiz\n";
push(@SecNamePrev,$SectionNam);
push(@SecSizePrev,$SecionSiz);
#$startSize=$1;
}

$memgateline22=<MEMDIFF22>;
}
}
else
{
die"Not able to open the file";
}
close(MEMDIFF12);
close(MEMDIFF22);
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Latest.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
while ( $memgateline ne "" ) {
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$componentName=$9;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName1=$1;
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
$componentName1=$9;
if($funcName eq $funcName1)
{
$diffText=($textName-$textName1);
$diffRodata=($rodataName-$rodataName1);
$diffData=($dataName-$dataName1);
$diffSdata=($sdataName-$sdataName1);
$diffSbss=($sbssName-$sbssName1);
$diffBss=($bssName-$bssName1);
if(($diffText != 0) || ($diffRodata != 0) || ($diffData != 0) ||($diffSdata != 0) ||($diffSbss != 0) ||($diffBss != 0))
{
$accumText+=$diffText;
$accumRodata+=$diffRodata;
$accumData+=$diffData;
$accumSdata+=$diffSdata;
$accumSbss+=$diffSbss;
$accumBss+=$diffBss;
push(@Funcs_in_Both,$funcName1);
push(@TextDiff,$diffText);
push(@RodataDiff,$diffRodata);
push(@DataDiff,$diffData);
push(@SdataDiff,$diffSdata);
push(@SbssDiff,$diffSbss);
push(@BssDiff,$diffBss);
push(@CompDiff,$componentName1);
}
goto lab123;
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"out:Not able to open the file";
}
push(@FuncAadded,$funcName);
push(@TextExtra,$textName);
push(@RodataExtra,$rodataName);
push(@DataExtra,$dataName);
push(@SdataExtra,$sdataName);
push(@SbssExtra,$sbssName);
push(@BssExtra,$bssName);
push(@CompExtra,$componentName);
lab123:
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:Not able to open the file";
}
close(MEMFILE);
close(MEMFILE1);
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
while ( $memgateline ne "" ) {
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$componentName=$9;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Latest.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName1=$1;
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
$componentName1=$9;
if($funcName eq $funcName1)
{
goto lab231;
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"out:Not able to open the file";
}
push(@FuncDel,$funcName);
push(@TextDel,$textName);
push(@RodataDel,$rodataName);
push(@DataDel,$dataName);
push(@SdataDel,$sdataName);
push(@SbssDel,$sbssName);
push(@BssDel,$bssName);
push(@CompDel,$componentName);
lab231:
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:Not able to open the file";
}
print MEMDIFF ("Libs that differ in Memory:\n");
$equalLen=@Funcs_in_Both;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
#$row_target=0;
$worksheet2->write("$row_target", $col_target, "Libs that differ in Memory", $format3);
$col_target=0;
$row_target++;
$worksheet2->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
#die"out";
#print"\n$row_target\n";
for($i=0;$i<$equalLen;$i++)
{
$row_target++;
$col_target=0;
$BothFunName=$Funcs_in_Both[$i];
$worksheet2->write("$row_target", $col_target, "$BothFunName", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssDiff[$i]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssDiff[$i]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDiff[$i]+$RodataDiff[$i]+$DataDiff[$i]+$SdataDiff[$i]+$SbssDiff[$i]+$BssDiff[$i];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$CompDiff[$i]", $format4);
$col_target++;
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\t$CompDiff[$i]\n");
}
#print"\n$row_target\n";
#print"\npramod\n";
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet2->write("$row_target", 0, "Libs that are added:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet2->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
for($j=0;$j<$addLen;$j++)
{
$row_target++;
$col_target=0;
$worksheet2->write("$row_target", $col_target, "$FuncAadded[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssExtra[$j]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssExtra[$j]", $format4);
$col_target++;
$Total_Comp_Mem=$TextExtra[$j]+$RodataExtra[$j]+$DataExtra[$j]+$SdataExtra[$j]+$SbssExtra[$j]+$BssExtra[$j];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$CompExtra[$j]", $format4);
$col_target++;
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\t$CompExtra[$j]\n");
}
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet2->write("$row_target", 0, "Libs that are Deleted:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
#die"out:456";
$col_target=0;
$worksheet2->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet2->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
#die"out:123";
for($k=0;$k<$delLen;$k++)
{
$row_target++;
$col_target=0;
$worksheet2->write("$row_target", $col_target, "$FuncDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$TextDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$RodataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$DataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SdataDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$SbssDel[$k]", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$BssDel[$k]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDel[$k]+$RodataDel[$k]+$DataDel[$k]+$SdataDel[$k]+$SbssDel[$k]+$BssDel[$k];
$worksheet2->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet2->write("$row_target", $col_target, "$CompDel[$k]", $format4);
$col_target++;
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\t$CompDel[$k]\n");
}
$row_target++;
$row_target++;
$row_target++;
#die"out";
#push(@SecNameLatest,$SectionNam);
#push(@SecSizeLatest,$SecionSiz);
$SecLatestLen=@SecNameLatest;
$SecPrevLen=@SecNamePrev;
#push(@SecNamePrev,$SectionNam);
#push(@SecSizePrev,$SecionSiz);
for($ii=0;$ii<$SecLatestLen;$ii++)
{
$eqflag=0;
for($iii=0;$iii<$SecPrevLen;$iii++)
{
if($SecNameLatest[$ii] eq $SecNamePrev[$iii])
	{
		$worksheet2->write("$row_target", 0, "Memory size diff in $SecNameLatest[$ii] sections:", $format4);
		$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet2->write("$row_target", 1, "$SectionSize", $format4);
        $row_target++;
		$eqflag = 1;
	}
}
if($eqflag == 0)
{
push(@SectionAdded,$SecNameLatest[$ii]);
push(@SectionAddedSize,$SecSizeLatest[$ii]);
}
}

for($ii=0;$ii<$SecPrevLen;$ii++)
{
$eqflag=0;
for($iii=0;$iii<$SecLatestLen;$iii++)
{
if($SecNamePrev[$ii] eq $SecNameLatest[$iii])
	{
		#$worksheet2->write("$row_target", 0, "Memory size diff in $SecNameLatest[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		#$worksheet2->write("$row_target", 1, "$SectionSize", $format4);
        #$row_target++;
		$eqflag = 1;
	}
}
if($eqflag == 0)
{
push(@Sectiondeleted,$SecNamePrev[$ii]);
push(@SectiondeletedSize,$SecSizePrev[$ii]);
}
}
$SectionaddLen=@SectionAdded;
$SectiondelLen=@Sectiondeleted;
if($SectionaddLen != 0)
{
$row_target++;
$worksheet2->write("$row_target", 0, "Sections Added:", $format4);
$row_target++;
$row_target++;
for($ii=0;$ii<$SectionaddLen;$ii++)
{
		$worksheet2->write("$row_target", 0, "Memory size of added $SectionAdded[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet2->write("$row_target", 1, "$SectionAddedSize[$ii]", $format4);
        $row_target++;
}
}
if($SectiondelLen != 0)
{
$row_target++;
$worksheet2->write("$row_target", 0, "Sections Deleted:", $format4);
$row_target++;
$row_target++;
for($ii=0;$ii<$SectiondelLen;$ii++)
{
		$worksheet2->write("$row_target", 0, "Memory size of deleted $Sectiondeleted[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet2->write("$row_target", 1, "$SectiondeletedSize[$ii]", $format4);
        $row_target++;
}
}
#die"end";
print MEMDIFF ("\nMemory diff in Hole size  due to Align btw the sections in KB:\t$holeSizediff");
################################################################################################################
##################################################################################################################################################	use Spreadsheet::WriteExcel;
print MEMDIFF ("Target \t LatestADSPSizeLimt \t LatestADSPSizeConsumed \t PrevADSPSizeLimt \t PrevADSPSizeConsumed \t FreeSpace\n");
print MEMDIFF ("$targetName \t $OverallImageSizeLatest \t $overallpasssize_curr \t $OverallImageSizeLatest \t $overallpasssize_prev \t $overallpasssizediff\n");
if(($targetName == 8974) || ($targetName == 8626) || ($targetName == 8994))
{
#print"This excel is for sensors\n";
$worksheet3 = $workbook->addworksheet("Memory_Analysis_$targetName\_SensorsPD");
	my $format3 = $workbook->add_format(border => 1);
	my $format3   = $workbook->add_format( bg_color => 'yellow' );
	$format3->set_bold();
	$format3->set_color('Red');
$format4 = $workbook->add_format(border => 1);	
$format4->set_bold();
#my $workbook = Spreadsheet::WriteExcel->new("MemDiff.xls");



###################################################################################################################################################################
$accumText=0;
$accumRodata=0;
$accumData=0;
$accumSdata=0;
$accumSbss=0;
$accumBss=0;
#print"\npramodp\n";
$comp_entry_flag = 0;
$comp_entry2_flag = 0;
}
if(($targetName == 8974) || ($targetName == 8626) || ($targetName == 8994))
{
#open(MEMDIFF,">Memory_diff_$targetName\.xls");
#print"\npramodp\n";
#die"out";
undef @Funcs_in_Both;
undef @TextDiff;
undef @RodataDiff;
undef @DataDiff;
undef @SdataDiff;
undef @SbssDiff;
undef @BssDiff;
undef @FuncAadded;
undef @TextExtra;
undef @RodataExtra;
undef @DataExtra;
undef @SdataExtra;
undef @SbssExtra;
undef @BssExtra;
undef @FuncDel;
undef @TextDel;
undef @RodataDel;
undef @DataDel;
undef @SdataDel;
undef @SbssDel;
undef @BssDel;
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Sensors_Latest\.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
#die"out";
while ( $memgateline ne "" ) {
if(($memgateline =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry_flag == 1))
{
$comp_entry_flag = 1;
#print"\nentry to component allowed\n";
#die"out";
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{

$funcName=$1;
#print"\n$funcName\n";
#print"$memgateline";
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$comp_entry2_flag = 0;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Sensors_Prev\.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry2_flag == 1))
{
$comp_entry2_flag = 1;
#print"\nentry to component2 allowed\n";
#die"out";
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName1=$1;
#print"\n$funcName1\n";
#die"out";
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
if($funcName eq $funcName1)
{
#print"\nEqual:$funcName1\n";
$diffText=($textName-$textName1);
#print"\n$textName : $textName1\n";
$diffRodata=($rodataName-$rodataName1);
$diffData=($dataName-$dataName1);
$diffSdata=($sdataName-$sdataName1);
$diffSbss=($sbssName-$sbssName1);
$diffBss=($bssName-$bssName1);
if(($diffText != 0) || ($diffRodata != 0) || ($diffData != 0) ||($diffSdata != 0) ||($diffSbss != 0) ||($diffBss != 0))
{
$accumText+=$diffText;
$accumRodata+=$diffRodata;
$accumData+=$diffData;
$accumSdata+=$diffSdata;
$accumSbss+=$diffSbss;
$accumBss+=$diffBss;
push(@Funcs_in_Both,$funcName1);
push(@TextDiff,$diffText);
push(@ComponentNameFin1,$funcName1);
push(@CompLatestTextSize1,$textName);
push(@CompPrevTextSize1,$textName1); 
$CompLatestOADataSize1=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
$CompPrevOADataSize1=$rodataName1+$dataName1+$sdataName1+$sbssName1+$bssName1;
push(@CompLatestDataSize1,$CompLatestOADataSize1);
push(@CompPrevDataSize1,$CompPrevOADataSize1);
push(@RodataDiff,$diffRodata);
push(@DataDiff,$diffData);
push(@SdataDiff,$diffSdata);
push(@SbssDiff,$diffSbss);
push(@BssDiff,$diffBss);
}
goto lab123;
}
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"Out:Previous File";
}
push(@FuncAadded,$funcName);
#print"\nAdded:$funcName\n";
push(@TextExtra,$textName);
push(@RodataExtra,$rodataName);
push(@DataExtra,$dataName);
push(@SdataExtra,$sdataName);
push(@SbssExtra,$sbssName);
push(@BssExtra,$bssName);
push(@ComponentNameFin1,$funcName);
push(@CompLatestTextSize1,$textName);
$textName1=0;
push(@CompPrevTextSize1,$textName1); 
$CompLatestOADataSize1=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
push(@CompLatestDataSize1,$CompLatestOADataSize1);
$CompPrevOADataSize1=0;
push(@CompPrevDataSize1,$CompPrevOADataSize1);

lab123:
#####################################################################
}
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:No latest file";
}
close(MEMFILE);
close(MEMFILE1);
#die"out";
$comp_entry_flag = 0;
$comp_entry2_flag = 0;
#################################################################################################
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Sensors_Prev.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
#die"out";
while ( $memgateline ne "" ) {
if(($memgateline =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry_flag == 1))
{
$comp_entry_flag = 1;
#print"\nentry to component allowed\n";
#die"out";
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Sensors_Latest.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /Component Name\,\.text\,\.rodata\,\.data\,\.sdata\,\.sbss\,\.bss\,Total Size KB/) || ($comp_entry2_flag == 1))
{
$comp_entry2_flag = 1;
#print"\nentry to component2 allowed\n";
#die"out";
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)/))
{
$funcName1=$1;
#print"\n$funcName1\n";
#die"out";
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
if($funcName eq $funcName1)
{

goto lab231;
}
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"Out:Previous File";
}
push(@FuncDel,$funcName);
push(@TextDel,$textName);
push(@RodataDel,$rodataName);
push(@DataDel,$dataName);
push(@SdataDel,$sdataName);
push(@SbssDel,$sbssName);
push(@BssDel,$bssName);
push(@ComponentNameFin1,$funcName);
push(@CompPrevTextSize1,$textName);
$textName1=0;
push(@CompLatestTextSize1,$textName1); 
$CompPrevOADataSize1=$rodataName+$dataName+$sdataName+$sbssName+$bssName;
push(@CompPrevDataSize1,$CompPrevOADataSize1);
$CompLatestOADataSize1=0;
push(@CompLatestDataSize1,$CompLatestOADataSize1);

lab231:
#####################################################################
}
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:No latest file";
}
close(MEMFILE);
close(MEMFILE1);
############################################################################################3

print MEMDIFF ("Componenets that differ in Memory:\n");
$equalLen=@Funcs_in_Both;
print MEMDIFF ("Component Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
#################################################3
$col_target=0;
$row_target=0;
#print"$row_target";
$worksheet3->write("$row_target", "$col_target", "Components that differ in Memory", $format3);
$col_target++;
$row_target++;
$col_target=0;
#print"pramod";
#die"out";
$worksheet3->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
#die"out";
#print"\n$row_target\n";
for($i=0;$i<$equalLen;$i++)
{
$row_target++;
$col_target=0;
$BothFunName=$Funcs_in_Both[$i];
$worksheet3->write("$row_target", $col_target, "$BothFunName", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssDiff[$i]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDiff[$i]+$RodataDiff[$i]+$DataDiff[$i]+$SdataDiff[$i]+$SbssDiff[$i]+$BssDiff[$i];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\t$CompDiff[$i]\n");
}
#print"\n$row_target\n";
#print"\npramod\n";
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet3->write("$row_target", 0, "Components that are added:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet3->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
for($j=0;$j<$addLen;$j++)
{
$row_target++;
$col_target=0;
$worksheet3->write("$row_target", $col_target, "$FuncAadded[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssExtra[$j]", $format4);
$col_target++;
$Total_Comp_Mem=$TextExtra[$j]+$RodataExtra[$j]+$DataExtra[$j]+$SdataExtra[$j]+$SbssExtra[$j]+$BssExtra[$j];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\t$CompExtra[$j]\n");
}
$row_target++;
$row_target++;
$row_target++;
$worksheet3->write("$row_target", 0, "Components that are Deleted:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet3->write("$row_target", $col_target, "Component Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
for($k=0;$k<$delLen;$k++)
{
$row_target++;
$col_target=0;
$worksheet3->write("$row_target", $col_target, "$FuncDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssDel[$k]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDel[$k]+$RodataDel[$k]+$DataDel[$k]+$SdataDel[$k]+$SbssDel[$k]+$BssDel[$k];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\t$CompDel[$k]\n");
}
$row_target++;
$row_target++;
$row_target++;

####################################################3
for($i=0;$i<$equalLen;$i++)
{
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\n");
}
print MEMDIFF ("\nComponents that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
for($j=0;$j<$addLen;$j++)
{
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\n");
}
print MEMDIFF ("\nComponents that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss\n");
for($k=0;$k<$delLen;$k++)
{
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\n");
}

#die"end";


####################################################################################################################################################################


$accumText=0;
$accumRodata=0;
$accumData=0;
$accumSdata=0;
$accumSbss=0;
$accumBss=0;
#$targetName=$ARGV[0];
undef @Funcs_in_Both;
undef @TextDiff;
undef @RodataDiff;
undef @DataDiff;
undef @SdataDiff;
undef @SbssDiff;
undef @BssDiff;
undef @CompDiff;
undef @FuncAadded;
undef @TextExtra;
undef @RodataExtra;
undef @DataExtra;
undef @SdataExtra;
undef @SbssExtra;
undef @BssExtra;
undef @CompExtra;
undef @FuncDel;
undef @TextDel;
undef @RodataDel;
undef @DataDel;
undef @SdataDel;
undef @SbssDel;
undef @BssDel;
undef @CompDel;
$OverallImageSizeLatest=$ARGV[1];
$OverallImageSizePrev=$ARGV[2];


undef @SecNameLatest;
undef @SecSizeLatest;
undef @SecNamePrev;
undef @SecSizePrev;
#open(MEMDIFF,">Memory_diff_$targetName\.xls");
$Latest_Mem_File="$pathName\\Memory_Info\_$targetName\_Sensors_Latest.csv";
#print"\n$Latest_Mem_File\n";
if (open(MEMDIFF12,"$Latest_Mem_File")){
$memgateline12=<MEMDIFF12>;
$memgateline12=<MEMDIFF12>;
while ( $memgateline12 ne "" ) {
if($memgateline12 =~ /Holes btw the sections in KB\:,(.+)/)
{
$holename="Hole";
$holeSize=$1;
#print"\n$holename\:$holeSize\n";
push(@SecNameLatest,$holename);
push(@SecSizeLatest,$holeSize);
}
#The Size of .ukernel.island in KB:,25.85
if($memgateline12 =~ /The Size of (.+) in KB\:\,(.+)/)
{
$SectionNam=$1;
$SecionSiz=$2;
if (($SectionNam eq ".text") || ($SectionNam eq ".rodata") || ($SectionNam eq ".data") || ($SectionNam eq ".bss") || ($SectionNam eq ".sdata"))
{
push(@FinSecname1, $SectionNam);
push(@FinSecsize1, $SecionSiz);
}

#print"\n$SectionNam\:$SecionSiz\n";
push(@SecNameLatest,$SectionNam);
push(@SecSizeLatest,$SecionSiz);
#$startSize=$1;
}
$memgateline12=<MEMDIFF12>;
}
}
else
{
die"Not able to open the file";
}
close(MEMDIFF12);

if (open(MEMDIFF22,"$pathName\\Memory_Info_$targetName\_Sensors_Prev.csv")){
$memgateline22=<MEMDIFF22>;
$memgateline22=<MEMDIFF22>;
while ( $memgateline22 ne "" ) {
if($memgateline22 =~ /Holes btw the sections in KB\:,(.+)/)
{
$holename="Hole";
$holeSize=$1;
#print"\n$holename\:$holeSize\n";
push(@SecNamePrev,$holename);
push(@SecSizePrev,$holeSize);
}
#The Size of .ukernel.island in KB:,25.85
if($memgateline22 =~ /The Size of (.+) in KB\:\,(.+)/)
{
$SectionNam=$1;
$SecionSiz=$2;
if (($SectionNam eq ".text") || ($SectionNam eq ".rodata") || ($SectionNam eq ".data") || ($SectionNam eq ".bss") || ($SectionNam eq ".sdata"))
{
push(@PrevFinSecname1, $SectionNam);
push(@PrevFinSecsize1, $SecionSiz);
}
#print"\n$SectionNam\:$SecionSiz\n";
push(@SecNamePrev,$SectionNam);
push(@SecSizePrev,$SecionSiz);
#$startSize=$1;
}
$memgateline22=<MEMDIFF22>;
}
}
else
{
die"Not able to open the file";
}
close(MEMDIFF12);
close(MEMDIFF22);
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Sensors_Latest.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
while ( $memgateline ne "" ) {
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$componentName=$9;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Sensors_Prev.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName1=$1;
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
$componentName1=$9;
if($funcName eq $funcName1)
{
$diffText=($textName-$textName1);
$diffRodata=($rodataName-$rodataName1);
$diffData=($dataName-$dataName1);
$diffSdata=($sdataName-$sdataName1);
$diffSbss=($sbssName-$sbssName1);
$diffBss=($bssName-$bssName1);
if(($diffText != 0) || ($diffRodata != 0) || ($diffData != 0) ||($diffSdata != 0) ||($diffSbss != 0) ||($diffBss != 0))
{
$accumText+=$diffText;
$accumRodata+=$diffRodata;
$accumData+=$diffData;
$accumSdata+=$diffSdata;
$accumSbss+=$diffSbss;
$accumBss+=$diffBss;
push(@Funcs_in_Both,$funcName1);
push(@TextDiff,$diffText);
push(@RodataDiff,$diffRodata);
push(@DataDiff,$diffData);
push(@SdataDiff,$diffSdata);
push(@SbssDiff,$diffSbss);
push(@BssDiff,$diffBss);
push(@CompDiff,$componentName1);
}
goto lab123;
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"out:Not able to open the file";
}
push(@FuncAadded,$funcName);
push(@TextExtra,$textName);
push(@RodataExtra,$rodataName);
push(@DataExtra,$dataName);
push(@SdataExtra,$sdataName);
push(@SbssExtra,$sbssName);
push(@BssExtra,$bssName);
push(@CompExtra,$componentName);
lab123:
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:Not able to open the file";
}
close(MEMFILE);
close(MEMFILE1);
if (open(MEMFILE,"$pathName\\Memory_Info_$targetName\_Sensors_Prev.csv")){
$memgateline=<MEMFILE>;
$memgateline=<MEMFILE>;
while ( $memgateline ne "" ) {
if(($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName=$1;
$textName=$2;
$rodataName=$3;
$dataName=$4;
$sdataName=$5;
$sbssName=$6;
$bssName=$7;
$totalSize=$8;
$componentName=$9;
if (open(MEMFILE1,"$pathName\\Memory_Info_$targetName\_Sensors_Latest.csv")){
$memgateline1=<MEMFILE1>;
while ( $memgateline1 ne "" ) {
if(($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+\.\d+)\,(\w+)/)||($memgateline1 =~ /(\w+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\d+)\,(\w+)/))
{
$funcName1=$1;
$textName1=$2;
$rodataName1=$3;
$dataName1=$4;
$sdataName1=$5;
$sbssName1=$6;
$bssName1=$7;
$totalSize1=$8;
$componentName1=$9;
if($funcName eq $funcName1)
{
goto lab231;
}
}
$memgateline1=<MEMFILE1>;
}
}
else
{
die"out:Not able to open the file";
}
push(@FuncDel,$funcName);
push(@TextDel,$textName);
push(@RodataDel,$rodataName);
push(@DataDel,$dataName);
push(@SdataDel,$sdataName);
push(@SbssDel,$sbssName);
push(@BssDel,$bssName);
push(@CompDel,$componentName);
lab231:
}
$memgateline=<MEMFILE>;
}
}
else
{
die"out:Not able to open the file";
}
print MEMDIFF ("Libs that differ in Memory:\n");
$equalLen=@Funcs_in_Both;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
#$row_target=0;
$worksheet3->write("$row_target", $col_target, "Libs that differ in Memory", $format3);
$col_target=0;
$row_target++;
$worksheet3->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
#die"out";
#print"\n$row_target\n";
for($i=0;$i<$equalLen;$i++)
{
$row_target++;
$col_target=0;
$BothFunName=$Funcs_in_Both[$i];
$worksheet3->write("$row_target", $col_target, "$BothFunName", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssDiff[$i]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssDiff[$i]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDiff[$i]+$RodataDiff[$i]+$DataDiff[$i]+$SdataDiff[$i]+$SbssDiff[$i]+$BssDiff[$i];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$CompDiff[$i]", $format4);
$col_target++;
print MEMDIFF ("$Funcs_in_Both[$i]\t$TextDiff[$i]\t$RodataDiff[$i]\t$DataDiff[$i]\t$SdataDiff[$i]\t$SbssDiff[$i]\t$BssDiff[$i]\t$CompDiff[$i]\n");
}
#print"\n$row_target\n";
#print"\npramod\n";
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet3->write("$row_target", 0, "Libs that are added:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are added:\n");
$addLen=@FuncAadded;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
$col_target=0;
$worksheet3->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "Total Memory", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
for($j=0;$j<$addLen;$j++)
{
$row_target++;
$col_target=0;
$worksheet3->write("$row_target", $col_target, "$FuncAadded[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssExtra[$j]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssExtra[$j]", $format4);
$col_target++;
$Total_Comp_Mem=$TextExtra[$j]+$RodataExtra[$j]+$DataExtra[$j]+$SdataExtra[$j]+$SbssExtra[$j]+$BssExtra[$j];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$CompExtra[$j]", $format4);
$col_target++;
print MEMDIFF ("$FuncAadded[$j]\t$TextExtra[$j]\t$RodataExtra[$j]\t$DataExtra[$j]\t$SdataExtra[$j]\t$SbssExtra[$j]\t$BssExtra[$j]\t$CompExtra[$j]\n");
}
#die"out";
$row_target++;
$row_target++;
$row_target++;
$worksheet3->write("$row_target", 0, "Libs that are Deleted:", $format4);
$row_target++;
print MEMDIFF ("\nLibs that are Deleted:\n");
$delLen=@FuncDel;
print MEMDIFF ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t ComponentName\n");
#die"out:456";
$col_target=0;
$worksheet3->write("$row_target", $col_target, "Library Name", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".text", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".rodata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".data", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sdata", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".sbss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, ".bss", $format3);
$col_target++;
$worksheet3->write("$row_target", $col_target, "ComponentName", $format3);
$col_target++;
#die"out:123";
for($k=0;$k<$delLen;$k++)
{
$row_target++;
$col_target=0;
$worksheet3->write("$row_target", $col_target, "$FuncDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$TextDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$RodataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$DataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SdataDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$SbssDel[$k]", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$BssDel[$k]", $format4);
$col_target++;
$Total_Comp_Mem=$TextDel[$k]+$RodataDel[$k]+$DataDel[$k]+$SdataDel[$k]+$SbssDel[$k]+$BssDel[$k];
$worksheet3->write("$row_target", $col_target, "$Total_Comp_Mem", $format4);
$col_target++;
$worksheet3->write("$row_target", $col_target, "$CompDel[$k]", $format4);
$col_target++;
print MEMDIFF ("$FuncDel[$k]\t$TextDel[$k]\t$RodataDel[$k]\t$DataDel[$k]\t$SdataDel[$k]\t$SbssDel[$k]\t$BssDel[$k]\t$CompDel[$k]\n");
}
$row_target++;
$row_target++;
$row_target++;
#die"out";
#die"out";
#push(@SecNameLatest,$SectionNam);
#push(@SecSizeLatest,$SecionSiz);
$SecLatestLen=@SecNameLatest;
$SecPrevLen=@SecNamePrev;
undef @SectionAdded;
undef @SectionAddedSize;
undef @Sectiondeleted;
undef @SectiondeletedSize;
#push(@SecNamePrev,$SectionNam);
#push(@SecSizePrev,$SecionSiz);
for($ii=0;$ii<$SecLatestLen;$ii++)
{
$eqflag=0;
for($iii=0;$iii<$SecPrevLen;$iii++)
{
if($SecNameLatest[$ii] eq $SecNamePrev[$iii])
	{
		$worksheet3->write("$row_target", 0, "Memory size diff in $SecNameLatest[$ii] sections:", $format4);
		$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet3->write("$row_target", 1, "$SectionSize", $format4);
        $row_target++;
		$eqflag = 1;
	}
}
if($eqflag == 0)
{
push(@SectionAdded,$SecNameLatest[$ii]);
push(@SectionAddedSize,$SecSizeLatest[$ii]);
}
}

for($ii=0;$ii<$SecPrevLen;$ii++)
{
$eqflag=0;
for($iii=0;$iii<$SecLatestLen;$iii++)
{
if($SecNamePrev[$ii] eq $SecNameLatest[$iii])
	{
		#$worksheet3->write("$row_target", 0, "Memory size diff in $SecNameLatest[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		#$worksheet3->write("$row_target", 1, "$SectionSize", $format4);
        #$row_target++;
		$eqflag = 1;
	}
}
if($eqflag == 0)
{
push(@Sectiondeleted,$SecNamePrev[$ii]);
push(@SectiondeletedSize,$SecSizePrev[$ii]);
}
}
$SectionaddLen=@SectionAdded;
$SectiondelLen=@Sectiondeleted;
if($SectionaddLen != 0)
{
$row_target++;
$worksheet3->write("$row_target", 0, "Sections Added:", $format4);
$row_target++;
$row_target++;
for($ii=0;$ii<$SectionaddLen;$ii++)
{
		$worksheet3->write("$row_target", 0, "Memory size of added $SectionAdded[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet3->write("$row_target", 1, "$SectionAddedSize[$ii]", $format4);
        $row_target++;
}
}
if($SectiondelLen != 0)
{
$row_target++;
$worksheet3->write("$row_target", 0, "Sections Deleted:", $format4);
$row_target++;
$row_target++;
for($ii=0;$ii<$SectiondelLen;$ii++)
{
		$worksheet3->write("$row_target", 0, "Memory size of deleted $Sectiondeleted[$ii] sections:", $format4);
		#$SectionSize=$SecSizeLatest[$ii]-$SecSizePrev[$iii];
		$worksheet3->write("$row_target", 1, "$SectiondeletedSize[$ii]", $format4);
        $row_target++;
}
}
#die"end";
print MEMDIFF ("\nMemory diff in Hole size  due to Align btw the sections in KB:\t$holeSizediff");

if (open(MEMFILE2,"$pathName\\Memory_Info_$targetName\_Sensors_Latest.csv")){
$memgateline2=<MEMFILE2>;
while ( $memgateline2 ne "" ) {
if($memgateline2 =~ /The Total Image Size in KB\:\,(.+)/)
{
$overallpasssize_curr_sensor=$1;
$overallpasssize_curr_sensor =~ s/^\s+//; #remove leading spaces
$overallpasssize_curr_sensor =~ s/\s+$//; #remove trailing spaces
#print"\n$overallpasssize_curr_sensor\n";
#die"out";
}
$memgateline2=<MEMFILE2>;
}
}
else
{
die"Not able to open the file";
}
close(MEMFILE2);

if (open(MEMFILE2,"$pathName\\Memory_Info_$targetName\_Sensors_Prev.csv")){
$memgateline2=<MEMFILE2>;
while ( $memgateline2 ne "" ) {
if($memgateline2 =~ /The Total Image Size in KB\:\,(.+)/)
{
$overallpasssize_prev_sensor=$1;
$overallpasssize_prev_sensor =~ s/^\s+//; #remove leading spaces
$overallpasssize_prev_sensor =~ s/\s+$//; #remove trailing spaces
#print"\n$overallpasssize_prev_sensor\n";
#die"out";
}
$memgateline2=<MEMFILE2>;
}
}
else
{
die"Not able to open the file";
}
close(MEMFILE2);
}
################################################################################################################

if (open(MEMFILE2,"$pathName\\Memory_Info_$targetName\_Latest.csv")){
$memgateline2=<MEMFILE2>;
while ( $memgateline2 ne "" ) {
if($memgateline2 =~ /The Total Image Size in KB\:\,(.+)/)
{
$overallpasssize_curr=$1;
$overallpasssize_curr =~ s/^\s+//; #remove leading spaces
$overallpasssize_curr =~ s/\s+$//; #remove trailing spaces
#print"\n$overallpasssize_curr\n";
#die"out";
}
$memgateline2=<MEMFILE2>;
}
}
else
{
die"Not able to open the file";
}

if (open(MEMFILE2,"$pathName\\Memory_Info_$targetName\_Prev.csv")){
$memgateline2=<MEMFILE2>;
while ( $memgateline2 ne "" ) {
if($memgateline2 =~ /The Total Image Size in KB\:\,(.+)/)
{
$overallpasssize_prev=$1;
$overallpasssize_prev =~ s/^\s+//; #remove leading spaces
$overallpasssize_prev =~ s/\s+$//; #remove trailing spaces
#print"\n$overallpasssize_prev\n";
#die"out";
}
$memgateline2=<MEMFILE2>;
}
}
else
{
die"Not able to open the file";
}
#if(($targetName == 8974) || ($targetName == 8626) || ($targetName == 8994))
#{
#$overallpasssize_curr = $overallpasssize_curr+$overallpasssize_curr_sensor;
#$overallpasssize_prev = $overallpasssize_prev+$overallpasssize_prev_sensor;
#$overallpasssizediff= $overallpasssize_curr-$overallpasssize_prev;
#}else
#{
#$overallpasssizediff= $overallpasssize_curr-$overallpasssize_prev;
#}

###$col_summary=0;
###$row_summary++;
###$worksheet1->write("$row_summary", $col_summary, "$targetName", $format1);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "$Latestbuildname", $format1);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "$dec_image_size_from_file_latest", $format1);
###$col_summary++;
###$overallpasssize_curr_MB=$overallpasssize_curr/1024;
###$overallpasssize_curr_MB=sprintf("%.2f", $overallpasssize_curr_MB);
###$worksheet1->write("$row_summary", $col_summary, "$overallpasssize_curr_MB", $format1);
###$col_summary++;
###$overallpasssize_curr_sensor_MB=$overallpasssize_curr_sensor/1024;
###$overallpasssize_curr_sensor_MB=sprintf("%.2f", $overallpasssize_curr_sensor_MB);
###$worksheet1->write("$row_summary", $col_summary, "$overallpasssize_curr_sensor_MB", $format1);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "$Previousbuildname", $format1);
###$col_summary++;
###$worksheet1->write("$row_summary", $col_summary, "$dec_image_size_from_file_prev", $format1);
###$col_summary++;
###$overallpasssize_prev_MB=$overallpasssize_prev/1024;
###$overallpasssize_prev_MB=sprintf("%.2f", $overallpasssize_prev_MB);
###$worksheet1->write("$row_summary", $col_summary, "$overallpasssize_prev_MB", $format1);
###$col_summary++;
###$overallpasssize_prev_sensor_MB=$overallpasssize_prev_sensor/1024;
###$overallpasssize_prev_sensor_MB=sprintf("%.2f", $overallpasssize_prev_sensor_MB);
###$worksheet1->write("$row_summary", $col_summary, "$overallpasssize_prev_sensor_MB", $format1);
###$col_summary++;
###$row_summary++;
$row_summary=0;
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "Latest Build:", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$Latestbuildname", $format9);
$col_summary++;
$col_summary=0;
$row_summary++;
$worksheet1->write("$row_summary", $col_summary, "Previous Build:", $format9);
$col_summary++;
#$worksheet->merge_range($x,$y,Summary, $format);
$worksheet1->write("$row_summary", $col_summary, "$Previousbuildname", $format9);
$row_summary++;
$worksheet1->write("$row_summary", 0, "Memory Diff in CommonPD from Latest to Prev in KB:", $format1);
$overallpasssize_curr_diff=$overallpasssize_curr-$overallpasssize_prev;
$worksheet1->write("$row_summary", 1, "$overallpasssize_curr_diff", $format1);
$row_summary++;
$worksheet1->write("$row_summary", 0, "Memory Diff in SensorsPD from Latest to Prev in KB:", $format1);
$overallpasssize_curr_sensor_diff=$overallpasssize_curr_sensor-$overallpasssize_prev_sensor;
$worksheet1->write("$row_summary", 1, "$overallpasssize_curr_sensor_diff", $format1);
$row_summary++;
$row_summary++;
$row_summary++;
$row_summary++;
$row_summary++;
$worksheet1->write("$row_summary", 0, "GUESTOS", $format9);
$row_summary++;

$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "SectionName", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Latest Size in KB", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Prev Size in KB", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Sec Diff in KB", $format9);
$row_summary++;
$SecLen=@PrevFinSecname;
for($i=0;$i<$SecLen;$i++)
{
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "$FinSecname[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$FinSecsize[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$PrevFinSecsize[$i]", $format1);
$col_summary++;
$SectionDiffFin=$FinSecsize[$i]-$PrevFinSecsize[$i];
$worksheet1->write("$row_summary", $col_summary, "$SectionDiffFin", $format1);
$col_summary++;
$row_summary++;
}
$row_summary++;
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "ComponentName", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem On Latest", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem On Previous", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem Diff", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem On Latest", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem On Previous", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem Diff", $format9);
$col_summary++;
$row_summary++;
$CompLength=@ComponentNameFin;
$CompLength1=@ComponentNameFin1;
for($i=0;$i<$CompLength;$i++)
{
$col_summary=0;
$ComName=$ComponentNameFin[$i];
$worksheet1->write("$row_summary", $col_summary, "$ComName", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompLatestTextSize[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompPrevTextSize[$i]", $format1);
$col_summary++;
$CompTextDiff=$CompLatestTextSize[$i]-$CompPrevTextSize[$i];
$worksheet1->write("$row_summary", $col_summary, "$CompTextDiff", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompLatestDataSize[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompPrevDataSize[$i]", $format1);
$col_summary++;
$CompDataDiff=$CompLatestDataSize[$i]-$CompPrevDataSize[$i];
$worksheet1->write("$row_summary", $col_summary, "$CompDataDiff", $format1);
$col_summary++;
$row_summary++;
}
$row_summary++;
$row_summary++;
$worksheet1->write("$row_summary", 0, "SensorsPD", $format9);
$row_summary++;
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "SectionName", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Latest Size in KB", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Prev Size in KB", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "Sec Diff in KB", $format9);
$row_summary++;
$SecLen=@PrevFinSecname;
for($i=0;$i<$SecLen;$i++)
{
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "$FinSecname1[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$FinSecsize1[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$PrevFinSecsize1[$i]", $format1);
$col_summary++;
$SectionDiffFin1=$FinSecsize1[$i]-$PrevFinSecsize1[$i];
$worksheet1->write("$row_summary", $col_summary, "$SectionDiffFin1", $format1);
$col_summary++;
$row_summary++;
}
$row_summary++;
$col_summary=0;
$worksheet1->write("$row_summary", $col_summary, "ComponentName", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem On Latest", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem On Previous", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "TextMem Diff", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem On Latest", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem On Previous", $format9);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "DataMem Diff", $format9);
$col_summary++;
$row_summary++;
for($i=0;$i<$CompLength1;$i++)
{
$col_summary=0;
$ComName=$ComponentNameFin1[$i];
$worksheet1->write("$row_summary", $col_summary, "$ComName", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompLatestTextSize1[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompPrevTextSize1[$i]", $format1);
$col_summary++;
$CompTextDiff1=$CompLatestTextSize1[$i]-$CompPrevTextSize1[$i];
$worksheet1->write("$row_summary", $col_summary, "$CompTextDiff1", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompLatestDataSize1[$i]", $format1);
$col_summary++;
$worksheet1->write("$row_summary", $col_summary, "$CompPrevDataSize1[$i]", $format1);
$col_summary++;
$CompDataDiff1=$CompLatestDataSize1[$i]-$CompPrevDataSize1[$i];
$worksheet1->write("$row_summary", $col_summary, "$CompDataDiff1", $format1);
$col_summary++;
$row_summary++;
}


#die"out";
print MEMDIFF ("\nOverall Mmeory Diff in KB:$overallpasssizediff");
print MEMDIFF ("Target \t LatestADSPSizeLimt \t LatestADSPSizeConsumed \t PrevADSPSizeLimt \t PrevADSPSizeConsumed \t FreeSpace\n");
print MEMDIFF ("$targetName \t $OverallImageSizeLatest \t $overallpasssize_curr \t $OverallImageSizeLatest \t $overallpasssize_prev \t $overallpasssizediff\n");
##################################################################################################################################################	
	
	#sleep(5);#Wait time
	#print "continue: wake for next new build check\n";;
#}

$oldBuildInafoName1[$overallloop]=$latestBuildinfo;
$oldBuildInfo1[$overallloop]=$latestBuildpath;
#if (open(STATFILE1,">Prev_Build_Info1.txt"))
#{

#}
#else
#{
#die"\nNot Able to Write to STATS File\n";
#}
#close(STATFILE1);
SkipentireTarget:
if($NoDiffFlag == 1)
{
print STATFILE1 ("$oldBuildInafoName[$overallloop]");
print STATFILE1 ("$oldBuildInfo[$overallloop]");
$NoDiffFlag=0;
}
else
{
print STATFILE1 ("$oldBuildInafoName1[$overallloop]\n");
print STATFILE1 ("$oldBuildInfo1[$overallloop]\n");
}
##}
close(STATFILE1);
close(STATFILE);
#print"\nThe end\n";
#system("perl renametxt.pl");
#$old_file="Prev_Build_Info1.txt";
#    $new_file="Prev_Build_Info.txt";	
#	unlink($new_file);
#    rename 	$old_file, $new_file;
#print"\nThe end\n";

$workbook->close();
#copy("MemDiff.xlsx","/MEM_Diff/");
#$old_file="MemDiff.xlsx";
#    $new_file="/MEM_Diff/MemDiff.xlsx";	
#    rename 	$old_file, $new_file;
#system("perl movefile.pl");
##system("perl sendmail.pl $pathName");