$warnings = 0;
$time_period = 1/400;
$current_freq = 2;
@time_periods = (1/75, 1/150, 1/400);

$wall_clock_ts           =hex('0');    # This offset in wall clock time
$last_freq_sequence      =hex('0');   # will display this wall_clock time.

$last_change_complete = 0;
$max_ts_low = 0;
$hist_highest_index = -1;
sub get_last_offset
{
  # If the parameter was a directory, scan the directory for other AOP files and
  # see if we can correlate the last sequence of frequency change events with a
  # change in either MSGRAM11 or in MSGRAM6.  MSGRAM6 has the low bits more accurate, 
  # MSGRAM11 has the high octets.   MSGRAM6 is less reliable as to whether it has 
  # any data that would be helpful however.
  $msgram_dir = shift;
  $try = undef;
  @trials = qw(MSGRAM6.BIN aop_msg6_0xc360000--0xc3603ff.lst);
  foreach $trial (@trials) {
    $try = $msgram_dir . "/" . $trial;
    if (-f $try)
    {
      open LOGFILE, "< $try" || return;
      binmode LOGFILE;
      $bin_file = 1;
      while (get_bytes(LOGFILE, $bindata, 16) == 16)
      {
	($ts_low, $str, $byte1, $byte2, $byte3, $byte4) =
	   unpack("I A8 C C C C", $bindata);
	#printf("%X %s %X %X %X %X\n", $ts_low, $str, $byte1, $byte2, $byte3, $byte4);
	if (($str =~ m/DDR ISR/) && ($byte4 == 0x60)) # Seq 0 Done (ISR)
	{
	  if (($max_ts_low != 0) && (0 == ($max_ts_low & hex('F0000000'))) && 
	                            (hex('F0000000') == ($ts_low & hex('F0000000'))))
	  {
	    $max_ts_low |= hex('100000000');
	  }
	  $max_ts_low = $ts_low if ($ts_low > $max_ts_low);
	}
      }
    }
  }
  @trials = qw(MSGRAM11.BIN aop_msg11_0xc3b0000--0xc3b03ff.lst);
  $hist_index = -1;
  $change_in_progress=0;
  foreach $trial (@trials) {
    $try = $msgram_dir . "/" . $trial;
    if (-f $try)
    {
      open LOGFILE, "< $try" || return;
      binmode LOGFILE;
      $bin_file = 1;
      $cur_offset = get_bytes(LOGFILE, $bindata, 16);
      ($event_offset, $num_event, $stats_offset, $config_offset) =
	 unpack("I I I I", $bindata);
      if ($event_offset != $cur_offset)
      {
	 close LOGFILE;
	 next;
      }
      $i = 0;
      $last_change_started = 0;
      $first_ts = undef;
      while ($i < $num_event)
      {
	$i=$i+1;
	if (get_bytes(LOGFILE, $eventdata, 16) != 16)
	{
	  last;
	}
	($word1, $word2, $word3, $word4) = unpack("I I I I", $eventdata);
	$timestamp = ($word2 << 32) + $word1;
	$event      = $word3 & 0xF;

	$first_timestamp = $timestamp if (!defined($first_timestamp));

        $word4 = $word4 >> 16;  # Don't care about what BCM wants
	$act_mc_curr= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Current clock plan
	$act_mc_next= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Next clock plan
	$act_sh_curr= $word4 & 0xF; $word4 = $word4 >> 4; # SHUB's Current clock plan
	$act_sh_next= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Next clock plan

	if ($change_in_progress > 0)
	{
	  $change_in_progress = 0;
	  $history[$hist_index]{END_TIME} = $timestamp;
	  #printf("Index %d: Start %X", $hist_index, $history[$hist_index]{START_TIME});
	  if (defined($history[$hist_index]{MC_CHANGE})) {
	    #printf(" MC -> %d", $history[$hist_index]{MC_CHANGE});
	  }
	  if (defined($history[$hist_index]{SHUB_CHANGE})) {
	    #printf(" SHUB -> %d", $history[$hist_index]{SHUB_CHANGE});
	  }
	  #printf(" End %X\n", $history[$hist_index]{END_TIME});
	}
	if (($act_mc_curr != $act_mc_next) || ($act_sh_curr != $act_sh_next))
	{
	  if ($event == 0)
	  {
	    $change_in_progress = 1;
	    $hist_index = $hist_index+1;
	    $history[$hist_index]{START_TIME} = $timestamp;
	    $history[$hist_index]{MC_CHANGE} = $act_mc_next if ($act_mc_curr != $act_mc_next);
	    $history[$hist_index]{SHUB_CHANGE} = $act_sh_next if ($act_sh_curr != $act_sh_next);

	    if (($hist_highest_index < 0) || ($timestamp > $history[$hist_highest_index]{START_TIME}))
	    {
	      $hist_highest_index = $hist_index;
	    }
	  }
	}

	$last_change_complete = $timestamp if (($last_change_started > 0) && ($timestamp > $last_change_complete));
	$last_change_started = 0;
	$last_change_started = 1 if ($event == 0);
      }
      if ($change_in_progress > 0)
      {
	$change_in_progress = 0;
	$history[$hist_index]{END_TIME} = $first_timestamp;
	#printf("Index %d: Start %X", $hist_index, $history[$hist_index]{START_TIME});
	if (defined($history[$hist_index]{MC_CHANGE})) {
	  #printf(" MC -> %d", $history[$hist_index]{MC_CHANGE});
	}
	if (defined($history[$hist_index]{SHUB_CHANGE})) {
	  #printf(" SHUB -> %d", $history[$hist_index]{SHUB_CHANGE});
	}
	#printf(" End %X\n", $history[$hist_index]{END_TIME});
      }
      close LOGFILE;
      last if ($last_change_complete > 0);
    }
  }
  #printf("MSGRAM11 says Last Change Completed at %X Hist Index %d\n", $last_change_complete, $hist_highest_index);
}

