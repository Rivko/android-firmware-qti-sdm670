$warnings = 0;
$file = $ARGV[0];
if (-d $file) {
  @trials = qw(MSGRAM15.BIN aop_msg15_0xc3f0000--0xc3f03ff.lst);
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
  print "Usage memdump15.pl <filename>\n";
  exit;
}
if (! -e $file) 
{
  print "Error: $file does not exists\n";
  print "Usage memdump15.pl <filename>\n";
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

$cx_collapse_sig = hex('64737863');
$aoss_sleep_sig = hex('64736f61');

$sleep_seen = 0;
$sleep_count = 0;
$sleep_enter = 0;
$sleep_exit = 0;
$sleep_total = 0;

$collapse_seen = 0;
$collapse_count = 0;
$collapse_enter = 0;
$collapse_exit = 0;
$collapse_total = 0;

# First read the dictionary and validate that a bit.
while (get_bytes(LOGFILE, $bindata, 16) == 16)
{
  ($data1, $data2, $data3, $data4) = unpack("I I I I", $bindata);
  # printf ("%08.8X: %08.8X %08.8X %08.8X %08.8X\n", $address, $data1, $data2, $data3, $data4);
  if ($address == hex('40'))
  {
    if ($data3 == $aoss_sleep_sig)
    {
      $sleep_seen = 1;
      $sleep_count = $data4;
    }
    else
    {
      printf ("Address: %08.8X, Expected %X got %X\n", $address+8, $aoss_sleep_sig, $data3);
    }
  }
  if (($address == hex('50')) && ($sleep_seen))
  {
    $sleep_enter = $data2 << 32 | $data1;
    $sleep_exit = $data4 << 32 | $data3;
  }
  if ($address == hex('60'))
  {
    $sleep_total = ($data2 << 32 | $data1) if ($sleep_seen);
    if ($data3 == $cx_collapse_sig)
    {
      $collapse_seen = 1;
      $collapse_count = $data4;
    }
    else
    {
      printf ("Address %08.8X Expected %X got %X\n", $address+8, $aoss_collapse_sig, $data3);
    }
  }
  if (($address == hex('70')) && ($collapse_seen))
  {
    $collapse_enter = $data2 << 32 | $data1;
    $collapse_exit = $data4 << 32 | $data3;
  }
  if (($address == hex('80')) && ($collapse_seen))
  {
    $collapse_total = ($data2 << 32 | $data1);
  }

  $address+=16;
}

if ($sleep_seen) {
  printf("No AOSS Sleep Cycles recorded in log\n") if ($sleep_count == 0);
  printf("%12X AOSS Sleep Last Entered.   Sleep Count = %X(%d)\n", $sleep_enter, $sleep_count, $sleep_count) if ($sleep_enter > 0);
  $duration = 'N/A';
  $duration = relative($sleep_exit, $sleep_enter) if ($sleep_enter < $sleep_exit);
  $total_duration = relative($sleep_total, 0);
  printf("%12X AOSS Sleep Last Exited.  Duration: %s Total Duration = %s\n", $sleep_exit, $duration, $total_duration) if ($sleep_exit > 0);
}

if ($collapse_seen) {
  printf("No CX Collapse Cycles recorded in log\n") if ($collapse_count == 0);
  printf("%12X CX Collapse Last Entered. Collapse Count = %d\n", $collapse_enter, $collapse_count) if ($collapse_enter > 0);
  $duration = 'N/A';
  $duration = relative($collapse_exit, $collapse_enter) if ($collapse_enter < $collapse_exit);
  $total_duration = relative($collapse_total, 0);
  printf("%12X CX Collapse Last Exited.  Duration: %s Total Duration = %s\n", $collapse_exit, $duration, $total_duration) if ($collapse_exit > 0);
}

exit;
