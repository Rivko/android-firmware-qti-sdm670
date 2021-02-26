$targetName=$ARGV[0];
$inputfilename=$ARGV[1];
$PDName=$ARGV[2];
$arg_len=@ARGV;
if($arg_len != 3)
{
print"**********Usage of the script:**********\n";
print"perl <name of the script> <Target Name> <path to the map file> <PD Name>\n";
print"Example:\n";
print"perl Image_Break_Down.pl 8974 <path>/<filename>.map <sensors>\n";
print"****************************************\n";
}
open(OUTRPTFILE, ">Memory_Info_$targetName\_$PDName.xls") or die "not able to open file \"Memory_Info.xls\" in write mode";#Writing to a file "system.xls" which is output.
undef $libarraytext_name;
undef $libarraytext_name1;
undef $libarrayrodata_name;
undef $libarrayrodata_name1;
undef $libarraydata_name;
undef $libarraydata_name1;
undef $libarraysdata_name;
undef $libarraysdata_name1;
undef $libarraysbss_name;
undef $libarraysbss_name1;
undef $libarraybss_name;
undef $libarraybss_name1;
undef $componentText;
undef $componentRodata;
undef $componentData;
undef $componentSdata;
undef $componentSbss;
undef $componentBss;
$qsr_stringdec = 0;
$qsr_stringdecKB = 0;
$single_lin = 0;
$bssSingleLine = 0;
$flagCurr=0;
$textStartFlagCurr=0;
$bssStartFlagCurr=0;
$flagCurrRodata=0;
$flagCurrData=0;
$flagCurrSdata=0;
$flagCurrSbss=0;
$flagCurrBss=0;
$startStartFlag = 0;
$textStartFlag = 0;
$finiStartFlag = 0;
$rodataStartFlag = 0;
$dataStartFlag = 0;
$eh_frameStartFlag = 0;
$gcc_except_tableStartFlag = 0;
$qurt_context_tcmStartFlag = 0;
$a8974_devcfg_data=0;
$a8x26_devcfg_data=0;
$a9625_devcfg_data=0;
$a8610_devcfg_data=0;
$a8092_devcfg_data=0;
$a8094_devcfg_data=0;
$devcfg_dataStartFlag = 0;
$ctorsStartFlag = 0;
$dtorsStartFlag = 0;
$sdataStartFlag = 0;
$sbssStartFlag = 0;
$bssStartFlag = 0;
$dynstrStartFlag = 0;
$dynsymStartFlag = 0;
$hashStartFlag = 0;
$dynamicStartFlag = 0;
$interpStartFlag = 0;
$gotStartFlag = 0;
if (open(TRACEFILE,"$inputfilename")){
$inputline=<TRACEFILE>;
while ( $inputline ne "" ) {
#################################START#####################################
if (($inputline =~ /^\.start\s+0x(\w+)\s+0x(\w+)/) || ($startStartFlag == 1))
{
$startStartAddrHex=$1;
$overallStartSizeHex=$2;
$startStartAddrDec=hex($startStartAddrHex);
$overallStartSizeDec=hex($overallStartSizeHex);
$startStartFlag = 0;
}
#################################INIT#####################################
if (($inputline =~ /^\.init\s+0x(\w+)\s+0x(\w+)/) || ($initStartFlag == 1))
{
$startStartFlag = 0;
$initStartAddrHex=$1;
$overallInitSizeHex=$2;
$initStartAddrDec=hex($initStartAddrHex);
$overallInitSizeDec=hex($overallInitSizeHex);
$initStartFlag = 0;
}
#################################TEXT#####################################
if (($inputline =~ /^\.text\s+0x(\w+)\s+0x(\w+)/) || ($textStartFlag == 1))
{
#print"\n$inputline\n";
#die"out";
$initStartFlag = 0;
$textStartAddrHex=$1;
$overallTextSizeHex=$2;
$textStartAddrDec=hex($textStartAddrHex);
$textStartAddrDec2=$textStartAddrDec;
$overallTextSizeDec=hex($overallTextSizeHex);
$textEndAddrDec=$textStartAddrDec+$overallTextSizeDec;
$textStartFlag = 1;
#if(($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/))
if(($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.text\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.text\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.text\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{	$single_lin=1;										
	$flag=0;
	$textStartAddressHex=$1;
	$libraryTextName=$5;
	$level1=$3;
	$level2=$4;
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
    $textStartAddressDec=hex($textStartAddressHex);

	if($flagCurr==1)
	{		
	$textStartAddressDec=hex($textStartAddressHex);
	if($textStartAddressDec == 0)
	{
		goto textoutSection;
	}
	$SizeOfLib=$textStartAddressDec-$textStartAddressDecPrev;
	$libarraytext_name{$libraryTextNamePrev}=$libarraytext_name{$libraryTextNamePrev}+$SizeOfLib;
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{
	#print"\n$level1Prev : $level2Prev\n";
	$level1Prev = $level2Prev;
	#die"out";
	}
	$libarraytext_name1{$libraryTextNamePrev}=$level1Prev;
	}
	$flagCurr=1;
	goto labelText;
}
elsif (($inputline =~ /\s+\.text\.(\w+)/) || ($inputline =~ /^\s+QURTK\.(.+)\.text/) || ($inputline =~ /^\s+QURT\.(.+)\.text/))
{
	$inputline=<TRACEFILE>;
	if(($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/)  || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;		
		$textStartAddressHex=$1;
		$libraryTextName=$5;
		$level1=$3;
		$level2=$4;		
		if($level2 eq "HEXAGON_Tools")
		{		        
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";			
		}
		$textStartAddressDec=hex($textStartAddressHex);
		#print"\n$textStartAddressDec\n";
		#print"\n$textStartAddrDec2\n";
		#die"out";
		if(($textStartFlagCurr==0) && ($single_lin==0))
		{
			if($textStartAddressDec == $textStartAddrDec2)
			{
			}
			else
			{
			#print"\n$textStartAddressDec\n";
			#print"\n$textStartAddrDec2\n";
			$SizeOfLib=$textStartAddressDec-$textStartAddrDec2;
			#print"\n$SizeOfLib\n";
			$libraryTextNamePrev="TextAlignmentStart";
			$libarraytext_name{$libraryTextNamePrev}=$libarraytext_name{$libraryTextNamePrev}+$SizeOfLib;
			#print"\n$libarraytext_name{$libraryTextNamePrev}\n";
			$libarraytext_name1{$libraryTextNamePrev}="TextAlignmentStart";
			#print"\n$libarraytext_name1{$libraryTextNamePrev}\n";
			#$textStartAddressDecPrev=$textStartAddressDec2;
			#$level1="TextAlignmentStart";
			#$level2="TextAlignmentStart";
			#die"outtext";
			}
		}
		$textStartFlagCurr=1;
		if($flagCurr==1)
		{		
		$textStartAddressDec=hex($textStartAddressHex);
		if($textStartAddressDec == 0)
		{
			goto textoutSection;
		}		
		$SizeOfLib=$textStartAddressDec-$textStartAddressDecPrev;
		$libarraytext_name{$libraryTextNamePrev}=$libarraytext_name{$libraryTextNamePrev}+$SizeOfLib;
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;		
		}
		$libarraytext_name1{$libraryTextNamePrev}=$level1Prev;
		}
		$flagCurr=1;
		goto labelText;
	}
}
labelText:
if($textStartAddressDec!=0)
{
$textStartAddressHexPrev=$textStartAddressHex;
$textStartAddressDecPrev=hex($textStartAddressHexPrev);
$libraryTextNamePrev=$libraryTextName;
$level1Prev=$level1;
$level2Prev=$level2;
$textlevel1Prev=$level1Prev;
$textStartAddrDec2=$textStartAddrDec;
}
textoutSection:
}
#################################FINI#####################################
if (($inputline =~ /^\.fini\s+0x(\w+)\s+0x(\w+)/) || ($finiStartFlag == 1))
{
$textStartFlag = 0;
$finiStartAddrHex=$1;
$overallFiniSizeHex=$2;
$finiStartAddrDec=hex($finiStartAddrHex);
$overallFiniSizeDec=hex($overallFiniSizeHex);
}		
#################################RODATA#####################################		
if (($inputline =~ /^\.rodata\s+0x(\w+)\s+0x(\w+)/) || ($rodataStartFlag == 1))
{
$finiStartFlag = 0;
$textStartFlag = 0;
$rodataStartAddrHex=$1;
$overallRodataSizeHex=$2;
$rodataStartAddrDec=hex($rodataStartAddrHex);
$overallRodataSizeDec=hex($overallRodataSizeHex);
$rodataEndAddrDec=$rodataStartAddrDec+$overallRodataSizeDec;
#print"\n$rodataStartAddrHex:$overallRodataSizeHex\n";
#die"out";
$rodataStartFlag = 1;
if(($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/HEXAGON_Tools\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/HEXAGON_Tools\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/)|| ($inputline =~ /^\s+\.rodata\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.rodata\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{											
	$flag=0;
	$rodataStartAddressHex=$1;
	$libraryRodataName=$5;
	$level1=$3;
	$level2=$4;
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
	$rodataStartAddressDec=hex($rodataStartAddressHex);
	if($flagCurrRodata==1)
	{		
	$rodataStartAddressDec=hex($rodataStartAddressHex);
	if($rodataStartAddressDec == 0)
	{
		goto outSection;
	}
	$SizeOfLib=$rodataStartAddressDec-$rodataStartAddressDecPrev;
	$libarrayrodata_name{$libraryRodataNamePrev}=$libarrayrodata_name{$libraryRodataNamePrev}+$SizeOfLib;	
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{		
	$level1Prev = $level2Prev;		
	}	
	$libarrayrodata_name1{$libraryRodataNamePrev}=$level1Prev;
	}
	$flagCurrRodata=1;
	goto labelRodata;
}
elsif (($inputline =~ /^\s+\.rodata\.(\w+)/)|| ($inputline =~ /^\s+QURTK\.(.+)\.rodata/) || ($inputline =~ /^\s+QURT\.(.+)\.rodata/) || ($inputline =~ /^\s+\.rodata\.(.+)/) || ($inputline =~ /^\s+\.striplib\.QSR_STR/) || ($inputline =~ /^\s+QSR_STR\.fmt\.rodata\./) || ($inputline =~ /^\s+QSR_STR\.fmt\.rodata\.(.+)/))
{
    #if (($inputline =~ /^\s+QSR_STR\.fmt\.rodata\./) || ($inputline =~ /^\s+QSR_STR\.fmt\.rodata\.(.+)/))
	#{
	#$inputline=<TRACEFILE>;
	#print "\n$inputline=\n";
	#die"out";
	#}
	$inputline=<TRACEFILE>;
	if(($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.rodata\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;
		$rodataStartAddressHex=$1;
		$libraryRodataName=$5;
		$level1=$3;
		$level2=$4;	
		if($level2 eq "HEXAGON_Tools")
		{
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";
		}
		$rodataStartAddressDec=hex($rodataStartAddressHex);
		if($flagCurrRodata==1)
		{		
		$rodataStartAddressDec=hex($rodataStartAddressHex);
		if($rodataStartAddressDec == 0)
		{				
		goto outSection;
		}
		$SizeOfLib=$rodataStartAddressDec-$rodataStartAddressDecPrev;
		$libarrayrodata_name{$libraryRodataNamePrev}=$libarrayrodata_name{$libraryRodataNamePrev}+$SizeOfLib;		
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;	
		}
		$libarrayrodata_name1{$libraryRodataNamePrev}=$level1Prev;
		}
		$flagCurrRodata=1;
		goto labelRodata;
	}
labelRodata:
#if(($rodataStartAddressDec!=0) || ($rodataStartFlag != 1))
if($rodataStartAddressDec!=0)
{
$rodataStartAddressHexPrev=$rodataStartAddressHex;
$rodataStartAddressDecPrev=hex($rodataStartAddressHexPrev);
$libraryRodataNamePrev=$libraryRodataName;
$level1Prev=$level1;
$level2Prev=$level2;
$rodatalevel1Prev=$level1Prev;
}
outSection:	
}
}
#################################8974_DEVCFG_DATA#####################################
if (($inputline =~ /^\.8974\_DEVCFG\_DATA/) || ($a8974_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a8974_devcfg_dataStartAddrHex=$1;
$overalla8974_devcfg_dataSizeHex=$2;
}
$a8974_devcfg_dataStartAddrDec=hex($a8974_devcfg_dataStartAddrHex);
$overalla8974_devcfg_dataSizeDec=hex($overalla8974_devcfg_dataSizeHex);
$a8974_devcfg_data = 0;
}
#################################8626_DEVCFG_DATA#####################################
if (($inputline =~ /^\.8x26\_DEVCFG\_DATA/) || ($inputline =~ /^\.8626\_DEVCFG\_DATA/) || ($a8626_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a8626_devcfg_dataStartAddrHex=$1;
$overalla8626_devcfg_dataSizeHex=$2;
}
$a8626_devcfg_dataStartAddrDec=hex($a8626_devcfg_dataStartAddrHex);
$overalla8626_devcfg_dataSizeDec=hex($overalla8626_devcfg_dataSizeHex);
$a8626_devcfg_data = 0;
}
#################################9625_DEVCFG_DATA#####################################
if (($inputline =~ /^\.9625\_DEVCFG\_DATA/) || ($inputline =~ /^\.9x25\_DEVCFG\_DATA/) || ($a9625_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a9625_devcfg_dataStartAddrHex=$1;
$overalla9625_devcfg_dataSizeHex=$2;
}
$a9625_devcfg_dataStartAddrDec=hex($a9625_devcfg_dataStartAddrHex);
$overalla9625_devcfg_dataSizeDec=hex($overalla9625_devcfg_dataSizeHex);
$a9625_devcfg_data = 0;
}
#################################8610_DEVCFG_DATA#####################################
if (($inputline =~ /^\.8610\_DEVCFG\_DATA/) || ($inputline =~ /^\.8x10\_DEVCFG\_DATA/) || ($a8610_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a8610_devcfg_dataStartAddrHex=$1;
$overalla8610_devcfg_dataSizeHex=$2;
}
$a8610_devcfg_dataStartAddrDec=hex($a8610_devcfg_dataStartAddrHex);
$overalla8610_devcfg_dataSizeDec=hex($overalla8610_devcfg_dataSizeHex);
$a8610_devcfg_data = 0;
}
#################################8092_DEVCFG_DATA#####################################
if (($inputline =~ /^\.8092\_DEVCFG\_DATA/) || ($a8092_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a8092_devcfg_dataStartAddrHex=$1;
$overalla8092_devcfg_dataSizeHex=$2;
}
$a8092_devcfg_dataStartAddrDec=hex($a8092_devcfg_dataStartAddrHex);
$overalla8092_devcfg_dataSizeDec=hex($overalla8092_devcfg_dataSizeHex);
$a8092_devcfg_data = 0;
}
#################################8094_DEVCFG_DATA#####################################
if (($inputline =~ /^\.8094\_DEVCFG\_DATA/) || ($a8094_devcfg_data == 1))
{
$dataStartFlag=0;
$rodataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$a8094_devcfg_dataStartAddrHex=$1;
$overalla8094_devcfg_dataSizeHex=$2;
}
$a8094_devcfg_dataStartAddrDec=hex($a8094_devcfg_dataStartAddrHex);
$overalla8094_devcfg_dataSizeDec=hex($overalla8094_devcfg_dataSizeHex);
$a8094_devcfg_data = 0;
}
#################################DATA#####################################
if (($inputline =~ /^\.data\s+0x(\w+)\s+0x(\w+)/) || ($dataStartFlag == 1))
{
$textStartFlag = 0;
$rodataStartFlag = 0;
$dataStartAddrHex=$1;
$overallDataSizeHex=$2;
$dataStartAddrDec=hex($dataStartAddrHex);
$overallDataSizeDec=hex($overallDataSizeHex);
$dataEndAddrDec=$dataStartAddrDec+$overallDataSizeDec;
#print"\npramod1:$dataStartAddrDec $dataEndAddrDec $overallDataSizeDec\n";
#print"\n$inputline\n";
#die"dataend";
$dataStartFlag = 1;
if(($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.data\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.data\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{							
    #print"\n$inputline\n";
    #die"out";	
	$flag=0;
	$dataStartAddressHex=$1;
	$libraryDataName=$5;
	$level1=$3;
	$level2=$4;
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
	$dataStartAddressDec=hex($dataStartAddressHex);
	if($flagCurrData==1)
	{		
	$dataStartAddressDec=hex($dataStartAddressHex);
	if($dataStartAddressDec == 0)
	{
		goto dataoutSection;
	}
	$SizeOfLib=$dataStartAddressDec-$dataStartAddressDecPrev;
			#print"\n$libraryDataNamePrev\n";
		#print"\n$dataStartAddressDec $dataStartAddressDecPrev $SizeOfLib\n";
	#print"\n$SizeOfLib\n";
	$libarraydata_name{$libraryDataNamePrev}=$libarraydata_name{$libraryDataNamePrev}+$SizeOfLib;
	#print"\n$libraryDataNamePrev\n";
	#print"\n$libarraydata_name{$libraryDataNamePrev}\n";
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{		
	$level1Prev = $level2Prev;		
	}
	$libarraydata_name1{$libraryDataNamePrev}=$level1Prev;
	}
	$flagCurrData=1;
	goto labelData;
}
elsif (($inputline =~ /^\s+\.data\.(\w+)/) || ($inputline =~ /^\s+QURTK\.(.+)\.data/) || ($inputline =~ /^\s+QURT\.(.+)\.data/) || ($inputline =~ /^\s+\.data\.(.+)/))
{
    #print"\n$inputline\n";
	#die"out";
	$inputline=<TRACEFILE>;
	if(($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.data\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;
		$dataStartAddressHex=$1;
		$libraryDataName=$5;
		$level1=$3;
		$level2=$4;
		if($level2 eq "HEXAGON_Tools")
		{
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";
		}
		$dataStartAddressDec=hex($dataStartAddressHex);
		if($flagCurrData==1)
		{		
		$dataStartAddressDec=hex($dataStartAddressHex);
		if($dataStartAddressDec == 0)
		{		
		goto dataoutSection;
		}
		$SizeOfLib=$dataStartAddressDec-$dataStartAddressDecPrev;
		#print"\n$libraryDataNamePrev\n";
		#print"\n$dataStartAddressDec $dataStartAddressDecPrev $SizeOfLib\n";
		$libarraydata_name{$libraryDataNamePrev}=$libarraydata_name{$libraryDataNamePrev}+$SizeOfLib;
	    #print"\n$libraryDataNamePrev\n";
	    #print"\n$libarraydata_name{$libraryDataNamePrev}\n";
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;		
		}		
		$libarraydata_name1{$libraryDataNamePrev}=$level1Prev;
		}
		$flagCurrData=1;
		goto labelData;
	}
labelData:
if($dataStartAddressDec!=0)
{
$dataStartAddressHexPrev=$dataStartAddressHex;
$dataStartAddressDecPrev=hex($dataStartAddressHexPrev);
$libraryDataNamePrev=$libraryDataName;
$level1Prev=$level1;
$level2Prev=$level2;
$datalevel1Prev=$level1Prev;
}
dataoutSection:	
}
}
#################################QURTK.CONTEXTS.TCM.data#####################################
if (($inputline =~ /^QURTK\.CONTEXTS\.TCM\.data/) || ($qurt_context_tcmStartFlag == 1))
{
$dataStartFlag = 0;
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$qurt_context_tcmStartAddrHex=$1;
$overallqurt_context_tcmSizeHex=$2;
}
$qurt_context_tcmStartAddrDec=hex($qurt_context_tcmStartAddrHex);
$overallqurt_context_tcmSizeDec=hex($overallqurt_context_tcmSizeHex);
$qurt_context_tcmStartFlag = 0;
}
#################################DEVCFG_DATA#####################################
if (($inputline =~ /^\.DEVCFG\_DATA\s+0x(\w+)\s+0x(\w+)/) || ($devcfg_dataStartFlag == 1))
{
$dataStartFlag = 0;
$textStartFlag = 0;
$rodataStartFlag = 0;
$devcfg_dataStartAddrHex=$1;
$overalldevcfg_dataSizeHex=$2;
$devcfg_dataStartAddrDec=hex($devcfg_dataStartAddrHex);
$overalldevcfg_dataSizeDec=hex($overalldevcfg_dataSizeHex);
$devcfg_dataStartFlag = 0;
$qurt_context_tcmStartFlag = 0;
}
#################################EH_FRAME#####################################
if (($inputline =~ /^\.eh\_frame\s+0x(\w+)\s+0x(\w+)/) || ($eh_frameStartFlag == 1))
{
$dataStartFlag = 0;
$eh_frameStartAddrHex=$1;
$overallEh_frameSizeHex=$2;
$eh_frameStartAddrDec=hex($eh_frameStartAddrHex);
$overallEh_frameSizeDec=hex($overallEh_frameSizeHex);
$eh_frameStartFlag = 0;
$devcfg_dataStartFlag=0;
}
#################################GCC_EXPECT_TABLE#####################################
if (($inputline =~ /^\.gcc_except_table/) || ($gcc_except_tableStartFlag == 1))
{
$inputline=<TRACEFILE>;
if($inputline =~ /\s+0x(\w+)\s+0x(\w+)/)
{
$gcc_except_tableStartAddrHex=$1;
$overallGcc_except_tableSizeHex=$2;
}
$eh_frameStartFlag = 0;
$gcc_except_tableStartAddrDec=hex($gcc_except_tableStartAddrHex);
$overallGcc_except_tableSizeDec=hex($overallGcc_except_tableSizeHex);
$gcc_except_tableStartFlag=0;
}
#################################CTORS#####################################
if (($inputline =~ /^\.ctors\s+0x(\w+)\s+0x(\w+)/) || ($ctorsStartFlag == 1))
{
$gcc_except_tableStartFlag = 0;
$ctorsStartAddrHex=$1;
$overallCtorsSizeHex=$2;
$ctorsStartAddrDec=hex($ctorsStartAddrHex);
$overallCtorsSizeDec=hex($overallCtorsSizeHex);
$ctorsStartFlag = 0;
}
#################################DTORS#####################################
if (($inputline =~ /^\.dtors\s+0x(\w+)\s+0x(\w+)/) || ($dtorsStartFlag == 1))
{
$ctorsStartFlag = 0;
$dtorsStartAddrHex=$1;
$overallDtorsSizeHex=$2;
$dtorsStartAddrDec=hex($dtorsStartAddrHex);
$overallDtorsSizeDec=hex($overallDtorsSizeHex);
$dtorsStartFlag = 0;
}
#################################GOT#####################################
if (($inputline =~ /^\.got\s+0x(\w+)\s+0x(\w+)/) || ($gotStartFlag == 1))
{
$dtorsStartFlag = 0;
$gotStartAddrHex=$1;
$overallGotSizeHex=$2;
$gotStartAddrDec=hex($gotStartAddrHex);
$overallGotSizeDec=hex($overallGotSizeHex);
$gotStartFlag = 0;
}
#################################INTERP#####################################
if (($inputline =~ /^\.interp\s+0x(\w+)\s+0x(\w+)/) || ($interpStartFlag == 1))
{
$gotStartFlag = 0;
$interpStartAddrHex=$1;
$overallInterpSizeHex=$2;
$interpStartAddrDec=hex($interpStartAddrHex);
$overallInterpSizeDec=hex($overallInterpSizeHex);
$interpStartFlag = 0;
}
#################################DYNAMIC#####################################
if (($inputline =~ /^\.dynamic\s+0x(\w+)\s+0x(\w+)/) || ($dynamicStartFlag == 1))
{
$interpStartFlag = 0;
$dynamicStartAddrHex=$1;
$overallDynamicSizeHex=$2;
$dynamicStartAddrDec=hex($dynamicStartAddrHex);
$overallDynamicSizeDec=hex($overallDynamicSizeHex);
$dynamicStartFlag = 0;
}
#################################HASH#####################################
if (($inputline =~ /^\.hash\s+0x(\w+)\s+0x(\w+)/) || ($hashStartFlag == 1))
{
$dynamicStartFlag = 0;
$hashStartAddrHex=$1;
$overallHashSizeHex=$2;
$hashStartAddrDec=hex($hashStartAddrHex);
$overallHashSizeDec=hex($overallHashSizeHex);
$hashStartFlag = 0;
}
#################################DYNSYM#####################################
if (($inputline =~ /^\.dynsym\s+0x(\w+)\s+0x(\w+)/) || ($dynsymStartFlag == 1))
{
$hashStartFlag = 0;
$dynsymStartAddrHex=$1;
$overallDynsymSizeHex=$2;
$dynsymStartAddrDec=hex($dynsymStartAddrHex);
$overallDynsymSizeDec=hex($overallDynsymSizeHex);
$dynsymStartFlag = 0;
}
#################################DYNSTR#####################################
if (($inputline =~ /^\.dynstr\s+0x(\w+)\s+0x(\w+)/) || ($dynstrStartFlag == 1))
{
$dynsymStartFlag = 0;
$dynstrStartAddrHex=$1;
$overallDynstrSizeHex=$2;
$dynstrStartAddrDec=hex($dynstrStartAddrHex);
$overallDynstrSizeDec=hex($overallDynstrSizeHex);
$dynstrStartFlag = 0;
}
#################################SDATA#####################################
if (($inputline =~ /^\.sdata\s+0x(\w+)\s+0x(\w+)/) || ($sdataStartFlag == 1))
{
$dtorsStartFlag = 0;
$textStartFlag = 0;
$rodataStartFlag = 0;
$dataStartFlag = 0;
$sdataStartAddrHex=$1;
$overallSdataSizeHex=$2;
$sdataStartAddrDec=hex($sdataStartAddrHex);
$overallSdataSizeDec=hex($overallSdataSizeHex);
$sdataEndAddrDec=$sdataStartAddrDec+$overallSdataSizeDec;
$sdataStartFlag = 1;
if(($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.scommon\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\..+\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\..+\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\..+\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\..+\.\w+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.lita\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
#if(($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+[\.sdata\.\w+,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+[\.sdata,\.scommon\.\w+]\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{	
	$flag=0;
	$sdataStartAddressHex=$1;
	$librarySdataName=$5;	
	$level1=$3;
	$level2=$4;	
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
	$sdataStartAddressDec=hex($sdataStartAddressHex);
	if($flagCurrSdata==1)
	{		
	$sdataStartAddressDec=hex($sdataStartAddressHex);
	if($sdataStartAddressDec == 0)
	{
		goto sdataoutSection;
	}
	$SizeOfLib=$sdataStartAddressDec-$sdataStartAddressDecPrev;
	$libarraysdata_name{$librarySdataNamePrev}=$libarraysdata_name{$librarySdataNamePrev}+$SizeOfLib;
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{		
	$level1Prev = $level2Prev;		
	}
	$libarraysdata_name1{$librarySdataNamePrev}=$level1Prev;
	}
	$flagCurrSdata=1;
	goto labelSdata;
}
elsif(($inputline =~ /^\s+\.sdata\.(\w+)/) || ($inputline =~ /^\s+QURTK\.(.+)\.sdata/) || ($inputline =~ /^\s+QURT\.(.+)\.sdata/) || ($inputline =~ /^\s+\.gnu\..+/) || ($inputline =~ /^\s+\.sdata\.(.+)/))
{
	$inputline=<TRACEFILE>;
	if(($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sdata\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;
		$sdataStartAddressHex=$1;
		$librarySdataName=$5;
		$level1=$3;
		$level2=$4;
		if($level2 eq "HEXAGON_Tools")
		{
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";
		}
		$sdataStartAddressDec=hex($sdataStartAddressHex);
		if($flagCurrSdata==1)
		{		
		$sdataStartAddressDec=hex($sdataStartAddressHex);
		if($sdataStartAddressDec == 0)
		{		
		goto sdataoutSection;
		}
		$SizeOfLib=$sdataStartAddressDec-$sdataStartAddressDecPrev;
		$libarraysdata_name{$librarySdataNamePrev}=$libarraysdata_name{$librarySdataNamePrev}+$SizeOfLib;
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;		
		}
		$libarraysdata_name1{$librarySdataNamePrev}=$level1Prev;
		}
		$flagCurrSdata=1;
		goto labelSdata;
	}
labelSdata:
if($sdataStartAddressDec != 0)
{
$sdataStartAddressHexPrev=$sdataStartAddressHex;
$sdataStartAddressDecPrev=hex($sdataStartAddressHexPrev);
$librarySdataNamePrev=$librarySdataName;
$level1Prev=$level1;
$level2Prev=$level2;
$sdatalevel1Prev=$level1Prev;
}
sdataoutSection:	
}
}

#################################SBSS#####################################
if (($inputline =~ /^\.sbss\s+0x(\w+)\s+0x(\w+)/) || ($sbssStartFlag == 1))
{
$textStartFlag = 0;
$rodataStartFlag = 0;
$dataStartFlag = 0;
$sdataStartFlag = 0;
$sbssStartAddrHex=$1;
$overallSbssSizeHex=$2;
$sbssStartAddrDec=hex($sbssStartAddrHex);
$overallSbssSizeDec=hex($overallSbssSizeHex);
$sbssEndAddrDec=$sbssStartAddrDec+$overallSbssSizeDec;
$sbssStartFlag = 1;
if(($inputline =~ /^\s+\.scommon\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.sbss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{
	$flag=0;
	$sbssStartAddressHex=$1;
	$librarySbssName=$5;
	$sbssLengthHex=$2;
	$sbssLengthDec=hex($sbssLengthHex);
	$level1=$3;
	$level2=$4;
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
	$sbssStartAddressDec=hex($sbssStartAddressHex);
	if($flagCurrSbss==1)
	{		
	$sbssStartAddressDec=hex($sbssStartAddressHex);
	if($sbssStartAddressDec == 0)
	{
		goto sbssoutSection;
	}
	$SizeOfLib=$sbssStartAddressDec-$sbssStartAddressDecPrev;
	$libarraysbss_name{$librarySbssNamePrev}=$libarraysbss_name{$librarySbssNamePrev}+$SizeOfLib;
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{		
	$level1Prev = $level2Prev;		
	}
	$libarraysbss_name1{$librarySbssNamePrev}=$level1Prev;
	}
	$flagCurrSbss=1;
	goto labelSbss;
}
elsif(($inputline =~ /^\s+\.sbss\.(\w+)/)|| ($inputline =~ /^\s+QURTK\.(.+)\.sbss/) || ($inputline =~ /^\s+QURT\.(.+)\.sbss/) || ($inputline =~ /^\s+\.sbss\.(.+)/))
{
	$inputline=<TRACEFILE>;
	if(($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.sbss\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;
		$sbssStartAddressHex=$1;
		$librarySbssName=$5;
		$sbssLengthHex=$2;
		$sbssLengthDec=hex($sbssLengthHex);
		$level1=$3;
		$level2=$4;
		if($level2 eq "HEXAGON_Tools")
		{
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";
		}
		$sbssStartAddressDec=hex($sbssStartAddressHex);
		if($flagCurrSbss==1)
		{		
		$sbssStartAddressDec=hex($sbssStartAddressHex);
		if($sbssStartAddressDec == 0)
		{		
		goto sbssoutSection;
		}
		$SizeOfLib=$sbssStartAddressDec-$sbssStartAddressDecPrev;
		$libarraysbss_name{$librarySbssNamePrev}=$libarraysbss_name{$librarySbssNamePrev}+$SizeOfLib;
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;		
		}
		$libarraysbss_name1{$librarySbssNamePrev}=$level1Prev;
		}
		$flagCurrSbss=1;
		goto labelSbss;
	}
labelSbss:
if($sbssStartAddressDec != 0)
{
$sbssStartAddressHexPrev=$sbssStartAddressHex;
$sbssStartAddressDecPrev=hex($sbssStartAddressHexPrev);
$sbssLengthDecPrev=$sbssLengthDec;
$librarySbssNamePrev=$librarySbssName;
$level1Prev=$level1;
$level2Prev=$level2;
$sbsslevel1Prev=$level1Prev;
}
sbssoutSection:	
}
}
#################################BSS#####################################
if (($inputline =~ /^\.bss\s+0x(\w+)\s+0x(\w+)/) || ($bssStartFlag == 1))
{
$textStartFlag = 0;
$rodataStartFlag = 0;
$dataStartFlag = 0;
$sdataStartFlag = 0;
$sbssStartFlag = 0;
$bssStartAddrHex=$1;
$overallBssSizeHex=$2;
$bssStartAddrDec=hex($bssStartAddrHex);
$bssStartAddrDec2=$bssStartAddrDec;
$overallBssSizeDec=hex($overallBssSizeHex);
$bssEndAddrDec=$bssStartAddrDec+$overallBssSizeDec;
$bssEndAddrHex=dectohex($bssEndAddrDec);
$bssStartFlag = 1;
if(($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\w+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+).\w+/) || ($inputline =~ /^\s+\.COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\.\d+\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /^\s+LPMDEC\.data\s+0x(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) ||  ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\COMMON\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+\.bss\..+\s+0x(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
{
    $bssSingleLine = 1;
	$flag=0;
	$bssStartAddressHex=$1;
	$libraryBssName=$5;
	$bssLengthHex=$2;
	$bssLengthDec=hex($bssLengthHex);
	$level1=$3;
	$level2=$4;
	if($level2 eq "HEXAGON_Tools")
	{
	$level1 = "qurt_Tools";
	$level2 = "Hexagon_Tools";
	}
	$bssStartAddressDec=hex($bssStartAddressHex);
	if($flagCurrBss==1)
	{		
	$bssStartAddressDec=hex($bssStartAddressHex);
	if($bssStartAddressDec == 0)
	{
		goto bssoutSection;
	}	
	$SizeOfLib=$bssStartAddressDec-$bssStartAddressDecPrev;
	$libarraybss_name{$libraryBssNamePrev}=$libarraybss_name{$libraryBssNamePrev}+$SizeOfLib;
	if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
	{
	$level1Prev = $level2Prev;
	}
	if(($level1Prev eq "avs") || ($level1Prev eq "core"))
	{		
	$level1Prev = $level2Prev;		
	}
	$libarraybss_name1{$libraryBssNamePrev}=$level1Prev;
	print OUTRPTFILE1 ("\n$SizeOfLib:$libraryBssNamePrev:$level1Prev:$bssStartAddressHexPrev:$bssStartAddressDecPrev:$bssStartAddressHex:$bssStartAddressDec\n");
	}
	$flagCurrBss=1;
	goto labelBss;
}
elsif(($inputline =~ /^\s+\.bss\.(\w+)/)|| ($inputline =~ /^\s+QURTK\.(.+)\.bss/) || ($inputline =~ /^\s+QURT\.(.+)\.bss/) || ($inputline =~ /^\s+lpa\_audio\_lpm\.data/) || ($inputline =~ /^\s+\.bss\.(.+)/))
{
	$inputline=<TRACEFILE>;
	if(($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+(\w+)\:\/.+\/(HEXAGON_Tools)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /^\s+\.bss\s+0x(\w+)\s+(\w+)\s+(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /^\s+(\w+)\s+(\w+)\s+\.\/(\w+)\/(.+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)(\/)(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/adsp_proc\/(\w+)\/(\w+)\/(\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+\-\w+)\/.+\/(\w+\-\w+)\.\w+(.+)/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+/) || ($inputline =~ /\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+)\/.+\/(\w+)\.\w+(.+)/))
	{
		$flag=0;
		$bssStartAddressHex=$1;
		$libraryBssName=$5;
		$bssLengthHex=$2;
		$bssLengthDec=hex($bssLengthHex);
		$level1=$3;
		$level2=$4;
		if($level2 eq "HEXAGON_Tools")
		{
			$level1 = "qurt_Tools";
			$level2 = "Hexagon_Tools";
		}
		$bssStartAddressDec=hex($bssStartAddressHex);			
		if(($bssStartFlagCurr==0) && ($bssSingleLine ==0))
		{
		    #@die"pramod";
			if($bssStartAddressDec == $bssStartAddrDec2)
			{
			}
			else
			{
			#print"\n$textStartAddressDec\n";
			#print"\n$textStartAddrDec2\n";
			$SizeOfLib=$bssStartAddressDec-$bssStartAddrDec2;
			#print"\n$SizeOfLib\n";
			$libraryBssNamePrev="BssAlignmentStart";
			$libarraybss_name{$libraryBssNamePrev}=$libarraybss_name{$libraryBssNamePrev}+$SizeOfLib;
			#print"\n$libarraytext_name{$libraryTextNamePrev}\n";
			$libarraybss_name1{$libraryBssNamePrev}="BssAlignmentStart";
			#print"\n$libarraytext_name1{$libraryTextNamePrev}\n";
			#$textStartAddressDecPrev=$textStartAddressDec2;
			#$level1="TextAlignmentStart";
			#$level2="TextAlignmentStart";
			#die"outtext";
			}
		}
		$bssStartFlagCurr=1;		
		if($flagCurrBss==1)
		{		
		$bssStartAddressDec=hex($bssStartAddressHex);
		if($bssStartAddressDec == 0)
		{		
		goto bssoutSection;
		}		
		$SizeOfLib=$bssStartAddressDec-$bssStartAddressDecPrev;
		$libarraybss_name{$libraryBssNamePrev}=$libarraybss_name{$libraryBssNamePrev}+$SizeOfLib;
		if(($level2Prev eq "voc") || ($level2Prev eq "vocproc"))
		{
		$level1Prev = $level2Prev;
		}
		if(($level1Prev eq "avs") || ($level1Prev eq "core"))
		{		
		$level1Prev = $level2Prev;		
		}
		$libarraybss_name1{$libraryBssNamePrev}=$level1Prev;
		print OUTRPTFILE1 ("\n$SizeOfLib:$libraryBssNamePrev:$level1Prev:$bssStartAddressHexPrev:$bssStartAddressDecPrev:$bssStartAddressHex:$bssStartAddressDec\n");
		}
		$flagCurrBss=1;
		goto labelBss;
	}
labelBss:
if($bssStartAddressDec != 0)
{
$bssStartAddressHexPrev=$bssStartAddressHex;
$bssStartAddressDecPrev=hex($bssStartAddressHexPrev);
$bssLengthDecPrev=$bssLengthDec;
$libraryBssNamePrev=$libraryBssName;
$level1Prev=$level1;
$level2Prev=$level2;
$bsslevel1Prev=$level1Prev;
}
bssoutSection:	
}
}
$inputline=<TRACEFILE>;
}
}
else
{
die"out:Not able to open the map file";
}
$SizeOfLib=$textEndAddrDec-$textStartAddressDecPrev;
#print"\n$textEndAddrDec\n";
#print"\n$textStartAddressDecPrev\n";
#die"out";
$libarraytext_name{$libraryTextNamePrev}=$libarraytext_name{$libraryTextNamePrev}+$SizeOfLib;
$libarraytext_name1{$libraryTextNamePrev}=$textlevel1Prev;
$SizeOfLib=$rodataEndAddrDec-$rodataStartAddressDecPrev;
#print"\n$rodataEndAddrDec\n";
#print"\n$rodataStartAddressDecPrev\n";
#die"out";
$libarrayrodata_name{$libraryRodataNamePrev}=$libarrayrodata_name{$libraryRodataNamePrev}+$SizeOfLib;
$libarrayrodata_name1{$libraryRodataNamePrev}=$rodatalevel1Prev;
$SizeOfLib=$dataEndAddrDec-$dataStartAddressDecPrev;
#print "\n$dataEndAddrDec $dataStartAddressDecPrev $SizeOfLib\n";
#die "prem:out";
$libarraydata_name{$libraryDataNamePrev}=$libarraydata_name{$libraryDataNamePrev}+$SizeOfLib;
$libarraydata_name1{$libraryDataNamePrev}=$datalevel1Prev;
$SizeOfLib=$sdataEndAddrDec-$sdataStartAddressDecPrev;
$libarraysdata_name{$librarySdataNamePrev}=$libarraysdata_name{$librarySdataNamePrev}+$SizeOfLib;
$libarraysdata_name1{$librarySdataNamePrev}=$sdatalevel1Prev;
if($sbssLengthDecPrev != 0)
{
$SizeOfLib=$sbssEndAddrDec-$sbssStartAddressDecPrev;
$libarraysbss_name{$librarySbssNamePrev}=$libarraysbss_name{$librarySbssNamePrev}+$SizeOfLib;
$libarraysbss_name1{$librarySbssNamePrev}=$sbsslevel1Prev;
}
else
{
$libarraysbss_name{$librarySbssNamePrev}=$libarraysbss_name{$librarySbssNamePrev}+0;
$libarraysbss_name1{$librarySbssNamePrev}=$sbsslevel1Prev;
}
$SizeOfLib=$bssEndAddrDec-$bssStartAddressDecPrev;
$libarraybss_name{$libraryBssNamePrev}=$libarraybss_name{$libraryBssNamePrev}+$SizeOfLib;
$libarraybss_name1{$libraryBssNamePrev}=$bsslevel1Prev;
print OUTRPTFILE ("Library Name \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t Total Size KB \t ComponentName\n");
$i=keys( %libarraytext_name );
$j=keys( %libarrayrodata_name );
$k=keys( %libarraydata_name );
$l=keys( %libarraysdata_name );
$m=keys( %libarraysbss_name );
$n=keys( %libarraybss_name );
$textcmp=0;
$rodatacmp=0;
$datacmp=0;
$sdatacmp=0;
$sbsscmp=0;
$bsscmp=0;
#print"\n$i\n";
#print"\n$j\n";
#print"\n$k\n";
#print"\n$l\n";
#print"\n$m\n";
#print"\n$n\n";
if(($i>=$j) && ($i>=$k) && ($i>=$l) && ($i>=$m) && ($i>=$n))
{
#print"\n1\n";
#print"\n$i\n";
foreach $keys (keys %libarraytext_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
#$componentBss{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
$flag=1;
foreach $keys (keys %libarraydata_name) {
foreach $keys1 (keys %libarraytext_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab431;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}
}
lab431:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysbss_name) {
foreach $keys1 (keys %libarraytext_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab432;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab432:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraybss_name) {
foreach $keys1 (keys %libarraytext_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab433;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab433:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraysdata_name) {
foreach $keys1 (keys %libarraytext_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab434;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab434:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarrayrodata_name) {
foreach $keys1 (keys %libarraytext_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab435;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab435:
$flag=1;
}
}
elsif(($j>=$i) && ($j>=$k) && ($j>=$l) && ($j>=$m) && ($j>=$n))
{
#print"\n2\n";
#print"\n$j\n";
foreach $keys (keys %libarrayrodata_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}


$flag=1;
foreach $keys (keys %libarraydata_name) {
foreach $keys1 (keys %libarrayrodata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab436;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}
}
lab436:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysbss_name) {
foreach $keys1 (keys %libarrayrodata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab437;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab437:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraybss_name) {
foreach $keys1 (keys %libarrayrodata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab438;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab438:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraysdata_name) {
foreach $keys1 (keys %libarrayrodata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab439;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab439:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraytext_name) {
foreach $keys1 (keys %libarrayrodata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab440;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab440:
$flag=1;
}



}
elsif(($k>=$i) && ($k>=$j) && ($k>=$l) && ($k>=$m) && ($k>=$n))
{
#print"\n3\n";
#print"\n$k\n";
foreach $keys (keys %libarraydata_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}


$flag=1;
foreach $keys (keys %libarrayrodata_name) {
foreach $keys1 (keys %libarraydata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab441;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}
}
lab441:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysbss_name) {
foreach $keys1 (keys %libarraydata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab442;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab442:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraybss_name) {
foreach $keys1 (keys %libarraydata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab443;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab443:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraysdata_name) {
foreach $keys1 (keys %libarraydata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab444;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab444:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraytext_name) {
foreach $keys1 (keys %libarraydata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab445;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab445:
$flag=1;
}


}
elsif(($l>=$i) && ($l>=$j) && ($l>=$k) && ($l>=$m) && ($l>=$n))
{
#print"\n4\n";
#print"\n$l\n";
foreach $keys (keys %libarraysdata_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

$flag=1;
foreach $keys (keys %libarrayrodata_name) {
foreach $keys1 (keys %libarraysdata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab446;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}
}
lab446:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysbss_name) {
foreach $keys1 (keys %libarraysdata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab447;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab447:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraybss_name) {
foreach $keys1 (keys %libarraysdata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab448;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab448:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraydata_name) {
foreach $keys1 (keys %libarraysdata_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab449;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab449:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraytext_name) {
foreach $keys1 (keys %libarraysdata_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab450;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab450:
$flag=1;
}



}
elsif(($m>=$i) && ($m>=$j) && ($m>=$k) && ($m>=$l) && ($m>=$n))
{
#print"\n5\n";
#print"\n$m\n";
foreach $keys (keys %libarraysbss_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
}


$flag=1;
foreach $keys (keys %libarrayrodata_name) {
foreach $keys1 (keys %libarraysbss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab451;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}
}
lab451:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysdata_name) {
foreach $keys1 (keys %libarraysbss_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab452;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab452:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraybss_name) {
foreach $keys1 (keys %libarraysbss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab453;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab453:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraydata_name) {
foreach $keys1 (keys %libarraysbss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab454;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab454:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraytext_name) {
foreach $keys1 (keys %libarraysbss_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab455;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab455:
$flag=1;
}



}
elsif(($n>=$i) && ($n>=$j) && ($n>=$k) && ($n>=$l) && ($n>=$m))
{
#print"\n6\n";
#print"\n$n\n";
foreach $keys (keys %libarraybss_name) {
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraybss_name1{$keys}}=$componentText{$libarraybss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraybss_name1{$keys}}=$componentRodata{$libarraybss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraybss_name1{$keys}}=$componentData{$libarraybss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraybss_name1{$keys}}=$componentSdata{$libarraybss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraybss_name1{$keys}}=$componentSbss{$libarraybss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraybss_name1{$keys}}=$componentBss{$libarraybss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraybss_name1{$keys}\n");
}

$flag=1;
foreach $keys (keys %libarrayrodata_name) {
foreach $keys1 (keys %libarraybss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
#print OUTRPTFILE123 ("Inner flag is: $flag\n");
goto lab456;
}
}

#print OUTRPTFILE123 ("outer flag is: $flag\n");
if($flag == 1)
{

$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarrayrodata_name1{$keys}}=$componentText{$libarrayrodata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarrayrodata_name1{$keys}}=$componentRodata{$libarrayrodata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarrayrodata_name1{$keys}}=$componentData{$libarrayrodata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarrayrodata_name1{$keys}}=$componentSdata{$libarrayrodata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarrayrodata_name1{$keys}}=$componentSbss{$libarrayrodata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarrayrodata_name1{$keys}}=$componentBss{$libarrayrodata_name1{$keys}}+$libarraybss_name{$keys};
#print"\n1:$keys\n";
##push(@extralibs,$keys);
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarrayrodata_name1{$keys}\n");
}
}
lab456:
$flag=1;
}
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
$flag=1;
foreach $keys (keys %libarraysdata_name) {
foreach $keys1 (keys %libarraybss_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab457;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab432;
##}
##}
##push(@extralibs,$keys);

#print"\n2:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysdata_name1{$keys}}=$componentText{$libarraysdata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysdata_name1{$keys}}=$componentRodata{$libarraysdata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysdata_name1{$keys}}=$componentData{$libarraysdata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysdata_name1{$keys}}=$componentSdata{$libarraysdata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysdata_name1{$keys}}=$componentSbss{$libarraysdata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysdata_name1{$keys}}=$componentBss{$libarraysdata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysdata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab457:
$flag=1;
}
$flag=1;
##print"\n@extralibs\n";
##$length=@extralibs;
##print"\n$length\n";
foreach $keys (keys %libarraysbss_name) {
foreach $keys1 (keys %libarraybss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab458;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraysbss_name1{$keys}}=$componentText{$libarraysbss_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraysbss_name1{$keys}}=$componentRodata{$libarraysbss_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraysbss_name1{$keys}}=$componentData{$libarraysbss_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraysbss_name1{$keys}}=$componentSdata{$libarraysbss_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraysbss_name1{$keys}}=$componentSbss{$libarraysbss_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraysbss_name1{$keys}}=$componentBss{$libarraysbss_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraysbss_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab458:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraydata_name) {
foreach $keys1 (keys %libarraybss_name) {
#print OUTRPTFILE123 ("\n$keys = $keys1\n");
if($keys eq $keys1)
{
$flag=0;
goto lab459;
}
}

if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraydata_name1{$keys}}=$componentText{$libarraydata_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraydata_name1{$keys}}=$componentRodata{$libarraydata_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraydata_name1{$keys}}=$componentData{$libarraydata_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraydata_name1{$keys}}=$componentSdata{$libarraydata_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraydata_name1{$keys}}=$componentSbss{$libarraydata_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraydata_name1{$keys}}=$componentBss{$libarraydata_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraydata_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab459:
$flag=1;
}
$flag=1;
foreach $keys (keys %libarraytext_name) {
foreach $keys1 (keys %libarraybss_name) {
if($keys eq $keys1)
{
$flag=0;
goto lab460;
}
}
if($flag == 1)
{
##for($el=0;$el<$length;$el++)
##{
##if($extralibs[$el] eq $keys)
##{
##goto lab433;
##}
##}
##push(@extralibs,$keys);

#print"\n3:$keys\n";
$textcmp=$textcmp+$libarraytext_name{$keys};
$rodatacmp=$rodatacmp+$libarrayrodata_name{$keys};
$datacmp=$datacmp+$libarraydata_name{$keys};
$sdatacmp=$sdatacmp+$libarraysdata_name{$keys};
$sbsscmp=$sbsscmp+$libarraysbss_name{$keys};
$bsscmp=$bsscmp+$libarraybss_name{$keys};
$totalSize=$libarraytext_name{$keys}+$libarrayrodata_name{$keys}+$libarraydata_name{$keys}+$libarraysdata_name{$keys}+$libarraysbss_name{$keys}+$libarraybss_name{$keys};
$totalSizeKB=$totalSize/1024;
$totalSizeKB=sprintf("%.2f", $totalSizeKB);
$componentText{$libarraytext_name1{$keys}}=$componentText{$libarraytext_name1{$keys}}+$libarraytext_name{$keys};
$componentRodata{$libarraytext_name1{$keys}}=$componentRodata{$libarraytext_name1{$keys}}+$libarrayrodata_name{$keys};
$componentData{$libarraytext_name1{$keys}}=$componentData{$libarraytext_name1{$keys}}+$libarraydata_name{$keys};
$componentSdata{$libarraytext_name1{$keys}}=$componentSdata{$libarraytext_name1{$keys}}+$libarraysdata_name{$keys};
$componentSbss{$libarraytext_name1{$keys}}=$componentSbss{$libarraytext_name1{$keys}}+$libarraysbss_name{$keys};
$componentBss{$libarraytext_name1{$keys}}=$componentBss{$libarraytext_name1{$keys}}+$libarraybss_name{$keys};
if ($libarraytext_name{$keys} eq "")
{
$libarraytext_name{$keys}=0;
}
if ($libarrayrodata_name{$keys} eq "")
{
$libarrayrodata_name{$keys}=0;
}
if ($libarraydata_name{$keys} eq "")
{
$libarraydata_name{$keys}=0;
}
if ($libarraysdata_name{$keys} eq "")
{
$libarraysdata_name{$keys}=0;
}
if ($libarraysbss_name{$keys} eq "")
{
$libarraysbss_name{$keys}=0;
}
if ($libarraybss_name{$keys} eq "")
{
$libarraybss_name{$keys}=0;
}
#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
if($keys ne "")
{
print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}

#print OUTRPTFILE ("$keys \t $libarraytext_name{$keys} \t $libarrayrodata_name{$keys} \t $libarraydata_name{$keys} \t $libarraysdata_name{$keys} \t $libarraysbss_name{$keys} \t $libarraybss_name{$keys} \t $totalSizeKB \t $libarraytext_name1{$keys}\n");
}
lab460:
$flag=1;
}



}
print OUTRPTFILE ("\n\n\n");
print OUTRPTFILE ("ComponentName \t .text \t .rodata \t .data \t .sdata \t .sbss \t .bss \t Total Size KB\n");
foreach $keys (keys %componentText) {
$totalComSize=$componentText{$keys}+$componentRodata{$keys}+$componentData{$keys}+$componentSdata{$keys}+$componentSbss{$keys}+$componentBss{$keys};
$totalComSizeKB=$totalComSize/1024;
$totalComSizeKB=sprintf("%.2f", $totalComSizeKB);
print OUTRPTFILE ("$keys \t $componentText{$keys} \t $componentRodata{$keys} \t $componentData{$keys} \t $componentSdata{$keys} \t $componentSbss{$keys} \t $componentBss{$keys} \t $totalComSizeKB\n");
}
$overallStartSizeDecKB=$overallStartSizeDec/1024;
$overallStartSizeDecKB=sprintf("%.2f", $overallStartSizeDecKB);
print OUTRPTFILE ("\nThe total .start size in KB:\t$overallStartSizeDecKB");
$overallInitSizeDecKB=$overallInitSizeDec/1024;
$overallInitSizeDecKB=sprintf("%.2f", $overallInitSizeDecKB);
print OUTRPTFILE ("\nThe total .init size in KB:\t$overallInitSizeDecKB");
$textcmpKB=$textcmp/1024;
$textcmpKB=sprintf("%.2f", $textcmpKB);
print OUTRPTFILE ("\nThe total .text size in KB:\t$textcmpKB");
$overallFiniSizeDecKB=$overallFiniSizeDec/1024;
$overallFiniSizeDecKB=sprintf("%.2f", $overallFiniSizeDecKB);
print OUTRPTFILE ("\nThe total .fini size in KB:\t$overallFiniSizeDecKB");
$rodatacmpKB=$rodatacmp/1024;
$rodatacmpKB=sprintf("%.2f", $rodatacmpKB);
print OUTRPTFILE ("\nThe total .rodata size in KB:\t$rodatacmpKB");
$datacmpKB=$datacmp/1024;
$datacmpKB=sprintf("%.2f", $datacmpKB);
print OUTRPTFILE ("\nThe total .data size in KB:\t$datacmpKB");
$overallqurt_context_tcmSizeDecKB=$overallqurt_context_tcmSizeDec/1024;
$overallqurt_context_tcmSizeDecKB=sprintf("%.2f", $overallqurt_context_tcmSizeDecKB);
print OUTRPTFILE ("\nThe total QURTK.CONTEXTS.TCM.data size in KB:\t$overallqurt_context_tcmSizeDecKB");
$overalla8974_devcfg_dataSizeDecKB=$overalla8974_devcfg_dataSizeDec/1024;
$overalla8974_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla8974_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a8974_DEVCFG_DATA size in KB:\t$overalla8974_devcfg_dataSizeDecKB");
$overalla8626_devcfg_dataSizeDecKB=$overalla8626_devcfg_dataSizeDec/1024;
$overalla8626_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla8626_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a8x26_DEVCFG_DATA size in KB:\t$overalla8626_devcfg_dataSizeDecKB");
$overalla9625_devcfg_dataSizeDecKB=$overalla9625_devcfg_dataSizeDec/1024;
$overalla9625_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla9625_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a9625_DEVCFG_DATA size in KB:\t$overalla9625_devcfg_dataSizeDecKB");
$overalla8610_devcfg_dataSizeDecKB=$overalla8610_devcfg_dataSizeDec/1024;
$overalla8610_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla8610_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a8610_DEVCFG_DATA size in KB:\t$overalla8610_devcfg_dataSizeDecKB");
$overalla8092_devcfg_dataSizeDecKB=$overalla8092_devcfg_dataSizeDec/1024;
$overalla8092_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla8092_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a8092_DEVCFG_DATA size in KB:\t$overalla8092_devcfg_dataSizeDecKB");
$overalla8094_devcfg_dataSizeDecKB=$overalla8094_devcfg_dataSizeDec/1024;
$overalla8094_devcfg_dataSizeDecKB=sprintf("%.2f", $overalla8094_devcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total a8094_DEVCFG_DATA size in KB:\t$overalla8094_devcfg_dataSizeDecKB");
$overalldevcfg_dataSizeDecKB=$overalldevcfg_dataSizeDec/1024;
$overalldevcfg_dataSizeDecKB=sprintf("%.2f", $overalldevcfg_dataSizeDecKB);
print OUTRPTFILE ("\nThe total .DEVCFG_DATA size in KB:\t$overalldevcfg_dataSizeDecKB");
$overallEh_frameSizeDecKB=$overallEh_frameSizeDec/1024;
$overallEh_frameSizeDecKB=sprintf("%.2f", $overallEh_frameSizeDecKB);
print OUTRPTFILE ("\nThe total .eh_frame size in KB:\t$overallEh_frameSizeDecKB");


$overallGotSizeDecKB=$overallGotSizeDec/1024;
$overallGotSizeDecKB=sprintf("%.2f", $overallGotSizeDecKB);
print OUTRPTFILE ("\nThe total .got size in KB:\t$overallGotSizeDecKB");
$overallInterpSizeDecKB=$overallInterpSizeDec/1024;
$overallInterpSizeDecKB=sprintf("%.2f", $overallInterpSizeDecKB);
print OUTRPTFILE ("\nThe total .interp size in KB:\t$overallInterpSizeDecKB");
$overallDynamicSizeDecKB=$overallDynamicSizeDec/1024;
$overallDynamicSizeDecKB=sprintf("%.2f", $overallDynamicSizeDecKB);
print OUTRPTFILE ("\nThe total .dynamic size in KB:\t$overallDynamicSizeDecKB");
$overallHashSizeDecKB=$overallHashSizeDec/1024;
$overallHashSizeDecKB=sprintf("%.2f", $overallHashSizeDecKB);
print OUTRPTFILE ("\nThe total .hash size in KB:\t$overallHashSizeDecKB");
$overallDynsymSizeDecKB=$overallDynsymSizeDec/1024;
$overallDynsymSizeDecKB=sprintf("%.2f", $overallDynsymSizeDecKB);
print OUTRPTFILE ("\nThe total .dynsym size in KB:\t$overallDynsymSizeDecKB");
$overallDynstrSizeDecKB=$overallDynstrSizeDec/1024;
$overallDynstrSizeDecKB=sprintf("%.2f", $overallDynstrSizeDecKB);
print OUTRPTFILE ("\nThe total .dynstr size in KB:\t$overallDynstrSizeDecKB");


$overallGcc_except_tableSizeDecKB=$overallGcc_except_tableSizeDec/1024;
$overallGcc_except_tableSizeDecKB=sprintf("%.2f", $overallGcc_except_tableSizeDecKB);
print OUTRPTFILE ("\nThe total .gcc_except_table size in KB:\t$overallGcc_except_tableSizeDecKB");
$overallCtorsSizeDecKB=$overallCtorsSizeDec/1024;
$overallCtorsSizeDecKB=sprintf("%.2f", $overallCtorsSizeDecKB);
print OUTRPTFILE ("\nThe total .ctors size in KB:\t$overallCtorsSizeDecKB");
$overallDtorsSizeDecKB=$overallDtorsSizeDec/1024;
$overallDtorsSizeDecKB=sprintf("%.2f", $overallDtorsSizeDecKB);
print OUTRPTFILE ("\nThe total .dtors size in KB:\t$overallDtorsSizeDecKB");
$sdatacmpKB=$sdatacmp/1024;
$sdatacmpKB=sprintf("%.2f", $sdatacmpKB);
print OUTRPTFILE ("\nThe total .sdata size in KB:\t$sdatacmpKB");
$sbsscmpKB=$sbsscmp/1024;
$sbsscmpKB=sprintf("%.2f", $sbsscmpKB);
print OUTRPTFILE ("\nThe total .sbss size in KB:\t$sbsscmpKB");
$bsscmpKB=$bsscmp/1024;
$bsscmpKB=sprintf("%.2f", $bsscmpKB);
print OUTRPTFILE ("\nThe total .bss size in KB:\t$bsscmpKB");
#if(($targetName eq "8x10") || ($targetName eq "8610"))
#{
$qsr_stringdec = 0;
$qsr_stringdecKB = 0;
if($PDName eq "CommonPD")
{
if (open(SECFILE,"section_info.txt")){
$sectionline=<SECFILE>;
while ( $sectionline ne "" ) {
if(($sectionline =~ /\d+\s+\.(\w+)\s+(\w+)\s+/) || ($sectionline =~ /\d+\s+(\w+)\s+(\w+)\s+(\w+)/))
{
$section_name=$1;
$size_section=$2;
$end_section_addr=$3;

#print"\n$section_name\n";
if($section_name eq "QSR_STRING")
{
$Sectrion_end_address=$end_section_addr;
#print"\n$section_name\n";
#print"\n$Sectrion_end_address\n";
$Sectrion_end_address=hex($Sectrion_end_address);
$size_section=hex($size_section);
$qsr_stringdec=$size_section;
$bssEndAddrDec=$Sectrion_end_address+$size_section;
$qsr_stringdecKB=$qsr_stringdec/1024;
$qsr_stringdecKB=sprintf("%.2f", $qsr_stringdecKB);
}
}
$sectionline=<SECFILE>;
}
}
else
{
die"Out\: Not able to open Sectio\_Info\.txt file";
}
#}
print OUTRPTFILE ("\nThe total QSR_STRING size in KB:\t$qsr_stringdecKB");
}
$OverallImageSize1=$textcmp+$rodatacmp+$datacmp+$sdatacmp+$sbsscmp+$bsscmp+$overallStartSizeDec+$overallInitSizeDec+$overallFiniSizeDec+$overallEh_frameSizeDec+$overallGcc_except_tableSizeDec+$overallCtorsSizeDec+$overallDtorsSizeDec+$overalldevcfg_dataSizeDec+$overallqurt_context_tcmSizeDec+$overalla8974_devcfg_dataSizeDec+$overalla8626_devcfg_dataSizeDec+$overalla9625_devcfg_dataSizeDec+$overalla8610_devcfg_dataSizeDec+$overalla8092_devcfg_dataSizeDec+$overalla8094_devcfg_dataSizeDec+$overallDynstrSizeDec+$overallDynsymSizeDec+$overallHashSizeDec+$overallDynamicSizeDec+$overallInterpSizeDec+$overallGotSizeDec+$qsr_stringdec;
$OverallImageSize=$OverallImageSize1/1024;
$OverallImageSize=$OverallImageSize/1024;
$OverallImageSize=sprintf("%.2f", $OverallImageSize);

$sizeIncludeExtAlign=$bssEndAddrDec-$startStartAddrDec;
#print"\n$bssEndAddrDec\n";
#print"\n$startStartAddrDec\n";
$sizeIncludeExtAlignKB=$sizeIncludeExtAlign/1024;
$sizeIncludeExtAlignKB=sprintf("%.2f", $sizeIncludeExtAlignKB);
$extHoleBtwSections=abs($sizeIncludeExtAlign-$OverallImageSize1);
$extHoleBtwSections=$extHoleBtwSections/1024;
$extHoleBtwSections=sprintf("%.2f", $extHoleBtwSections);
print OUTRPTFILE ("\nHole size  due to Align btw the sections in KB:\t$extHoleBtwSections\n");
print OUTRPTFILE ("\nThe Overall Image size is in KB:\t$sizeIncludeExtAlignKB\n");
###############################################################################################################################################
#Function: dectohex
###############################################################################################################################################
#Description: This funtion is used to convert the decimal number to hex.
#Inputs: decnum: decimal number.
#Outputs: hexnum: hexadecimal.
################################################################################################################################################
sub dectohex {
my $decnum = $_[0];
my $hexnum;
my $tempval;
while ($decnum != 0) {
$tempval = $decnum % 16;
if ($tempval > 9)
{
$tempval = chr($tempval + 55);
}
$hexnum = $tempval . $hexnum ;
$decnum = int($decnum / 16);
if ($decnum < 16)
{
if ($decnum > 9)
{
$decnum = chr($decnum + 55);
}
$hexnum = $decnum . $hexnum;
$decnum = 0
}
}
return $hexnum;
}