$file = $ARGV[0];
if ($#ARGV > 1) {
  $wall_clock_ts           =hex($ARGV[1]);       # This offset in wall clock time
  $last_freq_sequence      =hex($ARGV[2]);       # will display this wall_clock time.
}
if (-d $file) {
  get_last_offset($file);
  @trials = qw(SHRM_MEM.BIN SHRM_MEM.BIN.ls_0x13e0000--0x13effff.lst);
  foreach $trial (@trials) {
    print "Trying $trial\n";
    $try = $file . "/" . $trial;
    if (-f $try)
    {
      $file = $try;
      last;
    }
  }
}

$bin_file = 0;

$cur_offset = 0;
print "Process $file\n";
open LOGFILE, "< $file" || die "Cannot open $file: $!";
if (!defined($file)) {
  print "Usage memdump_shrm.pl <filename> [Displayed_Timestamp Desired_Timestamp]\n";
  print "The optional parameters at the end adjust the timestamp to better match other AOP logs\n\n";
  print "If memdump_shrm.pl <filename>\n";
  print "Displayed |  371489B (    580 ns): Response: CLOCK_POST, SHUB, 0 (offset:6680) (SHRM_TIME:495370CF)\n";
  print "          |  ======= \n";
  print "then memdump_shrm.pl <filename> 371489b 1bd44b1b\n";
  print "Would Show| 1BD44B1B (    580 ns): Response: CLOCK_POST, SHUB, 0 (offset:6680) (SHRM_TIME:495370CF)\n";
  print "          | ======== \n";
  exit;
}
if (! -e $file) 
{
  print "Error: $file does not exists\n";
  print "Usage memdump_shrm.pl <filename>\n";
  exit;
}
if (($file =~ m/.bin$/i) || ($file =~ m/.lst$/i))
{
  binmode LOGFILE;
  $bin_file = 1;
}

sub relative
{
   $last = shift;
   $first = shift;
   $rel = int(1000*($last - $first) * $time_period);
   $units = "ns";
   if ($rel >= 1000)
   {
      if ($rel >= 1000000000) {
         $rel = $rel / 1000000;
         $rel = int($rel+0.5)/1000;
         $units = "sec";
      } elsif ($rel >= 1000000) {
         $rel = $rel / 1000;
         $rel = int($rel+0.5)/1000;
         $units = "ms";
      } else {
         $rel = int($rel+0.5)/1000;
         $units = "us";
      }
   }
   return "$rel $units";
}

# When we read a 4 byte hex string, the endian-ness of the data is often reversed from
# what we need.  In this case, return the second octet first, and the first octet second.
# e.g. "1234" should be interpreted as hex(3412);
sub swap
{
  if ($_[0] =~ m/(..)(..)/)
  {
    return $2.$1;
  }
  else
  {
    return $_[0];
  }
}

