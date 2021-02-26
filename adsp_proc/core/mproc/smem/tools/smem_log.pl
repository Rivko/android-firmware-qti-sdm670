#!/usr/bin/perl -Xw

#****************************************************************************
#**
#**         smem_log.pl
#**
#** 
#** Copyright (c) 2005 - 2010 by QUALCOMM, Incorporated.  
#** All Rights Reserved.
#****************************************************************************
#**
#**                        EDIT HISTORY FOR MODULE
#**
#**  $Header: //components/rel/core.qdsp6/2.1/mproc/smem/tools/smem_log.pl#1 $
#**
#** when       who     what, where, why
#** --------   ---     ------------------------------------------------------
#** 06/28/13   aep     Add support for IPCROUTER, QCCI & QCSI from android
#** 06/28/13   bt      Interpret timstamps from smem_parser as decimal.
#** 11/30/12   bt      Correct timestamp conversion for B-family: 19.2Mhz(TCXO)
#** 10/18/12   bt      Correct timestamp conversion to 32768 to be accurate.
#** 05/15/12   bm      Support for Legacy QMI messages.
#** 02/28/12   bm      Support for SMSM debug logging
#** 11/21/11   bm      Remove smem static log and sleep voter related logging.
#** 10/26/11   bt      Added processor identifier for WCNSS -> WCNS.
#** 09/19/11   aep     Removed support for DEM, DCVS and SLEEP
#** 02/18/11   aep     Support for IPC Router messages
#** 09/29/10   ap      Cleaned up program number to program name conversion.
#**                    If p4 is installed, the name is got directly from
#**                    mproc's program number database.
#** 09/29/10   ap      Added support for prog and proc in oncrpc reply messages.
#** 06/24/10   tl      Support debugger use of setup.var, which may change how
#**                    data fields are formatted. Handle rare corner cases in
#**                    log indices.
#** 06/24/10   tl      Checked in fix for Chris Medrano: Use lead bits to
#**                    determine which proc a sleep name log comes from.
#** 01/28/10   tl      Added SMD messages already in mainline
#** 01/12/10   ap      Added support for router clients waiting on server reg.
#** 09/30/09   ih      Added profiling support
#** 09/09/09   haa     Added CLKRGM Power logging
#** 07/30/09   ajf     Updated DEM state names to reflect sleep prepare state,
#**                    and support for extended sleep messages.
#** 06/24/09   aep     Support extra smem logging in oncrpc.
#** 05/07/09   sa      Added parsing of router ping message
#** 03/06/09   tl      Suppress log output if log was not initialized
#** 01/16/09   taw     Added support for the DEM INIT event.
#** 01/16/08   ajf     Refactored some of the DEM messages to be more n-way generic.
#** 01/09/09   tl      Added support for power circular log
#** 01/05/09   hwu     Revert the SMSM bits cleanup changes to be backward compatible.
#** 11/26/08   hwu     SMSM bits cleanup.
#** 11/17/08   ajf     Updated the DEM message IDs and added additional decode
#**                    support for new N-way DEM messages.
#** 11/11/08   ih      Added new SMSM bits
#** 10/15/08   ajf     Support for new DEM time sync messages.
#** 08/20/08   taw     Added support for ext. wakeup reasons to DEM logging.
#** 03/20/08   sa      Added support for rpc router messages.
#** 03/11/08   taw     Fix printing of modem non-voters.
#** 01/28/08   taw     Fix the way the sleep voter names are parsed.
#** 01/03/08   ptm     Fix problem with circular log show log entries multiple
#**                    times.
#** 10/05/07   taw     When logging sleep voters, don't reverse the names.
#** 09/26/07   taw     Added support for a new DEM event; also, updated the
#**                    list of wakeup interrupts.
#** 09/14/07   hwu     Added to parse QDSP log. Also update SMD stream states.
#** 08/26/07   taw     Check for the existence of static log files before
#**                    trying to parse them.
#** 08/24/07   taw     Changed the name of the ADSP common interrupt; removed
#**                    sleep_shared_int_names (not used).
#** 08/16/07   ebb     Added support to parse static log files.
#** 08/13/07   ebb     Added support to parse the static sleep voters.
#** 07/11/07   ebb     Added support for SLEEP log events.
#** 07/04/07   hwu     Updates the smsm state machine states.
#** 04/04/07   ptm     Added support for SMEM DM and interrupt events.
#** 03/01/07   taw     Added support for new DEM event, "SETUP APPS SUSPEND".
#** 12/04/06   hwu     Added support for newly added PROGs.
#** 11/21/06   hwu     Added to parse DCVS log messages.
#** 10/03/06   ddh     Corrected info in DEM debug
#** 08/08/06   cab     Added UMTS support
#** 06/27/06   ptm     Added RPC ASYNC call and error fatal task events.
#** 05/27/06   ptm     Added SMSM state printing and new ONCRPC events.
#** 05/24/06   ddh     Added new DEM event for when the apps executes SWFI
#** 05/16/06   ptm     Added ERR support
#** 05/10/06   taw     Added new DEM events, and support for printing 
#**                    DEM wakeup interrupts.
#** 04/10/06   ptm     Changed file parser to handle trace32 wp.v.v files.
#** 04/06/06   ddh     Added DEM support
#** 07/19/05   ptm     
#** 03/23/05   ~SN     Initial Version.
#****************************************************************************

use FileHandle;

# This has to match with NUM_LOG_ENTRIES in drivers/smem/smem_logi.h
my $MAX_LENGTH = 2000;

my $HT_TIMER_CLOCK_RATE = 19200000; # HT Timer
my $SLEEP_CLOCK_RATE    = 32768;    # Sleep clock
my $TIMESTAMP_CLOCK_RATE = $HT_TIMER_CLOCK_RATE; # Default to HT Timer

my $LSB_MASK      = 0x0000ffff;
my $CONTINUE_MASK = 0x30000000;
my $BASE_MASK     = 0x0fff0000;

