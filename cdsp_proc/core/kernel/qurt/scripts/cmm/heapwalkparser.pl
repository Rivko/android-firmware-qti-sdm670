################################################################################
#                          H E A P P A R S E R                                 #
#                                                                              #
#     This tool generates a report with all the information (useful)           #
# from the heapwalker output file. The CRC information handling has been       #
# disabled for the sake speed of execution . This is generally used to parse   #
# the output of qurtos_heapwalker.cmm script                                   #
#                                                                              #
# Heap Walker Output Processor                                                 #
# 1. Parses the heap walker output and computes                                #
#    a. Summary of heap state
#       o Number of free blocks                                                #
#       o Number of used blocks                                                #
#       o Total Free Space                
#       o Maximum possible Allocation Size
#       o Fragmentation Factor            
#       o Total Used Bytes                
#       o % of Allocated Space Wasted     
#    b. Blocks Summary
#       o Cumulative free blocks List                                          #
#       o Used Blocks List                                                     #
#    c. Free fragments list                                                    #
#    d. Allocations Summary for hash:lineno combo [Max. Allocated First]       #
#    e. Some notes on understanding the results                                #
#                                                                              #  
# Usage                                                                        #
# perl heapwalkparser.pl -i heapmem.txt -o heapsum.txt                         #
#                                                                              #
#   Copyright (c) 2016 Qualcomm Technologies, Inc.                             #
#   All Rights Reserved.                                                       #
#   Confidential and Proprietary - Qualcomm Technologies, Inc.                 #
#                                                                              #
################################################################################

#some Constants
use constant FREEFLAG=>"0XFF";
use constant SIZEOFMEMBLK=>16;  # Equals sizeof(mem_block_header_type)
use constant VERSTRING=>"00.00.01";
use constant CMMSTRING=>"00.00.01";
#commandlinearguments
my $heapwalkerfile = "";
my $outputfile = "";
my $sizeBlockHeader=16;


my %hashCostSummary = ();
my %usedHeapSummary = ();
my %freeHeapSummary = ();
my %fragSummary     = ();
my $HeapMemBuffer;
my $HeapVariable;
my $CMMVersion;
my $TotalNoOfFragments = 0;
my $TotalNoOfBlocksUsed = 0;
my $TotalNoOfBlocksFree = 0;
my $TotalFreeMem = 0;
my $MaxFreeMemChunk = 0;
my $TotalWstdBytesinHeap = 0;
my $TotalAllocatedBytes = 0;

# Call the main subroutine here
main();

sub main {
   processCmdLine();
   open(OUTPUTFILE,">$outputfile") || die "Unable to create $outputfile\n";
   processHeapTable($heapwalkerfile);
   processFreeFragments();
   processUsedFragments();
   select(OUTPUTFILE);
   printSummary(\*OUTPUTFILE);
}

