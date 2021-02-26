################################################################################
#                          H E A P P A R S E R                                 #
#                                                                              #
#     This tool generates a report with all the information (useful)           #
# from the heapwalker output file. The CRC information handling has been       #
# disabled for the sake speed of execution .                                   #
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
# perl heapwalkparser.pl -i heapmem.txt -s <headersize> -o heapsum.txt         #
#                                                                              #
#   Copyright (c) 2006,2012 by QUALCOMM Incorporated. All Rights Reserved.     #
#                                                                              #
################################################################################

################################################################################
#                            EDIT HISTORY FOR FILE                             #
#   This section contains comments describing changes made to the module.      #
#   Notice that changes are listed in reverse chronological order.             #
#                                                                              #
# $Header: //components/rel/dspcore.adsp/4.0/scripts/crashman/memheap2_heapwalkparser_Latest.pl#3 $
# $Author: pwbldsvc $
# $DateTime: 2017/07/26 05:14:42 $
#                                                                              #
# ver    when     who  what, where, why                                        #
# ----  -------   ---  --------------------------------------------------------#
#  v1   10/01/07  MK   Created this file                                       #
#                                                                              #
################################################################################

#some Constants
use constant FREEFLAG=>"0XFF";
use constant SIZEOFMEMBLK=>8;  # Equals sizeof(mem_block_header_type)
use constant VERSTRING=>"00.00.01";
use constant CMMSTRING=>"00.00.01";

#commandlinearguments
my $heapwalkerfile = "";
my $dbWalkerFile = "";
my $outputfile = "";
my $sizeBlockHeader=16;


#Common variables
my %hashCostSummary = ();
my %usedHeapSummaryHeapWalk = ();
my %hashCostSummarytcb = ();

#Heap summary using information in Heap (and debug memory if present)
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
my $ignoreFragmentationFactor = 95; #if heap usage are more than 95% then fragmentation factor will be ignored 

#Heap summary using information in debug memory
my %usedHeapSummaryMemDbg = ();
my %hashCostSummaryMemDbg = ();
my $TotalNoOfBlocksUsedMemDbg = 0;
my $TotalAllocatedBytesMemDbg = 0;
my $debugCheck = 0;

# Call the main subroutine here
main();

sub main {
   processCmdLine();
   print "$heapwalkerfile \n $dbWalkerFile \n $outputfile \n $sizeBlockHeader \n";

   open(OUTPUTFILE,">$outputfile") || die "Unable to create $outputfile\n";
   if ($heapwalkerfile) {
      processHeapTable($heapwalkerfile);
      processFreeFragments();
      processUsedFragmentsHeapWalk();
      select(OUTPUTFILE);
      printSummaryHeapWalker(\*OUTPUTFILE);
   }else{
      if($debugCheck == 1){
         processMemDebugTable($dbWalkerFile);
         processUsedFragmentsMemDbg();
         select(OUTPUTFILE);
         printSummaryMemDbg(\*OUTPUTFILE);
      }
   }
}

sub processCmdLine { 
   my $TotalArgSig = 0;
   my $i=0; 

   if ($#ARGV < 3) {
      print_usage();
      exit 0;
   }
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
                   if($sizeBlockHeader > 0x8){
                      print "debugCheck = 1";
                      $debugCheck = 1;
                   }
           }		   
           elsif ($arg eq "-d" || $arg eq "-D")
           {
                 $i++;
                 $TotalArgSig = $TotalArgSig & 0x4 ;
                 $dbWalkerFile = $ARGV[$i];
                 if(!(-e $dbWalkerFile))
                 {
                    die "$heapwalkerfile does not exist!!!\n";
                 }
           }
           else
           {
                   print_usage();
                   exit 0;
           }
   }
}

sub print_usage {
   print "Usage \n ";
   print "perl heapwalkparser.pl -i heapmem.txt -o heapsum.txt -s DebugHeaderSize \n";

}