# Generic read which will return binary data.   If we have a binary file, return the number
# of bytes requested.  Otherwise, read the lines and take 16 bytes of data at a time and
# return it.  For now, data is always a multiple of 16 bytes, so this routine requires $len
# to be a multiple of 16 for text based files.
sub get_bytes
{
  if ($bin_file)
  {
    return read($_[0], $_[1], $_[2]);
  }
  else
  {
    $len = $_[2];   # Length of bytes to read.
    $total = 0;     # Number of bytes read so far
    $_[1] = undef;  # What we return.
    while (<LOGFILE>)
    {
      # Check different line formats.
      if (/([0-9A-F]{8}) +([0-9A-F]{8} +([0-9A-F]{8})) +([0-9A-F]{8})/)
      {
        # If the line containts 4 groups of 8 hex digits, pack that into the line
        $_[1] = $_[1] . pack("I I I I", hex($1), hex($2), hex($3), hex($4));
        $total = $total + 16;
        last if ($total >= $len);
      }
      elsif (/([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4}) +([0-9A-F]{4})/)
      {
        # If the line containts 8 groups of 4 hex digits, pack that into the line
        $a = hex(swap($1)) << 16 | hex(swap($2));
        $b = hex(swap($3)) << 16 | hex(swap($4));
        $c = hex(swap($5)) << 16 | hex(swap($6));
        $d = hex(swap($7)) << 16 | hex(swap($8));
        $_[1] = $_[1] . pack("I I I I", hex($a), hex($b), hex($c), hex($d));
      }
    }

    # Return number of bytes read.
    return $total;
  }
  return 0;
}

@reqs = qw(GPR GPW REQ UNDEF);
@opcodes = qw(CLOCK_PREWORK CLOCK_WORK CLOCK_POST
              VOLTAGE_PRE VOLTAGE_WORK VOLTAGE_POST
	      POWERSTATE QUERY FLUSH ABORT
	      ACV ALC);
@rrs = qw(FF RR);
@lasts = qw(NOTLAST LAST);
@ids = ("SHUB", "MC", "0 2",  "0 3",
        "SHUB", "MC", "1 2",  "1 3",
        "SHUB", "MC", "2 2",  "2 3",
        "MX",   "CX", "VDDA", "3 3", 
        "MX",   "CX", "VDDA", "3 3", 
        "MX",   "CX", "VDDA", "3 3", 
        "DDRSS", "LLC", "6 2", "6 3", 
        "7 0", "7 1", "ACTIVE", "DIRTY", 
        "8 0", "8 1", "8 2", "8 3", 
        "9 0", "9 1", "9 2", "9 3", 
        "10 0", "10 1", "10 2", "10 3", 
        "", "11 1", "11 2", "11 3");

sub rsi_cmd_decode
{
  $cmd = shift;
  $rsi_req = ($cmd & 0xC000000) >> 26;
  $rsi_rr  = ($cmd & 0x2000000) >> 25;
  $rsi_lst = ($cmd & 0x1000000) >> 24;
  $rsi_opc = ($cmd & 0x0F80000) >> 19;
  $rsi_id  = ($cmd & 0x0070000) >> 16;
  $rsi_data= ($cmd & 0x000FFFF) >> 0;

  $res = "";
  $res .= sprintf ("%s ", $opcodes[$rsi_opc]);
  $res .= sprintf ("%s, ", $ids[$rsi_opc*4+$rsi_id]);
  $res .= sprintf ("DATA=%1X,", $rsi_data);
  $res .= sprintf ("RSI_RR=%s, ", $rrs[$rsi_rr]);
  $res .= sprintf ("RSI_LAST=%s ", $lasts[$rsi_lst]);

  return $res;
}

sub rsi_resp_decode
{
  $cmd = shift;
  $rsi_req = ($cmd & 0xC000000) >> 26;
  $rsi_rr  = ($cmd & 0x2000000) >> 25;
  $rsi_lst = ($cmd & 0x1000000) >> 24;
  $rsi_opc = ($cmd & 0x0F80000) >> 19;
  $rsi_id  = ($cmd & 0x0070000) >> 16;
  $rsi_data= ($cmd & 0x000FFFF) >> 0;

  $res = "";
  $res .= sprintf ("%s, ", $opcodes[$rsi_opc]);
  $res .= sprintf ("%s, ", $ids[$rsi_opc*4+$rsi_id]);
  $res .= sprintf ("%1X", $rsi_data);
  return $res;
}