sub processCmdLine { 
   my $TotalArgSig = 0;
   my $i=0;

   for( $i=0; $i<$#ARGV+1; $i++)
   {

           my $arg = $ARGV[$i];
           if ($arg eq "-i" || $arg eq "-I")
           {
                   $i++;
                   $TotalArgSig = $TotalArgSig & 0x1 ;
                   $heapwalkerfile = $ARGV[$i];
                   if(!(-e $heapwalkerfile))
                   {
                      die "$heapwalkerfile does not exist!!!\n";
                   }
           }
           elsif ($arg eq "-o" || $arg eq "-O")
           {
                   $i++;
                   $TotalArgSig = $TotalArgSig & 0x2;
                   $outputfile = $ARGV[$i];
           }
		   elsif ($arg eq "-s" || $arg eq "-S")
           {
                   $i++;
                   $TotalArgSig = $TotalArgSig & 0x3;
                   $sizeBlockHeader = hex($ARGV[$i]);
           }
		   
           else
           {
                   print_usage();
                   exit 0;
           }
   }
}

sub processHeapTable {
   my $heapfilename = shift;
   my $block_count = 0;   
   my $temp_string = "";
   my $prevflag = 0xAB; # Something that is uninitalized
   my ($blockno, $address,$caller_ptr,$caller_symbol,$allocated_bytes,$extra_bytes,$freeflag); 
   my ($lastflag,$padbyte);  


   open (HFH, "<$heapfilename") || die "Unable to open $heapfilename";

  # Get the initial string table
   # Version : 00.00.01
   # Heap Variable : tmc_heap
   # Heap Buffer   : \\LYRQ\Global\tmc_heap_mem_buffer
   <HFH>;
   <HFH>;
   <HFH> =~ /(\D+) : (\S+)/;
   $CMMVersion=$2;
   if($CMMVersion ne CMMSTRING)
   {
      die "Expected CMM Script Version : ".CMMSTRING." ; Found : $CMMVersion\n";
   }
   <HFH> =~ /(\D+) : (\S+)/;
   $HeapVariable=$2;
   <HFH> =~ /(\D+) : (\S+)/;
   $HeapMemBuffer=$2;
   my $validBlock=0;
   while(<HFH>)
   {

#Block No       Address    CallerPtr   CallerPtr_Symbol   Allocated   Extra  Free  Last    
#                                                          Bytes     Bytes  Flag  Flag  
#=============================================================================== 
# 
#   1    0x01F48A30      0x43f67582                          1127       4304    0x0  0x00   
	  $validBlock=0;
     # print "$_";
	  if(/(\d+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)/)
	  {
		 $validBlock=1;
		 $blockno = $1;
         $address = $2;
         $caller_ptr    = $3;         
		 $caller_symbol =$5;
         $allocated_bytes = $6;
         $extra_bytes = $7;
         $freeflag = $8;
         $lastflag = $9;     
	  
	  }
	  
      else 
	  {
		if(/(\d+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)/)
		{
#         print "Blk : $1 Add: $2 Hash: $3 Lineno : $4 Allbyt: $5 ext : $6 free : $7 last : $8 pad : $9 \n";
         $validBlock=1;
		 $blockno = $1;
         $address = $2;
         $caller_ptr    = $3;         
		 $caller_symbol =$4;
         $allocated_bytes = $5;
         $extra_bytes = $6;
         $freeflag = $7;
         $lastflag = $8;         
         
		}
	  }
	  
	  if($validBlock eq "1")
	  {
		if($freeflag ne "0x01")
         {
            # Used Block  
            $TotalNoOfBlocksUsed++; # Starts with 1...
            $usedHeapSummary{$TotalNoOfBlocksUsed}{BLKNO} = $blockno;                   
            $usedHeapSummary{$TotalNoOfBlocksUsed}{BLKADDR} = $address;           
            $usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_PTR} = $caller_ptr;  
			$usedHeapSummary{$TotalNoOfBlocksUsed}{CALLER_SYMBOL} = $caller_symbol; 
            $usedHeapSummary{$TotalNoOfBlocksUsed}{ALLOCBYTES} = $allocated_bytes;
            $usedHeapSummary{$TotalNoOfBlocksUsed}{EXTRABYTES} = $extra_bytes;    
            $usedHeapSummary{$TotalNoOfBlocksUsed}{FREEFLAG} =  $freeflag;        
            $usedHeapSummary{$TotalNoOfBlocksUsed}{LASTFLAG} = $lastflag;         
			#$prevflag = $freeflag;
            
         }
         else
         {
           # if(defined($prevflag) && $prevflag eq $freeflag)
           # {
               # Merge the two adjacent free blocks and show them as a single
               # free block
            #   $freeHeapSummary{$TotalNoOfBlocksFree}{ALLOCBYTES} += $allocated_bytes;
            #   $freeHeapSummary{$TotalNoOfBlocksFree}{EXTRABYTES} += $extra_bytes;
            #   $freeHeapSummary{$TotalNoOfBlocksFree}{ENDBLK} = $blockno;
            #   $freeHeapSummary{$TotalNoOfBlocksFree}{ENDADDR} = sprintf("0x%X",hex($address)+$allocated_bytes);
            #}
            #else
            #{
               # Start of a new free block
               #$prevflag = $freeflag;
               $TotalNoOfBlocksFree++;
               $freeHeapSummary{$TotalNoOfBlocksFree}{STARTBLK} = $blockno;
               $freeHeapSummary{$TotalNoOfBlocksFree}{ENDBLK} = $blockno;
               $freeHeapSummary{$TotalNoOfBlocksFree}{STARTADDR} = $address;
               $freeHeapSummary{$TotalNoOfBlocksFree}{ENDADDR} = sprintf("0x%X",hex($address)+$allocated_bytes);
               #$freeHeapSummary{$TotalNoOfBlocksFree}{DEBUGINFO} = $debugInfo;               
               $freeHeapSummary{$TotalNoOfBlocksFree}{ALLOCBYTES} = $allocated_bytes;
               $freeHeapSummary{$TotalNoOfBlocksFree}{EXTRABYTES} = $extra_bytes;
               $freeHeapSummary{$TotalNoOfBlocksFree}{FREEFLAG} =  $freeflag;
               $freeHeapSummary{$TotalNoOfBlocksFree}{LASTFLAG} = $lastflag;                
            #}

         }
         $TotalNoOfFragments++;
	}
   }
   close(HFH);
 
}

