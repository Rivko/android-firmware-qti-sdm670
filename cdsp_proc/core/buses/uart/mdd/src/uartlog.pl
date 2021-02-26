#!/cygdrive/c/utils/perl/bin/perl -w

#-----------------------------------------------------------------------------
#  uartlog
#
#  Create trace output from a trace32 dump.  Used with the UART_LOG macros.
#-----------------------------------------------------------------------------

use warnings;
use strict;

#-----------------------------------------------------------
#-----------------------------------------------------------
#  Timestamp
#-----------------------------------------------------------
#-----------------------------------------------------------

package Timestamp;

#  timetick clock runs at 19.2 MHz

$Timestamp::kTicksPerSecond = 19200000;

#-----------------------------------------------------------
#  Timestamp::create
#-----------------------------------------------------------

sub create
{
   my ( $class, %args ) = @_;

   my $self =
   {
      msw => $args{msw},    # Most significant word.
      lsw => $args{lsw},    # Least significant word.
      seq => $args{seq},    # Sequence number.
   };

   bless $self, $class;

   return $self;
}

#-----------------------------------------------------------
#  Timestamp::compareTo
#-----------------------------------------------------------

sub compareTo
{
   my ( $self, %args ) = @_;


   my $other = $args{other};

   if( $self->{msw} != $other->{msw} )
   {
      return( $self->{msw} - $other->{msw} );
   }

   if( $self->{lsw} != $other->{lsw} )
   {
      return( $self->{lsw} - $other->{lsw} );
   }

   return( $self->{seq} - $other->{seq} );
}

#-----------------------------------------------------------
#  Timestamp::parse
#-----------------------------------------------------------

sub parse
{
   my ( $self, %args ) = @_;


   my $hours    = 0;
   my $minutes  = 0;
   my $seconds  = 0;
   my $useconds = 0;

   #  Do everything with arbitrary size integers.  Perl doesn't
   #  handle 64 bit ints well.
   
   use bigint;

   my $ticks = ( $self->{msw} << 32 ) | $self->{lsw};

   $useconds = ($ticks * 1000000) / $Timestamp::kTicksPerSecond;

   $seconds = $useconds / 1000000;
   $useconds -= (1000000 * $seconds);

   $minutes = $seconds / 60;
   $seconds -= (60 * $minutes);

   $hours = $minutes / 60;
   $minutes -= (60 * $hours);

   return( $hours, $minutes, $seconds, $useconds );
}

#-----------------------------------------------------------
#  Timestamp::toHexString
#-----------------------------------------------------------

sub toHexString
{
   my ( $self, %args ) = @_;

   

   my $msw = $self->{msw};
   my $lsw = $self->{lsw};

   my $hexString = sprintf("%08X%08X", $msw, $lsw );

   $hexString =~ s/^0+//;  # truncate leading 0s.

   return( $hexString );
}

#-----------------------------------------------------------
#  Timestamp::toDecimalString
#-----------------------------------------------------------

sub toDecimalString
{
   my ( $self, %args ) = @_;


   #  Do everything with arbitrary size integers.  Perl doesn't
   #  handle 64 bit ints well.
   
   use bigint;

   my $ticks = ( $self->{msw} << 32 ) | $self->{lsw};

   my $decimalString = "";

   $decimalString = $ticks;

   return( $decimalString );
}

#-----------------------------------------------------------
#-----------------------------------------------------------
#  TraceRecord
#-----------------------------------------------------------
#-----------------------------------------------------------

package TraceRecord;

@TraceRecord::kIsPrintable = 
(
#  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 

   0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0,      # 0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # 1
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      # 2
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      # 3
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      # 4
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      # 5
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,      # 6
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,      # 7
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # 8
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # 9
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # A
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # B
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # C
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # D
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,      # E
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0       # F
);

#-----------------------------------------------------------
#  TraceRecord::create
#-----------------------------------------------------------

sub create
{
   my ( $class, %args ) = @_;

   my $self =
   {
      fileId       => $args{fileId},
      lineNumber   => $args{lineNumber},
      magic        => $args{magic},
      timestamp    => $args{timestamp},
      args         => $args{args},
      buffer       => $args{buffer},
   };

   bless $self, $class;

   return $self;
}

#-----------------------------------------------------------
#  TraceRecord::dump
#-----------------------------------------------------------