my $SMEM_LOG_DEBUG_EVENT_BASE         = 0x00000000;
my $SMEM_LOG_ONCRPC_EVENT_BASE        = 0x00010000;
my $SMEM_LOG_SMEM_EVENT_BASE          = 0x00020000;
my $SMEM_LOG_TMC_EVENT_BASE           = 0x00030000;
my $SMEM_LOG_TIMETICK_EVENT_BASE      = 0x00040000;
my $SMEM_ERR_EVENT_BASE               = 0x00060000;
my $SMEM_LOG_RPC_ROUTER_EVENT_BASE    = 0x00090000;
my $SMEM_LOG_CLKREGIM_EVENT_BASE      = 0x000A0000;
my $SMEM_LOG_IPC_ROUTER_EVENT_BASE    = 0x000D0000;
my $SMEM_LOG_QMI_CCI_EVENT_BASE       = 0x000E0000;
my $SMEM_LOG_QMI_CSI_EVENT_BASE       = 0x000F0000;

my $smem_log_events_file = "smem_log_events.lst";

my $LINE_HEADER;

main();

exit;

sub parse_args {
  my $arg = shift @_;

  my $help          = FALSE;
  my $relative_time = FALSE;
  my $ticks         = FALSE;

  while( defined $arg ) {
    if( $arg eq "--help" ) {
      $help = TRUE;
      $arg = shift @_;
      next;
    }

    if( $arg eq "-r" ) {
      $relative_time = TRUE;
      $arg = shift @_;
      next;
    }

    if( $arg eq "-t" ) {
      $ticks = TRUE;
      $arg = shift @_;
      next;
    }
    
    if( $arg eq "-af" ) {
      $TIMESTAMP_CLOCK_RATE = $SLEEP_CLOCK_RATE;
      $arg = shift @_;
      next;
    }

    if( $arg eq "-l" ) {
      $smem_log_events_file = shift @_;
      if(not defined($smem_log_events_file)) {
        print "ERROR: -l option requires an argument for the events log file\n";
        exit;
      }
      $arg = shift @_;
      next;
    }
  }

  if( $help eq TRUE ) {
    print STDERR "usage: smem_log.pl [-r]\n";
    print STDERR "       -r : print relative time instead of absolute\n";
    print STDERR "       -t : print time in ticks instead of seconds\n";
    exit
  }

  return $relative_time, $ticks;
} # parse_args

###########################################################################
#        Trace32 file read and parse routines
###########################################################################


sub create_identifier
{
  my $proc_str   = shift;
  my $subsys_str = shift;
  my $event      = shift;
  my $cont       = shift;

  my $proc_flag = 0x00000000;
  my $subsys_flag = $SMEM_LOG_DEBUG_EVENT_BASE;

  # Compute the flag for proc
  if($proc_str eq "APPS") {
    $proc_flag = 0x80000000;
  } elsif($proc_str eq "QDSP") {
    $proc_flag = 0x40000000;
  }  elsif($proc_str eq "WCNSS") {
    $proc_flag = 0xC0000000;
  }

  # Compute the flag from subsys
  if($subsys_str eq "DEBUG") {
    $subsys_flag = $SMEM_LOG_DEBUG_EVENT_BASE;
  } elsif($subsys_str eq "ONCRPC") {
    $subsys_flag = $SMEM_LOG_ONCRPC_EVENT_BASE;
  } elsif($subsys_str eq "SMEM") {
    $subsys_flag = $SMEM_LOG_SMEM_EVENT_BASE;
  } elsif($subsys_str eq "TMC") {
    $subsys_flag = $SMEM_LOG_TMC_EVENT_BASE;
  } elsif($subsys_str eq "TIMETICK") {
    $subsys_flag = $SMEM_LOG_TIMETICK_EVENT_BASE;
  } elsif($subsys_str eq "ROUTER") {
    $subsys_flag = $SMEM_LOG_RPC_ROUTER_EVENT_BASE;
  } elsif($subsys_str eq "CLKREGIM") {
    $subsys_flag = $SMEM_LOG_CLKREGIM_EVENT_BASE;
  } elsif($subsys_str eq "ERR") {
    $subsys_flag = $SMEM_ERR_EVENT_BASE;
  } elsif($subsys_str eq "IPCROUTER") {
    $subsys_flag = $SMEM_LOG_IPC_ROUTER_EVENT_BASE;
  } elsif($subsys_str eq "QCCI") {
    $subsys_flag = $SMEM_LOG_QMI_CCI_EVENT_BASE;
  } elsif($subsys_str eq "QCSI") {
    $subsys_flag = $SMEM_LOG_QMI_CSI_EVENT_BASE;
  } elsif($subsys_str =~ /^([\da-fA-F]+)$/) {
    $subsys_flag = hex($1);
  }

  my $id = $subsys_flag | $proc_flag | ($event & 0xffff) | ($cont << 28);

  return $id;
}

sub parse_log_file {
  my( $file_name ) = @_;
  my $fd = new FileHandle;
  my @recs;

  open $fd, "<$file_name" or die "Unable to open \"$file_name\"";

 while( <$fd> ) {
   if( m/smem_log_events = (0x[0-9a-f]+)\b/i ) {
     # If this smem log is not initialized, this log pointer will be NULL.
     # In this case, skip the log.
     if( $1 eq '0x0' ) {
       return [];
     }
   }
   # CMM Format 
   if( m/.*?= (\w+).*?= (\w+).*?= (\w+).*?= (\w+).*?= (\w+)/ ) {
     push @recs, [hex $1, hex $2, hex $3, hex $4, hex $5 ];

   # smem_parser.exe format
   } elsif(m/^0x([\da-fA-F]+)\s+(-?\d+)\s+0x([\da-fA-F]+)\s+0x([\da-fA-F]+)\s+0x([\da-fA-F]+)\s+/) {
     my $tsp = $2 + 0;
     if($tsp < 0) {
      $tsp = ($tsp + 0xffffffff) + 1;
     }
     push @recs, [hex $1, $tsp, hex $3, hex $4, hex $5];   
   } elsif(m/^([\da-fA-F]+)\s+([\da-fA-F]+)\s+([\da-fA-F]+)\s+([\da-fA-F]+)\s+([\da-fA-F]+)\s+/) {
     push @recs, [hex $1, hex $2, hex $3, hex $4, hex $5];   
   # Android format
   } elsif(m/^(\w+):\s+(\d+)\s+(\w+):\s+([\da-fA-f]+):\s+([\da-fA-f]+)\s+([\da-fA-f]+)\s+([\da-fA-f]+)\s*(.*)$/) {
     my $proc_str   = $1;
     my $timestamp  = $2 + 0;
     my $subsys_str = $3;
     my $event      = hex $4;
     my $d1         = hex $5;
     my $d2         = hex $6;
     my $d3         = hex $7;
     my $rem        = $8;

     my $id = create_identifier($proc_str, $subsys_str, $event, 0);
     push @recs, [$id, $timestamp, $d1, $d2, $d3];

     if($rem =~ /([\da-fA-f]+)\s+([\da-fA-f]+)\s+([\da-fA-f]+)/) {
       $id = create_identifier($proc_str, $subsys_str, $event, 1);
       push @recs, [$id, $timestamp, hex $1, hex $2, hex $3];
     }
   }
 }

 # Adjust max length
 if(scalar(@recs) > $MAX_LENGTH) {
   $MAX_LENGTH = scalar(@recs) + 1;
 }

  return \@recs;
} # parse_log_file