sub printSummary {
   my $OUTPUTFILEHANDLE = shift;
   print $OUTPUTFILEHANDLE "             Heap Walker Results Version  ".VERSTRING."\n";
   print $OUTPUTFILEHANDLE "       =================================================\n\n";
   printOverallSummary($OUTPUTFILEHANDLE);
   printBlockSummary($OUTPUTFILEHANDLE);
   printFreeFragmentsSummary($OUTPUTFILEHANDLE);
   printCostSummary($OUTPUTFILEHANDLE);
   printNotes($OUTPUTFILEHANDLE);
}

sub printOverallSummary {
   my $OUTPUTFILEHANDLE = shift;
   
print $OUTPUTFILEHANDLE "Overall Summary\n";
print $OUTPUTFILEHANDLE "===============\n";
print $OUTPUTFILEHANDLE " \n";
print $OUTPUTFILEHANDLE "    Heap State Variable                   : $HeapVariable\n"; 
print $OUTPUTFILEHANDLE "    Heap Buffer                           : $HeapMemBuffer\n";
print $OUTPUTFILEHANDLE "    Total No. Of Blocks                   : $TotalNoOfFragments \n";
print $OUTPUTFILEHANDLE "    Total No. Of Free Blocks              : $TotalNoOfBlocksFree\n";
print $OUTPUTFILEHANDLE "    Total No. Of Used Blocks              : $TotalNoOfBlocksUsed\n";
print $OUTPUTFILEHANDLE "    Total Free Space                      : $TotalFreeMem\n";
print $OUTPUTFILEHANDLE "    Maximum possible Allocation Size      : $MaxFreeMemChunk\n";
if($TotalFreeMem ne 0)
{
	print $OUTPUTFILEHANDLE "    Fragmentation Factor                  : ".sprintf("%3.2f%%",(1- ($MaxFreeMemChunk/$TotalFreeMem))*100)."\n";
}
else
{
	print $OUTPUTFILEHANDLE "    Fragmentation Factor                  : ".sprintf("%3.2f%%",(0)*100)."\n";
}
print $OUTPUTFILEHANDLE "    Total Used Bytes                      : $TotalAllocatedBytes\n";
print $OUTPUTFILEHANDLE "    % of Allocated Space Wasted           : ".sprintf("%3.2f%%",($TotalWstdBytesinHeap*100/$TotalAllocatedBytes))."\n";
print $OUTPUTFILEHANDLE "      (Includes Overhead : 16 or 12 bytes/blk)\n";
print $OUTPUTFILEHANDLE "\n\n";

}

