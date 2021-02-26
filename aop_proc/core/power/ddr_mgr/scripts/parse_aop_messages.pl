use File::Spec;

my %formats = ();
my $dir = undef;
my $file = undef;

sub build_formats
{
  my $dir = shift;

  my %format_125 = (
      1=>"ID 1(TRACER_EVENT_RESERVE_0):",
      79=>"ID 79(SPMI_EVENT_FIRST):placeholder first spmi swevent",
      80=>"ID 80(SPMI_EVENT_FATAL_ERROR):spmi_fatal_error: (result %d) (file %x) (line %d)",
      81=>"ID 81(SPMI_EVENT_ERROR):spmi_error: (result %x) (file %x) (line %d)",
      82=>"ID 82(SPMI_EVENT_WARNING):spmi_warning: (file %x) (line %d)",
      100=>"ID 100(SPMI_EVENT_LAST):placeholder last spmi swevent",
      101=>"ID 101(rpmh_log_create_handle):rpmh_create_handle (client: %s) (drv_id: 0x%08x)",
      102=>"ID 102(rpmh_log_delete_handle):rpmh_delete_handle: (client: %s) (drv_id: 0x%08x)",
      103=>"ID 103(rpmh_log_churn_all):rpmh_churn_all: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      104=>"ID 104(rpmh_log_churn_all_complete):rpmh_churn_all_complete",
      105=>"ID 105(rpmh_log_churn_single):rpmh_churn_single: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      106=>"ID 106(rpmh_log_churn_single_complete):rpmh_churn_single_complete",
      107=>"ID 107(rpmh_log_barrier_all):rpmh_barrier_all: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      108=>"ID 108(rpmh_log_barrier_all_complete):rpmh_barrier_all_complete: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      109=>"ID 109(rpmh_log_barrier_single):rpmh_barrier_single: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      110=>"ID 110(rpmh_log_barrier_single_complete):rpmh_barrier_single_complete: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      111=>"ID 111(rpmh_log_notify_completion_all):rpmh_notify_completion_all: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      112=>"ID 112(rpmh_log_notify_completion_all_complete):rpmh_notify_completion_all_complete: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      113=>"ID 113(rpmh_log_notify_completion_single):rpmh_notify_completion_single: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      114=>"ID 114(rpmh_log_notify_completion_single_complete):rpmh_notify_completion_single_complete: (handle: %s) (drv_id: 0x%08x) (req_id: %d)",
      115=>"ID 115(rpmh_log_finish_amc):rpmh_finish_amc (hw_drv: 0x%08x) (sw_drv: 0x%08x)",
      116=>"ID 116(rpmh_log_finished_req):	finished req: (client: %s)(req_id: %d)",
      117=>"ID 117(rpmh_log_no_amc_completed):	no_amc_completed",
      118=>"ID 118(rpmh_log_finish_amc_complete):rpmh_finish_amc_complete",
      119=>"ID 119(rpmh_log_issue_command_set):rpmh_issue_command_set (client: %s)(drv_id: 0x%08x)(set: %d)(num_cmds: %d)",
      120=>"ID 120(rpmh_log_resource_command):	resource_command (address: 0x%08x)(data: 0x%08x)(completion: %d",
      121=>"ID 121(rpmh_log_redundant_commands):	redundant commands, short circuiting",
      122=>"ID 122(rpmh_log_standalone_mode):	standalone mode, short circuiting",
      123=>"ID 123(rpmh_log_issue_command_set_complete):issue_command_set_complete: (req_id %x)",
      124=>"ID 124(rpmh_log_invalidate_command):rpmh_invlidate_command (client: %s)(address: 0x%08x)(set: %d)",
      125=>"ID 125(rpmh_log_issue_read_command):rpmh_issue_read_command (client: %s)(address: 0x%08x)(polling?: %d)",
      126=>"ID 126(rpmh_log_issue_read_command_complete):rpmh_issue_read_command_complete (client: %s)(address: 0x%08x)(data: 0x%08x)",
      127=>"ID 127(rpmh_log_adding_to_sleep_wake_tcs):	adding command set to sleep/wake tcs (set: %d)",
      128=>"ID 128(rpmh_log_adding_to_sleep_wake_amc):	adding command set to sleep/wake amc (set: %d)",
      129=>"ID 129(rpmh_log_enter_sleep):rpmh_enter_sleep (drv_id: 0x%08x)",
      130=>"ID 130(rpmh_log_enter_sleep_short_circuit):rpmh_enter_sleep short circuit (standalone mode)",
      131=>"ID 131(rpmh_log_flushing_amcs):	flushing sleep amcs",
      132=>"ID 132(rpmh_log_enter_sleep_complete):rpmh_enter_sleep_complete",
      133=>"ID 133(rpmh_log_exit_sleep):rpmh_exit_sleep, flushing wake amcs (drv_id: 0x%08x)",
      134=>"ID 134(rpmh_log_exit_sleep_short_circuit):rpmh_exit_sleep short circuit (standalone mode)",
      135=>"ID 135(rpmh_log_exit_sleep_complete) => rpmh_exit_sleep_complete",
      136=>"ID 136(rpmh_log_rsc_clocks_init):rpmh_rsc_clocks_init",
      137=>"ID 137(rpmh_log_rsc_clocks_init_complete):rpmh_rsc_clocks_init_complete",
      138=>"ID 138(rpmh_log_client_init):rpmh_client_init",
      139=>"ID 139(rpmh_log_client_init_complete):rpmh_client_init_complete",
      140=>"ID 140(rpmh_log_holding_request):	rpmh_holding_request (rcs_idle?: %d)",
      141=>"ID 141(rpmh_log_holding_read_request):	rpmh_holding_read_request (no free amcs?: %d)",
      142=>"ID 142(rpmh_log_tcs_idle):	tcs idle (tcs_id: %d)",
      143=>"ID 143(rpmh_log_configuring_tcs):	configuring tcs (drv_id: %d)(tcs_id: %d)(amc?: %d)",
      144=>"ID 144(rpmh_log_configuring_tcs2):		(client: %s)(req_id: %d)",
      145=>"ID 145(rpmh_log_tcs_sent):	tcs sent (tcs index: %d) (tcs_id: %d)",
      146=>"ID 146(rpmh_log_epcb_timeout):epcb timeout",
      147=>"ID 147(rpmh_log_tcs_stuck):!!!!tcs stuck!!!!",
      148=>"ID 148(rpmh_direct_send_log):rpmh_direct_send",
      228=>"ID 228(rpmh_log_event_last):placeholder last rpmh driver swevent",
      300=>"ID 300(ddr_rail_change_handler):rail_change_handler (arc: %d) (event: %d) (needs: %d) (seq_lvl: %d)",
      301=>"ID 301(ddr_arc_continue_processing):arc_continue_processing (arc: %d) (seq_lvl: %d)",
      302=>"ID 302(ddr_arc_unblock_powerstate_arc):arc_unblock_powerstate_prework (arc: %d)",
      303=>"ID 303(ddr_arc_unblock_rm_prework):arc_unblock_rm_prework (arc: %d) (seq_lvl: %d)",
      304=>"ID 304(ddr_voltage_sequence_done):voltage sequence done (arc: %d) (cur_lvl: %d) (seq_lvl: %d)",
      305=>"ID 305(ddr_Running_Voltage_Seq):running voltage seq (idx: %d) (mask: 0x%x)",
      306=>"ID 306(ddr_NOT_Running_Voltage_Seq):not running voltage seq (mx_pend: %d) (cx_pend: %d) (vdda_pend: %d)",
      307=>"ID 307(ddr_FREQ_FSM_PROCESS_DONE):freq_fsm process done (next_mc: %d) (next_shub: %d) (next_state: %d)",
      308=>"ID 308(ddr_Starting_Frequency_Seq):starting fequency seq (next_mc: %d) (next_shub: %d) (start_addr: 0x%x)",
      309=>"ID 309(ddr_Frequency_Seq_complete):frequency seq complete (curr_mc: %d) (curr_shub: %d)",
      310=>"ID 310(ddr_Flush_readdata_complete):llc flush readdata complete (mask: 0x%x)",
      311=>"ID 311(ddr_Event_during_LLC_Flush):event during llc flush  (event: %d) (next: %d) (status: %d) (mask: 0x%x)",
      312=>"ID 312(ddr_MAIN_LOOP_PROCESSING):main loop processing (signals: 0x%x)",
      313=>"ID 313(ddr_DDR_MGR_ISR_FUNC):ddr_mgr_isr_func isr (irq: %d) (signals 0x%x)",
      314=>"ID 314(ddr_GET_AOP_NEEDS):get_aop_needs           (arc: %d), (result: %d)",
      315=>"ID 315(ddr_ARC_SEND_VOTE):arc_send_vote           (arc: %d), (client: %d), (level: %d)",
      316=>"ID 316(ddr_INVALID_CX_VOLTAGE_SEQ):invalid cx voltage seq  (desired seq lvl: %d), (cx_lvl: %d)",
      317=>"ID 317(ddr_VOLTAGE_DEPENDENCY_READY):voltage_dependency_ready (voltage_arc_status[0]: %d), (voltage_arc_status[1]: %d), (voltage_arc_status[2]: %d), (ret_val %d)",
      318=>"ID 318(ddr_UNBLOCK_BCM_MC):unblock_bcm_mc          (current_mc_cp: %d), (ddr_bcm_mc_level: %d), (ddr_reg_mc_level: %d)",
      319=>"ID 319(ddr_UNBLOCK_BCM_SHUB):unblock_bcm_shub        (current_shub_cp: %d), (ddr_bcm_mc_level: %d), (ddr_reg_mc_level: %d)",
      320=>"ID 320(ddr_UNBLOCK_POWERSTATE_ARC):unblock_powerstate_arc  (current_mc_cp: %d), (ddr_rm_ack_lvl: %d), (ddr_arc_level: %d)",
      321=>"ID 321(ddr_CMD_DB_MEM_TEST):cmd_db_memory_test      (current_mc_cp: %d), (current_shub_cp: %d)",
      322=>"ID 322(ddr_CMD_DB_MEM_TEST_FAIL):cmd_db_memory_test_fail      (current_mc_cp: %d), (current_shub_cp: %d)",
      323=>"ID 323(ddr_CMD_DB_MEM_TEST_PASS):cmd_db_memory_test_pass      (current_mc_cp: %d), (current_shub_cp: %d)",
      324=>"ID 324(ddr_SERVICE_FAL_TCS2):service_fal: tcs2 used  %d",
      325=>"ID 325(ddr_READDATA_TCS0_INUSE):readdata: tcs 0 in use  %d",
      326=>"ID 326(ddr_FLUSHENTRY_TCS0_INUSE):flush_entry: tcs0 used  %d",
      327=>"ID 327(ddr_FLUSHEXIT_TCS1_INUSE):flush_exit: tcs1 used   %d",
      328=>"ID 328(ddr_BADRETVAL_MSG_RAM_ADDR):bad retval: msg_ram_get_smem_addr %d",
      329=>"ID 329(ddr_SMEM_TABLE_RETURNS):smem_table returns (version: major: %d, minor: %d)",
      330=>"ID 330(ddr_LOCAL_SWITCH_LOG_INDEX):local_switch_log_index  (local switch log index: %d), (index: %d), (freq_switch: %d)",
      331=>"ID 331(ddr_freq_needs):frequency resource state (pmic pending: %d), (mx: 0x%x), (cx: 0x%x) (vdda: 0x%x)",
      332=>"ID 332(ddr_below_mol_cleared):below mol agg cleared by solving (resource : %d), (status : 0x%x)",
      333=>"ID 333(ddr_below_mol_not_cleared):below mol agg not cleared by solving (resource : %d), (status : 0x%x)",
      334=>"ID 334(DDRAUX_CLKPLAN_BRANCH):DDR_AUX Debug (GCC_CLKPLAN: %X) (SEQ_BRANCH_EVENT: %X)",
      335=>"ID 335(DDRAUX_SEQUENCER_STATUS):DDR_AUX Debug (SEQn_STAT: %X) (SEQn_CFG: %X) (Instruction: %X) (Elapsed: %d)",
      336=>"ID 336(TCS_BUSY_AT_ERROR):TCS %d BUSY (RSI_TCSm_STAT: %X) (RSI_TCSm_CMD0: %X) (RSI_TCSm_CMD1: %X)",
      337=>"ID 337(INVESTIGATE_SHRM_FREQUENCY):SHRM did not respond to Frequency Cmd (TCS: %d) (RSI_TCSm_STAT: %X) (elapsed: %d)",
      338=>"ID 338(INVESTIGATE_SHRM_VOLTAGE):SHRM did not respond to Voltage Cmd (TCS: %d) (RSI_TCSm_STAT: %X) (elapsed: %d)",
      339=>"ID 339(INVESTIGATE_SHRM_FLUSH):SHRM did not respond to Flush Related cmd (RSI_TCSm_STAT: %X) (elapsed: %d)",
      340=>"ID 340(SHRM_STATUS_AT_TIMEOUT):SHRM status (SPROC_STATUS: %X) (APB_DEBUG_STATUS: %X)",
      341=>"ID 341(ddr_WLAN):apply_wlan_wa (current cx: %d) (target cx: %d), (wcssaon_config: 0x%x)",
      360=>"ID 360(ddr_log_event_last):placeholder for last ddr_mgr_event",
      383=>"ID 383(RAMDUMP_FATAL_ERROR):fatal_error... DDR_On:%d ...init RAM DUMP",
      384=>"ID 384(CLOCK_EVENT_FIRST):Clock first event placeholder",
      385=>"ID 385(CLOCK_EVENT_INIT):Clock Initialize",
      386=>"ID 386(CLOCK_EVENT_POWER_COLLAPSE):Clock Power Collapse: %plugin[3]<clock>.",
      387=>"ID 387(CLOCK_EVENT_POWER_RESTORE):Clock Power Restore:  %plugin[3]<clock>.",
      388=>"ID 388(CLOCK_EVENT_PSCBC_CHECK):Clock PSCBC ON at XO shutdown (CBCR_ADDR) : 0x%x = 0x%x",
      511=>"ID 511(CLOCK_EVENT_LAST):Clock last event placeholder",
      600=>"ID 600(AOP_SLEEP_FIRST):AOP_SLEEP_FIRST",
      601=>"ID 601(CX_COLLAPSE_ENTER):Entering CX collapse start",
      602=>"ID 602(CX_COLLAPSE_EXIT):Exiting CX collapse start",
      603=>"ID 603(CX_COLLAPSE_ENTER_COMPLETE):Entering CX collapse complete",
      604=>"ID 604(CX_COLLAPSE_EXIT_COMPLETE):Exiting CX collapse complete",
      605=>"ID 605(AOSS_SLEEP_ENTER):Entering AOSS Sleep: (wakeup hi: 0x%x), (wakeup_lo: 0x%x)",
      606=>"ID 606(AOSS_SLEEP_EXIT):Exiting AOSS Sleep",
      607=>"ID 607(HALT_ENTER):Entering halt: (wakeup hi: 0x%x), (wakeup_lo: 0x%x)",
      608=>"ID 608(HALT_EXIT):Exiting halt",
      609=>"ID 609(IMAGE_NAME_INVALID):PIL ERROR:Invalid image name recieved over QMP!!!  (val: %s)",
      610=>"ID 610(PIL_MALFORMED_JSON):PIL ERROR:Invalid JSON format, could not parse",
      680=>"ID 680(AOP_SLEEP_LAST):AOP_SLEEP_LAST",
      681=>"ID 681(DDR_ATTEMPT_READ):Reading from DDR",
      682=>"ID 682(DDR_READ_DONE):Read from DDR done",
      690=>"ID 690(QDSS_TRACE_INITIALIZED):QDSS Trace initialized",
      691=>"ID 691(QDSS_TEST_EVENT_0):QDSS Test event 0 no args",
      692=>"ID 692(QDSS_TEST_EVENT_1):QDSS Test event 1 arg1=%d",
      693=>"ID 693(QDSS_TEST_EVENT_2):QDSS Test event 2 arg1=%d arg2=%d",
      694=>"ID 694(QDSS_TEST_EVENT_3):QDSS Test event 3 arg1=%d arg2=%d arg3=%d",
      695=>"ID 695(QDSS_TEST_EVENT_4):QDSS Test event 4 arg1=%d arg2=%d arg3=%d arg4=%d",
      696=>"ID 696(AOP_NON_SECURE_WD_BITE_INT_RECEIVED):aop_abort_interrupt_received (APPS NON SECURE WD BITE) ... aborting",
      697=>"ID 697(AOP_APPS_HANG_STATUS):aop_apps_hang_status (Core: 0x%x) (Gladiator: 0x%x) (GIC: 0x%x) (WDOG: 0x%x)",
      698=>"ID 698(AOP_LOG_REGISTER):aop_log_register (PhysAddress: 0x%0.8x) (Value: 0x%0.8x)",
      699=>"ID 699(AOP_GIC_ERR):aop_gic_mem_corr (ERR_INT: %d), (FAULT_INT: %d)",
      700=>"ID 700(QMP_MSG_DRVR_MAP_EXPAND):MSG DRV: Map is expanding.Current Size: %d",
      701=>"ID 701(CLASS_STRING_NOT_FOUND):MSG DRV: Class string recieved was not found in map",
      702=>"ID 702(MSG_Q_OVERFLOW):MSG DRV: Queue overflowed",
      703=>"ID 703(ERROR_MSG_Q_LEN):QMP MSG: error (size: %d) > 0x64",
  );
  %dirs = (
    114=>"//snowcone/builds675/PROD/AOP.HO.1.0-00114-SDM845AAAAANAZO-1",
    115=>"//snowcone/builds710/TEST/AOP.HO.1.0-00115-SDM845AAAAANAAO-1",
    116=>"//snowcone/builds709/TEST/AOP.HO.1.0-00116-SDM845AAAAANAAO-1",
    117=>"//snowcone/builds674/TEST/AOP.HO.1.0-00117-SDM845AAAAANAAO-1",
    118=>"//snowcone/builds786/TEST/AOP.HO.1.0-00118-SDM845AAAAANAAO-1",
    119=>"//snowcone/builds672/TEST/AOP.HO.1.0-00119-SDM845AAAAANAAO-1",
    120=>"//snowcone/builds671/TEST/AOP.HO.1.0-00120-SDM845AAAAANAAO-1",
    121=>"//snowcone/builds797/TEST/AOP.HO.1.0-00121-SDM845AAAAANAAO-1",
    122=>"//snowcone/builds671/TEST/AOP.HO.1.0-00122-SDM845AAAAANAAO-1",
    123=>"//snowcone/builds674/TEST/AOP.HO.1.0-00123-SDM845AAAAANAAO-1",
    124=>"//snowcone/builds674/TEST/AOP.HO.1.0-00124-SDM845AAAAANAAO-1",
    124=>"//snowcone/builds674/TEST/AOP.HO.1.0-00124-SDM845AAAAANAAO-1",
    125=>"//snowcone/builds672/TEST/AOP.HO.1.0-00125-SDM845AAAAANAAO-1"
  );
  $suffix = "\\aop_proc\\core\\api\\debugtrace\\tracer_event_tbl.h";
  my $aop_version = 0;
  my $aop_path = undef;
  foreach $buildid_fname ("Crashscope_RDET/txtReports/Summary.txt",
			  "Crashscope/txtReports/Summary.txt",
                          "buildid.txt", "AOP_AAAAANAZO.elf"
			  )
  {
    $buildid = File::Spec->catfile($dir, $buildid_fname);
    if (-e $buildid)
    {
      open ID, "<$buildid";
      while (<ID>) {
	print $_ if ($buildid =~ m/.txt$/);
	if (/AOP.HO.1.*\s\s*-\s*(\S*)/i)
	{
	  $aop_path = $1;
	  print "Setting AOP Path to $aop_path\n";
	}
	if (/AOP.HO.1.0-(\d+)-SDM845/i)
	{
	  if ($aop_version != 0)
	  {
	    $new_version = $1+0;
	    if ($aop_version != $new_version) {
	      print "\nWARNING: AOP Version previously received ($aop_version) does not match version in $buildid ($new_version)\n\n";
	    }
	  }
	  else
	  {
	    $aop_version = $1+0;
	    print "Setting AOP Version to $aop_version\n";
	  }
	}
	if (/\/(AOP.HO.1.0-[^\/]*)\//i)
	{
	  $aop_build = $1;
	  print "AOP ELF indicates build $aop_build\n";
	}

      }
      close ID;
    }
    else {
      #print "No $buildid\n";
    }
  }
  print "Checking path $aop_path and version $aop_version\n";
  foreach $main_dir ($aop_path, $dirs{$aop_version})
  {
    next if (length($main_dir) == 0);
    print "See if we can rebuild formats for version $main_dir\n";

    $in_enum = 0;
    $val = 0;
    %strings=();
    $tracer_file = File::Spec->catfile($main_dir, $suffix);
    print "Checking $tracer_file\n";
    open TBL, "< $tracer_file";
    while (<TBL>)
    {
      if (!$in_enum)
      {
	$in_enum=1 if (/tracer_event_tbl/);
	next;
      }
      if (/\};/)
      {
	$in_enum=0;
	last;
      }

      if (/{([^\,]*),"([^\"]*)".*},*/)
      {
	$key = $1;
	$str = $2;
	$str =~ s/^\s+|\s+$//g;
	if ($key =~ /(.*)=(\d+)/)
	{
	  $key=$1;
	  $val=$2;
	}
	elsif (/(.*),/)
	{
	  $val = $val+1;
	}
	#$strings{$val}="MSGID $val($key) => $str";
	$strings{$val}="$str";
      }
      #print "Line ($val): $_";
    }
    close TBL;
    #foreach $key (sort {$a<=>$b} keys %strings)
    #{
      #print "Key $key: Message $strings{$key}\n";
    #}
    %formats = %strings;
    if (%formats) {
      print "Using $tracer_file \n";
      last;
    }
  }
  if (!%formats)
  {
    print "Using default\n";
    %formats = %format_125;
  }
}