sub processHeapTable {
   my $heapfilename = shift;
   my $block_count = 0;   
   my $temp_string = "";
   my $prevflag = 0xAB; # Something that is uninitalized
   my ($blockno, $address,$caller_ptr,$caller_symbol,$task_name, $thread_id,$allocated_bytes,$extra_bytes,$freeflag, $section_index, $startflag); 
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
   <HFH>;
   <HFH>;
   <HFH>;
   <HFH>;
   my $validBlock=0;
   while(<HFH>)
   {

#Print the following if caller debug information is present in the data.
#
#Block No       Address    CallerPtr   CallerPtr_Symbol   Allocated   Extra  Free  Last    
#                                                          Bytes     Bytes  Flag  Flag  
#=============================================================================== 
# 
#   1    0x01F48A30      0x43f67582                          1127       4304    0x0  0x00   
#
#OR print the following
#
#Block No       Address       Allocated   Extra  Free  Last    
#                             Bytes     Bytes  Flag  Flag  
#=============================================================================== 
# 
#   1    0x01F48A30           1127       4304    0x0  0x00   
	  $validBlock=0;
	  my @values = split(',', $_);
      my $numFields = 12;
      if ($debugCheck == 0) {
         $numFields = 7;
      }
	  if(scalar @values eq $numFields)
	  {
        my $index = 0;
		$validBlock=1;
		$blockno =$values[$index]; $index++;
		$address = $values[$index]; $index++;
      if ($debugCheck == 1) {
        $caller_ptr    = $values[$index]; $index++;
        $caller_symbol =$values[$index]; $index++;
        $task_name = $values[$index]; $index++;
		$thread_id=$values[$index]; $index++;
      }
		$allocated_bytes = $values[$index]; $index++;
		$extra_bytes = $values[$index]; $index++;
		$section_index=$values[$index]; $index++;
		$startflag=$values[$index]; $index++;
		$freeflag = $values[$index]; $index++;
		$lastflag = $values[$index];
		if($freeflag ne "0x1")
        {
            # Used Block  
            $TotalNoOfBlocksUsed++; # Starts with 1...
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{BLKNO} = $blockno;                   
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{BLKADDR} = $address;            
            if ($debugCheck == 1) {
               $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{CALLER_PTR} = $caller_ptr;  
               $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{CALLER_SYMBOL} = $caller_symbol; 
               $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{TASK_NAME} = $task_name; 
			   $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{THREAD_ID} = $thread_id; 
            }
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{ALLOCBYTES} = $allocated_bytes;
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{EXTRABYTES} = $extra_bytes;    
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{FREEFLAG} =  $freeflag;        
            $usedHeapSummaryHeapWalk{$TotalNoOfBlocksUsed}{LASTFLAG} = $lastflag;         
        }
        else
        {
            # Start of a new free block
            $TotalNoOfBlocksFree++;
            $freeHeapSummary{$TotalNoOfBlocksFree}{STARTBLK} = $blockno;
            $freeHeapSummary{$TotalNoOfBlocksFree}{ENDBLK} = $blockno;
            $freeHeapSummary{$TotalNoOfBlocksFree}{STARTADDR} = $address;
            $freeHeapSummary{$TotalNoOfBlocksFree}{ENDADDR} = sprintf("0x%X",hex($address)+$allocated_bytes);
            $freeHeapSummary{$TotalNoOfBlocksFree}{ALLOCBYTES} = $allocated_bytes;
            $freeHeapSummary{$TotalNoOfBlocksFree}{EXTRABYTES} = $extra_bytes;
            $freeHeapSummary{$TotalNoOfBlocksFree}{FREEFLAG} =  $freeflag;
            $freeHeapSummary{$TotalNoOfBlocksFree}{LASTFLAG} = $lastflag;
         }
         $TotalNoOfFragments++;
      }
   }
   close(HFH);
}