sub printBlockSummary {
   my $OUTPUTFILEHANDLE = shift;
   my ($sno,  $startblk, $endblk, $hash, $lineno, $AlctdBytes, $ExtraBytes);
   
   print $OUTPUTFILEHANDLE "Blocks Summary\n";
   print $OUTPUTFILEHANDLE "==============\n\n";
   print $OUTPUTFILEHANDLE "Free Contigous Blocks available in the heap:\n";
   print $OUTPUTFILEHANDLE "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
   format FreeBlkHdrFormat = 
@>>>     [@>>>> -  @>>>]    @>>>>>>>>>>
$sno,  $startblk, $endblk, $AlctdBytes
.
  $~ = FreeBlkHdrFormat;
  $sno = "";
  $startblk= "Start";
  $endblk = "End";
  $AlctdBytes="Available";
  write();
  $sno = "S.NO";
  $startblk= "Blk";
  $endblk = "Blk";
  $AlctdBytes="Bytes";
  write();

  print $OUTPUTFILEHANDLE "----------------------------------------\n";

  format FreeBlkNumFormat = 
@>>>     [@>>>> -  @>>>]    @>>>>>>>>>>
$sno,  $startblk, $endblk, $AlctdBytes
.
     $~ = FreeBlkNumFormat;

   foreach  (sort {$a <=> $b} keys %freeHeapSummary) {

      $sno = $_;
      $startblk= $freeHeapSummary{$_}{STARTBLK};
      $endblk = $freeHeapSummary{$_}{ENDBLK};     
      $AlctdBytes=$freeHeapSummary{$_}{ALLOCBYTES};
      write();


   }
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE "Used Blocks in the heap:\n";
   print $OUTPUTFILEHANDLE "~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";

   format UsedBlkHdrFormat = 
@>>>   @>>>>>     @>>>>>>>>> @>>>>>>>>>>>>>>>>>>>>>>>>>>>  @>>>>>  @>>>>>>>>> @>>>>>>
$sno,  $startblk, $debugInfo, $debugInfoSymbol, $AlctdBytes, $ExtraBytes
.
  $~ = UsedBlkHdrFormat;
  $sno = "";
  $startblk= "Block";
  $debugInfo= "Caller";  
  $debugInfoSymbol= "Caller";   
  $AlctdBytes="Allocted";
  $ExtraBytes="Extra";
  write();
  $sno = "S.NO";
  $startblk= "Number";
  $debugInfo= "Pointer";  
  $debugInfoSymbol= "Symbol"; 
  $AlctdBytes="Bytes";
  $ExtraBytes="Bytes";
  write();

  print $OUTPUTFILEHANDLE "-------------------------------------------------------------------------------------------\n";

  format UsedBlkNumFormat = 
@>>>   @>>>>>     @>>>>>>>>>   @>>>>>>>>>>>>>>>>>>>>>>>>>>>   @>>>>>  @>>>>>>>>> @>>>>>>
$sno,  $startblk, $debugInfo , $debugInfoSymbol, $AlctdBytes, $ExtraBytes
.
     $~ = UsedBlkNumFormat;

   foreach  (sort {$a <=> $b} keys %usedHeapSummary) {
       $sno = $_;
       $startblk= $usedHeapSummary{$_}{BLKNO};
       $debugInfo= $usedHeapSummary{$_}{CALLER_PTR};
	   $i=rindex($usedHeapSummary{$_}{CALLER_SYMBOL},"\\");
	   $debugInfoSymbol=substr($usedHeapSummary{$_}{CALLER_SYMBOL},$i+1);          
       $AlctdBytes=$usedHeapSummary{$_}{ALLOCBYTES};
       $ExtraBytes=$usedHeapSummary{$_}{EXTRABYTES};
       write();

   }
   print $OUTPUTFILEHANDLE "\n\n\n";
}

sub processFreeFragments {

   foreach  (sort keys %freeHeapSummary) 
   {
      if(!defined($fragSummary{$freeHeapSummary{$_}{ALLOCBYTES}}))
      {
         $fragSummary{$freeHeapSummary{$_}{ALLOCBYTES}}= 1; 
      }
      else
      {
         $fragSummary{$freeHeapSummary{$_}{ALLOCBYTES}}++; 
      }
      $TotalFreeMem+= $freeHeapSummary{$_}{ALLOCBYTES};
      if($MaxFreeMemChunk < $freeHeapSummary{$_}{ALLOCBYTES})
      {

         $MaxFreeMemChunk = $freeHeapSummary{$_}{ALLOCBYTES};
      }
   }
   
}

