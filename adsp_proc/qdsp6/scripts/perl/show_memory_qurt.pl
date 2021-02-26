use POSIX;
use File::Path;
# Written by Waleed Eshmawy
#$Hex_img_size = $ARGV[0];
#$Dec_img_size = hex($ARGV[0]);
#$Hex_image_start_addr = $ARGV[1];
#$Dec_image_start_addr = hex($ARGV[1]);
$image_name=$ARGV[0];
$CHIPSET_folder = $ARGV[1];
$BUILD_Flavour = $ARGV[2];

$HexMemSize{"SIZE_16M"}="0x1000000";
$HexMemSize{"SIZE_8M"}="0x800000";
$HexMemSize{"SIZE_4M"}="0x400000";
$HexMemSize{"SIZE_2M"}="0x200000";
$HexMemSize{"SIZE_1M"}="0x100000";
$HexMemSize{"SIZE_64K"}="0x10000";
$HexMemSize{"SIZE_32K"}="0x8000";
$HexMemSize{"SIZE_16K"}="0x4000";
$HexMemSize{"SIZE_8K"}="0x2000";
$HexMemSize{"SIZE_4K"}="0x1000";
$HexMemSize{"SIZE_2K"}="0x800";
$HexMemSize{"SIZE_1K"}="0x400";
$DecMemSize{"SIZE_16M"}=hex($HexMemSize{"SIZE_16M"});
$DexMemSize{"SIZE_8M"}=hex($HexMemSize{"SIZE_8M"});
$DecMemSize{"SIZE_4M"}=hex($HexMemSize{"SIZE_4M"});
$DecMemSize{"SIZE_2M"}=hex($HexMemSize{"SIZE_2M"});
$DecMemSize{"SIZE_1M"}=hex($HexMemSize{"SIZE_1M"});
$DecMemSize{"SIZE_64K"}=hex($HexMemSize{"SIZE_64K"});
$DecMemSize{"SIZE_32K"}=hex($HexMemSize{"SIZE_32K"});
$DecMemSize{"SIZE_16K"}=hex($HexMemSize{"SIZE_16K"});
$DecMemSize{"SIZE_8K"}=hex($HexMemSize{"SIZE_8K"});
$DecMemSize{"SIZE_4K"}=hex($HexMemSize{"SIZE_4K"});
$DecMemSize{"SIZE_2K"}=hex($HexMemSize{"SIZE_2K"});
$DecMemSize{"SIZE_1K"}=hex($HexMemSize{"SIZE_1K"});

#print "dec 16M = $DecMemSize{\"SIZE_16M\"}\n";
#print "Hex 16M = $HexMemSize{\"SIZE_16M\"}\n";
print "\nAnalyzing memory...\n\n";

#$output_file = "> memory.csv";
#open output_file or die "failed to open $output_file :$!";  


#&getTotalHeap;
&getDefault_phy_mem_size;
#&getAPP_heapSize;
#&getStaticAllocatedMemSize;
&getImageAnalysis;
&getExternalHeapSize;
#&getFreeMemSize;
#&PrintOutput;
#&deleteMetafile;