sub printSummaryHeapWalker {
   my $OUTPUTFILEHANDLE = shift;
   print $OUTPUTFILEHANDLE "             Heap Walker Results Version  ".VERSTRING."\n";
   print $OUTPUTFILEHANDLE "       =================================================\n\n";
   printOverallSummaryHeapWalker($OUTPUTFILEHANDLE);
   printBlockSummary($OUTPUTFILEHANDLE);
   printFreeFragmentsSummary($OUTPUTFILEHANDLE);
   if ($debugCheck == 1) {
      printCostSummary($OUTPUTFILEHANDLE);
      printCostSummaryTcb($OUTPUTFILEHANDLE);
   }
   printNotes($OUTPUTFILEHANDLE);
}

sub printOverallSummaryHeapWalker {
   my $OUTPUTFILEHANDLE = shift;
   my $usedMemory = 0;
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
$usedMemory = ((1-$TotalFreeMem/($TotalFreeMem+$TotalAllocatedBytes))*100);
if (($TotalFreeMem ne 0) and ($usedMemory<$ignoreFragmentationFactor) ) {
   print $OUTPUTFILEHANDLE "    Fragmentation Factor                  : ".sprintf("%3.2f%%",(1- ($MaxFreeMemChunk/$TotalFreeMem))*100)."\n";
}else{
   print $OUTPUTFILEHANDLE "    Heap is exhausted so fragmentation factor does not make sense hence not calculated memory used % is:".sprintf("%3.2f%%",$usedMemory)."\n"; 
}
print $OUTPUTFILEHANDLE "    Total Used Bytes                      : $TotalAllocatedBytes\n";
if ($TotalAllocatedBytes ne 0) {
   print $OUTPUTFILEHANDLE "    % of Allocated Space Wasted           : ".sprintf("%3.2f%%",($TotalWstdBytesinHeap*100/$TotalAllocatedBytes))."\n";
}
if ($debugCheck == 1) {
   print $OUTPUTFILEHANDLE "      (Includes Overhead : 16 or 12 bytes/blk)\n";
}else{
   print $OUTPUTFILEHANDLE "      (Includes Overhead : 8 bytes/blk)\n";
}
print $OUTPUTFILEHANDLE "\n\n";

}


sub printBlockSummary {
   my $OUTPUTFILEHANDLE = shift;
   my ($sno,  $startblk, $endblk,  $AlctdBytes, $ExtraBytes);
   
   print $OUTPUTFILEHANDLE "Free Contigous Blocks available in the heap:\n";
   print $OUTPUTFILEHANDLE "S.NO, StartBlock, EndBlock, Available Bytes\n";
   foreach  (sort {$a <=> $b} keys %freeHeapSummary) {

      $sno = $_;
      $startblk= $freeHeapSummary{$_}{STARTBLK};
      $endblk = $freeHeapSummary{$_}{ENDBLK};     
      $AlctdBytes=$freeHeapSummary{$_}{ALLOCBYTES};
      print $OUTPUTFILEHANDLE "$sno, $startblk, $endblk, $AlctdBytes\n";
   }
   print $OUTPUTFILEHANDLE "\n";
   print $OUTPUTFILEHANDLE "Used Blocks in the heap:\n";

   if ($debugCheck == 1){
      print $OUTPUTFILEHANDLE "S.NO, BlockNumber, Caller Pointer, Caller Symbol, Thread Index, Task Name, AllocatedBytes, ExtraBytes\n";
   }else{
      print $OUTPUTFILEHANDLE "S.NO, BlockNumber, AllocatedBytes, ExtraBytes\n";
   }

   foreach  (sort {$a <=> $b} keys %usedHeapSummaryHeapWalk) {
       $sno = $_;
       $startblk= $usedHeapSummaryHeapWalk{$_}{BLKNO};      
       if ($debugCheck == 1){
          $debugInfo= $usedHeapSummaryHeapWalk{$_}{CALLER_PTR};
          $i=rindex($usedHeapSummaryHeapWalk{$_}{CALLER_SYMBOL},"\\");
          $debugInfoSymbol=substr($usedHeapSummaryHeapWalk{$_}{CALLER_SYMBOL},$i+1);
          $thread_id = $usedHeapSummaryHeapWalk{$_}{THREAD_ID};
          $task_name = $usedHeapSummaryHeapWalk{$_}{TASK_NAME};
       }
       $AlctdBytes=$usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};
       $ExtraBytes=$usedHeapSummaryHeapWalk{$_}{EXTRABYTES};
       if ($debugCheck == 1){
           print $OUTPUTFILEHANDLE "$sno, $startblk,$debugInfo,$debugInfoSymbol,$thread_id,$task_name,$AlctdBytes, $ExtraBytes\n";
       }else{
           print $OUTPUTFILEHANDLE "$sno, $startblk, $AlctdBytes, $ExtraBytes\n";
       }
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
   my (@fragarray, $index);
   my ($windex,  $blksize, $numofblks);
   $index = 1;

   print $OUTPUTFILEHANDLE "S.NO, FragmentSize, NumberofFragments\n";
   foreach  (sort {$a <=> $b} keys %fragSummary) 
   {
      $windex = $index;
      $blksize = $_;
      $numofblks = $fragSummary{$_};
      $index++;
	   print $OUTPUTFILEHANDLE "$windex, $blksize, $numofblks\n";
   }
   print $OUTPUTFILEHANDLE "\n\n\n";
}