sub printFreeFragmentsSummary {
   my ($OUTPUTFILEHANDLE) = @_;
   print $OUTPUTFILEHANDLE "Number of Free Fragments available (sorted by size)\n";
   print $OUTPUTFILEHANDLE "====================================================\n\n";
   my (@fragarray, $index);
   my ($windex,  $blksize, $numofblks);
   $index = 1;

   format FreeFragHdrFormat = 
@>>>   @>>>>>>>     @>>>>>>>>
$windex,  $blksize, $numofblks
.

  $~ = FreeFragHdrFormat;
  $windex = "";
  $blksize = "Fragment";
  $numofblks = "Number of";
  write();
  $windex = "S.No.";
  $blksize = "Size";
  $numofblks = "Fragments";
  write();
  print $OUTPUTFILEHANDLE "------------------------------\n";

   foreach  (sort {$a <=> $b} keys %fragSummary) 
   {
#      printf("%12d   %12d\n",$_,$fragSummary{$_});
      $windex = $index;
      $blksize = $_;
      $numofblks = $fragSummary{$_};
      $index++;
      write();

   }
   print $OUTPUTFILEHANDLE "\n\n\n";
}

sub processUsedFragments {
   my $keyCost;
foreach  (sort keys %usedHeapSummary) {
      $keyCost = $usedHeapSummary{$_}{CALLER_PTR}; 
      if(!defined($hashCostSummary{$keyCost}))
         {
            # Currently we just show the allocated bytes and NOT the bytes 
            # requested... that would be alloc - 12( from struct) - EXtra bytes
            # for each fragment
            $hashCostSummary{$keyCost}{ALLOCBYTES} = 0;
            $hashCostSummary{$keyCost}{MAXALLOC} = 0;
            $hashCostSummary{$keyCost}{MINALLOC} = 999999999;
            $hashCostSummary{$keyCost}{TOTWSTD} = 0;
            $hashCostSummary{$keyCost}{TOTALLOCS} = 0;
			$hashCostSummary{$keyCost}{CALLER_SYMBOL} = "";
         }
      $hashCostSummary{$keyCost}{ALLOCBYTES} += $usedHeapSummary{$_}{ALLOCBYTES};
      if($hashCostSummary{$keyCost}{MAXALLOC} < $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummary{$keyCost}{MAXALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }
      if($hashCostSummary{$keyCost}{MINALLOC} > $usedHeapSummary{$_}{ALLOCBYTES})
      {
         $hashCostSummary{$keyCost}{MINALLOC} = $usedHeapSummary{$_}{ALLOCBYTES};
      }
      $hashCostSummary{$keyCost}{TOTWSTD} += $usedHeapSummary{$_}{EXTRABYTES};
      $hashCostSummary{$keyCost}{TOTALLOCS} += 1;
	  if($sizeBlockHeader == SIZEOFMEMBLK)
	  {
		if($usedHeapSummary{$_}{ALLOCBYTES} == $sizeBlockHeader)
		{
			$TotalWstdBytesinHeap += $usedHeapSummary{$_}{EXTRABYTES} + $sizeBlockHeader -4;
		}else
	    {
		    $TotalWstdBytesinHeap += $usedHeapSummary{$_}{EXTRABYTES} + $sizeBlockHeader;
	    }
	  }
	  else
	  {
		$TotalWstdBytesinHeap += $usedHeapSummary{$_}{EXTRABYTES} + $sizeBlockHeader;
	  }
      $TotalAllocatedBytes += $usedHeapSummary{$_}{ALLOCBYTES};
	   $i=rindex($usedHeapSummary{$_}{CALLER_SYMBOL},"\\");
	   $hashCostSummary{$keyCost}{CALLER_SYMBOL}=substr($usedHeapSummary{$_}{CALLER_SYMBOL},$i+1);   	    
   }
}

# Sort the costliest to the economic by the hashno+lineno combo
sub printCostSummary {
   my $OUTPUTFILEHANDLE = shift;
   my ($DebugInfo,  $DebugInfoSymbol,  $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd);
   my ($AvgWstdBytes, $TotalWstdBytes, $WasteRatio);
   print $OUTPUTFILEHANDLE "Allocations Summary for DebugInfo (Sorted in total allocated bytes)\n";
   print $OUTPUTFILEHANDLE "==========================================================================================\n\n";
   format HeaderFormat = 
@>>>>>>>>>> @>>>>>>>>>>>>>>>>>>>>  @>>>>>>   @>>>>>   @>>>>>>  @>>>>>  @>>>>>>>   @>>>>>    @>>>>>>>
$DebugInfo, $DebugInfoSymbol, $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $AvgWstdBytes, $TotalWstdBytes, $WasteRatio
.
  $~ = HeaderFormat;
  $DebugInfo = "Caller";
  $DebugInfoSymbol = "Caller";
  $NumAllctns = "Num. of";
  $MaxAllctd = "Max.";
  $MinAllctd = "Min.";
  $TotAllctd = "Total";
  $AvgWstdBytes = "Avg.";
  $TotalWstdBytes = "Total";
  $WasteRatio  = "% Wasted";
  write();

  $DebugInfo = "Pointer";
  $DebugInfoSymbol = "Symbol";
  $NumAllctns = "Alctns";
  $MaxAllctd = "Alctd";
  $MinAllctd = "Alctd";
  $TotAllctd = "Alctd";
  $AvgWstdBytes = "Wasted";
  $TotalWstdBytes = "Wasted";
  $WasteRatio  = "Bytes";
  write();
  print $OUTPUTFILEHANDLE "---------------------------------------------------------------------------------------------------\n";

  format SummaryFormat = 
@>>>>>>>>>>  @>>>>>>>>>>>>>>>>>>>>  @######  @######   @###### @######   @######  @######     @###.##
$DebugInfo, $DebugInfoSymbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $AvgWstdBytes, $TotalWstdBytes, $WasteRatio
.

   $~ = SummaryFormat;
   foreach (sort { $hashCostSummary{$b}{ALLOCBYTES} <=> $hashCostSummary{$a}{ALLOCBYTES}  } keys %hashCostSummary) {
      $DebugInfo = $_;
      $NumAllctns = $hashCostSummary{$_}{TOTALLOCS};
      $MaxAllctd = $hashCostSummary{$_}{MAXALLOC};
      $MinAllctd = $hashCostSummary{$_}{MINALLOC};
	  $DebugInfoSymbol=$hashCostSummary{$_}{CALLER_SYMBOL};
      $TotAllctd = $hashCostSummary{$_}{ALLOCBYTES};
      $AvgWstdBytes = $hashCostSummary{$_}{TOTWSTD}/$hashCostSummary{$_}{TOTALLOCS};
      $TotalWstdBytes = $hashCostSummary{$_}{TOTWSTD};
      $WasteRatio  = ($hashCostSummary{$_}{TOTWSTD} * 100)/$hashCostSummary{$_}{ALLOCBYTES};
	  
      write();
   }
   print $OUTPUTFILEHANDLE "\n\n";
}

sub printNotes {
   my $OUTPUTFILEHANDLE = shift;
   print $OUTPUTFILEHANDLE "Notes :\n";
   print $OUTPUTFILEHANDLE "========\n\n";
   print $OUTPUTFILEHANDLE " 1. The Allocated Bytes represents the size of the block allocated and not\n";
   print $OUTPUTFILEHANDLE "    the bytes requested. To get the bytes requested the formula is:\n";
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE "      Req Bytes = Allocated Bytes - sizeof(mem_block_header_type) - Extra Bytes\n";
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE " 2. The % of bytes wasted is a good indicator of the possible cause for\n";
   print $OUTPUTFILEHANDLE "    fragmentation.\n";
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE " 3. The Fragmentation Factor shown in overall summary is calculated as follows:\n";
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE "                           (Max. Size Allocatable)         \n";
   print $OUTPUTFILEHANDLE "    Frag. Factor = ( 1 -   ----------------------- ) * 100 \n";
   print $OUTPUTFILEHANDLE "                           (Total Free heap bytes)         \n";
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE "    The higher the Frag. Factor, the greater is the heap fragmentation. \n";
   print $OUTPUTFILEHANDLE "    High % of Wasted Bytes and/or high Frag. factor would  mean that a \n";
   print $OUTPUTFILEHANDLE "    new heap might be needed to handle requests for small allocations \n";
}        