sub doprintf
{
   # $line = doprintf($code, $time, $data1, $data2, $data3, $data4);
   $code = shift;
   $time = shift;
   $data1 = shift;
   $data2 = shift;
   $data3 = shift;
   $data4 = shift;

  if (defined ($formats{$code}))
  {
     return $time . ": " . sprintf($formats{$code}, $data1, $data2, $data3, $data4);
  }
  else
  {
     return $time . sprintf(": AOP Log Code %X(%d) %08.8X %08.8X %08.8X %08.8X", $code, $code, $data1, $data2, $data3, $data4);
  }
}
  
$param = shift;
$nofile = 0;
$nofile = 1 if (-d $param);
($volume, $directories, $fname) = File::Spec->splitpath($param, $nofile);

$dir = File::Spec->catdir($volume, $directories);
foreach $fname ($fname, "CODERAM.BIN", "aop_code_0xb000000--0xb017fff.lst")
{
  $file = File::Spec->catfile($dir, $fname);
  print "Checking $dir $file\n";
  last if (-e $file);
}

if (!-e $file) {
  print "Cannot find file $file in $dir\n";
  exit;
}
print "Using $file\n";

sub relative
{
   my $last = shift;
   my $first = shift;
   $rel = int(($last - $first) / 19.2);
   $units = "us";
   if ($rel >= 1000)
   {
      $rel = $rel / 1000;
      if ($rel >= 1000) {
         $rel = int($rel+0.5)/1000;
         $units = "sec";
      } else {
         $rel = int($rel*1000+0.5)/1000;
         $units = "ms";
      }
   }
   return "$rel $units";
}

