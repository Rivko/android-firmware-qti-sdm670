$warnings = 0;
$file = $ARGV[0];
if (-d $file) {
  @trials = qw(MSGRAM11.BIN aop_msg11_0xc3b0000--0xc3b03ff.lst);
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

sub relative
{
   $last = shift;
   $first = shift;
   $rel = int(($last - $first) / 19.2);
   $units = "us";
   if ($rel >= 100000)
   {
      $rel = $rel / 1000;
      if ($rel >= 1000) {
         $rel = int($rel+0.5)/1000;
         $units = "sec";
      } else {
         $rel = int($rel+0.5);
         $units = "ms";
      }
   }
   return "$rel $units";
}

$cur_offset = 0;
print "Process $file\n";
open LOGFILE, "< $file" || die "Cannot open $file: $!";
if (!defined($file)) {
  print "Usage ddr_stats.pl <filename>\n";
  exit;
}
if (! -e $file) 
{
  print "Error: $file does not exists\n";
  print "Usage ddr_stats.pl <filename>\n";
  exit;
}
if (($file =~ m/.bin$/i) || ($file =~ m/.lst$/i))
{
  binmode LOGFILE;
  $bin_file = 1;
}

sub print_hms
{
  $seconds = shift;

  $unit = 24*60*60;  # Seconds in a day
  $days = int($seconds/$unit);
  $seconds -= $days * $unit;
  $unit /= 24;
  $hours = int($seconds/$unit);
  $seconds -= $hours * $unit;
  $unit /= 60;
  $minutes = int($seconds/$unit);
  $seconds -= $minutes * $unit;

  $ret = "";
  $ret .= "$days Days " if ($days > 0);
  $ret .= sprintf("%d:%02.2d:%02.2d", $hours, $minutes, $seconds);

  return $ret;
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

# Print the 3 line header, with either the detail line first or last.
# If $arg == 0, the data follows, so put the detail line at the bottom.  If $arg == 1, 
# the data was above us, so print the detail line nearest the data.
sub print_event_header
{
  $arg = shift;
  if ($arg == 1) {
    print "  Timestamp |       | Event |ALC|ACV|  PS   |BCM|Curr|Next|BCM|Curr|Next|NonIdle|\n";
  }
  print "            Relative|       |   |   |       |     MC      |     SHUB    |Elapsed|\n";
  print "            |  Time |       |   |   |       | Clock Plan  | Clock Plan  | Time  |\n";
  if ($arg == 0) {
    print "  Timestamp |  (us) | Event |ALC|ACV|  PS   |BCM|Curr|Next|BCM|Curr|Next| (us)  |\n";
  }
}

# Compares two timestamps in the record for sorting based on that.
sub by_timestamp
{
  ($a_ts_lo, $a_ts_hi) = unpack("I I", $a);
  ($b_ts_lo, $b_ts_hi) = unpack("I I", $b);

  # If the hi levels are different, lows don't matter.
  if ($a_ts_hi != $b_ts_hi)
  {
    return $a_ts_hi <=> $b_ts_hi;
  }

  # If the hi levels are the same, compare the low 32 bits.
  return $a_ts_lo <=> $b_ts_lo;
}

# First read the dictionary and validate that a bit.
if (get_bytes(LOGFILE, $bindata, 16) != 16)
{
  print "Could not read dictionary in $file\n";
  exit;
}
$cur_offset = 16;
($event_offset, $num_event, $stats_offset, $config_offset) =
   unpack("I I I I", $bindata);

printf("Offset is %04.4X, Num events is %d.  Stats starts at %04.4X\n\n",
       $event_offset, $num_event, $stats_offset);
if ($event_offset != $cur_offset)
{
  print "Unexpected data in dictionary.   $event_offset != $cur_offset\n";
  exit;
}

# Next read the events.
$i = 0;
while ($i < $num_event)
{
  $i++;
  if (get_bytes(LOGFILE, $eventdata, 16) != 16)
  {
    print "Could not read the stats[$i]\n";
    exit;
  }
  push @events, $eventdata;
}

# Now that we have all the events, let's parse those out. 
@ps = ("0 (D3)","1 (D3)","2 (D0)","3 (D0)",
     " 4 CP1", " 5 CP2", " 6 CP3", " 7 CP4", " 8 CP5", " 9 CP6",
     " A CP7", " B CP8", " C CP9", "D CP10", "E CP11", "F CP11");
$i = 0;
$last_ts=0;
$first_ts=0;
$elapsed_start = 0;

print_event_header(0);

# For each event (sorted in timestamp order) print the data associated with it.
foreach $eventdata (sort by_timestamp @events)
{
  $i++;
  ($word1, $word2, $word3, $word4) = unpack("I I I I", $eventdata);
  $timestamp = ($word2 << 32) + $word1;
  #Words 1 and 2 are the timestamp.
  if ($last_ts == 0)
  {
    $rel = "N/A ";
    $first_ts = $timestamp;
  }
  else
  {
    $rel = int(($timestamp - $last_ts) / 19.2);
    $big_rel = 0;
    $big_rel = 1 if ($rel > 2000);

    if ($rel >= 100000) {
      $rel = $rel / 1000;
      if ($rel >= 1000) {
        $rel = $rel / 1000;
        $rel = int($rel+0.5) . " sec";
      } else {
        $rel = int($rel+0.5) . " ms";
      }
    }
    else
    {
      $rel =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
    }
  }

  # Word3 contains the Event/Powerstate/ALC and ACV values 
  $event      = $word3 & 0xF; $word3= $word3 >> 8;
  $powerstate = $word3 & 0xF; $word3= $word3 >> 8;
  $acv        = $word3 & 0xF; $word3= $word3 >> 8;
  $alc        = $word3 & 0xf; $word3= $word3 >> 8;

  if ($event == 2) {
    $event = "Idle";
  } elsif ($event == 1) {
    $event = "Flush";
  } elsif ($event == 0) {
    $event = "Change";
  }

  # Word4 contains the Clock Plan info for MC/SHUB both current values and BCM requests
  $bcm_mc_next= $word4 & 0xF; $word4 = $word4 >> 4; # Next value to process for BCM for MC
  $bcm_mc_curr= $word4 & 0xF; $word4 = $word4 >> 4; # BCM value being processed for MC
  $bcm_sh_next= $word4 & 0xF; $word4 = $word4 >> 4; # Next value to process for BCM for SHUB
  $bcm_sh_curr= $word4 & 0xF; $word4 = $word4 >> 4; # BCM value being processed for SHUB

  $act_mc_curr= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Current clock plan
  $act_mc_next= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Next clock plan
  $act_sh_curr= $word4 & 0xF; $word4 = $word4 >> 4; # SHUB's Current clock plan
  $act_sh_next= $word4 & 0xF; $word4 = $word4 >> 4; # MC's Next clock plan

  # If a clock plan is transitioning, Draw the arrow, otherwise, default to a vertical line
  $line2="| ";
  $line4="| ";
  if (!($event cmp "Change"))
  {
    $line2 = "+>" if ( $act_mc_curr != $act_mc_next);
    $line4 = "+>" if ( $act_sh_curr != $act_sh_next);
  }

  # If a Vote has changed, mark that as different (starting on line2)
  $ps_chg = " "; $mc_chg = " "; $sh_chg = " ";
  if ($i > 1) 
  {
    # Up votes are marked with a carat, down with a period
    $ps_chg = "*" if ($powerstate != $last_ps); 
    $mc_chg = "^" if ($bcm_mc_curr > $last_mc); 
    $mc_chg = "." if ($bcm_mc_curr < $last_mc); 
    $sh_chg = "^" if ($bcm_sh_curr > $last_sh); 
    $sh_chg = "." if ($bcm_sh_curr < $last_sh); 
  }

  #If this is the first non-idle event after an idle, mark the start time for elapsed time
  if (($elapsed_start == 0) && ($event != 2)) {
    $elapsed_start = $timestamp;
  } elsif ($elapsed_start != 0) {
    # Increment elapsed time until we are idle again.
    $elapsed_time = int(($timestamp - $elapsed_start) / 19.2);
    $gasp = " ";
    $gasp = "!" if ($big_rel == 1);
  }

  $gasp = $elapsed_time = "" if ($elapsed_time == 0);
  #Print the line of data.
  printf("%11X |%6s%1s|%6.6s |%2d |%2d |%6s%1s|%2d%1s| %2d %s%2d |%2d%1s| %2d %s%2d | %-5s |\n",
         $timestamp, $rel, $gasp, $event, $alc, $acv, $ps[$powerstate], $ps_chg,
         $bcm_mc_curr, $mc_chg, $act_mc_curr, $line2, $act_mc_next,
         $bcm_sh_curr, $sh_chg, $act_sh_curr, $line4, $act_sh_next,
         $elapsed_time);
  #Once we are idle, reset the elapsed time variables
  if (!($event cmp "Idle"))
  {
    $elapsed_start = 0;
    $elapsed_time = 0;
  }
  $last_ts = $timestamp;
  $last_ps = $powerstate;
  $last_mc = $bcm_mc_curr;
  $last_sh = $bcm_sh_curr;
}

print_event_header(1);

# If we don't end on Idle, that could mean an error in the system.   Mark the fact
# we were not Idle and let the user know where the log ended.
if ($event cmp "Idle")
{
  $warnings++;
  print "\n";
  print "*** WARNING DDR_MGR is NOT IDLE at end of log ";
  print " ****************\n";
  print "*** ";
  print "Flush in progress      " if (!($event cmp "Flush"));
  if (!($event cmp "Change"))
  {
    if ($act_mc_next == 0)    { print "Collapse in progress   "; }
    elsif ($act_mc_curr == 0) { print "Restore in progress    "; }
    else                      { print "Freq Change in progress" }
    print " ***********************************\n";
  }
} else {
  print "DDR_MGR is IDLE - Last Transitions completed\n";
}

# Skip over the ddr_config.  The user won't care.
$cur_offset += (16 * $num_event);
$need = $stats_offset - $cur_offset;
if (get_bytes(LOGFILE, $eventdata, $need) != $need)
{
  print "Could not read the config data of $need bytes\n";
  exit;
}
$cur_offset += $need;

# Now get and print the statictics portion of the data.
$need = 176;
if (get_bytes(LOGFILE, $stats_data, $need) != $need)
{
  print "Could not read the config data of $need bytes\n";
  exit;
}

($mc_count, $shub_count, $restore_count, $collapse_count,
  $mc_cp_history_lo, $mc_cp_history_hi, $shub_cp_history_lo, $shub_cp_history_hi,
  $last_mc_begin_lo, $last_mc_begin_hi, $last_shub_begin_lo, $last_shub_begin_hi,
  $prev_mc_begin_lo, $prev_mc_begin_hi, $prev_shub_begin_lo, $prev_shub_begin_hi,
  $collapse_begin_lo, $collapse_begin_hi, $restore_begin_lo, $restore_begin_hi,
  $mc_max_lo, $mc_max_hi, # x8
  $shub_max_lo, $shub_max_hi, # x8
  $collapse_max_lo, $collapse_max_hi, # x8
  $restore_max_lo, $restore_max_hi, # x8
  $flush_count, # x4
  $flush_begin_lo, $flush_begin_hi, 
  $flush_max_lo, $flush_max_hi, # x8
) = unpack("I I I I I I I I I I I I I I I I I I I I I I x8 I I x8 I I x8 I I x8 I x4 I I I I", $stats_data);

$uptime=int($last_ts/19200000);
$avg_mc = int(10*$mc_count/$uptime)/10;
$avg_shub = int(10*$shub_count/$uptime)/10;
$mc_count =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
$shub_count =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
$restore_count =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
$collapse_count =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
$flush_count =~ s/(\d)(?=(\d{3})+(\D|$))/$1\,/g;
print "\nMC_Changes:$mc_count - SHUB_CHANGE:$shub_count - ($avg_mc MC/Second)($avg_shub SHUB/Second)\n";
print "RESTORES:$restore_count, COLLAPSES:$collapse_count, FLUSHES:$flush_count\n";
printf ("Last two MC Changes were at:%X and %X\n",
      ($last_mc_begin_hi << 32) + $last_mc_begin_lo,
      ($prev_mc_begin_hi << 32 )+ $prev_mc_begin_lo);
printf ("Last two SHUB Changes were at:%X and %X\n",
      ($last_shub_begin_hi << 32) + $last_shub_begin_lo,
      ($prev_shub_begin_hi << 32) + $prev_shub_begin_lo);
printf ("The 16 most recent  MC  frequencies were OLDEST->%08.8X%08.8X<-Current\n", $mc_cp_history_hi, $mc_cp_history_lo);
printf ("The 16 most recent SHUB frequencies were OLDEST->%08.8X%08.8X<-Current\n", $shub_cp_history_hi, $shub_cp_history_lo);
printf ("Last Restore was at: %X, Last Flush was at: %X, Last Collapse was at: %X\n",
      ($restore_begin_hi << 32) + $restore_begin_lo,
      ($flush_begin_hi << 32) + $flush_begin_lo,
      ($collapse_begin_hi << 32) + $collapse_begin_lo);
$rel = relative($last_ts, ($collapse_begin_hi << 32) + $collapse_begin_lo);
print ("Last Collapse was started: ");
if (($rel =~ m/sec/) && ($rel > 120)) {
  print print_hms($rel);
} else {
  print "$rel";
}
print (" ago.  ");
print ("Last Restore was started: ");
$rel = relative($last_ts, ($restore_begin_hi << 32) + $restore_begin_lo);
if (($rel =~ m/sec/) && ($rel > 120)) {
  print print_hms($rel);
} else {
  print "$rel";
}
print (" ago\n");
$rel = relative($last_ts, ($flush_begin_hi << 32) + $flush_begin_lo);
print ("Last Flush was actually performed: ");
if (($rel =~ m/sec/) && ($rel > 120)) {
  print print_hms($rel);
} else {
  print "$rel";
}
print (" ago.\n");
printf ("Worstcase timing (us) was- MC Change:%d SHUB Change:%d COLLAPSE:%d Restore:%d Flush:%d\n",
      int($mc_max_lo/19.2+0.5),
      int($shub_max_lo/19.2+0.5),
      int($collapse_max_lo/19.2+0.5),
      int($restore_max_lo/19.2+0.5),
      int($flush_max_lo/19.2+0.5));

print "System Uptime: ";
print print_hms($uptime);
$rel = relative($last_ts, $first_ts);
print ".  Activity Log contains $rel of information.\n";
print "\n";

# Finally, if we printed warnings above, remind the user to look at that.
print "\n*** WARNINGS PRINTED ABOVE! ***\n" if ($warnings);