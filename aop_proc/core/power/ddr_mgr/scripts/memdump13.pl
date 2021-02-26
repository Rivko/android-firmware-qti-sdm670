$warnings = 0;
$file = $ARGV[0];
if (-d $file) {
  @trials = qw(MSGRAM9.BIN aop_msg9_0xc390000--0xc3903ff.lst);
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
  print "Usage memdump.pl <filename>\n";
  exit;
}
if (! -e $file) 
{
  print "Error: $file does not exists\n";
  print "Usage memdump.pl <filename>\n";
  exit;
}
if (($file =~ m/.bin$/i) || ($file =~ m/.lst$/i))
{
  binmode LOGFILE;
  $bin_file = 1;
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
      if (/([0-9A-F]{8}) +([0-9A-F]{8}) +([0-9A-F]{8}) +([0-9A-F]{8})/)
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


sub by_timestamp
{
  $a_time = $1 if ($a =~ m/ *(.*) /);
  $b_time = $1 if ($b =~ m/ *(.*) /);

  return hex($a_time) <=> hex($b_time);
}

sub relative
{
   my $last = shift;
   my $first = shift;
   if ($first == 0) {
     return "N/A";
   }

   $rel = int(($last - $first) / 19.2);
   $units = "us";
   if ($rel >= 1000)
   {
      if ($rel >= 1000000) {
	$rel = $rel / 1000;
        $rel = int($rel+0.5)/1000;
        $units = "sec";
      } else {
        $rel = int($rel+0.5)/1000;
        $units = "ms";
      }
   }
   return "$rel $units";
}

sub print_percentage
{
  my $msg = shift;
  my $val = shift;
  
  return if ($val == 0);
  my $percent = (($val-1) * 4);
  if ($percent == 0) 
  {
    $percent = "< 0.4";
  } else {
    $percent = $percent / 10;
  }

  print "$msg: $percent" . "\%\n";
}

sub decode_status 
{
  $val = shift;
  $ret = "Unsafe($val)";

  $ret = "Safe(0)" if ($val == 0);
  $ret = "Safe(RESET)" if ($val == 1);
  $ret = "Unsafe(INIT_PWR_ON)" if ($val == hex('02'));
  $ret = "Unsafe(RCG_RST_REL)" if ($val == hex('04'));
  $ret = "Unsafe(CLAMP_DIS)" if ($val == hex('08'));
  $ret = "Unsafe(WCSS_ON)" if ($val == hex('10'));
  $ret = "Unsafe(CLK_DIS)" if ($val == hex('20'));
  $ret = "Unsafe(S_CLAMP_EN)" if ($val == hex('40'));
  $ret = "Safe(Power Off)" if ($val == hex('80'));
  $ret = "Unsafe(WLAN_ON)" if ($val == hex('100'));
  $ret = "Safe(PBS_WAIT)" if ($val == hex('200'));
  $ret = "Safe(WLAN_DISABLE)" if ($val == hex('400'));
  $ret = "Unsafe(IN_CLAMP_DIS)" if ($val == hex('800'));
  $ret = "Unsafe(S_CLK_EN)" if ($val == hex('1000'));

  return $ret;
}

$valid_sig = 0;
# First read the dictionary and validate that a bit.
if (get_bytes(LOGFILE, $bindata, 32) == 32)
{
  ($sig, $sleep1, $sleep2, $sleep3,
   $wake1, $wake2, $wake_err, $sleep_err) = unpack("I I I I I I I I", $bindata);
  if ($sig == hex('4e414c57'))
  {
    $valid_sig = 1;
  }
  
   if ($valid_sig) {
      print_percentage("Sleep Delay 0-49us",   $sleep1 % 256); $sleep1 = $sleep1 / 256;
      print_percentage("Sleep Delay 50-99us", $sleep1 % 256); $sleep1 = $sleep1 / 256;
      print_percentage("Sleep Delay 100-149us", $sleep1 % 256); $sleep1 = $sleep1 / 256;
      print_percentage("Sleep Delay 150-199us", $sleep1 % 256);
      print_percentage("Sleep Delay 200-249us", $sleep2 % 256); $sleep2 = $sleep2 / 256;
      print_percentage("Sleep Delay 250-299us", $sleep2 % 256); $sleep2 = $sleep2 / 256;
      print_percentage("Sleep Delay 300-349us", $sleep2 % 256); $sleep2 = $sleep2 / 256;
      print_percentage("Sleep Delay 350-399us", $sleep2 % 256);
      print_percentage("Sleep Delay 400-449us", $sleep3 % 256); $sleep3 = $sleep3 / 256;
      print_percentage("Sleep Delay 450-499us", $sleep3 % 256); $sleep3 = $sleep3 / 256;
      print_percentage("Sleep Delay 500-549us", $sleep3 % 256); $sleep3 = $sleep3 / 256;
      print_percentage("Sleep Delay 550-more",  $sleep3 % 256);

      print_percentage("Wake Delay 0-99us",    $wake1 % 256); $wake1 = $wake1 / 256;
      print_percentage("Wake Delay 100-149us", $wake1 % 256); $wake1 = $wake1 / 256;
      print_percentage("Wake Delay 150-199us", $wake1 % 256); $wake1 = $wake1 / 256;
      print_percentage("Wake Delay 200-249us", $wake1 % 256);
      @values = split(//, $wake2);
      print_percentage("Wake Delay 250-299us", $wake2 % 256); $wake2 = $wake2 / 256;
      print_percentage("Wake Delay 300-349us", $wake2 % 256); $wake2 = $wake2 / 256;
      print_percentage("Wake Delay 350-399us", $wake2 % 256); $wake2 = $wake2 / 256;
      print_percentage("Wake Delay 400-more",  $wake2 % 256);
      printf("Wake Errors: %d, Sleep Errors: %d\n\n", $wake_err, $sleep_err);
   }
}

# Now read the rest
if ($valid_sig) {
  while (get_bytes(LOGFILE, $bindata, 16) == 16)
  {
    ($timestamp_lo, $timestamp_hi, $flag2, $flag3) = unpack("I I I I", $bindata);
    $flag1 = ($timestamp_hi >> 24);
    $timestamp_hi = $timestamp_hi & hex('00FFFFFF');
    $timestamp = $timestamp_hi << 32 | $timestamp_lo;
    if ($timestamp > 0) {
      if ($flag1 == 0) {
	$workaround_used = 0;
	if ($flag2 & 1) {
	  $used = "";
	  $workaround_used = 1;
	} else {
	  $used = " - NO EFFECT DUE TO WCSSAON_CONFIG"
	}
	if ($flag2 & hex('80000000')) {
	  $wake_sleep = "Wake";
	  $flag2 = $flag2 & hex('7FFFFFFF');
	  $str = sprintf ("%11X %5s MPM_WCSSAON_CONFIG:%08X%s\n", $timestamp, $wake_sleep, $flag2, $used);
	} else {
	  $wake_sleep = "Sleep";
	  $flag3_decode = decode_status($flag3 & 0x1FFF);
	  if ($workaround_used) {
	    $str = sprintf ("%11X %5s MPM_WCSSAON_CONFIG:%08X  WCSSAON_MPM_STATUS:%08X(%s)%s\n", $timestamp, $wake_sleep, $flag2, $flag3, $flag3_decode, $used);
	  } else {
	    $str = sprintf ("%11X %5s MPM_WCSSAON_CONFIG:%08X%s\n", $timestamp, $wake_sleep, $flag2, $used);
	  }
	}
      } else {
	$str = sprintf ("%11X ERROR(%d): DATA1:%08X  DATA2:%08X\n", $timestamp, $flag1, $flag2, $flag3);
      }
      push @events, $str;
    }
  }

  $last_ts = 0;
  $first_ts = 0;
  foreach $event (sort by_timestamp @events)
  {
    if ($event =~ /^ *([0-9A-F]+) (.*)/)
    {
      $ts = hex($1);
      $first_ts = $ts if ($first_ts == 0);
      $rel = relative($ts, $last_ts);
      $last_ts = $ts;
      printf "%12X (%10.10s): %s\n", $ts, $rel, $2;
    }
  }

  $total_rel = relative($last_ts, $first_ts);
  print "\nTotal time in WLAN workaround history: $total_rel\n";
}
else
{
  print "Log contained unexpected data.  It doesn't look like the WLAN Workaround history\n";
}
exit;