sub dump
{
   my ( $self, %args ) = @_;

   my @buffer = @{$self->{buffer}};

   print("\n");
   if( $#buffer >= 0 )
   {
      printf("BufLogBufferMacro\n");
   }
   else
   {
      printf("BufLogMacro\n");
   }
   printf("------------------------------------------------------\n");

   printf("fileId        = %d\n",     $self->{fileId} );
   printf("lineNumber    = %d\n",     $self->{lineNumber} );
   printf("timestamp.msw = 0x%08x\n", $self->{timestamp}->{msw} );
   printf("timestamp.lsw = 0x%08x\n", $self->{timestamp}->{lsw} );
   printf("timestamp.seq = %d\n",     $self->{timestamp}->{seq} );

   my @pargs = @{$self->{args}};
   printf("numArgs      = %d\n", $#pargs + 1 );
   for( my $i = 0; $i <= $#pargs; $i++ )
   {
      printf("arg[%d]       = %d\n", $i, $pargs[$i] );
   }

   printf("numBytes     = %d\n", $#buffer + 1 );
   for( my $i = 0; $i <= $#buffer; $i++ )
   {
      if(( $i & 0xf ) == 0 )
      {
         if( $i == 0 )
         {
            printf("data         =");
         }
         else
         {
            printf("\n              ");
         }
      }

      printf(" %02x", $buffer[$i] );
   }
   printf("\n");
}

#-----------------------------------------------------------
#  TraceRecord::writeLog
#-----------------------------------------------------------

sub writeLog
{
   my ( $self, %args ) = @_;


   my $traceInfo          = $args{traceInfo};
   my $rawTimestampFormat = $args{rawTimestampFormat};

   my $timestamp = $self->{timestamp};

   my ( $hours, $minutes, $seconds, $useconds ) = $timestamp->parse();

   printf("%d:%02d:%02d.%06d   ",
      $hours,
      $minutes,
      $seconds,
      $useconds );

   if( $rawTimestampFormat == $TraceLog::kRawTimestampNone )
   {
      #  Do nothing.
   }
   elsif( $rawTimestampFormat == $TraceLog::kRawTimestampHex )
   {
      printf("%-16.16s   ", $timestamp->toHexString());
   }
   elsif( $rawTimestampFormat == $TraceLog::kRawTimestampDecimal )
   {
      printf("%-21.21s   ", $timestamp->toDecimalString());
   }

   printf("%-6.6s  %-20.20s %8d   ", 
      $TraceLog::kLogLevelName[$traceInfo->{logLevel}],
      $traceInfo->{fileName}, 
      $self->{lineNumber} );

   my $format = $traceInfo->{format};

   #  Format might be undefined in the case where there
   #  are conflicting file IDs.

   if( !defined( $format ))
   {
      $format = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";
   }
   $format =~ s/\\n/\n/g;

   #  Print out format string.

   my @args = @{$self->{args}};

   #  Make sure all args are defined.  This covers the
   #  case where the log doesn't match the code.  You 
   #  don't want to print perl runtime errors.

   if( $traceInfo->{numArgs} > 0 )
   {
      my $i = $traceInfo->{numArgs} - 1;
      if( !defined( $args[$i] ))
      {
         $format = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";
         $traceInfo->{numArgs} = 0;
      }
   }

   if( $traceInfo->{numArgs} == 0 )
   {
      print("$format");
   }
   elsif( $traceInfo->{numArgs} == 1 )
   {
      printf("$format", $args[0] );
   }
   elsif( $traceInfo->{numArgs} == 2 )
   {
      printf("$format", $args[0], $args[1] );
   }
   elsif( $traceInfo->{numArgs} == 3 )
   {
      printf("$format", $args[0], $args[1], $args[2] );
   }
   elsif( $traceInfo->{numArgs} == 4 )
   {
      printf("$format", $args[0], $args[1], $args[2], $args[3] );
   }
   elsif( $traceInfo->{numArgs} == 5 )
   {
      printf("$format", $args[0], $args[1], $args[2], $args[3], $args[4] );
   }
   else
   {
      die "Internal error.";
   }

   #  If the hours field in the timestamp takes up more
   #  than the expected single digit, note it see we can
   #  line up data.

   my $temp = sprintf("%d", $hours );
   my $hoursLen = length( $temp );

   #  Print out buffer data.

   my @buffer = @{$self->{buffer}};

   if( $self->{magic} == $TraceLog::kMagic1 ) 
   {
      printf("\n");

      use integer;
      my $numLines = ($#buffer + 1 + (16-1))/16;

      for( my $i = 0; $i < $numLines; $i++ )
      {
         printf("                                                        ");

         #  If the hours field takes up more than the expected single digit, 
         #  print extra spaces to make the data line up. 

         my $extra = "";
         if( $hoursLen > 1 )
         {
            $extra .= " " x ($hoursLen - 1);
         }

         #  If we're printing out the timestamp column, add spaces for that.

         if( $rawTimestampFormat == $TraceLog::kRawTimestampHex )
         {
            $extra .= " " x 19;
         }
         elsif( $rawTimestampFormat == $TraceLog::kRawTimestampDecimal )
         {
            $extra .= " " x 24;
         }

         if( length( $extra ) > 0 )
         {
            print( $extra );
         }

         my $j;
         my $first = (16 * $i);
         my $ind   = $first;
         for( $j = 0; $j < 16; $j++, $ind++ )
         {
            if( $ind > $#buffer )
            {
               printf("   ");
            }
            else
            {
               printf(" %02x", $buffer[$ind]);
            }
         }

         printf("  ");
         $ind = $first;
         for( $j = 0; $j < 16; $j++, $ind++ )
         {
            if( $ind > $#buffer )
            {
               printf("  ");
            }
            elsif( !$TraceRecord::kIsPrintable[$buffer[$ind]] )
            {
               printf(" .", $buffer[$ind]);
            }
            else
            {
               if( $buffer[$ind] == 0x08 )
               {
                  # backspace.
                  printf("\\b", $buffer[$ind]);
               }
               elsif( $buffer[$ind] == 0x09 )
               {
                  # tab
                  printf("\\t", $buffer[$ind]);
               }
               elsif( $buffer[$ind] == 0x0A )
               {
                  # newline
                  printf("\\n", $buffer[$ind]);
               }
               elsif( $buffer[$ind] == 0x0C )
               {
                  # formfeed
                  printf("\\f", $buffer[$ind]);
               }
               elsif( $buffer[$ind] == 0x0D )
               {
                  # carriage return
                  printf("\\r", $buffer[$ind]);
               }
               else
               {
                  printf("%2c", $buffer[$ind]);
               }
            }
         }

         printf("\n");
      }
   }
   elsif( $self->{magic} == $TraceLog::kMagic2 ) 
   {
      #  Convert array of uint8 into string.
      
      my $msg;
      for( my $i = 0; $i <= $#buffer; $i++ )
      {
         $msg .= chr( $buffer[$i] );
      }

      print( $msg );
      print("\n");
   }
}

#-----------------------------------------------------------
#-----------------------------------------------------------
#  TraceInfo
#-----------------------------------------------------------
#-----------------------------------------------------------

package TraceInfo;

$TraceInfo::kBufLogMacro        = 0xb1;
$TraceInfo::kBufLogBufferMacro  = 0xb2;
$TraceInfo::kBufLogDalMsgMacro  = 0xb3;

$TraceInfo::kTraceLogLevel      = 1;
$TraceInfo::kLowLogLevel        = 2;
$TraceInfo::kMedLogLevel        = 3;
$TraceInfo::kHighLogLevel       = 4;
$TraceInfo::kErrorLogLevel      = 5;
$TraceInfo::kFatalLogLevel      = 6;
$TraceInfo::kUnknownLogLevel    = 7;

#-----------------------------------------------------------
#  TraceInfo::create  
#-----------------------------------------------------------

sub create
{
   my ( $class, %args ) = @_;

   my $self =
   {
      fileName  => $args{fileName},
      macroType => $args{macroType},
      logLevel  => $args{logLevel},
      format    => $args{format}, 
      numArgs   => $args{numArgs},
   };

   bless $self, $class;

   return $self;
}

#-----------------------------------------------------------
#  TraceInfo::dump
#-----------------------------------------------------------

sub dump
{
   my ( $self, %args ) = @_;

   printf("file = %s, type = %d, logLevel = %s, numArgs = %d, format = \"%s\"\n", 
          $self->{fileName},   
          $self->{macroType},   
          $TraceLog::kLogLevelName[$self->{logLevel}],
          $self->{numArgs},
          $self->{format});
}

#-----------------------------------------------------------
#-----------------------------------------------------------
#  TraceLog
#-----------------------------------------------------------
#-----------------------------------------------------------

package TraceLog;

$TraceLog::kMagic1   = 0xfabdeed1;
$TraceLog::kMagic2   = 0xfabdeed2;
$TraceLog::kSentinel = 0xdeadba11;

$TraceLog::kRawTimestampNone    = 0;
$TraceLog::kRawTimestampHex     = 1;
$TraceLog::kRawTimestampDecimal = 2;

$TraceLog::debug     = 0;

@TraceLog::kLogLevelName = 
(
   "INVALID",
   "TRACE",
   "LOW",
   "MED",
   "HIGH",
   "ERROR",
   "FATAL",
   "XXXXX"
);

#-----------------------------------------------------------
#  TraceLog::usage
#-----------------------------------------------------------

sub usage
{
   printf STDERR <<USAGE;

usage:  uartlog  dumpfile  srcDir...

        uartlog  -?        #  prints man page to stdout

USAGE

   exit(1);
}

#-----------------------------------------------------------
#  TraceLog::manPage
#-----------------------------------------------------------

sub manPage
{
   printf STDOUT <<MANPAGE;

uartlog(1)

NAME

    uartlog - create a trace log

SYNOPSIS

    uartlog  [-td|-th]  dumpfile  srcDir...

    uartlog  -?        #  prints man page to stdout

DESCRIPTION

    uartlog is used to produce a diag-like log file from
    the trace buffer contents.

    The idea is to use the UART_LOG macros to store the minimal
    amount of data needed for postprocessing, and avoid printf 
    like processing when saving data.  This makes the UART_LOG 
    macros extremely lightweight.  

    For example, if file.c contains the UART_LOG macro:

        UART_LOG_1( TRACE, "The value of x is %%d.", x );

    What's stored to the trace buffer is: 
    
        1) A file id given by value of the UART_TRACE_BUFFER_FILE_ID macro.
           This macro is defined with a unique number for each file
           that is using the UART_LOG macros.

        2) A line number given by the C processor macro __LINE__.
    
        3) A timestamp from the appropriate system call.

        4) The argument values.  (In this case, the value
           of x.)

    For each record in the trace buffer, the postprocessing step 
    (this script) will look for the file with the UART_TRACE_BUFFER_FILE_ID 
    macro equal to the file id, look at the line number, extract 
    the printf string, and call the perl printf function with the 
    arguments from the trace buffer.  This results in the following 
    diag-like output:

    Time             Level   File                     Line   Message
    ------------------------------------------------------------------------------
    0:04:23.105120   TRACE   file.c                     28   The value of x is 23.

    Note that since the file line numbers are contained in the trace
    buffer, uartlog must be run on the exact same source that produced 
    the trace buffer.  Otherwise uartlog will not be able to find the 
    format strings associated with the trace buffer data.

    There is one recognized format for the dump file:

        1) Output produced by the view command:

           view s_uartTraceLogInfo

    Each srcDir specifies an entire tree.

OPTIONS

    -td
    -th

    Print out a column for the raw timestamp.  th prints the timestamp 
    in hex; td prints it in decimal.

    Hex:

    Time             Timestamp          Level   File                     Line   Message
    ---------------------------------------------------------------------------------------------------
    0:04:23.105120   123456789ABCDEF0   TRACE   file.c                     28   The value of x is 23.

    Decimal:

    Time             Timestamp               Level   File                     Line   Message
    ------------------------------------------------------------------------------------------------------
    0:04:23.105120   123456789012345678901   TRACE   file.c                     28   The value of x is 23.

TO USE

    1) Make sure the file that you want to put logging in has a
       define for UART_TRACE_BUFFER_FILE_ID.  

    2) Insert UART_LOG macros into the code.  Note that the macro
       name corresponds to the number of printf arguments.  Use
       UART_LOG_1 for one argument, UART_LOG_2 for two arguments,
       etc.  Up to three arguments are supported.  UART_LOG is
       equivalent to UART_LOG_0.

       The first argument of the macro is the trace level: TRACE,
       LOW, MEDIMUM, HIGH, ERROR, FATAL.  The TRACE level logs only
       to the trace buffer.  All other levels log to the trace 
       buffer and call the corresponding diag macro (e.g. MSG_LOW).

    3) Build, run.

    4) Save the trace buffer contents to a file.  The trace buffer 
       is the contents of the variable s_uartTraceLogInfo.

       trace32 does not make this easy.  What works is the following
       steps:

       1) printer.file c:\\dump000.txt

          This sets the "printer" to be the file dump000.txt.  
          Note that by including a number in the file name, the 
          printer output file will automatically be incremented 
          each time you print.  The next print will be written to 
          file dump001.txt.

       2) view s_uartTraceLogInfo

          This will open a window showing the trace log buffer.
          You need to expand the array of bytes that contains
          the trace log data.

       3) Select "print all" from the system icon (upper left
          corner) of the window displaying the s_uartTraceLogInfo 
          variable.  This will save the text of the window to
          your printer file.

    5) Run the uartlog script with the dump file and the location 
       of your source code as arguments.  I suggest redirecting the
       output to a file.  For fuller logs, it takes about 15-20 
       seconds to run. 