build_formats($dir);
open FILE, "<$file";
binmode FILE;
read (FILE, $data, 192*1024);
@results = split(/AOP External Log/, $data);
if ($#results > 0) {
   print "Results returned $#results\n";
   $log_data = $results[$#results];
} else {
   print "No Log data found in $file\n";
}
printf ("Data was %d and is now %d\n", length($data), length($log_data));
$skip_bytes = hex("64");
while ($skip_bytes < length($log_data))
{
  $format = "x" . $skip_bytes . " I I I I I I I";
  ($time_lo, $time_hi, $code, $data1, $data2, $data3, $data4) = 
     unpack($format, $log_data);
  $time = $time_hi << 32 | $time_lo;
  if ($time > 0)
  {
    $line = doprintf($code, $time, $data1, $data2, $data3, $data4);
    push @events, "$line";
  }
  $skip_bytes += 32;
}

sub by_timestamp
{
  $a_time = $1 if ($a =~ m/(.*):/);
  $b_time = $1 if ($b =~ m/(.*):/);

  return hex($a_time) <=> hex($b_time);
}

$last_ts = 0;
foreach $event (sort by_timestamp @events)
{
  if ($event =~ /([0-9]+): (.*)/)
  {
    if ($last_ts > 0)
    {
      $rel = relative($1, $last_ts);
      int(($1-$last_ts)/19.2);
    }
    else
    {
      $rel = "N/A";
    }
    $last_ts=$1;
    printf "%12X (%10.10s): %s\n", $last_ts, $rel, $2;
  }
}

