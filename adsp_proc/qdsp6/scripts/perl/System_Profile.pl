require "./qdsp6/scripts/perl/util.pl";
$cfg_name = $ARGV[0];
$product = $ARGV[1];
$custid = $ARGV[2];

$cfg= "$cfg_name";
&Parse_config($cfg);



system("cp -f ./obj/qdsp6v4_ReleaseG/bootimg.pbn.map ./map.txt");
&CreateDIR_4FinalOutput;
&run_profile($cfg);

sub run_profile {

      print "\ncreating image_database.txt\n";
      system("perl $ABS_DIR_SPT_PERL/libdatabase.pl 0 $cfg ");   
      system("perl $ABS_DIR_SPT_PERL/show_memory_qurt.pl ./obj/qdsp6v4_ReleaseG/bootimg.pbn");
 
   print "\nCleaning temporary files....\n";
   &run_cleaning;
   
}

sub CreateDIR_4FinalOutput {
   system("mkdir $ABS_DIR_SPT_OUTPUT -p");
}

sub run_cleaning {
  
   system("rm -rf $WORKING_DIRECTORY");

   
}