sub processUsedFragmentsMemDbg {
   processUsedFragmentsTcb(\%usedHeapSummaryMemDbg);
   processUsedFragmentsCallerMemDbg();
}

sub printSummaryMemDbg {
   my $OUTPUTFILEHANDLE = shift;
   print $OUTPUTFILEHANDLE "             Heap Walker Results Version  ".VERSTRING."\n";
   print $OUTPUTFILEHANDLE "       =================================================\n\n";
   printOverallSummaryMemDbg($OUTPUTFILEHANDLE);
   printCostSummaryTcb($OUTPUTFILEHANDLE);
   printCostSummaryCaller($OUTPUTFILEHANDLE);
   printNotes($OUTPUTFILEHANDLE);
}

sub printOverallSummaryMemDbg {
   my $OUTPUTFILEHANDLE = shift;
   
print $OUTPUTFILEHANDLE "Overall Summary\n";
print $OUTPUTFILEHANDLE "===============\n";
print $OUTPUTFILEHANDLE " \n";
print $OUTPUTFILEHANDLE "    Total No. Of Used Blocks              : $TotalNoOfBlocksUsedMemDbg\n";
print $OUTPUTFILEHANDLE "    Total Used Bytes                      : $TotalAllocatedBytesMemDbg\n";
print $OUTPUTFILEHANDLE "\n\n";

}
sub processUsedFragmentsTcb {
   my $keyCost;   
   my %usedHeapSum = %{$_[0]};
   foreach  (sort keys %usedHeapSum) {
      $keyCost = $usedHeapSum{$_}{THREAD_ID}; 

      if(!defined($hashCostSummarytcb{$keyCost}))
         {
            # Currently we just show the allocated bytes and NOT the bytes 
            # requested... that would be alloc - 12( from struct) - EXtra bytes
            # for each fragment
            $hashCostSummarytcb{$keyCost}{ALLOCBYTES} = 0;
            $hashCostSummarytcb{$keyCost}{MAXALLOC} = 0;
            $hashCostSummarytcb{$keyCost}{MINALLOC} = 999999999;            
            $hashCostSummarytcb{$keyCost}{TOTALLOCS} = 0;
			$hashCostSummarytcb{$keyCost}{TASK_NAME}="";
         }
      $hashCostSummarytcb{$keyCost}{ALLOCBYTES} += $usedHeapSum{$_}{ALLOCBYTES};
      if($hashCostSummarytcb{$keyCost}{MAXALLOC} < $usedHeapSum{$_}{ALLOCBYTES})
      {
         $hashCostSummarytcb{$keyCost}{MAXALLOC} = $usedHeapSum{$_}{ALLOCBYTES};
      }
      if($hashCostSummarytcb{$keyCost}{MINALLOC} > $usedHeapSum{$_}{ALLOCBYTES})
      {
         $hashCostSummarytcb{$keyCost}{MINALLOC} = $usedHeapSum{$_}{ALLOCBYTES};
      }      
      $hashCostSummarytcb{$keyCost}{TOTALLOCS} += 1;	 
      $TotalAllocatedBytesMemDbg += $usedHeapSum{$_}{ALLOCBYTES};
	   $hashCostSummarytcb{$keyCost}{TASK_NAME}=$usedHeapSum{$_}{TASK_NAME};
   }
}


