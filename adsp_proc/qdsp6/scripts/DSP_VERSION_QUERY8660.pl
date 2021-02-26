#!/usr/bin/perl -w
# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
#
#                    DSP_VERSION_QUERY
#
# GENERAL DESCRIPTION
#   Given a dsp image or dump, if the build is an official DSP release, will return the reference build used to generate the DSP.
#
#   Usage: #usage :Perl DSP_VERSION_QUERY.pl <DSP image>
#
#   Limitations: User must have access to reference build location.  Tested on Perl 5.8.8
#
#
#  Copyright(c) 2009 by QUALCOMM, Incorporated. All Rights Reserved.
# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====

# ===========================================================================
#
#                       EDIT HISTORY FOR FILE
#
#   This section contains comments describing changes made to this file.
#   Notice that changes are listed in reverse chronological order.
#
#  when       who     what, where, why
# --------   ---     ---------------------------------------------
#  10/20/10   kevwang  First version
#
# ===========================================================================
use strict;
my $NumArgs=$#ARGV;
sub usage
{
print "Usage: $0 <DSP image/dump>\n
Eg: $0 qdsp6/obj/qdsp6v2_ReleaseG/dsp1.mbn O:\users\m8660_dsp_ref_builds \n";
exit;
}



#Declare/Set variables
my $Current_Timestamp;
my $Temp_Timestamp;
my $ref_dir_root;
my $item;
my $drive;
my @list;
my $os = $^O;


if (@ARGV < 1) {
   &usage;
}

#Main Function
#Find current timestamp
print "Running in: $os\n";

$Current_Timestamp = `qdsp6-strings $ARGV[0] | grep "Q6_BUILD"`;
print "Current DSP Image timestamp is $Current_Timestamp \n";

if ($os eq "linux") {
   $ref_dir_root ="/prj/vocoder/appdsp5/users/m8660_dsp_ref_builds";
}
else {
   if (@ARGV == 2) {
      $ref_dir_root=$ARGV[1];
   }
   else {
      print "Windows:  Please provide drive letter Mapped path to \\\\arawn\\vocoder_appdsp5\\users\\m8660_dsp_ref_builds.\n";
      print "  i.e perl DSP_VERSION_QUERY.pl dsp1.mbn O:\\users\\m8660_dsp_ref_builds\n";
      exit;
   }
}

chdir($ref_dir_root) or die "Can't chdir to $ref_dir_root $!";

@list = <*>;
print "Looking for match... \n";
foreach $item (@list) {
   if (-d $item && -f "$item/obj/qdsp6v3_ReleaseG/dsp1.mbn") {
      $Temp_Timestamp = `qdsp6-strings $item/obj/qdsp6v3_ReleaseG/dsp1.mbn | grep "Q6_BUILD"`;
      if ($Current_Timestamp =~ m/^$Temp_Timestamp/i && $Temp_Timestamp =~ m/^Q6_BUILD_TS/i ) {
         print "Found Timestamp Match At Reference Build $ref_dir_root/$item\n";
         exit;
      }
   }
}

print "Reference Build Match not found for current DSP\n";