sub parse_index_file {
  my( $file_name ) = @_;
  my $fd = new FileHandle;
  my $idx = 0;

  if(! -e $file_name) {
    print STDERR "Warning: smem_log_index.lst is missing, assuming index is 0\n";
    return 0;
  }

  open $fd, "<$file_name" or die "Unable to open \"$file_name\"";

  # Skip first line - header junk
  <$fd>;

  $_ = <$fd>;
  if( m/.*?= (\w+)/ ) {
    $idx = hex $1;
  }

  return $idx;
} # parse_index_file

###########################################################################
#        ONCRPC Definitions and print routines
###########################################################################

BEGIN {        # Use begin block so that these variables are defined at runtime
  $RPC_PRINT_TABLE = [ "GOTO SMD WAIT",
                       "GOTO RPC WAIT",
                       "GOTO RPC BOTH WAIT",
                       "GOTO RPC INIT",
                       "GOTO RUNNING",
                       "APIS INITED",
                       "AMSS RESET - GOTO SMD WAIT",
                       "SMD RESET - GOTO SMD WAIT",
                       "ONCRPC RESET - GOTO SMD WAIT",
                       EVENT_CB,
                       STD_CALL,
                       STD_REPLY,
                       STD_CALL_ASYNC,
                       ERR_NO_PROC,
                       ERR_DECODE,
                       ERR_SYSTEM,
                       ERR_AUTH,
                       ERR_NO_PROG,
                       ERR_PROG_LOCK,
                       ERR_PROG_VERS,
                       ERR_VERS_MISMATCH,
                       CALL_START,
                       DISPATCH_PROXY,
                       HANDLE_CALL,
                       MSG_DONE ];

  $QMI_PRINT_TABLE = [ "TX",
                       "RX",
                       "ERROR" ];
  $QMI_CNTL_PRINT_TABLE = [ "REQ ",
                            "UNK ",
                            "RESP",
                            "UNK ",
                            "IND " ];

  $RPC_PROG_NAMES = { };

} # BEGIN


sub HasCommand
{
  my $cmd = shift;
  my $tmp;
  if($^O eq "linux") {
    $tmp = `which $cmd 2> /dev/null`;
  } else {
    $tmp = `which $cmd 2> NUL`;
  }

  if($tmp =~ /^\s*$/) {
    return 0;
  }
  return 1;
}

sub OpenP4
{
  my $refFileHandle          = shift;
  my $file                   = shift;

  local *TMP;
  open TMP,"p4 print -q $file | " or return 0;
  $$refFileHandle = *TMP;
  return 1;
}

sub GetProgramName
{
  my $prog_id = shift;
  $prog_id = lc(sprintf("0x%08x",$prog_id));
  if(defined($$RPC_PROG_NAMES{$prog_id})) {
    return $$RPC_PROG_NAMES{$prog_id};
  }
  return $prog_id;
}


sub PopulateProgNames
{
  my $p4path = "//source/qcom/qct/core/mproc/tools/main/latest/oncrpc_api_init/ONCRPC_program_numbers.csv";

  if(HasCommand("p4") == 0) {
    print STDERR "P4 not installed. Program number => name translation will be skipped\n";
    return;
  }

  my $INF;
  # Leave the hash table alone if failure
  if(OpenP4(\$INF, $p4path) != 1) {
    print STDERR "Could not open perforce path of ONCRPC_program_numbers.csv.\n";
    print STDERR "Program number => name translation will be skipped\n";
    return;
  }

  # Ignore header
  my $line = <$INF>;

  while($line = <$INF>) {
    chomp($line);
    my @tmp = split(/,/,$line);
    my ($prog, $name, $apps, $modem) = (lc($tmp[0]), lc($tmp[1]), lc($tmp[5]), lc($tmp[6]));

    next if($apps =~ /reserved/ or $modem =~ /reserved/);
 
    $$RPC_PROG_NAMES{$prog} = $name;
    $prog =~ s/0x30/0x31/g;
    $$RPC_PROG_NAMES{$prog} = "$name CB";
  }
  close($INF);
}

my $QCCI_ERR_DATA1 = ".";
my $QCCI_ERR_DATA2 = ".";
my $QCCI_ERR_DATA3 = ".";

sub qmi_cci_print {
  my( $id, $d1, $d2, $d3 ) = @_;

  my $cont = $id & $CONTINUE_MASK;
  $id = $id & 0xffff;

  if($id == 0x3) {
    if($cont eq 0) {
      $QCCI_ERR_DATA1 = $d1;
      $QCCI_ERR_DATA2 = $d2;
      $QCCI_ERR_DATA3 = $d3;
    } else {
      printf("QCCI:   ERROR File = %c%c%c%c%c, Line=%d", 
      $QCCI_ERR_DATA1, $QCCI_ERR_DATA2, $QCCI_ERR_DATA3,
      $d1, $d2, $d3);
    }
  } elsif($id == 0x0 or $id == 0x1) {
    # Legacy TX and RX
    my $type = $$QMI_PRINT_TABLE[$id];
    printf( "QCCI:   $type $$QMI_CNTL_PRINT_TABLE[$d1>>16] Txn:0x%x Msg:0x%x Len:%d", $d1 & 0xffff, $d2 >> 16, $d2 & 0xffff);
  } elsif($id == 0x4 or $id == 0x5) {
    # Extended TX and RX
    my $type = $$QMI_PRINT_TABLE[$id - 0x4];
    if($cont eq 0) {
      printf( "QCCI:   $type $$QMI_CNTL_PRINT_TABLE[$d1>>16] Txn:0x%x Msg:0x%x Len:%d svc_id:0x%x", $d1 & 0xffff, $d2 >> 16, $d2 & 0xffff, $d3);
    } else {
      printf(" svc_addr: %04x:%04x:%04x", $d1, $d2, $d3);
    }
  }
}