EXAMPLE

    # Look for UART_LOG macros in all files in the current directory.

    uartlog  dump000.txt  .  > log.txt

    # Look for UART_LOG macros in all files starting at the tree rooted 
    # three directories up.

    uartlog  dump000.txt  ../../..  > log.txt  

BUGS

    1) Each file that uses the UART_LOG macros needs to have a definition
       for the UART_TRACE_BUFFER_FILE_ID macro.  The value needs to be unique 
       across all files.  

    2) The UART_LOG macro needs to be contained on a single line.  If it
       isn't, everything will still work but the entry in the log file
       for the offending macro won't print correctly.

    3) This version of uartlog.pl requires the perl bigint package. 
       I'm not sure what version of perl that implies.  I think bigint
       first came in around perl 5.8, with important bug fixes in 
       perl 5.8.1 and upgrades in perl 5.8.6, perl 5.8.7, and perl 5.8.9. 
       This script has been tested with perl 5.8.8, so consider that the
       minimum version.

MANPAGE

   exit(0);
}

#-----------------------------------------------------------
#  TraceLog::checkPerlVersion
#-----------------------------------------------------------

sub checkPerlVersion
{
   my ( $class, %args ) = @_;



   #  Require perl v5.8.8 as the minimum version.
   #  If we can't determine the version, continue
   #  anyway.  If there's a problem, the user will
   #  find out soon enough.  (It WOULD be nice to
   #  print a useful message though.)
   #
   #  Not sure if this does any good.  Does the test
   #  for bigint execute on load or when it executes
   #  the function that requires bigint?  At least
   #  it doesn't do any harm.

   if( !open( PERL_VERSION, "perl -v |"))
   {
      return;
   }

   #  Suck in the whole version string.

   my @lines = <PERL_VERSION>;
   close( PERL_VERSION );

   my $i;
   for( $i = 0; $i <= $#lines; $i++ )
   {
      my $version = $lines[$i];

      if( $version =~ /v[0-9]+\.[0-9]+/)
      {
         chomp( $version );
         # print("version = <$version>\n");  

         my $v1 = 0;
         my $v2 = 0;
         my $v3 = 0;
   
         if( $version =~ /v([0-9]+)\.([0-9]+)\.([0-9]+)/)
         {
            #  For three digit version numbers, e.g. 5.8.3.
      
            $v1 = $1;
            $v2 = $2;
            $v3 = $3;
         }
         elsif( $version =~ /v([0-9]+)\.([0-9]+)/)
         {
            #  For two digit version numbers, e.g. 5.8.
      
            $v1 = $1;
            $v2 = $2;
         }

         # printf("version = v%d.%d.%d\n", $v1, $v2, $v3 );

         if( $v1 > 0 )
         {
            #  perl v5.8.8 is the minimum version.

            my $goodVersion = 1;

            if( $v1 < 5 )
            {
               $goodVersion = 0;
            }
            elsif( $v1 == 5 )
            {
               if( $v2 < 8 )
               {
                  $goodVersion = 0;
               }
               elsif( $v2 == 8 )
               {
                  if( $v3 < 8 )
                  {
                     $goodVersion = 0;
                  }
               }
            }

            if( !$goodVersion )
            {
               print( STDERR "\n");
               print( STDERR "uartlog:  this script requires perl version >= 5.8.8.  Type \"perl -v\" to see your perl version.\n");
               print( STDERR "\n");
               exit(1);
            }
         }
      }
   }
}

#-----------------------------------------------------------
#  TraceLog::createFromCommandLine
#-----------------------------------------------------------

sub createFromCommandLine
{
   my ( $class, %args ) = @_;






   #  Process command line.

   my $rawTimestampFormat = $TraceLog::kRawTimestampNone;

   my $i;
   for( $i = 0; $i <= $#ARGV; $i++ )
   {
      #  Process options.

      my $arg = $ARGV[$i];
      if( $arg eq "-?")
      {
         manPage();
      }
      elsif( $arg =~ /^-td/)
      {
         $rawTimestampFormat = $TraceLog::kRawTimestampDecimal;
      }
      elsif( $arg =~ /^-th/)
      {
         $rawTimestampFormat = $TraceLog::kRawTimestampHex;
      }
      elsif( $arg =~ /^-/)
      {
         usage();  # unrecognized option.
      }
      else
      {
         last;
      }
   }

   my $dumpFile = undef;
   my @sourceDirs;
   for( ; $i <= $#ARGV; $i++ )
   {
      my $arg = $ARGV[$i];

      if( !defined( $dumpFile ))
      {
         $dumpFile = $arg;

         if( ! -f $dumpFile )
         {
            print( STDERR "\n");
            print( STDERR "uartlog:  file $dumpFile not found.\n");
            exit(1);
         }
      }
      else
      {
         $arg =~ s/\\/\//g;  # backslashes suck.

         if( ! -d $arg )
         {
            print( STDERR "\n");
            print( STDERR "uartlog:  directory $arg not found.\n");
            exit(1);
         }

         push( @sourceDirs, $arg );
      }
   }

   #  dumpfile required.

   if( !defined( $dumpFile ))
   {
      usage();
   }

   #  One or more source dirs required.

   if( $#sourceDirs < 0 )
   {
      usage();
   }

   #  Now that we're going to do something, check the perl version first.

   TraceLog->checkPerlVersion();

   #  Do it.

   return( TraceLog->create( dumpFile           => $dumpFile,
                             sourceDirs         => \@sourceDirs,
                             rawTimestampFormat => $rawTimestampFormat ));
}

#-----------------------------------------------------------
#  TraceLog::create  
#-----------------------------------------------------------

sub create
{
   my ( $class, %args ) = @_;

   my @traceRecords;

   my $self =
   {
      dumpFile           => $args{dumpFile},   # dump file
      sourceDirs         => $args{sourceDirs}, # array of all source dirs
      rawTimestampFormat => $args{rawTimestampFormat},
      traceRecords       => \@traceRecords,
   };

   bless $self, $class;

   return( $self );
}

#-----------------------------------------------------------
#  TraceLog::getFileTraceInfo
#-----------------------------------------------------------

sub getFileTraceInfo
{
   my ( $self, %args ) = @_;


   my $filePathName = $args{filePathName};

   my $fileName = $filePathName;
   $fileName =~ s/.*\///;   # strip off path.

   local *SRC_FILE;
   if( !open( SRC_FILE, "<$filePathName"))
   {
      print( STDERR "\nuartlog:  cannot read $filePathName.\n");
      exit(1);
   }

# print("$filePathName\n");

   #  Pass one:  See if this file has file id stamp.

   my @lines = <SRC_FILE>;
   close( SRC_FILE );

   my $fileId;
   my $line;
   my $i;
   for( $i = 0; $i <= $#lines; $i++ )
   {
      $line = $lines[$i];
      if( $line =~ /define\s+UART_TRACE_BUFFER_FILE_ID\s+(\d+)/)
      {
         $fileId = $1;
      }
   }

   if( not defined( $fileId ))
   {
      #  No id.  Done. 
      return;
   }

   #  See if any other files have this id.

   if( defined( $self->{fileIdMap}{$fileId} ))
   {
      printf( STDERR "uartlog:  warning the following files both have file id %d:\n", $fileId );
      printf( STDERR "           %s\n", $filePathName );
      printf( STDERR "           %s\n", $self->{fileIdMap}{$fileId} );
      return;
   }

   #  Remember the fileId associated with this file.  Do this
   #  for error checking -- to see if more than one file has 
   #  the same id.

   $self->{fileIdMap}{$fileId} = $filePathName;

   #  Pass two:  Extract UART_LOG info.

   for( $i = 0; $i <= $#lines; $i++ )
   {
      $line = $lines[$i];
      chomp( $line );

      #  Match the name of the UART_LOG macro (as a word) + 0 or more spaces +
      #  an open paren.  This will more closely match actual invocations 
      #  of the UART_LOG macro and not places where UART_LOG appears in 
      #  a comment.  

      if(( $line =~ /\bUART_LOG\b\s*\(/) || 
         ( $line =~ /\bUART_LOG_[0-9]+\b\s*\(/) || 
         ( $line =~ /\bUART_BUFFER_LOG\b\s*\(/) ||
         ( $line =~ /\bUART_BUFFER_LOG_[0-9]+\b\s*\(/) ||
         ( $line =~ /\bUART_DAL_MSG_LOG\b\s*\(/) || 
         ( $line =~ /\bUART_DAL_MSG_LOG_[0-9]+\b\s*\(/))
      {
         my $macroType; 
         my $numArgs;
         my $logLevel;

         if( $line =~ /\bUART_LOG\b/)
         {
            $numArgs   = 0;
            $macroType = $TraceInfo::kBufLogMacro;
         }
         elsif( $line =~ /\bUART_LOG_(\d+)\b/)
         {
            $numArgs   = $1;
            $macroType = $TraceInfo::kBufLogMacro;
         }
         elsif( $line =~ /\bUART_BUFFER_LOG\b/)
         {
            $numArgs   = 0;
            $macroType = $TraceInfo::kBufLogBufferMacro;
            $logLevel  = $TraceInfo::kTraceLogLevel;
         }
         elsif( $line =~ /\bUART_BUFFER_LOG_(\d+)\b/)
         {
            $numArgs   = $1;
            $macroType = $TraceInfo::kBufLogBufferMacro;
            $logLevel  = $TraceInfo::kTraceLogLevel;
         }
         elsif( $line =~ /\bUART_DAL_MSG_LOG\b/)
         {
            $numArgs   = 0;
            $macroType = $TraceInfo::kBufLogDalMsgMacro;
            $logLevel  = $TraceInfo::kTraceLogLevel;
         }
         elsif( $line =~ /\bUART_DAL_MSG_LOG_(\d+)\b/)
         {
            $numArgs   = $1;
            $macroType = $TraceInfo::kBufLogDalMsgMacro;
            $logLevel  = $TraceInfo::kTraceLogLevel;
         }

         if( !defined( $macroType ))
         {
            printf( STDERR "uartlog:  %s, %d: unrecognized UART_LOG macro.\n", $filePathName, $i + 1 );
         }
         else
         {
            #  Extract the format string.  This is not very bullet proof parsing.
            #  But it's good enough for now.
            
            my $format = $line;

            if( $macroType == $TraceInfo::kBufLogMacro )
            {
               $format =~ s/^UART_LOG//;
            }
            elsif( $macroType == $TraceInfo::kBufLogBufferMacro )
            {
               $format =~ s/^UART_BUFFER_LOG//;
            }
            elsif( $macroType == $TraceInfo::kBufLogDalMsgMacro )
            {
               $format =~ s/^UART_DAL_MSG_LOG//;
            }

            $format =~ s/_[0-9]+//;
            $format =~ s/[^"]*"//;
            $format =~ s/".*//;


            #  Extract the log level.

            if( $line =~ /\bTRACE\b/)
            {
               $logLevel = $TraceInfo::kTraceLogLevel;
            }
            elsif( $line =~ /\bLOW\b/)
            {
               $logLevel = $TraceInfo::kLowLogLevel;
            }
            elsif( $line =~ /\bMED\b/)
            {
               $logLevel = $TraceInfo::kMedLogLevel;
            }
            elsif( $line =~ /\bHIGH\b/)
            {
               $logLevel = $TraceInfo::kHighLogLevel;
            }
            elsif( $line =~ /\bERROR\b/)
            {
               $logLevel = $TraceInfo::kErrorLogLevel;
            }
            elsif( $line =~ /\bFATAL\b/)
            {
               $logLevel = $TraceInfo::kFatalLogLevel;
            }

            my $traceInfo = TraceInfo->create( fileName  => $fileName,
                                               macroType => $macroType,
                                               logLevel  => $logLevel,
                                               numArgs   => $numArgs,
                                               format    => $format );

            if( $TraceLog::debug )
            {
               $traceInfo->dump();
            }

            $self->{traceInfo}{$fileId}{$i+1} = $traceInfo;
         }
      }
   }
}

#-----------------------------------------------------------
#  TraceLog::getDirTraceInfo
#-----------------------------------------------------------

sub getDirTraceInfo
{
   my ( $self, %args ) = @_;


   my $dir = $args{dir};

   local *DIR;
   if( !opendir( DIR, $dir ))
   {
      return;
   }

   if( $TraceLog::debug )
   {
      print("$dir\n");
   }

   my @allFiles = readdir( DIR );
   closedir( DIR );

   for( my $i = 0; $i <= $#allFiles; $i++ )
   {
      my $fileName     = $allFiles[$i];
      my $filePathName = $dir . "/" . $fileName;

      #  Recurse.  Process subdirectories.

      if( -d $filePathName )
      {
         #  Skip current directory and parent directory. 

         if(( $fileName eq "." ) || ( $fileName eq ".."))
         {
            next;
         }

         $self->getDirTraceInfo( dir => $filePathName );
      }

      #  Process files in this directory.

      if(( $fileName =~ /\.c$/) || 
         ( $fileName =~ /\.cc$/) ||
         ( $fileName =~ /\.cpp$/))
      {
         $self->getFileTraceInfo( filePathName => $filePathName );
      }
   }
}

#-----------------------------------------------------------
#  TraceLog::getAllTraceInfo
#-----------------------------------------------------------

sub getAllTraceInfo
{
   my ( $self, %args ) = @_;


   my @sourceDirs = @{$self->{sourceDirs}};
   for( my $i = 0; $i <= $#sourceDirs; $i++ )
   {
      $self->getDirTraceInfo( dir => $sourceDirs[$i] );
   }
}

#-----------------------------------------------------------
#  TraceLog::getNextTraceRecord
#-----------------------------------------------------------

sub getNextTraceRecord
{
   my ( $self, %args ) = @_;



   my @data = @{$args{data}};
   my $ind  = $args{ind};

   #  Need at least 5 dwords for a record:  
   #
   #    1 - magic
   #    1 - file id, line number
   #    1 - lsw
   #    1 - msw
   #    1 - seq, numArgs, numBytes
   #
   #  Think of the case where data is 5 entries, 
   #  $#data = 4.  ind = 0 is OK [0-5), but if 
   #  ind = 1,2,3,4 there's not enough data.

   if(($ind + 5) > ($#data + 1))
   {
      return(-1);
   }

   #  We can read in the required first 5 dwords.  Do it.

   my $magic = $data[$ind+0];
   if( !(( $magic == $TraceLog::kMagic1 ) || ( $magic == $TraceLog::kMagic2 )))
   {
      return(-1);
   }

   my $data1      = $data[$ind+1];
   my $fileId     = $data1 & 0xffff;  # bits [0,16).
   my $lineNumber = $data1 >> 16;     # bits [16,32).

   #  timestamp is now 64 bits.

   my $lsw = $data[$ind+2];
   my $msw = $data[$ind+3];

   my $data4        = $data[$ind+4];
   my $seq = $data4 & 0xff;             # bits [0,8).
   my $numArgs      = ($data4 >> 8) & 0xff;      # bits [8,16).
   my $numBytes     = ($data4 >> 16) & 0xffff;   # bits [16,32).

   #  Now that we know the numArgs and numBytes, see if there's 
   #  enough data to get the full message.

   my $size = 5 * 4;            # for the first 5 required dwords.
   $size += (4 * $numArgs);     # for the args.
   $size += $numBytes;          # for the data.
   $size = ($size + 3) & ~0x3;  # round to 4 byte (32 bit) boundary.

   my $numDwords = $size >> 2;
   if(($ind + $numDwords) > ($#data + 1))
   {
      return(-1);
   }

   my $retval = $ind + $numDwords;

   #  Fill the args array.

   my @args;
   my $i;
   $ind += 5;
   for( $i = 0; $i < $numArgs; $i++ )
   {
      $args[$i] = $data[$ind++];
   }

   #  Fill the data from the buffer.

   $numDwords = ($numBytes + 3) >> 2;  
   my @buffer;
   for( $i = 0; $i < $numDwords; $i++ )
   {
      my $dword = $data[$ind++];
      for( my $j = 0; ( $j < 4 ) && ( $numBytes > 0 ); $j++, $numBytes-- )
      {
         push( @buffer, $dword & 0xff );
         $dword >>= 8;
      }
   }

   my $timestamp = Timestamp->create( lsw => $lsw,
                                      msw => $msw,
                                      seq => $seq );

   my $traceRecord= TraceRecord->create( fileId     => $fileId,
                                         lineNumber => $lineNumber,
                                         magic      => $magic,
                                         timestamp  => $timestamp,
                                         args       => \@args,
                                         buffer     => \@buffer );

   push( @{$self->{traceRecords}}, $traceRecord );

   if( $TraceLog::debug )
   {
      $traceRecord->dump();
   }

   return( $retval );
}

#-----------------------------------------------------------
#  TraceLog::byTimestamp
#-----------------------------------------------------------

sub byTimestamp
{
   my $t1 = $a->{timestamp};
   my $t2 = $b->{timestamp};

   return( $t1->compareTo( other => $t2 ));
}

#-----------------------------------------------------------
#  TraceLog::parseTraceRecords
#-----------------------------------------------------------

sub parseTraceRecords
{
   my ( $self, %args ) = @_;


   my @data = @{$args{data}};

   #  Convert the array of data into an array of TraceRecords.
   #
   #  The array wraps around.  If it fills up the buffer, the last 
   #  record written may end in the middle of the record that's 
   #  overwritten.  If so, we need to find the start of the 
   #  next record by looking for the magic number at the start of
   #  each record.  
   #
   #  Let's always look for the start of a record by looking for the 
   #  the magic number, process the record, repeat.

   my $ind = 0;
   while( $ind <= $#data )
   {
      while(( $ind <= $#data ) && !(( $data[$ind] == $TraceLog::kMagic1 ) || ( $data[$ind] == $TraceLog::kMagic2 )))
      {
         $ind++;
      }

      if( $ind <= $#data )
      {
         $ind = $self->getNextTraceRecord( data => \@data,
                                           ind  => $ind );

         if( $ind < 0 )
         {
            last;
         }
      }
   }

   #  Sort the trace records by timestamp.

   my @temp = @{$self->{traceRecords}};
   @temp = sort byTimestamp @temp;

   #  Assign back.
   
   $self->{traceRecords} = \@temp;
}

#-----------------------------------------------------------
#  TraceLog::getAllTraceRecords1
#-----------------------------------------------------------

sub getAllTraceRecords1
{
   my ( $self, %args ) = @_;


   #  Read everything in as an array of integers.

   local *DUMP_FILE;
   if( !open( DUMP_FILE, "<$self->{dumpFile}"))
   {
      printf( STDERR "\nuartlog:  cannot read %s\n", $self->{dumpFile} );
      exit(1);
   }

   my $firstAddress;
   my $firstAddressSet = 0;
   my @data;
   my $line;
   while( $line = <DUMP_FILE> )
   {
      chomp( $line );

      if( !( $line =~ /^ SD:[0-9A-F]+/))
      {
         next;
      }

      my $address = hex( substr( $line, 4, 8 ));
      if( !defined( $firstAddress ))
      {
         $firstAddress = $address;
      }

      # xxxxx
      # B::data.dump_v.address(_s_uartTraceInfo_)--(v.address(_s_uartTraceInfo_)+v.sizeof(_s_uartTraceInfo_))
      # ___address__|________0________4________8________C________0________4 [skipping] _0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
      # SD:081B92C0|         >081B9398 0000FF30 0019F8A8 FA398030 0019F8A8             ....0.......0.9.......:...4..gE#......;...4..gE#.xV4............
      # SD:081B9300| FA398030 0019F8AE FA3A98F2 1234FAB1 23456789                      .......:...4..gE#......;...4..gE#.xV4............0.9.......:.
      #
      #012345678901234567890123456789
      #          1         2

      if( length( $line ) < 15 )
      {
         next;
      }

      #  Extract the data fields.  Note that the beginning of the buffer may not
      #  be in the first column of data.

      my $index = 0;
      $line = substr( $line, 14, 143 );
      while( length( $line ) >= 8 )
      {
         my $dword = substr( $line, 0, 8 );

         #  Not a hex dword?  Skip it.
         if( !( $dword =~ /^[0-9,a-f,A-F]+$/))
         {
            if( length( $line ) < 10 )
            {
               last;
            }
            else
            {
               $line = substr( $line, 9 ); 
            }
            $index++;
            next;
         }

         my $value = hex( $dword );

         #  Check if we've hit the sentinel.
         if( $value == $TraceLog::kSentinel )
         {
            push( @data, $value );
            last;
         }

         #  Got something.
         if( !$firstAddressSet )
         {
            $firstAddress += (4 * $index);
            $firstAddressSet = 1;
         }
         push( @data, $value );

         #  Next iteration.
         if( length( $line ) < 10 )
         {
            last;
         }
         else
         {
            $line = substr( $line, 9 ); 
            $index++;
         }
      }
   }
   close( DUMP_FILE );

   return( @data );
}

#-----------------------------------------------------------
#  TraceLog::getAllTraceRecords2
#-----------------------------------------------------------

sub getAllTraceRecords2
{
   my ( $self, %args ) = @_;


   #  Read everything in as an array of integers.

   local *DUMP_FILE;
   if( !open( DUMP_FILE, "<$self->{dumpFile}"))
   {
      printf( STDERR "\nuartlog:  cannot read %s\n", $self->{dumpFile} );
      exit(1);
   }

   my $traceBufferFound = 0;
   my @data;
   my $line;
   my $dword = 0;
   my $byte  = 0;
   my $lineNumber = 0;

   while( $line = <DUMP_FILE> )
   {
      $lineNumber++;
      if( $line =~ /traceBuffer =/)
      {
         $traceBufferFound = 1;
         next;
      }

      if( !$traceBufferFound )
      {
         next;
      }

      my $elemValue = 0;

      if( $line =~ /^\s+0[Xx]([0-9a-fA-F]+)/)
      {
         #  If elem is displayed as hex.
         $elemValue = hex( $1 );
      }
      elsif( $line =~ /^\s+([\-]?[0-9]+)/)
      {
         #  If elem is displayed as decimal.
         $elemValue = $1;
      }
      else
      {
         #  Something's wrong.
         printf( STDERR "\nuartlog:  error parsing dump file %s at line %d\n", $self->{dumpFile}, $lineNumber );
         exit(1);
      }

      #  Get the value.  It's a byte, but we want to create an array of 
      #  dwords.  Put the byte in the appropriate place within the dword.

      $elemValue &= 0xFF;
      $elemValue <<= ($byte * 8);
      $dword |= $elemValue;
      $byte++;

      #  If we've filled up the dword, put it in the data array and
      #  reset for the next dword.

      if( $byte == 4 )
      {
         push( @data, $dword );
         $byte  = 0;
         $dword = 0;
      }

      #  If line has closing ) for array (i.e. it's the last element
      #  of the array) we're done.

      if( $line =~ /\)/)
      {
         last;
      }
   }
   close( DUMP_FILE );

   #  Sanity check.  There should be an integral number of dwords. 

   if( $byte != 0 )
   {
      printf( STDERR "\nuartlog:  error parsing dump file %s at line %d\n", $self->{dumpFile}, $lineNumber );
      exit(1);
   }

   return( @data );
}

#-----------------------------------------------------------
#  TraceLog::getAllTraceRecords
#-----------------------------------------------------------

sub getAllTraceRecords
{
   my ( $self, %args ) = @_;


   my @data;

   #  See if the dump file is in format 1 -- output from the data.dump
   #  command.

   if( $#data == -1 )
   {
      @data = $self->getAllTraceRecords1();
   }

   #  See if the dump file is in format 2 -- output from the data.view
   #  command.

   if( $#data == -1 )
   {
      @data = $self->getAllTraceRecords2();
   }

   #  Dump file doesn't match any known format.

   if( $#data == -1 )
   {
      printf( STDERR "\nuartlog:  unrecognized format for dump file %s.\n", $self->{dumpFile} );
      exit(1);
   }

   $self->parseTraceRecords( data => \@data );
}

#-----------------------------------------------------------
#  TraceLog::writeLog
#-----------------------------------------------------------

sub writeLog
{
   my ( $self, %args ) = @_;


   print("\n");

   if( $self->{rawTimestampFormat} == $TraceLog::kRawTimestampNone )
   {
      print("Time             Level   File                     Line   Message\n");
      print("----------------------------------------------------------------------------------------------------------------\n");
   }
   elsif( $self->{rawTimestampFormat} == $TraceLog::kRawTimestampHex )
   {
      print("Time             Timestamp          Level   File                     Line   Message\n");
      print("--------------------------------------------------------------------------------------------------------------------------\n");
   }
   elsif( $self->{rawTimestampFormat} == $TraceLog::kRawTimestampDecimal )
   {
      print("Time             Timestamp               Level   File                     Line   Message\n");
      print("--------------------------------------------------------------------------------------------------------------------------\n");
   }

   my @traceRecords = @{$self->{traceRecords}};
   for( my $i = 0; $i <= $#traceRecords; $i++ )
   {
      my $traceRecord = $traceRecords[$i];

      my $fileId      = $traceRecord->{fileId};
      my $lineNumber  = $traceRecord->{lineNumber};

      if( $TraceLog::debug )
      {
         $traceRecord->dump();
      }

      my $traceInfo = $self->{traceInfo}{$fileId}{$lineNumber};

      if( !defined( $traceInfo ))
      {
         #  Got a TraceRecord that doesn't match up with 
         #  a TraceInfo.  There could be a number of reasons for
         #  this:
         #
         #    1) The code that produced the log is not the same
         #       as the code used when running uartlog.  
         #
         #       Solution:  Can't do anything about this.  If 
         #       working with a customer have them run the uartlog.pl
         #       script.
         #
         #    2) The UART_LOG macro needs to be on a single line.
         #       If not, the line number will be off and won't
         #       match up.  
         #
         #       Solution:  Fix the source code.
         #
         #    3) The directory that includes the file with the 
         #       UART_LOG macro needs to be included on the command
         #       line.
         #
         #       Solution:  Run the command again with the
         #       correct command line arguments.
         #
         #  Create traceInfo with as much information as we can
         #  figure out.

         my $fileName = "----------------";

         my $filePathName = $self->{fileIdMap}{$fileId};
         if( defined( $filePathName ))
         {
            $fileName = $filePathName;
            $fileName =~ s/.*\///;   # strip off path.
         }

         $traceInfo = TraceInfo->create( fileName  => $fileName,
                                         macroType => $TraceInfo::kBufLogMacro,
                                         logLevel  => $TraceInfo::kUnknownLogLevel,
                                         numArgs   => 0,
                                         format    => "XXXXXXXXXXXXXXXXXXXXXXXXXXX");
      }

      $traceRecord->writeLog( traceInfo          => $traceInfo,
                              rawTimestampFormat => $self->{rawTimestampFormat} );
   }
}

#-----------------------------------------------------------
#  TraceLog::processDump
#-----------------------------------------------------------

sub processDump
{
   my ( $self, %args ) = @_;

   #  Get all UART_LOG lines from all source files.
   $self->getAllTraceInfo();

   #  Convert dump file into trace records.
   $self->getAllTraceRecords();

   #  Match up trace records with trace info.  Output log.
   $self->writeLog();
}

#-----------------------------------------------------------
#  main
#-----------------------------------------------------------

package main;
no strict;

#-----------------------------------------------------------
#  Do it.
#-----------------------------------------------------------

$traceLog = TraceLog->createFromCommandLine();

$traceLog->processDump();

