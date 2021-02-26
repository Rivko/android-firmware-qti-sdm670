#!/bin/perl -w

use IO::File;

$targ_xml = "> ./q6_qurt_config.xml";
open targ_xml or die "Cannot open XML file $targ_xml\n";

$INP_XML = $ARGV[0];
$MEM_DEFNS_H = $ARGV[1];

open (INP_XML) or die "Can't open the file!";
@fileinput = <INP_XML>;

# find the length of the file
$length = @fileinput;

$index = 0;

while ($length) {

   if ( $fileinput[$index] =~ /insert memory sections here/ ) {
      open (REF_FILE, 'q6_memory_util.txt');
      while (<REF_FILE>) {
         chomp;
         $NAME = 0;
         $SIZE = 0;

         $NAME1 = 0;
         $SIZE1 = 0;

         # open reference file to create xml tag
         ($V_memsection, $V_start, $V_size) = split(" ");

         if ( $V_memsection eq "DRIVER_POOL") {

            open (INP_FILE, 'q6_memory_defns.h');
            # open q6_memory_defns.h and search for the start value and size value

            while (<INP_FILE>) {
               my $V_tag = "";
               chomp;
               #open and read q6_memory_defns 
               ($V_tag, $V_name, $V_value) = split(" ");

               # look for pattern with #define <name> <value>; ignore rest 
               if ( $V_tag =~ /define/ ) {
                  if ( $V_name eq $V_start ) {
                     $NAME = $V_value;
                     } elsif ( $V_name eq $V_size )
                     {
                        $SIZE = $V_value;
                     }
                     if ( $NAME & $SIZE ) {
                        Last;
                     }
               }
            }
            close(INP_FILE);

            open (INP_FILE, 'q6_memory_defns.h');
            # open q6_memory_defns.h and search for the start value and size value

            while (<INP_FILE>) {
               my $V_tag = "";
               chomp;
               #open and read q6_memory_defns 
               ($V_tag, $V_name, $V_value) = split(" ");

               # look for pattern with #define <name> <value>; ignore rest 
               if ( $V_tag =~ /define/ ) {
                  if ( $V_name eq "DRIVER_POOL_BASE_1" ) {
                     $NAME1 = $V_value;
                     } elsif ( $V_name eq "DRIVER_POOL_SIZE_1" )
                     {
                        $SIZE1 = $V_value;
                     }
                     if ( $NAME1 & $SIZE1 ) {
                        Last;
                     }
               }
            }
            close(INP_FILE);


         } else 
         {
            open (INP_FILE, 'q6_memory_defns.h');
            # open q6_memory_defns.h and search for the start value and size value

            while (<INP_FILE>) {
               my $V_tag = "";
               chomp;
               #open and read q6_memory_defns 
               ($V_tag, $V_name, $V_value) = split(" ");

               # look for pattern with #define <name> <value>; ignore rest 
               if ( $V_tag =~ /define/ ) {
                  if ( $V_name eq $V_start ) {
                     $NAME = $V_value;
                     } elsif ( $V_name eq $V_size )
                     {
                        $SIZE = $V_value;
                     }
                     if ( $NAME & $SIZE ) {
                        Last;
                     }
               }
            }
            close(INP_FILE);
         }

         if ( $V_memsection eq "DRIVER_POOL") {
            print targ_xml "<physical_pool name=\"$V_memsection\"><region base=\"$NAME\" size=\"$SIZE\"></region><region base=\"$NAME1\" size=\"$SIZE1\"></region></physical_pool>\n";
         } else 
         {
               print targ_xml "<physical_pool name=\"$V_memsection\"><region base=\"$NAME\" size=\"$SIZE\"></region></physical_pool>\n";
         }

      }
   } else 
   {
      print targ_xml $fileinput[$index];
   }

   $index = $index + 1;
   $length = $length - 1;
}

close(REF_FILE);

exit;

