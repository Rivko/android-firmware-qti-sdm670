use Win32::OLE;          # base functionality
use Win32::OLE::Variant; # support for OLE data types

# ###############################################################
# FUNCTION:  SendDmIcdPacketEx ( VARIANT, timeout );
#
# PARAMETERS:VARIANT byte array containing request (size included)
#            timeout is the number of milliseconds to wait for a rsp
#
# RETURN:    VARIANT byte array containing response (size included)
# ###############################################################

sub issue_command(@); # Forward declaration

# ###############################################################
#                        MAIN ROUTINE
#
#  First we will open the command input file
#  Next, we will ensure the QXDM application is running
#     NOTE: (script will start QXDM if it is not running).
#  Finally, we will issue each command in the file and print
#           both the cmd request hex and response hex to STDOUT
#
#  NOTE: Command input has the following syntax:
#        # <comment> -- if first char is a pound, line is ignored
#        ! <comment> -- if first char is an exclaimation mark,
#                       line is printed to stdout
#        { COMMAND } -- command is wrapped in curly braces and
#                       has the following form:
#        { 1, 0x2, 003 } -- list of bytes, decimal/hex/octal
#                           representations are allowed
# ###############################################################


sub usage
{
  print STDERR "Usage:   $0 <COMMAND_FILE>\n";
  print STDERR "Example: $0 command.inp\n";
  exit;
}

#-------------------------------------------------------------------
# Open & Parse the "command.inp" file for the commands
#-------------------------------------------------------------------
if ($#ARGV < 0) {         #No command line arguments specified
    usage();
}

@ARGV = qw(.) unless @ARGV;
my $timeout = 0;   #default value

if ($#ARGV == 1) {         #Time out is specified
    $timeout = $ARGV[1];
}

$CMD = shift;
unless (open CMD) {
   print STDERR "$CMD does not exist\n";
   usage();
}

my $qxdm_app = new Win32::OLE 'QXDM.Application';
if(!$qxdm_app)
{
   print "ERROR: Unable to invoke the QXDM application.\n";

   die;
}
#sleep(10);
$qxdm_app->SetLoggingOn( "0480000000000000000000000000000000000000000000000000000000000000000000000000000000DEADBEAF" );
printf( "\n\nscript started...\n" );
while (<CMD>) {
   if(/^#/) {               #Line is ignored
      next;
   } elsif(/^!/) {          # Line is printed
         print $_;
         next;
   } elsif ( /\{(.*)\}/ ) { # Line is issued
      $line = $1;
      $line =~ s/\s//g; # Strip out whitespaces
      #print "\n\n$line\n";
      @bytes = split/,/,$line;
      #for($i=0;$i<=$#bytes;++$i){print "Byte $i: $bytes[$i]\n";}
      issue_command(@bytes);
   }
}

sleep($timeout);

my $file = "log_file_name".time.".dlf";
$qxdm_app->SetLoggingOff( $file );

printf( "\n\n...script1 complete\n" );

$qxdm_app = NULL;

sub issue_command(@){
   my $timeout_ms     = 100000000; # One second timeout per command
   my $req_len        = $#_+1;
   # ####################################################################################
   # Build SAFEARRAY
   # ####################################################################################
   my $req_pkt = Win32::OLE::Variant->new( VT_UI1|VT_ARRAY , $req_len );
   for( $i=0; $i<$req_len; ++$i ) {
      $_[$i] = oct $_[$i] if $_[$i] =~ /^0/;
      $req_pkt->Put($i,chr($_[$i]));
   }

   # ####################################################################################
   # Let's Print the request in hex first
   # ####################################################################################
   #print "Req Packet [$req_len bytes] ";
   for( $i=0; $i < $req_len; ++$i ) {
      if( 0 == $i ) {
         printf("CMD_%-3u Req Packet [%3u bytes]:", $req_pkt->Get($i), $req_len);
      }
      printf(" %2.2X", $req_pkt->Get($i));
   }
   print "\n";

   # ####################################################################################
   # Issue the request
   # ####################################################################################
   $rsp_pkt = $qxdm_app->SendDmIcdPacketEx($req_pkt, $timeout_ms);

   # ####################################################################################
   # Let's Print the response in hex
   # ####################################################################################
   $rsp_len = length($rsp_pkt);

   # ####################################################################################
   # Make sure the packets actually were sent
   # ####################################################################################

   if( 0 == $rsp_len ){
   print <<failure_print;

================================================================================
FAILURE. QXDM is running but is QXDM connected to the Device?
================================================================================
failure_print
   exit;
   }


   #print "Rsp Packet [$rsp_len bytes] ";
   for( $i=0; $i < $rsp_len; ++$i ) {
      if( 0 == $i ) {
         printf("CMD_%-3u Rsp Packet [%3u bytes]:", unpack("x$i C",$rsp_pkt), $rsp_len);
       }
       printf(" %2.2X", unpack("x$i C",$rsp_pkt));
   }
   print "\n";

   # ####################################################################################
   # For all our packets, the second dword indicates the status of the request.
   # Inform the user about the success or failure of the request, based on the
   # contents of these bytes.
   # ####################################################################################
   my $status = unpack ("I", substr ($rsp_pkt, 4, 4));
   if ($status == 0) {
   print <<success_print;

================================================================================
SUCCESS. Diag command succeeded.
================================================================================
success_print
   }
   else {
   print <<failure_print;

================================================================================
FAILURE. Diag command failed.
================================================================================
failure_print
   }
}