sub processUsedFragmentsCaller {
   my $keyCost;
   foreach  (sort keys %usedHeapSummaryHeapWalk) {
      $keyCost = $usedHeapSummaryHeapWalk{$_}{CALLER_PTR}; 
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
      $hashCostSummary{$keyCost}{ALLOCBYTES} += $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};
      if($hashCostSummary{$keyCost}{MAXALLOC} < $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES})
      {
         $hashCostSummary{$keyCost}{MAXALLOC} = $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};
      }
      if($hashCostSummary{$keyCost}{MINALLOC} > $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES})
      {
         $hashCostSummary{$keyCost}{MINALLOC} = $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};
      }
      $hashCostSummary{$keyCost}{TOTWSTD} += $usedHeapSummaryHeapWalk{$_}{EXTRABYTES};
      $hashCostSummary{$keyCost}{TOTALLOCS} += 1;
      if($sizeBlockHeader == SIZEOFMEMBLK)
      {
        if($usedHeapSummaryHeapWalk{$_}{ALLOCBYTES} == $sizeBlockHeader)
        {
            $TotalWstdBytesinHeap += $usedHeapSummaryHeapWalk{$_}{EXTRABYTES} + $sizeBlockHeader -4;
        }else
        {
            $TotalWstdBytesinHeap += $usedHeapSummaryHeapWalk{$_}{EXTRABYTES} + $sizeBlockHeader;
        }
      }
      else
      {
        $TotalWstdBytesinHeap += $usedHeapSummaryHeapWalk{$_}{EXTRABYTES} + $sizeBlockHeader;
      }
      $TotalAllocatedBytes += $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};	   	    
       $i=rindex($usedHeapSummaryHeapWalk{$_}{CALLER_SYMBOL},"\\");
       $hashCostSummary{$keyCost}{CALLER_SYMBOL}=substr($usedHeapSummaryHeapWalk{$_}{CALLER_SYMBOL},$i+1);   	    
   }
}

sub processUsedFragmentsHeapWalk {
   if ($debugCheck == 0) {
      foreach  (sort keys %usedHeapSummaryHeapWalk) {      
         $TotalWstdBytesinHeap += $usedHeapSummaryHeapWalk{$_}{EXTRABYTES} + SIZEOFMEMBLK;	  
         $TotalAllocatedBytes += $usedHeapSummaryHeapWalk{$_}{ALLOCBYTES};	   	    
      }
   }else{
      processUsedFragmentsCaller();
      processUsedFragmentsTcb(\%usedHeapSummaryHeapWalk);
   }
}

