sub doprintf
{
   # $line = doprintf($code, $time, $data1, $data2, $data3, $data4);
   $code = shift;
   $code -= 300;
   $time = shift;
   $data1 = shift;
   $data2 = shift;
   $data3 = shift;
   $data4 = shift;
   @formats = (
   "rail_change_handler (arc: %d) (event: %d) (needs: %d) (seq_lvl: %d)",
   "arc_continue_processing (arc: %d) (seq_lvl: %d)",
   "arc_unblock_powerstate_prework (arc: %d)",
   "arc_unblock_rm_prework (arc: %d) (seq_lvl: %d)",
   "voltage sequence done (arc: %d) (cur_lvl: %d) (seq_lvl: %d)",
   "running voltage seq (idx: %d) (mask: 0x%x)",
   "not running voltage seq (mx_pend: %d) (cx_pend: %d) (vdda_pend: %d)",
   "freq_fsm process done (next_mc: %d) (next_shub: %d) (next_state: %d)",
   "starting frequency seq (next_mc: %d) (next_shub: %d) (start_addr: 0x%x)",
   "frequency seq complete (curr_mc: %d) (curr_shub: %d)",
   "llc flush readdata complete (mask: 0x%x)",
   "event during llc flush  (event: %d) (next: %d) (status: %d) (mask: 0x%x)",
   "main loop processing (signals: 0x%x)",
   "ddr_mgr_isr_func isr (irq: %d) (signals 0x%x)",
   "get_aop_needs           (arc: %d), (result: %d)",
   "arc_send_vote           (arc: %d), (client: %d), (level: %d)",
   "invalid cx voltage seq  (desired seq lvl: %d), (cx_lvl: %d)",
   "voltage_dependency_ready (voltage_arc_status[0]: %d), (voltage_arc_status[1]: %d), (voltage_arc_status[2]: %d), (ret_val %d)",
   "unblock_bcm_mc          (current_mc_cp: %d), (ddr_bcm_mc_level: %d), (ddr_reg_mc_level: %d)",
   "unblock_bcm_shub        (current_shub_cp: %d), (ddr_bcm_mc_level: %d), (ddr_reg_mc_level: %d)",
   "unblock_powerstate_arc  (current_mc_cp: %d), (ddr_rm_ack_lvl: %d), (ddr_arc_level: %d)",
   "cmd_db_memory_test      (current_mc_cp: %d), (current_shub_cp: %d)",
   "cmd_db_memory_test_fail      (current_mc_cp: %d), (current_shub_cp: %d)",
   "cmd_db_memory_test_pass      (current_mc_cp: %d), (current_shub_cp: %d)",
   "service_fal: tcs2 used  %d",
   "readdata: tcs 0 in use  %d",
   "flush_entry: tcs0 used  %d",
   "flush_exit: tcs1 used   %d",
   "bad retval: msg_ram_get_smem_addr %d",
   "smem_table returns (version: major: %d, minor: %d)",
   "local_switch_log_index  (local switch log index: %d), (index: %d), (freq_switch: %d)",
   "frequency resource state (pmic pending: %d), (mx: 0x%x), (cx: 0x%x) (vdda: 0x%x)",
   "below mol agg cleared by solving (resource : %d), (status : 0x%x)",
   "below mol agg not cleared by solving (resource : %d), (status : 0x%x)",
   "DDR_AUX Debug (GCC_CLKPLAN: %X) (SEQ_BRANCH_EVENT: %X)",
   "DDR_AUX Debug (SEQn_STAT: %X) (SEQn_CFG: %X) (Instruction: %X) (Elapsed: %d)",
   "TCS %d BUSY (RSI_TCSm_STAT: %X) (RSI_TCSm_CMD0: %X) (RSI_TCSm_CMD1: %X)",
   "SHRM did not respond to Frequency Cmd (TCS: %d) (RSI_TCSm_STAT: %X) (elapsed: %d)",
   "SHRM did not respond to Voltage Cmd (TCS: %d) (RSI_TCSm_STAT: %X) (elapsed: %d)",
   "SHRM did not respond to Flush Related cmd (RSI_TCSm_STAT: %X) (elapsed: %d)",
   "SHRM status (SPROC_STATUS: %X) (APB_DEBUG_STATUS: %X)",
   "apply_wlan_wa (current cx: %d) (target cx: %d), (wcssaon_config: 0x%x)",
  );
  if (defined ($formats[$code]))
  {
     return $time . ": " . sprintf($formats[$code], $data1, $data2, $data3, $data4);
  }
  else
  {
     return $time . sprintf(": Unknown Code %X(%d) %08.8X %08.8X %08.8X %08.8X", $code, $code, $data1, $data2, $data3, $data4);
  }
}
  

$file = shift;
open FILE, "<$file";
binmode FILE;
read (FILE, $data, 96*1024);
$log_data = $data;
@results = split (/AOP DDR Log...../, $log_data);
printf ("Data was %d and skip_bytes is %d Results is %d\n", length($data), $skip_bytes, $#results);
if ($#results != 2)
{
   print "No AOP DDR Log found\n";
   exit;
}

$skip_bytes = hex(44);
$log_data = $results[$#results];
for $i (1..32)
{
  $format = "x" . $skip_bytes . " I I I I I I I";
  ($time_lo, $time_hi, $code, $data1, $data2, $data3, $data4) = 
     unpack($format, $log_data);
  $time = $time_hi << 32 | $time_lo;
  if ($time > 0) {
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
	 $rel = int(($1-$last_ts)/19.2);
      }
      else
      {
	 $rel = "N/A";
      }
      $last_ts=$1;
   }
   printf "%X (%s): %s\n", $last_ts, $rel, $2;
}