$offset = hex('6500');
get_bytes(LOGFILE, $bindata, $offset);

sub printSeq
{
  $idx = shift;
  printf ("Sequence %d: ", $idx);
  #printf ("Start %X - End %X Controlled ", $clock_events[$idx]{START}, $clock_events[$idx]{END});
  if (defined($clock_events[$idx]{MC}))
  {
    printf ("MC to %d ", $clock_events[$idx]{MC});
  }
  if (defined($clock_events[$idx]{SHUB}))
  {
    printf ("SHUB to %d", $clock_events[$idx]{SHUB});
  }
  printf("\n");
}

sub by_timestamp
{ $a_time = $1 if ($a =~ m/(.*):/); $b_time = $1 if ($b =~ m/(.*):/);

  return hex($a_time) <=> hex($b_time);
}
# First read the dictionary and validate that a bit.
print "\n";
$address = $offset;
for $count (1..64)
{
  last if (get_bytes(LOGFILE, $bindata, 16) != 16);

  ($data1, $data2, $data3, $data4) = unpack("I I I I", $bindata);
  printf ("%08.8X: %08.8X %08.8X %08.8X %08.8X\n", $address, $data1, $data2, $data3, $data4);

  $cmd_text = rsi_cmd_decode($data1);
  $cmd_time = $data2;
  $response_text = rsi_resp_decode($data3);
  $response_time = $data4;

  $line = sprintf("%08.8X Cmd: %s (offset:%4x)", $cmd_time, $cmd_text, $address);
  push @events, $line;
  $line = sprintf("%08.8X Response: %s (offset:%4x)", $response_time, $response_text, $address);
  push @events, $line;
  $address = $address + 16;
}
print "\n";

$last_ts = 0;