# Sort the costliest to the economic by the hashno+lineno combo
sub printCostSummary {
   my $OUTPUTFILEHANDLE = shift;
   my ($DebugInfo,  $DebugInfoSymbol,  $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAlloc);
   my ($AvgWstdBytes, $TotalWstdBytes, $WasteRatio);
   print $OUTPUTFILEHANDLE "Allocations Summary for DebugInfo (Sorted in total allocated bytes)\n";

   print $OUTPUTFILEHANDLE "Caller Pointer, Caller Symbol, Num of Allocations, Max Allocated,Min Allocated, Total Allocated, Percent Allocated, Avg Wasted Bytes, Total WstdBytes, Waste Percent\n";

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
      $PercentAlloc = $TotAllctd/$TotalAllocatedBytes*100;
	  print $OUTPUTFILEHANDLE " $DebugInfo, $DebugInfoSymbol, $NumAllctns,$MaxAllctd,$MinAllctd,$TotAllctd,$PercentAlloc,$AvgWstdBytes,$TotalWstdBytes,$WasteRatio\n";
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


sub processMemDebugTable {
   my $memdbfilename = shift;
   my $block_count = 0;   
   my $temp_string = "";   
   my ($ptr,$caller_ptr, $caller_symb,$tcb_ptr,$task_name,$size);   

   open (MEMDBG, "<$memdbfilename") || die "Unable to open $memdbfilename";
   <MEMDBG>;
   while(<MEMDBG>)
   {
	  my @values = split(',', $_);
	  if(scalar @values eq 6)
	  {
		 $ptr =$values[0];
		 $size = $values[1];
		 $caller_ptr = $values[2];	
		 $caller_symb = $values[3];
		 $tcb_ptr    = $values[4];  
		 $task_name = $values[5];          
		 $task_name =~ s/^\s+//;
		 $task_name =~ s/\s+$//;
		# Used Block  
		$TotalNoOfBlocksUsedMemDbg++; # Starts with 1...
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{PTR} = $ptr; 
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{CALLER_PTR} = $caller_ptr;                   
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{CALLER_SYMBOL} = $caller_symb;           
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{TCB_PTR} = $tcb_ptr;      
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{TASK_NAME} = $task_name;
		$usedHeapSummaryMemDbg{$TotalNoOfBlocksUsedMemDbg}{ALLOCBYTES} = $size;
	  }      
   }
   close(MEMDBG);
}


sub processUsedFragmentsCallerMemDbg {
   my $keyCost;
   foreach  (sort keys %usedHeapSummaryMemDbg) {
      $keyCost = $usedHeapSummaryMemDbg{$_}{CALLER_PTR}; 
      if(!defined($hashCostSummaryMemDbg{$keyCost}))
         {
            # Currently we just show the allocated bytes and NOT the bytes 
            # requested... that would be alloc - 12( from struct) - EXtra bytes
            # for each fragment
            $hashCostSummaryMemDbg{$keyCost}{ALLOCBYTES} = 0;
            $hashCostSummaryMemDbg{$keyCost}{MAXALLOC} = 0;
            $hashCostSummaryMemDbg{$keyCost}{MINALLOC} = 999999999;            
            $hashCostSummaryMemDbg{$keyCost}{TOTALLOCS} = 0;
			$hashCostSummaryMemDbg{$keyCost}{CALLER_SYMBOL}="";
         }
      $hashCostSummaryMemDbg{$keyCost}{ALLOCBYTES} += $usedHeapSummaryMemDbg{$_}{ALLOCBYTES};
      if($hashCostSummaryMemDbg{$keyCost}{MAXALLOC} < $usedHeapSummaryMemDbg{$_}{ALLOCBYTES})
      {
         $hashCostSummaryMemDbg{$keyCost}{MAXALLOC} = $usedHeapSummaryMemDbg{$_}{ALLOCBYTES};
      }
      if($hashCostSummaryMemDbg{$keyCost}{MINALLOC} > $usedHeapSummaryMemDbg{$_}{ALLOCBYTES})
      {
         $hashCostSummaryMemDbg{$keyCost}{MINALLOC} = $usedHeapSummaryMemDbg{$_}{ALLOCBYTES};
      }      
      $hashCostSummaryMemDbg{$keyCost}{TOTALLOCS} += 1;		  
	   $i=rindex($usedHeapSummaryMemDbg{$_}{CALLER_SYMBOL},"\\");
	   $hashCostSummaryMemDbg{$keyCost}{CALLER_SYMBOL}=substr($usedHeapSummaryMemDbg{$_}{CALLER_SYMBOL},$i+1);   	    
   }
}


# Sort the costliest to the economic by the hashno+lineno combo
sub printCostSummaryTcb {
   my $OUTPUTFILEHANDLE = shift;
   my ($task_name, $thread_id,  $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated);   
   print $OUTPUTFILEHANDLE "Allocations Summary for tasks (Sorted in total allocated bytes)\n";
   print $OUTPUTFILEHANDLE "Task Name, Thread Index,Num of Allocations, Max Allocated,Min Allocated, Total Allocated, Percent Allocated\n";
   $~ = SummaryFormat;
   foreach (sort { $hashCostSummarytcb{$b}{ALLOCBYTES} <=> $hashCostSummarytcb{$a}{ALLOCBYTES}  } keys %hashCostSummarytcb) {
      $thread_id = $_;
	  $task_name = $hashCostSummarytcb{$_}{TASK_NAME};
      $NumAllctns = $hashCostSummarytcb{$_}{TOTALLOCS};
      $MaxAllctd = $hashCostSummarytcb{$_}{MAXALLOC};
      $MinAllctd = $hashCostSummarytcb{$_}{MINALLOC};
      $TotAllctd = $hashCostSummarytcb{$_}{ALLOCBYTES}; 
      if ($heapwalkerfile) {
         $PercentAllocated  = ($hashCostSummarytcb{$_}{ALLOCBYTES} * 100)/$TotalAllocatedBytes;
      }else{
         $PercentAllocated  = ($hashCostSummarytcb{$_}{ALLOCBYTES} * 100)/$TotalAllocatedBytesMemDbg;
      }
      print $OUTPUTFILEHANDLE "$task_name,$thread_id, $NumAllctns,$MaxAllctd,$MinAllctd,$TotAllctd,$PercentAllocated\n";
   }
   print $OUTPUTFILEHANDLE "\n\n";
}

sub printCostSummaryCaller {
   my $OUTPUTFILEHANDLE = shift;
   my ($caller, $caller_symbol,   $NumAllctns, $MaxAllctd, $MinAllctd, $TotAllctd, $PercentAllocated);   
   print $OUTPUTFILEHANDLE "Allocations Summary for Caller (Sorted in total allocated bytes)\n";
   print $OUTPUTFILEHANDLE "Caller Pointer, Caller Symbol, Num of Allocations, Max Allocated,Min Allocated, Total Allocated, Percent Allocated\n";

   $~ = SummaryFormat1;
   foreach (sort { $hashCostSummaryMemDbg{$b}{ALLOCBYTES} <=> $hashCostSummaryMemDbg{$a}{ALLOCBYTES}  } keys %hashCostSummaryMemDbg) {
      $caller = $_;
	  $caller_symbol = $hashCostSummaryMemDbg{$_}{CALLER_SYMBOL};
      $NumAllctns = $hashCostSummaryMemDbg{$_}{TOTALLOCS};
      $MaxAllctd = $hashCostSummaryMemDbg{$_}{MAXALLOC};
      $MinAllctd = $hashCostSummaryMemDbg{$_}{MINALLOC};
      $TotAllctd = $hashCostSummaryMemDbg{$_}{ALLOCBYTES}; 
	  $PercentAllocated  = ($hashCostSummaryMemDbg{$_}{ALLOCBYTES} * 100)/$TotalAllocatedBytesMemDbg;
	  print $OUTPUTFILEHANDLE "$caller,$caller_symbol, $NumAllctns,$MaxAllctd,$MinAllctd,$TotAllctd,$PercentAllocated\n";
   }
   print $OUTPUTFILEHANDLE "\n\n";
}