my $QCSI_ERR_DATA1 = ".";
my $QCSI_ERR_DATA2 = ".";
my $QCSI_ERR_DATA3 = ".";

sub qmi_csi_print {
  my( $id, $d1, $d2, $d3 ) = @_;

  my $cont = $id & $CONTINUE_MASK;
  $id = $id & 0xffff;


  if($id == 0x3) {
    if($cont eq 0) {
      $QCSI_ERR_DATA1 = $d1 & 0xff;
      $QCSI_ERR_DATA2 = $d2 & 0xff;
      $QCSI_ERR_DATA3 = $d3 & 0xff;
    } else {
      printf("QCSI:   ERROR File = %c%c%c%c%c, Line=%d", 
      $QCCI_ERR_DATA1, $QCCI_ERR_DATA2, $QCCI_ERR_DATA3,
      $d1 & 0xff, $d2 & 0xff, $d3);
    }
  
  } elsif($id == 0x0 or $id == 0x1) {
    my $type = $$QMI_PRINT_TABLE[$id];
    printf( "QCSI:   $type $$QMI_CNTL_PRINT_TABLE[$d1>>16] Txn:0x%x Msg:0x%x Len:%d", $d1 & 0xffff, $d2 >> 16, $d2 & 0xffff);
  } elsif($id == 0x4 or $id == 0x5) {
    my $type = $$QMI_PRINT_TABLE[$id - 0x4];
    if($cont eq 0) {
      printf( "QCSI:   $type $$QMI_CNTL_PRINT_TABLE[$d1>>16] Txn:0x%x Msg:0x%x Len:%d svc_id:0x%x", $d1 & 0xffff, $d2 >> 16, $d2 & 0xffff, $d3);
    } else {
      printf(" clnt_addr: %04x:%04x:%04x", $d1, $d2, $d3);
    }
  }
}