$wall_clock_ns = undef;
$output_count = 0;
$max_sequence_end = 0;
$parsing_errors;
$clock_sequence_index = 0;
$clock_sequence_in_progress = 0;
$restore_sequence_in_progress = 0;
$collapse_sequence_in_progress = 0;
$clock_sequence_ending = 0;
foreach $event (sort by_timestamp @events)
{
  if ($event =~ /^([0-9A-F]+) (.*)/)
  {
    $time = hex($1);
    $text_line = $2;

    if (!defined($wall_clock_ns)) {
      $wall_clock_ns = 0;
    } else {
      $wall_clock_ns += int(1000*($time-$last_ts) * $time_period);
    }
    $last_ts=$time;
    $wall_clock_disp = int($wall_clock_ns*192/10000);
    $offset_str = "";
    if ($text_line =~ m/offset:([0-9a-f]+)/i) {
      $offset=hex($1);
      $last_offset=$offset if (!defined($last_offset)); 
      if (($offset == $last_offset) ||
	  ($offset == (16+$last_offset)) ||
	  (($offset == hex('6500')) && ($last_offset == hex('68F0'))))
      {
      }
      else
      {
	$offset_str=' ****** Logs missing data';
	$parsing_errors++;
      }
      $last_offset = $offset;
    }
   
    if ($text_line =~ m/Cmd: POWERSTATE DDRSS, DATA=0,RSI_RR=RR/)
    {
      if ($clock_sequence_ending > 0) {
	$clock_sequence_in_progress = 0; 
	$clock_events[$clock_sequence_index]{END} = $clock_sequence_ending;
	$clock_sequence_ending = 0; 
	#printSeq($clock_sequence_index);
	$clock_sequence_index++;
      }
      $restore_sequence_in_progress = 1; 
      $clock_events[$clock_sequence_index]{START} = $wall_clock_disp;
      $clock_events[$clock_sequence_index]{MC} = 2;
      $clock_events[$clock_sequence_index]{SHUB} = 1;
    }
    if ($text_line =~ m/Response: ALC/)
    {
      if ($restore_sequence_in_progress > 0)
      {
	$clock_events[$clock_sequence_index]{END} = $wall_clock_disp;
	#printSeq($clock_sequence_index);
	$clock_sequence_index++;
	$restore_sequence_in_progress = 0; 
      }
    }

    if ($text_line =~ m/Cmd: ALC , DATA=0,/)
    {
      if ($clock_sequence_ending > 0) {
	$clock_sequence_in_progress = 0; 
	$clock_events[$clock_sequence_index]{END} = $clock_sequence_ending;
	$clock_sequence_ending = 0; 
	#printSeq($clock_sequence_index);
	$clock_sequence_index++;
      }
      $collapse_sequence_in_progress = 1; 
      $clock_events[$clock_sequence_index]{START} = $wall_clock_disp;
      $clock_events[$clock_sequence_index]{MC} = 0;
      $clock_events[$clock_sequence_index]{SHUB} = 0;
    }
    if ($text_line =~ m/Response: POWERSTATE, DDRSS,/)
    {
      if ($collapse_sequence_in_progress == 1)
      {
	$collapse_sequence_in_progress = 0;
	$clock_events[$clock_sequence_index]{END} = $wall_clock_disp;
	#printSeq($clock_sequence_index);
	$clock_sequence_index++;
      }
    }


    if ($text_line =~ m/Cmd: CLOCK_PREWORK (\S+), DATA=([0-9A-F]+),RSI_RR=RR/i)
    {
      if ($clock_sequence_ending > 0) {
	$clock_sequence_in_progress = 0; 
	$clock_events[$clock_sequence_index]{END} = $clock_sequence_ending;
	$clock_sequence_ending = 0; 
	#printSeq($clock_sequence_index);
	$clock_sequence_index++;
      }
      if ($clock_sequence_in_progress == 0)
      {
	$clock_sequence_in_progress = 1; 
	$clock_events[$clock_sequence_index]{START} = $wall_clock_disp;
      }
      $clock_events[$clock_sequence_index]{$1}=hex($2);
    }
    if ($text_line =~ m/Response: CLOCK_POST, (\S+),/)
    {
      if ($clock_sequence_in_progress == 1)
      {
	$clock_sequence_ending = $wall_clock_disp;
      }
    }
    if (($text_line =~ m/Response: CLOCK_POST/) || ($text_line =~ m/Response: ALC/))
    {
      $max_sequence_end = $wall_clock_disp;
    }

    $lines[$output_count++] = sprintf ("%12X %s (SHRM_TIME:%X)%s", $wall_clock_disp, $text_line, $last_ts, $offset_str);
    $next_freq = $current_freq;
    if ($text_line =~ m/CMD: VOLTAGE_.*CX.*DATA=(\d+)/i) {
      $new_voltage = $1+0;
      $next_freq = 0;
      $next_freq = 1 if ($new_voltage >= 3);
      $next_freq = 2 if ($new_voltage >= 5);
    }
    if ($text_line =~ m/Cmd: POWERSTATE DDRSS.*DATA=(\d+)/i) {
      $new_state = $1+0;
      if ($new_state == 3)
      {
	$next_freq = 0;  # Assume powerstate 3 happens when we are at MinSVS
      }
    }
    if ($next_freq != $current_freq)
    {
      $time_period = $time_periods[$next_freq];
      $current_freq = $next_freq;
      $lines[$output_count++] = sprintf ("%12X Changing Clock to %d Mhz (SHRM_TIME:%X)", $wall_clock_disp, 1/$time_period, $last_ts);
    }
  }
  else
  {
    printf ("UNKNOWN LINE IN EVENT TABLE\n$event\n");
    last;
  }
}
if ($clock_sequence_ending > 0) {
  $clock_events[$clock_sequence_index]{END} = $clock_sequence_ending;
  #printSeq($clock_sequence_index);
}

#printf ("Max Sequence End at %X\n", $max_sequence_end);

if ($parsing_errors > 8) {
  print "Data does not look valid for SHRM commands\n";
  exit;
}

if (($wall_clock_ts == 0) && ($last_change_complete > 0)) {
  $wall_clock_ts           =$max_sequence_end;        # This offset in wall clock time
  if ($max_ts_low > 0) {
    $last_change_hi = $last_change_complete >> 32;
    #printf("Changing last_freq_sequence from %X to %X %X\n", $last_change_complete, $last_change_hi, $max_ts_low);
    $last_freq_sequence = ($last_change_hi << 32) + (hex('FFFFFFFF') & $max_ts_low) - 192;
  } else {
    $last_freq_sequence = $last_change_complete-(45*19.2);   # will display this wall_clock time.
  }
}

$time_offset = 0;

