#!/usr/bin/perl
########################
# Run the CASS Client Reference Application.
########################
#
use strict;
use Env;
# We use this package to canonicalize the handling of file / directory names
use File::Spec;
# We use this package to find JAR files
use File::Find;
# We find out where this script lives.
use FindBin qw($Bin);
# And adjust perl's internal pathing to include it.
use lib "$Bin";
use Getopt::Long;

########################
# Default settings.  Alter as appropriate.
########################
#
# Set this to the location relative to 'bin'.
# Examples:
#  If the JAR files are in 'bin/java' then set this to ''.
#  If the JAR files are in 'lib', then set this to '../lib'
my $DEFAULT_APP_LIB = '';

########################
# Set the Java environment.
########################
my $JAVA_PRG;
# Is this windows?
if ( $^O =~ /MSWin/ ) {
   $JAVA_PRG = 'java.exe';
}
else {  # *nix variant
   $JAVA_PRG = 'java';
}

my $JAVA_EXE;
#
# If user has a JAVA_HOME set, use that vs. the default.
#
if ( defined $ENV{'JAVA_HOME'} ) {
   $JAVA_EXE = File::Spec->catfile($ENV{'JAVA_HOME'}, 'bin', $JAVA_PRG);
}

# Test we can find Java
if ( ! -e $JAVA_EXE ) {
   print STDERR "
The java executable cannot be found!  Set JAVA_HOME.

(JAVA_HOME path: '$JAVA_EXE')
";
   exit -1;
}

########################
# Initialize
########################
my $getVersion;
my $beVerbose;
my $spPath;
my @PROPVALUES;

sub init {
   my $prgNm;
   (undef, undef, $prgNm) = File::Spec->splitpath($0);
   my $usage =  "Usage: $prgNm [-version] [-v] <Signing Package Path> [[<app.properties Key> <app.properties value>] ...]
";
   my $where = "where:
   [-version]            (optional) Print the version of Java being used.
   [-v]                  (optional) Produce more verbose output.
   <Signing Package Path> Mandatory path to the signing package being submitted.
   [<app.properties Key>  <app.properties Value>]  (optional) One or more pairs of Key/Value properties to set.
";

   my $result;
   $result = GetOptions(
       '-version' => \$getVersion
      ,'-v' => \$beVerbose
   );

   # Any options left?  If so, show usage and exit.
   if ( $#ARGV < 0 && ! defined $getVersion ) {
      print $usage;
      print $where;
      exit -1;
   }

   my $badArgs;         # House any error conditions we discover.

   # No check on signing package since path can be relative to client ref-appropriate
   $spPath = File::Spec->canonpath($ARGV[0]);
   shift @ARGV;

   # Next, process the remaining args as key/value pairs.
   if ( scalar(@ARGV) > 0 ) {
      # Check we have enough argument pairs
      if ( (scalar(@ARGV) % 2) != 0 ) {
         $badArgs .= "Invalid number of key/value pairs!\n";
      }
      else {
         for (my $inx = 0; $inx < scalar(@ARGV); $inx+=2) {
            push @PROPVALUES, '-D' . $ARGV[$inx] . '=' . $ARGV[$inx+1];
         }
      }
   }

   if ( defined $badArgs ) {
      print $badArgs;
      exit -2;
   }
}

########################
my $CLASSPATH;
my $lib; $lib = File::Spec->catdir($Bin, $DEFAULT_APP_LIB);
########################
# Main
########################

# Process the command line args
init();


# Build the jar path.
my $JARPATH = File::Spec->catfile($lib, 'cass-client-refapp');

# Build our command to execute.
my @CMD;
push @CMD, $JAVA_EXE;

# Add any key/value properties
foreach (@PROPVALUES) {
   push @CMD, $_;
}

push @CMD, '-jar';
push @CMD, $JARPATH;


if ( defined $getVersion ) {
   push @CMD, '-version';
}

push @CMD, $spPath;
# Be verbose?
if ( defined $beVerbose ) {
      push @CMD, '-v';
}
system(@CMD);