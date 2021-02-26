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
# perl heapwalkparser.pl -i heapmem.txt -o heapsum.txt                         #
#                                                                              #
#   Copyright (c) 2006,2007 by QUALCOMM Incorporated. All Rights Reserved.     #
#                                                                              #
################################################################################

################################################################################
#                            EDIT HISTORY FOR FILE                             #
#   This section contains comments describing changes made to the module.      #
#   Notice that changes are listed in reverse chronological order.             #
#                                                                              #
# $Header: //components/rel/dspcore.adsp/4.0/scripts/crashman/heapwalkparser_nondebug.pl#3 $
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
my $outputfile = "";



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
   my ($blockno, $address,$allocated_bytes,$extra_bytes,$freeflag); 
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

   while(<HFH>)
   {

#Block No       Address       Allocated   Extra  Free  Last    
#                             Bytes     Bytes  Flag  Flag  
#=============================================================================== 
# 
#   1    0x01F48A30           1127       4304    0x0  0x00   

      print "$_";
      if(/(\d+)\s+(\S+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)/)
      {
#         print "Blk : $1 Add: $2 Hash: $3 Lineno : $4 Allbyt: $5 ext : $6 free : $7 last : $8 pad : $9 \n";
         $blockno = $1;
         $address = $2;         
         $allocated_bytes = $3;
         $extra_bytes = $4;
         $freeflag = $5;
         $lastflag = $6;         
         if($freeflag ne "0x01")
         {
            # Used Block  
            $TotalNoOfBlocksUsed++; # Starts with 1...
            $usedHeapSummary{$TotalNoOfBlocksUsed}{BLKNO} = $blockno;                   
            $usedHeapSummary{$TotalNoOfBlocksUsed}{BLKADDR} = $address;            
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
print $OUTPUTFILEHANDLE "    Fragmentation Factor                  : ".sprintf("%3.2f%%",(1- ($MaxFreeMemChunk/$TotalFreeMem))*100)."\n";
print $OUTPUTFILEHANDLE "    Total Used Bytes                      : $TotalAllocatedBytes\n";
print $OUTPUTFILEHANDLE "    % of Allocated Space Wasted           : ".sprintf("%3.2f%%",($TotalWstdBytesinHeap*100/$TotalAllocatedBytes))."\n";
print $OUTPUTFILEHANDLE "      (Includes Overhead : 8 bytes/blk)\n";
print $OUTPUTFILEHANDLE "\n\n";

}

sub printBlockSummary {
   my $OUTPUTFILEHANDLE = shift;
   my ($sno,  $startblk, $endblk,  $AlctdBytes, $ExtraBytes);
   
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
@>>>   @>>>>>     @>>>>>  @>>>>>>>>> 
$sno,  $startblk,  $AlctdBytes, $ExtraBytes
.
  $~ = UsedBlkHdrFormat;
  $sno = "";
  $startblk= "Block";  
  $AlctdBytes="Allocted";
  $ExtraBytes="Extra";
  write();
  $sno = "S.NO";
  $startblk= "Number";  
  $AlctdBytes="Bytes";
  $ExtraBytes="Bytes";
  write();

  print $OUTPUTFILEHANDLE "-------------------------------------------------------------------------------------------\n";

  format UsedBlkNumFormat = 
@>>>   @>>>>>     @>>>>>  @>>>>>>>>> 
$sno,  $startblk,  $AlctdBytes, $ExtraBytes
.
     $~ = UsedBlkNumFormat;

   foreach  (sort {$a <=> $b} keys %usedHeapSummary) {
       $sno = $_;
       $startblk= $usedHeapSummary{$_}{BLKNO};      
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
	  $TotalWstdBytesinHeap += $usedHeapSummary{$_}{EXTRABYTES} + SIZEOFMEMBLK;	  
      $TotalAllocatedBytes += $usedHeapSummary{$_}{ALLOCBYTES};	   	    
   }
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