if ($wall_clock_ts != 0) {
  print "NOTE: SHRM Timestamps are approximate relative to matched events in MSGRAM11.\nUse SHRM_TIME for SHRM processor cycles\n";
  $time_offset = $last_freq_sequence - $wall_clock_ts;
  $clock_sequence_index-- if (!defined($clock_events[$clock_sequence_index]) && ($clock_sequence_index > 0));

  if ($hist_highest_index >= 0) {
    $hist_index = $hist_highest_index;

    while (defined($clock_events[$clock_sequence_index]) && ($clock_sequence_index >= 0))
    {
      #print "Comparing Clock Sequence Idx $clock_sequence_index to History Index $hist_index\n";
      #print "History $hist_index:";
      #print " MC $history[$hist_index]{MC_CHANGE}" if defined($history[$hist_index]{MC_CHANGE});
      #print " SHUB $history[$hist_index]{SHUB_CHANGE}" if defined($history[$hist_index]{SHUB_CHANGE});
      #print "<-> Clock Sequence:";
      #print " MC $clock_events[$clock_sequence_index]{MC}" if (defined($clock_events[$clock_sequence_index]{MC})); 
      #print " SHUB $clock_events[$clock_sequence_index]{SHUB}" if (defined($clock_events[$clock_sequence_index]{SHUB})); 
      #print "\n";

      if (($history[$hist_index]{MC_CHANGE}   == $clock_events[$clock_sequence_index]{MC}) &&
          ($history[$hist_index]{SHUB_CHANGE} == $clock_events[$clock_sequence_index]{SHUB}))
      {
	$start_system_time = $clock_events[$clock_sequence_index]{START} + $time_offset;
	$end_system_time   = $clock_events[$clock_sequence_index]{END}   + $time_offset;

	if (($start_system_time > $history[$hist_index]{START_TIME}) &&
	    ($end_system_time < $history[$hist_index]{END_TIME}))
	{
	  $clock_events[$clock_sequence_index]{OFFSET} = $time_offset;
	}
	else
	{
	  if (defined($history[$hist_index]{END_TIME}))
	  {
	    $time_offset = $history[$hist_index]{END_TIME} - $clock_events[$clock_sequence_index]{END} - 192;
	    $clock_events[$clock_sequence_index]{OFFSET} = $time_offset;
	    #printf ("Adjusting the offset for sequence %d to %X:", $clock_sequence_index, $time_offset);
	    #printf ("History ENDS %X Clock ends %X\n", $history[$hist_index]{END_TIME}, $clock_events[$clock_sequence_index]{END});
	  }
	}
      }

      $hist_index--;
      if ($hist_index < 0) { $hist_index = $#history; }

      last if ($hist_index == $hist_highest_index);
      $clock_sequence_index--;

      #$history[$hist_index]{START_TIME}
      #$history[$hist_index]{END_TIME}
      #$history[$hist_index]{MC_CHANGE}
      #$history[$hist_index]{SHUB_CHANGE}

      #$clock_events[$idx]{START},
      #$clock_events[$idx]{END};
      #$clock_events[$idx]{MC}
      #$clock_events[$idx]{SHUB}
    }
  }
} else {
  print "NOTE: SHRM Timestamps are approximate and relative to the start of the log.\nUse SHRM_TIME for SHRM processor cycles\n";
}

$my_index = 0;
$idx_started = 0;
$new_line=0;
$last_ts = -1;

for $i (0 .. ($output_count-1)) {
  if ($lines[$i] =~ m/^ *([0-9A-F]*) (.*)/i) {
    $timestamp = hex($1);  $line_data = $2;
  } else {
    print "NO MATCH \[$lines[$i]\]\n";
  }
  if ($timestamp == $clock_events[$my_index]{START})
  {
    if ($idx_started == 0)
    {
      print "\n" if ($new_line == 0);
      printSeq($my_index);
      $idx_started = 1;
    }
    $time_offset = $clock_events[$my_index]{OFFSET} if (defined($clock_events[$my_index]{OFFSET}));
  }

  $system_time = $timestamp + $time_offset;
  if ($last_ts >= 0)
  {
    $rel = relative($system_time, $last_ts);
  }
  else
  {
    $rel = "N/A";
  }
  $last_ts = $system_time;
  printf ("%12X (%10.10s): %s\n", $system_time, $rel, $line_data);

  $new_line = 0;
  if ($timestamp == $clock_events[$my_index]{END})
  {
    $new_line = 1;
    $idx_started = 0;
    $my_index++;
    print "\n";
  }

}