sub oncrpc_print {
  my( $id, $d1, $d2, $d3 ) = @_;
  my $subsys = $id & 0xf0;
  my $event = $id & $LSB_MASK;
  my $cntrl = $$RPC_PRINT_TABLE[$event];
  my $prog = GetProgramName($d2);

  if( $subsys )
  {
    # BW Compatible print for QCCI & QCSI
    if($subsys == 0x30) {
      qmi_cci_print($id & 0xffffff0f, $d1, $d2, $d3);
    } else {
      qmi_csi_print($id & 0xffffff0f, $d1, $d2, $d3);
    }
  }
  elsif( $cntrl eq ERR_NO_PROC ) {
      printf( "ONCRPC: ERROR PROC NOT SUPPORTED   xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_DECODE ) {
      printf( "ONCRPC: ERROR ARGS DECODE FAILED   xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_SYSTEM ) {
      printf( "ONCRPC: ERROR SYSTEM FAULT  xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_AUTH ) {
      printf( "ONCRPC: ERROR AUTHENTICATION FAILED  xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_NO_PROG ) {
      printf( "ONCRPC: ERROR PROG NOT EXPORTED  xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_PROG_LOCK ) {
      printf( "ONCRPC: ERROR PROG LOCKED  xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_PROG_VERS ) {
      printf( "ONCRPC: ERROR PROG VERS NOT SUPPORED  xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq ERR_VERS_MISMATCH ) {
      printf( "ONCRPC: ERROR RPC VERS NOT SUPPORTED   xid = %08x    proc = %3d    prog = $prog",
              $d1, $d3 );
    }
  elsif( $cntrl eq EVENT_CB ) {
    printf( "ONCRPC: EVENT_CB  %s ",
            $d1 eq 1 ? "LOCAL" : "REMOTE" );
    printf( " %sONCRPC: prev = %s", $LINE_HEADER, smsm_state_to_string($d2) );
    printf( " %sONCRPC: curr = %s", $LINE_HEADER, smsm_state_to_string($d3) );
  }
  elsif( $cntrl eq STD_CALL or $cntrl eq NBS_CALL ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First CALL record
        printf( "ONCRPC: CALL    xid = %08x    proc = %3d    prog = $prog",
                $d1, $d3 );
      }
      else {
      # Second CALL record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  elsif( $cntrl eq STD_REPLY or $cntrl eq NBS_REPLY ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First REPLY record
      printf( "ONCRPC: REPLY   xid = %08x    proc = %3d    prog = $prog", $d1, $d3 );
    }
    else {
      # Second REPLY record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  elsif( $cntrl eq STD_CALL_ASYNC or $cntrl eq NBS_CALL_ASYNC ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First CALL record
        printf( "ONCRPC: ASYNC   xid = %08x    proc = %3d    prog = $prog",
                $d1, $d3 );
      }
      else {
      # Second CALL record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  elsif( $cntrl eq CALL_START or $cntrl eq NBS_START ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First CALL record
        printf( "ONCRPC: START   xid = %08x    proc = %3d    prog = $prog",
                $d1, $d3 );
      }
      else {
      # Second CALL record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  elsif( $cntrl eq DISPATCH_PROXY or $cntrl eq NBS_DISPATCH ) {
    printf( "ONCRPC: DISPATCH  xid = %08x    func = %08x", $d1, $d2 );
  }
  elsif( $cntrl eq HANDLE_CALL or $cntrl eq NBS_HANDLE ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First CALL record
        printf( "ONCRPC: HANDLE  xid = %08x    proc = %3d    prog = $prog",
                $d1, $d3 );
      }
      else {
      # Second CALL record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  elsif( $cntrl eq MSG_DONE or $cntrl eq NBS_DONE ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First CALL record
        printf( "ONCRPC: DONE    xid = %08x    proc = %3d    prog = $prog",
                $d1, $d3 );
      }
      else {
      # Second CALL record
      my $name = pack( "I3", $d1, $d2, $d3);
      $name =~ s/\0.*//;
      print "    task = $name";
    }
  }
  else {
    generic_print( $RPC_PRINT_TABLE, "ONCRPC", @_ );
  }
} # oncrpc_print

sub smsm_state_to_string {
  my( $state ) = @_;
  my $string = "";

  $string .= ($state & 0x80000000) ? "UN " : "   ";
  $string .= ($state & 0x70000000) ? "ERR " : "    ";
  $string .= ($state & 0x08000000) ? "MWT " : "    ";
  $string .= ($state & 0x04000000) ? "MCNT " : "    ";
  $string .= ($state & 0x02000000) ? "MBRK " : "    ";
  $string .= ($state & 0x01000000) ? "RNQT " : "    ";
  $string .= ($state & 0x00800000) ? "SMLP " : "    ";
  $string .= ($state & 0x00400000) ? "ADWN " : "    ";
  $string .= ($state & 0x00200000) ? "PWRS " : "    ";
  $string .= ($state & 0x00100000) ? "DWLD " : "    ";
  $string .= ($state & 0x00080000) ? "SRBT " : "    ";
  $string .= ($state & 0x00040000) ? "SDWN " : "    ";
  $string .= ($state & 0x00020000) ? "ARBT " : "    ";
  $string .= ($state & 0x00010000) ? "REL " : "    ";
  $string .= ($state & 0x00008000) ? "SLE " : "    ";
  $string .= ($state & 0x00004000) ? "SLP " : "    ";
  $string .= ($state & 0x00002000) ? "WFPI " : "     ";
  $string .= ($state & 0x00001000) ? "EEX " : "    ";
  $string .= ($state & 0x00000800) ? "TIN " : "    ";
  $string .= ($state & 0x00000400) ? "TWT " : "    ";
  $string .= ($state & 0x00000200) ? "PWRC " : "     ";
  $string .= ($state & 0x00000100) ? "RUN " : "    ";
  $string .= ($state & 0x00000080) ? "SA " : "   ";
  $string .= ($state & 0x00000040) ? "RES " : "    ";
  $string .= ($state & 0x00000020) ? "RIN " : "    ";
  $string .= ($state & 0x00000010) ? "RWT " : "    ";
  $string .= ($state & 0x00000008) ? "SIN " : "    ";
  $string .= ($state & 0x00000004) ? "SWT " : "    ";
  $string .= ($state & 0x00000002) ? "OE " : "   ";
  $string .= ($state & 0x00000001) ? "I" : " ";

  return $string;
} # smsm_state_to_string

###########################################################################
#        SMEM Definitions and print routines
###########################################################################

BEGIN {        # Use begin block so that these variables are defined at runtime
  @smem_smsm_state_names = ( "IGNORE ", "WAIT   ", "INIT   ", "RUNNING" );
  @smem_stream_state_names = ( "CLOSED", "OPENING", "OPENED",
                               "FLUSHING", "CLOSING", "RESET",
                               "RESET_OPENING" );
  @smem_stream_event_names = ( "CLOSE", "OPEN", "REMOTE OPEN",
                               "REMOTE CLOSE", "FLUSH",
                               "FLUSH COMPLETE", "REMOTE_REOPEN",
                               "REMOTE_RESET");

  $SMEM_PRINT_TABLE = [ EVENT_CB,
                        "START",
                        "INIT",
                        "RUNNING",
                        "STOP",
                        "RESTART",
                        EVENT_SS,
                        EVENT_READ,
                        EVENT_WRITE,
                        EVENT_SIGS1,
                        EVENT_SIGS2,
                        "WRITE DM",
                        "READ DM",
                        "SKIP DM",
                        "STOP DM",
                        "ISR",
                        "TASK",
                        "EVENT_RS" ];
} # BEGIN

sub smem_print {
  my( $id, $d1, $d2, $d3 ) = @_;
  my $event = $id & $LSB_MASK;
  my $cntrl = $$SMEM_PRINT_TABLE[$event];

  if( $cntrl eq EVENT_CB ) {
    printf( "SMEM:   EVENT_CB  %s ",
            $d1 eq 1 ? "LOCAL" : "REMOTE" );
    printf( "%sSMEM:   prev = %s ", $LINE_HEADER, smsm_state_to_string($d2) );
    printf( "%sSMEM:   curr = %s ", $LINE_HEADER, smsm_state_to_string($d3) );
  }
  elsif( $cntrl eq EVENT_SS ) {
    printf( "SMEM: EVENT SS  port %d  state = %s  event = %s",
            $d1,
            $smem_stream_state_names[$d2],
            $smem_stream_event_names[$d3] );
  }
  elsif( $cntrl eq EVENT_READ ) {
    if( $d1 eq 0 ) {
      printf( "SMEM: EVENT READ enter port %d", $d2 );
    }
    else {
      printf( "SMEM: EVENT READ exit port %d (%d)", $d2, $d1 );
    }
  }
  elsif( $cntrl eq EVENT_WRITE ) {
    if( $d1 eq 0 ) {
      printf( "SMEM: EVENT WRITE enter port %d", $d2 );
    }
    else {
      printf( "SMEM: EVENT WRITE exit port %d (%d)", $d2, $d1 );
    }
  }
  elsif( $cntrl eq EVENT_SIGS1 ) {
    printf( "SMEM: EVENT SIGS  port %d received %08x converted to %08x",
            $d1, $d2, $d3 );
  }
  elsif( $cntrl eq EVENT_SIGS2 ) {
    printf( "SMEM: EVENT_SIGS  port %d  old = %08x  new = %08x",
            $d1, $d2, $d3 );
  }
  elsif( $cntrl eq EVENT_RS ) {
    printf( "SMEM: EVENT_REMOTE_STATE  port %d  old = %s  new = %s",
            $d1, $smem_stream_state_names[$d2], $smem_stream_state_names[$d3] );
  }
  elsif( $event == 0x2000 ) {
    printf( "SMEM: EVENT WRITE port %d  write_index = %d  byte_count = %d",
            $d1, $d2, $d3 );
  }
  elsif( $event == 0x2001 ) {
    printf( "SMEM: EVENT READ port %d  read_index = %d  byte_count = %d",
            $d1, $d2, $d3 );
  }
  elsif( $event == 0xbcbc ) {
    printf( "SMEM: SMDI INIT  Channel table entry = %d bytes",
             $d1 );
  }
  else {
    generic_print( $SMEM_PRINT_TABLE, "SMEM", @_ );
  }
} # smem_print

###########################################################################
#        RPC/IPC ROUTER Definitions and print routines
###########################################################################
BEGIN {        # Use begin block so that these variables are defined at runtime
  @ROUTER_PRINT_TABLE = ( "UNKNOWN",
                          "READ   ",
                          "WRITTEN",
                          "CNF REQ",
                          "CNF SNT",
                          "MID READ",
                          "MID WRITTEN",
                          "MID CNF REQ",
                          "PING",
                          "SERVER PENDING",
                          "SERVER REGISTERED",
                          "RESERVED", 
                          "RESERVED", 
                          "RESERVED", 
                          "RESERVED", 
                          "RESERVED", 
                          "IPC_ROUTER", # BW Compatible IPC Router messages 
                          "IPC_ROUTER",
                          "IPC_ROUTER"
                          );

  @IPC_ROUTER_PRINT_TABLE = ( "ERROR",
                          "TX",
                          "RX",
                          );

  @IPC_ROUTER_TYPE_TABLE = ( "INVALID",
                         "DATA",
                         "HELLO",
                         "BYE",
                         "NEW_SERVER",
                         "REMOVE_SERVER",
                         "REMOVE_CLIENT",
                         "RESUME_TX"
                         );
} # BEGIN

sub ipc_router_print {
  my ($id, $d1, $d2, $d3) = @_;
  my $event = $id & 0xff;
  my $cntl_type = ($id >> 8) & 0xff;
  my $cntrl = $IPC_ROUTER_PRINT_TABLE[$event];

  if( $cntrl eq "ERROR") {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First ERR record
      printf( "ROUTER: ERROR " );
      $ERR_DATA1 = $d1;
      $ERR_DATA2 = $d2;
      $ERR_DATA3 = $d3;
    }
    else {
      # Second ERR record
      my $name = pack( "I5", $ERR_DATA1, $ERR_DATA2, $ERR_DATA3, $d1, $d2 );
      $name =~ s/\0.*//;
      print "file: \"$name\" line: $d3";
    }
  }

  elsif( ($cntrl eq "TX") or ($cntrl eq "RX") ){
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      if( $cntl_type >= 4 && $cntl_type <= 5) {
        my $src_proc = $d1 >> 24;
        my $src_port = $d1 & 0xffffff;
        printf( "ROUTER: $cntrl \[@IPC_ROUTER_TYPE_TABLE[$cntl_type]\] (%x,%x) @ %02x:%06x", $d2, $d3, $src_proc, $src_port);
      }
      elsif( $cntl_type >= 6 && $cntl_type <= 7) {
        printf( "ROUTER: $cntrl *@IPC_ROUTER_TYPE_TABLE[$cntl_type]* %08x:%08x", $d1, $d2);
      }
      else {
        my $src_proc = $d1 >> 24;
        my $src_port = $d1 & 0xffffff;
        my $dst_proc = $d2 >> 24;
        my $dst_port = $d2 & 0xffffff;
        my $type = $d3 >> 24;
        my $conf_rx = ($d3 >> 16) & 1;
        my $size = $d3 & 0xffff;
        if($cntrl eq "TX") {
          printf( "ROUTER: $cntrl %02x:%06x -> %02x:%06x ",
              $src_proc, $src_port, $dst_proc, $dst_port );
        } else {
          printf( "ROUTER: $cntrl %02x:%06x <- %02x:%06x ",
              $dst_proc, $dst_port, $src_proc, $src_port);
        }
        printf ("\[@IPC_ROUTER_TYPE_TABLE[$type]\] Len:$size ");
        if($conf_rx) {
          printf( "*CONF_RX* ");
        }
      }
    }
    else {
      my $if = pack( "I1", $d1 );
      $if =~ s/\0.*//;
      my $task = pack( "I1", $d3 );
      $task =~ s/\0.*//;
      printf( "<$if> TID:%08x,\"$task\"", $d2); 
    }
  }
}

sub rpc_router_print {

  my($id, $d1, $d2, $d3 ) = @_;
  my $event = $id & 0xff;
  my $cntl_type = ($id >> 8) & 0xff;
  my $cntrl = $ROUTER_PRINT_TABLE[$event];

  if($cntrl eq "IPC_ROUTER") {
    # Old combination made the IPC Router start at 16, 
    # but the new ones start at 0.
    ipc_router_print($id - 16, $d1, $d2, $d3);
  } 
  elsif( ($cntrl eq "CNF REQ") or ($cntrl eq "CNF SNT")){
    printf( "ROUTER: %s pid = %08x    cid = %08x    tid = %08x", 
             $cntrl, $d1, $d2, $d3 );
  }
  elsif( $cntrl eq "MID READ"){
    printf( "ROUTER: READ    mid = %08x    cid = %08x    tid = %08x", 
             $d1, $d2, $d3 );
  }
  elsif( $cntrl eq "MID WRITTEN"){
    printf( "ROUTER: WRITTEN mid = %08x    cid = %08x    tid = %08x", 
             $d1, $d2, $d3 );
  }
  elsif( $cntrl eq "MID CNF REQ"){
    printf( "ROUTER: CNF REQ pid = %08x    cid = %08x    tid = %08x", 
             $d1, $d2, $d3 );
  }
  elsif( $cntrl eq "PING"){
     printf( "ROUTER: PING    pid = %08x    cid = %08x    tid = %08x", 
             $d1, $d2, $d3 );
  }
  elsif( $ctrl eq "SERVER PENDING"){
    printf("ROUTER: SERVER PENDING REGISTRATION    prog = 0x%08x vers=0x%08x tid = %08x",
            $d1, $d2, $d3  );
  }
  elsif ($ctrl eq "SERVER REGISTERED") {
    printf("ROUTER: PENDING SERVER REGISTERED      prog = 0x%08x vers=0x%08x tid = %08x",
            $d1, $d2, $d3  );
  }
  elsif( defined $cntrl ) {
    printf( "ROUTER: %s xid = %08x    cid = %08x    tid = %08x", 
              $cntrl, $d1, $d2, $d3 );
  } 
  else {
    generic_print( $ROUTER_PRINT_TABLE, "ROUTER:   ", @_ );
  }
} # rpc_router_print

###########################################################################
#        Error Definitions and print routines
###########################################################################

BEGIN {        # Use begin block so that these variables are defined at runtime
  $ERR_PRINT_TABLE = [ "UNUSED",
                       EVENT_FATAL,
                       EVENT_FATAL_TASK ];

  $ERR_DATA1 = "....";
  $ERR_DATA2 = "....";
  $ERR_DATA3 = "....";
} # BEGIN

sub err_print {
  my( $id, $d1, $d2, $d3 ) = @_;
  my $event = $id & $LSB_MASK;
  my $cntrl = $$ERR_PRINT_TABLE[$event];

  if( $cntrl eq EVENT_FATAL ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First ERR record
      printf( "ERR: FATAL " );
      $ERR_DATA1 = $d1;
      $ERR_DATA2 = $d2;
      $ERR_DATA3 = $d3;
    }
    else {
      # Second ERR record
      my $name = pack( "I5", $ERR_DATA1, $ERR_DATA2, $ERR_DATA3, $d1, $d2 );
      $name =~ s/\0.*//;
      print " file: \"$name\" line: $d3";
    }
  }
  elsif( $cntrl eq EVENT_FATAL_TASK ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      # First ERR record
      printf( "ERR: FATAL " );
      $ERR_DATA1 = $d1;
      $ERR_DATA2 = $d2;
      $ERR_DATA3 = $d3;
    }
    else {
      # Second ERR record
      my $name = pack( "I5", $ERR_DATA1, $ERR_DATA2, $ERR_DATA3, $d1, $d2 );
      $name =~ s/\0.*//;
      printf( " task: \"$name\" tcb addr: 0x%08x", $d3 );
    }
  }
  else {
    generic_print( $ERR_PRINT_TABLE, "ERR", @_ );
  }
} # err_print

###########################################################################
#        CLKRGM Definitions and print routines
###########################################################################

BEGIN {        # Use begin block so that these variables are defined at runtime
  $CLKRGM_PRINT_TABLE = [ "PLL_ON",
                        "PLL_OFF",
                        "CXO_ON",
                        "CXO_OFF",
                        "PXO_ON",
                        "PXO_OFF",
                        "ACPU_PWRCLPS",
                        ACPU_RESTORE,
                        "APC_RAIL_OFF",
                        "APC_RAIL_ON",
                        SET_MSMC1,
                        SET_MSMC2,
                        SWITCH_MCPU,
                        "SWITCH_SYSBUS",
                        SWITCH_ACPU ];
} # BEGIN

sub clkrgm_print {
  my( $id, $d1, $d2, $d3 ) = @_;
  my $event = $id & $LSB_MASK;
  my $cntrl = $$CLKRGM_PRINT_TABLE[$event];

  if( $cntrl eq SWITCH_MCPU ) {
    printf( "CLKRGM: SWITCH_MCPU - MCPU speed - new = %d, old = %d ", $d1, $d2);
  }
  elsif( $cntrl eq SWITCH_ACPU ) {
    printf( "CLKRGM: SWITCH_ACPU - ACPU speed - new = %d, old = %d ", $d1, $d2);
  }
  elsif( $cntrl eq SET_MSMC1 ) {
    printf( "CLKRGM: SET_MSMC1 - voltage - new = %d, old = %d ", $d1, $d2);
  }
  elsif( $cntrl eq SET_MSMC2 ) {
    printf( "CLKRGM: SET_MSMC2 - voltage - new = %d, old = %d ", $d1, $d2);
  }
  elsif( $cntrl eq ACPU_RESTORE ) {
    printf( "CLKRGM: ACPU_RESTORE - current perf level = %d ", $d1);
  }
  else  {
    generic_print( $CLKRGM_PRINT_TABLE, "CLKRGM", @_ );
  }
} # clkrgm_print


###########################################################################
#        Generic and common print routines
###########################################################################
my $TMC_PRINT_TABLE =
    [ "WAIT",
      "GOTO INIT",
      "BOTH INIT" ];

my $TIMETICK_PRINT_TABLE =
    [ "START",
      "GOTO WAIT",
      "GOTO INIT" ];

sub generic_print {
  my( $table, $name, $id, $d1, $d2, $d3 ) = @_;
  my $event = $id & $LSB_MASK;
  my $cntrl = $$table[$event];
  
  if( defined $cntrl ) {
      printf( "$name: $cntrl", $d1, $d2, $d3 );
  }
  else {
    default_print( @_ );
  }
} # generic_print

sub print_line_header {
  my( $proc_flag, $time, $ticks ) = @_;

  if( $ticks eq TRUE ) {
    if ( $proc_flag eq 0x80000000 ) {
      $LINE_HEADER = sprintf( "\nAPPS: 0x%08x    ",  $time );
    }
    elsif ( $proc_flag eq 0x40000000 ) {
      $LINE_HEADER = sprintf( "\nQDSP: 0x%08x    ",  $time );
    }
    elsif ( $proc_flag eq 0xC0000000 ) {
      $LINE_HEADER = sprintf( "\nWCNS: 0x%08x    ", $time );
    }
    else {
      $LINE_HEADER = sprintf( "\nMODM: 0x%08x    ", $time );
    }
  }
  else {
    # Convert time from ticks to seconds
    $time = $time / $TIMESTAMP_CLOCK_RATE;
        
    if ( $proc_flag eq 0x80000000 ) {
      $LINE_HEADER = sprintf( "\nAPPS: %14.6f    ",  $time );
    }
    elsif ( $proc_flag eq 0x40000000 ) {
      $LINE_HEADER = sprintf( "\nQDSP: %14.6f    ",  $time );
    }
    elsif ( $proc_flag eq 0xC0000000 ) {
      $LINE_HEADER = sprintf( "\nWCNS: %14.6f    ", $time );
    }
    else {
      $LINE_HEADER = sprintf( "\nMODM: %14.6f    ", $time );
    }
  }

  print $LINE_HEADER;
} # print_line_header
  
sub default_print {
  shift @_;                     # remove table
  printf( "%s: %08x    %08x    %08x    %08x", @_ );
} # default_print

sub debug_print {

  my @smsm_host = ( "APPS",
                    "MODM",
                    "ADSP",
                    "WCNS",
                    "DSPS" );

  my( $id, $d1, $d2, $d3 ) = @_;
  my $event = $id & $LSB_MASK;

  if( $event eq 100 ) {
    printf( "PROXY: dispatch to %08x, queue len = %d", $d1, $d2 );
  }
  elsif( $event eq 1 ) {
    printf( "SMSM Rx Intr: From = %s, Shadow=%08x,  Shared=%08x", $smsm_host[$d1], $d2, $d3 );
  }
  elsif( $event eq 2 ) {
    printf( "SMSM Tx Intr: To  =  %s, Shadow=%08x,  Shared=%08x", $smsm_host[$d1], $d2, $d3 );
  }
  elsif( $event eq 3 ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      printf( "QMUX: RX, len: %d, ctl_flags: 0x%02x, svc_type: %d, ", $d1, $d2, $d3 );
    } else {
      printf("clint_id: %d, qmi_inst: %d; msg_cntl: 0x%02x, tx_id: %d", $d1, ($d2>>16), ($d2 & 0xFFFF), $d3);
    }
  }
  elsif( $event eq 4 ) {
    if( ($id & $CONTINUE_MASK) eq 0 ) {
      printf( "QMUX: TX, len: %d, ctl_flags: 0x%02x, svc_type: %d, ", $d1, $d2, $d3 );
    } else {
      printf("clint_id: %d, qmi_inst: %d; msg_cntl: 0x%02x, tx_id: %d", $d1, ($d2>>16), ($d2 & 0xFFFF), $d3);
    }
  }
  else {
    if(($id & $CONTINUE_MASK) eq 0) {
      printf "DEBUG: 0x%08x 0x%08x 0x%08x ", $d1, $d2, $d3;
    } else {
      printf "0x%08x 0x%08x 0x%08x ", $d1, $d2, $d3;
    }
  }
} #debug_print

sub print_record {
  my( $rec ) = @_;
  my $id = $$rec[0] & $BASE_MASK;

  if( $id == $SMEM_LOG_DEBUG_EVENT_BASE ) {
    debug_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif ( $id == $SMEM_LOG_ONCRPC_EVENT_BASE )
  {
    oncrpc_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif ( $id == $SMEM_LOG_QMI_CCI_EVENT_BASE )
  {
    qmi_cci_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif ( $id == $SMEM_LOG_QMI_CSI_EVENT_BASE )
  {
    qmi_csi_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif( $id == $SMEM_LOG_SMEM_EVENT_BASE ) {
    smem_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif( $id == $SMEM_LOG_TMC_EVENT_BASE) {
    generic_print( $TMC_PRINT_TABLE, "TMC",
                   $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif( $id == $SMEM_LOG_TIMETICK_EVENT_BASE) {
    generic_print( $TIMETICK_PRINT_TABLE, "TIMETICK",
                   $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif( $id == $SMEM_ERR_EVENT_BASE) {
    err_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif ( $id == $SMEM_LOG_RPC_ROUTER_EVENT_BASE )
  {
    rpc_router_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif ($id == $SMEM_LOG_IPC_ROUTER_EVENT_BASE )
  {
    ipc_router_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  elsif( $id == $SMEM_LOG_CLKREGIM_EVENT_BASE) {
    clkrgm_print( $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
  else {
    # Unknown event base just do "default" print
    default_print( undef, "UNKNOWN", $$rec[0], $$rec[2], $$rec[3], $$rec[4] );
  }
} # print_record

sub main {
  my( $relative_time, $ticks ) = parse_args( @ARGV );
  my $base_time;

  if(-f $smem_log_events_file)
  {
    print "\n==============================\n" .
          "        CIRCULAR LOG          \n" .
          "==============================\n\n" ;

    print_circular_log($smem_log_events_file, "smem_log_index.lst",
                   $relative_time, $ticks);
  }

  if(-f "smem_log_power_events.lst" && -f "smem_log_power_index.lst")
  {
    print "\n==============================\n" .
          "      POWER CIRCULAR LOG      \n" .
          "==============================\n\n" ;
    print_circular_log("smem_log_power_events.lst", "smem_log_power_index.lst",
                       $relative_time, $ticks);
  }
} # main

sub print_circular_log {
  my $log_events    = parse_log_file  ( shift );
  my $index         = parse_index_file( shift );
  my $relative_time = shift;
  my $ticks         = shift;

  my $idx;
  my $base_time  = 0;

  # Suppress empty logs
  return unless @$log_events;

  # There are rare but documented cases where the index reported for a
  # particular log is greater than the log size. It's not clear whether this
  # represents memory corruption or a bug in the shared memory log itself or in
  # the reporting and parsing scripts or procedure. Regardless of the source of
  # the corruption, only show as many log events as we believe should exist.
  if($index > $MAX_LENGTH) {
    $index = $MAX_LENGTH;
  }

  for( $idx = $index; $idx < $MAX_LENGTH; $idx++ ) {
    my $rec = $$log_events[$idx];

    if( $$rec[0] != 0 ) {
      if( $relative_time eq TRUE ) {
        $base_time = $$rec[1];
        $relative_time = FALSE;
      }

      if( ($$rec[0] & $CONTINUE_MASK) eq 0 ) {
        # Not continuation event, print header
        print_line_header( $$rec[0] & 0xC0000000, # modem/apps/q6 flag
                           $$rec[1] - $base_time, # time
                           $ticks );              # seconds/ticks flag
      }

      print_record( $rec );
    }
  }

  for(  $idx = 0; $idx < $index; $idx++ ) {
    my $rec = $$log_events[$idx];

    if( $$rec[0] != 0 ) {
      if( $relative_time eq TRUE ) {
        $base_time = $$rec[1];
        $relative_time = FALSE;
      }

      if( ($$rec[0] & $CONTINUE_MASK) eq 0 ) {
        # Not continuation event, print header
        print_line_header( $$rec[0] & 0xC0000000, # modem/apps/q6 flag
                           $$rec[1] - $base_time, # time
                           $ticks );              # seconds/ticks flag
      }

      print_record( $rec );
    }
  }
} # print_circular_log