if($FreeMem_size_d<0){
   print "Run out of memory...\n";
   print "Deleting $image_name\n";
  # system("rm -f $image_name");
}
sub getTotalHeap {
   $src_file = "./build/chipset/$CHIPSET_folder/q6_memory_defns.h";
   open src_file or die "Cannot open file $src_file\n";

   #print " 1. Trying to get the TotalHeap from $src_file\n";
   
   while(<src_file>)
   {
      if ($_ =~ /TOTAL_HEAP/){
         
         #print "line:$_\n";
         @line_array = split(' ',$_);
         
         $Total_Heap =$line_array[2];
         $Hex_heap_size=sprintf("0x%x00000", $Total_Heap);
         $Dec_heap_size=hex($Hex_heap_size);
      }
   }
   if (!$Total_Heap ) {
      print "the Total_Heap not found in $src_file\n";
      exit;
   }else{
      
      #print "    Heap size = $Hex_heap_size\n";
     # print "    Heap size d= $Dec_heap_size\n";
    
     
   }
  
   close $src_file;
}
sub getDefault_phy_mem_size {
   $src_file = "./build/chipset/$CHIPSET_folder/qdsp6.xml";
   open src_file or die "Cannot open file $src_file\n";

   print " 1. Trying to get the DEFAULT_PHYSPOOL start address and size from $src_file\n";
   $lineCnt=0;
   $def_phy_pool_line_offset=-1;
   $tracesizelineoffset = -1;

   while(<src_file>)
   {
      if ($_ =~ /DEFAULT_PHYSPOOL/){
      
         #print "default phy pool section line num :$lineCnt\n";
         $def_phy_pool_line_offset=0;
        
      }
	  if ($_ =~ /trace_size/){
      
         #print "default phy pool section line num :$lineCnt\n";
		 #print"The trace size line is printed";
         $tracesizelineoffset=0;
		 #print "trace line is: $_";
		 #die"out";
        
      }
	  if($tracesizelineoffset == 0)
	  {
	  $tracesizelineoffset =1;
	  #print "line:$_\n";
	  $traceline=$_;
	  if($traceline =~ /.+\"(\d+)\".+/)
	  {
	  $tracesize = $1;
	  $tracesize = $tracesize*3;
	  $tarcenum=$tracesize/1024;	  
	  if($tarcenum <= 4)
	  {
	  $tracesizebytes=4*1024;
	  }
	  elsif(($tarcenum > 4) && ($tarcenum <= 16)) 
	  {	  
	  $tracesizebytes=16*1024;
	  }
	  elsif(($tarcenum > 16) && ($tarcenum <= 64))
	  {
	  $tracesizebytes=64*1024;
	  }
	  elsif(($tarcenum > 64) && ($tarcenum <= 256))
	  {
	  $tracesizebytes=256*1024;
	  }
	  #print"\n$tracesize\n";
	  }
	  $tracesizeinHWT=$tracesize*3;
	  #print "\n$tracesizeinMB\n";
	   #@trace_array = split('"',$_);
	   #print"\n@trace_array\n";
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
   if (!$Dec_start_addr_from_file ) {
      print "the start_address not found in $src_file\n";
      exit;
   }else{
      print "    Start address = $Hex_start_addr_from_file";
     
   }
   if (!$Dec_img_size_from_file ) {
      print "the image size not found in $src_file\n";
      exit;
   }else{
      print " ,Image size defined = $Hex_img_size_from_file\n";
     
   }
   close $src_file;
}

sub getStaticAllocatedMemSize {
   print output_file "QURT memory definition\n";
   print output_file "Start addr,End addr,Size,Acess,Cache attribute,Comment \n";
   $pgSize=0x1000;
   $startPgAddr= $Hex_start_addr_from_file;
   #$startPgAddr =~ s/000$//;
   
   $src_file = "./qdsp6/qurt_overrides/8650/memmap.def";
   open src_file or die "Cannot open file $src_file\n";

   print " 3. Trying to get the size of static allocated memory definition from $src_file\n";

   while(<src_file>)
   {  
      if($_ !~ /^\// ){
      
         $_ =~ s/\t//g;
         $_ =~ s/ //g;
         $_ =~ s/\r//;
         $_ =~ s/\n//;
         $_ =~ tr/[a-z]/[A-Z]/;
         $_ =~ tr/X/x/;
           
         if($_ =~ /^M/){
            
            #print "s= $_";
            @line_array = split(',',$_);
            
            $mmGloble=$line_array[0];
            $mmASID=$line_array[1];
            $mmVPN=$line_array[2]."000";
            #$mmACESS=$line_array[3];
            #$mmCacheAttr=$line_array[4];
            $mmPGSize=$line_array[5];
            $mmPPN=$line_array[6]."000"; 
            @line_array2 =split('\)',$_);
            $mmComment=$line_array2[1]; 
            #print "s= $_";
            #push(@mmVPNarr,$mmVPN);
            #print "mmVPN =$mmVPN\n";
            #print "dec mmVPN =";
            #print hex($mmVPN);
            #print "\n";
            $mmACESS{hex($mmVPN)}=$line_array[3];
            $mmCacheAttr{hex($mmVPN)}=$line_array[4];
            $mmVPN_DEC{hex($mmVPN)}=$mmVPN;
            $Hash_comment{$mmVPN}=$mmComment;
            $Total_Static_size{hex($mmVPN)}=$mmPGSize;
            
         }
        
      }
            
   }

   @mmVPN_DECarr = keys %mmVPN_DEC;
   @mmVPN_DECarr = sort { $a <=> $b }@mmVPN_DECarr;
   #@mmVPN_DECarr = sort(@mmVPN_DECarr);
   #print "start address, end address, size, comment\n";
   $staticStarted=-1;
   $static_memory_size=0;
   foreach (@mmVPN_DECarr){
      #check for contigeous allocated static memory from the start address
      
      
      $V_size=$DecMemSize{$Total_Static_size{$_}};
      #print "v_size =$Total_Static_size{$_}= $V_size\n";
      $V_end_addr =$_ + $V_size-1;
      $V_end_addr = sprintf("0x%x", $V_end_addr);
      $V_end_addr =~ tr/[a-z]/[A-Z]/;
      $V_end_addr =~ tr/X/x/;
      $V_size = sprintf("0x%x", $V_size);
      if($staticStarted ==1){
         if($mmVPN_DEC{$_} eq $nextCont_Startaddr){
            $static_memory_size+=hex($V_size);
            $hex_static_mem_size =sprintf ("0x%x",$static_memory_size);
            $nextCont_Startaddr= hex($mmVPN_DEC{$_})+hex($V_size);
            $nextCont_Startaddr = sprintf("0x%x", $nextCont_Startaddr);
            $nextCont_Startaddr =~ tr/[a-z]/[A-Z]/;
            $nextCont_Startaddr =~ tr/X/x/;
         }else{
            $staticStarted =-1;
        #    print "static memory ends, size = $hex_static_mem_size, external heap starts\n";
            
         }
      }
      if($mmVPN_DEC{$_} eq $startPgAddr){
       #  print "static memory starts \n";
         $staticStarted=1;
         $static_memory_size+=hex($V_size);
         $nextCont_Startaddr= hex($startPgAddr)+hex($V_size);
         $nextCont_Startaddr = sprintf("0x%x", $nextCont_Startaddr);
         $nextCont_Startaddr =~ tr/[a-z]/[A-Z]/;
         $nextCont_Startaddr =~ tr/X/x/;
  
      }
   
      print output_file "$mmVPN_DEC{$_},$V_end_addr,$V_size,$mmACESS{$_},$mmCacheAttr{$_},$Hash_comment{$mmVPN_DEC{$_}} \n";
      
      
      
   }
   #@m_sorted_VPN_keys = sort{ $Total_Static_size{$a} <=> $Total_Static_size{$b} } @mmVPNarr;
   #print "m_sorted keys : @m_sorted_VPN_keys\n";
   if (!$hex_static_mem_size) {
      print "\nStatic allocated memory not found!\n";
      #exit;
   }else{
      print "    Static allocated memory size = $hex_static_mem_size\n";
   }
   
   close $src_file;
}

sub getImageAnalysis {

   print " 2. Trying to calculate the image size from $image_name\n";
   system ("hexagon-objdump $image_name -h -w > section_info.txt");
	if (open(SECTIONINFO,"section_info.txt")){
	$sectionline=<SECTIONINFO>;
	while ( $sectionline ne "" ) {
		if($sectionline =~ /\d+\s+/)
		{	
			@SectionInfoNames=split(/\s+/, $sectionline);
			if(($SectionInfoNames[2] =~ /\.start/) || ($SectionInfoNames[2] =~ /\.start.+/))
			{
				$FirstSectionBase=hex($SectionInfoNames[4]);
				$FirstSectionBasePhy=hex($SectionInfoNames[5]);
				push(@mpdImageStart,$FirstSectionBase);
				push(@mpdImageStartPhy,$FirstSectionBasePhy);
				#print"\nstart address is:$FirstSectionBasePhy\n";
				#print"\nTested address is : $SectionInfoNames[5]\n";
			}
		$decSectionBase=hex($SectionInfoNames[4]);
		$decSectionBasePhy=hex($SectionInfoNames[5]);
			if($decSectionBase != 0)
			{
				$LastSectionBase=$decSectionBase;
				$LastSectionBasePhy=$decSectionBasePhy;
				$LastSectionSize=hex($SectionInfoNames[3]);
				$LastSectionEndAddress=$LastSectionBase+$LastSectionSize;
				$LastSectionEndAddressPhy=$LastSectionBasePhy+$LastSectionSize;
			}
			else
			{
				push(@mpdImageEnd,$LastSectionEndAddress);
				push(@mpdImageEndPhy,$LastSectionEndAddressPhy);
			}
		}
	$sectionline=<SECTIONINFO>;
		}
	}	
	else
	{
	die"Cannot Open File $section_file\n";
	}
	$len=@mpdImageStart;
	$TotalMpdImageSize=0;
	for($i=0;$i<$len;$i++)
	{
	#print"\nstart address is:$FirstSectionBasePhy\n";
	#print"\nEnd address is: $LastSectionEndAddressPhy\n";
	$TotalMpdImageSize+=$mpdImageEnd[$i]-$mpdImageStart[$i];
	}
	$arrayEnd=$len-1;
	#print"\nlength is: $arrayEnd\n";
	$OverallImageStart=$mpdImageStartPhy[0];
	$OverallImageEnd=$mpdImageEndPhy[$arrayEnd];
	$OveralImageEndNA=$OverallImageEnd - $OverallImageStart;
	#print"\n$OverallImageEnd\n";
	#print"\n$OverallImageStart\n";
	$OverallImageEnd=ceil($OverallImageEnd/(256*1024));
	#print"\nOverll size is : $OverallImageEnd\n";
	$alignedImageEndPhy=$OverallImageEnd*256*1024;
	$TotalMpdImageSizeMBFin=$alignedImageEndPhy - $OverallImageStart;	
	#print"\n$alignedImageEndPhy\n";
	#print"\n$OverallImageStart\n";
	#print"\n$TotalMpdImageSizeMBFin\n";
	$TotalMpdImageSizeMBFinPhy=$TotalMpdImageSizeMBFin+$tracesizebytes;
	$TotalMpdImageSizeMBFin=($TotalMpdImageSizeMBFin+$tracesizebytes)/(1024*1024);
	#print"\n$tracesizebytes\n";
	#print"\n$TotalMpdImageSizeMBFin\n";
	$TotalMpdImageSizeMBFin=sprintf("%.2f", $TotalMpdImageSizeMBFin);
	#print"\n$TotalMpdImageSizeMBFin\n";
	$TotalMpdImageSizeMB=$TotalMpdImageSize/(1024*1024);
	$TotalMpdImageSizeMB=sprintf("%.2f", $TotalMpdImageSizeMB);
	$total_image_size_d=$TotalMpdImageSize;
    $total_image_size_x=sprintf("0x%x",$TotalMpdImageSize);
    
   close $section_file;
   if(!$total_image_size_x){
      print "The image size not found!\n";
      exit;


   }

}
sub getExternalHeapSize {

	if(($TotalMpdImageSizeMBFinPhy) <= $Dec_img_size_from_file)
	{
		$Image_size_dec=hex($Hex_img_size_from_file);
		$Image_size_dec = $Image_size_dec/(1024*1024);
		$Static_size_dec = ($TotalMpdImageSize+$tracesizebytes)/(1024*1024);
		$Static_size_dec = sprintf("%.2f", $Static_size_dec);

		$Heap_size_dec = hex($Hex_heap_size);
		$Heap_size_dec = $Heap_size_dec/(1024*1024);
		print "*****************LPASS Memory FootPrint************************\n";
		print "Total Image size declared in MB			:$Image_size_dec\n";
		print "Static(Text+Data)+Heap Memory size in MB	:$Static_size_dec\n";
		#print "Heap Memory size in MB				:$Heap_size_dec\n";
		print "Unused Memory size in KB:\n";			  
		print "   DEFAULT_PHYSPOOL - \(Static\(Text+Data\) Memory size - Heap Memory size\)\n";
		print "   $Image_size_dec - \($Static_size_dec\) \n";		
		$Dec_EXT_HeapSize = $Image_size_dec - ($Static_size_dec);
		$Required_image_size=$Dec_EXT_HeapSize*1024;		
		print "   =$Required_image_size\n";
		print "**************************************************************\n";		
	}
	else 
	{
	    print "\n";
		print "****************************************\n";
		print "Error: Static\(Text\+Data\) \+ Heap size is greater than DEFAULT_PHYSPOOL defined in qdsp6\.xml\n";
		print "****************************************\n";
		$Image_size_dec=hex($Hex_img_size_from_file);
		$Image_size_decFin=$Image_size_dec;
		$Image_size_dec = $Image_size_dec/(1024*1024);
		$Image_size_decFinMB=$Image_size_dec;
		$Image_size_dec = Image_size_dec - (256*1024);
		$Image_size_decFin1=$Image_size_decFin-(256*1024);
		$tobesaved= ($OveralImageEndNA+$tracesizebytes) - $Image_size_decFin1;
		#print"\n$OveralImageEndNA\n";
		#print"\n$tracesizebytes\n";
		#print"\n$Image_size_decFin1\n";
		$Static_size_dec = $TotalMpdImageSizeMBFinPhy/(1024*1024);
		$Static_size_dec = sprintf("%.2f", $Static_size_dec);
		#$Heap_size_dec = hex($Hex_heap_size);
		#$Heap_size_dec = $Heap_size_dec/(1024*1024);		
		print "*****************LPASS Memory FootPrint************************\n";
		print "Total Image size declared in MB			:$Image_size_decFinMB\n";
		print "Static(Text+Data)+Heap Memory size in MB	:$Static_size_dec\n";
		#print "Heap Memory size in MB				:$Heap_size_dec\n";
		if (($CHIPSET_folder eq "msm8x26") || ($CHIPSET_folder eq "msm8974") || ($CHIPSET_folder eq "msm8626"))
		{
		print "Mandatory Size that needs to be optimized in SensorPD to fit with in ADSP pool in KB:\n";			  
		}
		else
		{
		print "Mandatory Size that needs to be optimized to fit with in ADSP pool in KB:\n";			  
		}
		#print "   DEFAULT_PHYSPOOL - \(Static\(Text+Data\) Memory size - Heap Memory size\)\n";
		#print "   $Image_size_dec - \($Static_size_dec\) \n";		
		$Dec_EXT_HeapSize = ($Static_size_dec) - $Image_size_dec;
		$Required_image_size=$Dec_EXT_HeapSize*1024;	
        $Required_image_size1 = $tobesaved/1024;		
		print "   =$Required_image_size1\n";
		print "****************************************************************\n";
		$filename="dsp\.elf";
		unlink $filename;
		#system("rm -rf \.\.\/dsp\.elf");
		rmtree('obj\/qdsp6v5_ReleaseG'); 
		#system("rm -rf \.\.\/obj\/qdsp6v5_ReleaseG");
	}
   
   
}

sub PrintOutput {
 
   print output_file "\n\nStatic allocated memory analysis\n";
   print output_file "PAddr_start(Hex),PAddr_end(Hex),SIZE(Byte),Section Name,PADING(Byte)\n";
   foreach (@m_sorted_keys) {
       print  output_file "$pa_x_start{$_},$pa_x_end{$_},$size_x{$_},$name{$_},$padding_x{$_}\n";
   }
   
   print output_file "Subtotal,,$tot_size_withoutPadding_x,,$tot_padding_x\n\n";
   print output_file ",,,Total,$total_image_size_x\n";
   close $output_file;

}
sub deleteMetafile {
  # system ("rm -f section_info.txt");
}
