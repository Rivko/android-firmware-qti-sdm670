require "./qdsp6/scripts/perl/util.pl";

$EnableLibDecomposition = $ARGV[0];
$cfg_name = $ARGV[1];

&Parse_config($cfg_name);

if ($EnableLibDecomposition) {
   print "Remove old object folder if exists\n";
   print "Enable decomposition of libraries\n";
   system("rm -R -f tempo\n");  
}

#1..read library code component affiliation 
%CodeComponent=0;
&ReadLibNameInput;

#2. Get all linked objects and libraries from map file.
%libobjname=0;
%m_virt_addr =0;
%m_section = 0;
%m_size = 0;
%m_libname = 0;
%m_objname = 0;

&ParseMapFile;
#3.....Get information about what is in the image

%varname=0;
%phys_addr = 0;
%virt_addr = 0;
%size = 0;
%libname = 0;
%objname = 0;
%onechar =0;
%section = 0;
%codecomp = 0;

&getSymbolAndSize;
&PrintObjInfo;
&getHeaderInfoFromImage;
&calPhyAddr;
&CreateImageDataBase;
&CreateObjList;
&CreateImageSizeOutput;
&WriteNewLibNameInput;
&CleanImageTempFile;


sub ReadLibNameInput {

   $library_name_input = "$ABS_DIR_SPT_INPUT/library_name_input.csv";
   print " \nReading lib\=\>region definition  file: $library_name_input\n";
   open library_name_input or die "Cannot open $library_name_input";

   while (<library_name_input>) {
         $_ =~ s/\t//g;
         $_ =~ s/ //g;
         $_ =~ s/\r//;
         $_ =~ s/\n//;
      @line = split(',',$_);
      $CodeComponent{$line[0]} = lc($line[1]);
      #print "code comp:$CodeComponent{$line[0]}\n";
   }
   close $library_name_input;
}

sub WriteNewLibNameInput {

   $library_name_input = "> $ABS_DIR_SPT_INPUT/library_name_input.csv";
   print " \nUpdating region definition file $library_name_input\n";
   open library_name_input or die "Cannot open $library_name_input";

   @m_keys_add = keys %CodeComponent;
   @m_sorted_keys = sort{ uc($CodeComponent{$a}) cmp uc($CodeComponent{$b}) }keys %CodeComponent;
   #@m_sorted_keys = sort{ %CodeComponent } @m_keys_add;
   #$m_keyCnt=scalar(@m_keys_add);

   foreach (@m_sorted_keys) {
      print library_name_input "$_,$CodeComponent{$_}\n";
   }
   close $library_name_input;
}
sub ParseMapFile{

   $map_file = "./map.txt";
   open map_file or die "Cannot open map file $map_file\n";
   
   system("mkdir tempo -p");
   
   $stage = 0;
   $flag_1 = 0;
   $lineNum=0;
   $firstConut=0;
   $thirdCount=0;
   $secondCount=0;
   print "\ Parsing map file:$map_file\n";
   while (<map_file>) {
      $lineNum++;
      @lsp = split(' ',$_);
      $first = substr($_,0,1);
      $second = substr($_,1,1);

      #print "$lineNum: --$_";
      
      if ($lsp[0] eq "COMMON") {
         
         #print "line:$lineNum; lsp[0] = COMMON, changed to .bss\n";
         $lsp[0] = ".bss";
         $second = ".";
      }
      
      if ($stage==0) {    
         if ($lsp[0] eq "Archive") {
            print "    stage:$stage, line:$lineNum,Reading needed object files\n";
            next;
         }
        
         if ($first eq " ") {
            @ll = split ('/',$lsp[0]);
            @lf = split ('\.',$ll[-1]);
           
            if ($lf[-1] ne "o") {
               @llo = split('\)',substr($lsp[1],1));
               $lout= $llo[0];
               
            } else {
               $lout=$ll[-1];
            }         
            $lob = "$lout"."@"."$ll[-1]";
            $libobjname{$lob} = $lob;
            next;
            #print "    ++libobjname{$lob}=$lob\n";
         }    
         if ($lsp[0] eq "Allocating") {
            print "    stage:$stage, line:$lineNum,Done\n";
            $stage = 1;
            next;
         }
         next;
         
      } 
      if ($stage==1) {
         if ($lsp[0] eq "Common") {
            print "    stage:$stage, line:$lineNum,Skipping common section info and Memory Configuration\n";
            next;
         }
         if ($lsp[0] eq "Linker") {
            print "    stage:$stage, line:$lineNum,Done\n";
            $stage=2;
            print "    stage:$stage, line:$lineNum,Parsing Linker Script\n";
            next;
         }
         next;
      }
      if ($stage==2) {
         if ($lsp[0] eq "LOAD") {
            $libfiles{$lsp[1]} = 1;
            if ($EnableLibDecomposition) {         
               @ll = split('/',$lsp[1]);
               @lll = split('\.',$ll[-1]);
               if ($lll[-1] ne "o") {
                  print "qdsp6-ar -xo \.\./\.\./$lsp[1]\n";
                  system("mkdir tempo/$ll[-1] -p");
                  if (substr($lsp[1],0,1) eq "/" ) {
                     system("cd tempo/$ll[-1]\nqdsp6-ar -xo $lsp[1]\n");
                  } else {
                     system("cd tempo/$ll[-1]\nqdsp6-ar -xo \.\./\.\./$lsp[1]\n");
                  }
               } else {
                  print "cp $lsp[1] tempo\n";
                  system("cp $lsp[1] tempo\n");
               }
            }
            next;
         }
         if ($lsp[0] eq '.CODE') {
            
            print "    stage:$stage, line:$lineNum,Done\n";
            $stage=3;
            print "    stage:$stage, line:$lineNum,Parsing Code sections\n";
            next;
         }
         next;
      } 
      if ($stage==3) {
        # print "    first: $first, second: $second, lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n\n ";
         if ($get_info_gcc_except_table) {#exception of tier1
            $get_info_gcc_except_table=0;
            $sectionname="q6.gcc_except_table";
            $section{$sectionname} = $sectionname;
            $section_virt_start{$sectionname} = substr($lsp[0],2);
            $section_size{$sectionname} = substr($lsp[1],2);
            next;
         }
         if ($first eq ".") {#tier 1
            $firstConut++;
            $secondCount=0;
            $thirdCount=0;
            #print "Tier1:$firstConut  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
             #print " $lineNum:firstConut=$firstConut; lsp[0]=$lsp[0],lsp[1]=$lsp[1],lsp[2]=$lsp[2]  \n ";
            if ($lsp[0] eq ".comment") {
               print "    stage:$stage, line:$lineNum,Done\n";
               print " Done parse memory map\n";
               $stage=4;#end parsing
               next;
            }
            if ($lsp[1] ne "" && $lsp[2] ne "") {  
               $sectionname = "q6$lsp[0]";
               $section{$sectionname} = $sectionname;
               $section_virt_start{$sectionname} = substr($lsp[1],2);
               $section_size{$sectionname} = substr($lsp[2],2);  
               # print "sectionName:$sectionname\n";
               next;
            }
            if ($lsp[0] eq ".gcc_except_table") {
               $get_info_gcc_except_table = 1;
               next;
            }
            next;
         }
         if ($first eq " ") {
            if ($second eq ".") {#tier 2
               $secondCount++;
               #print "Tier2:$secondCount  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
               #print "$lineNum  2nd: $secondCount   first: $first, second: $second, lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n\n ";
               if ($lsp[0] eq ".scommon") {
                  $lsp[0]=".sbss";
               }
               if($lsp[0] eq ".bss"){
                #  print "lsp0:$lsp[0]\n";
               }
               
               if ($lsp[1] ne "") {
                   if($lsp[0] eq ".bss"){
                 #     print "lsp1:$lsp[1]\n";
                   }
                  #print "Tier2_e:$secondCount  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
                  $flag_2 = 1;
                  $vaddr = substr($lsp[1],2);
                  $m_virt_addr{$vaddr} = $vaddr;
                  $m_section{$vaddr} = $sectionname;
                  $m_size{$vaddr} = substr($lsp[2],2);

                 
                   @ll = split ('/',$lsp[3]);                                  
                   if ($ll[0] eq "tempo") {                      
                      $m_libname{$vaddr} = $ll[1];
                   } else {
                      $m_libname{$vaddr} = $ll[-1]; 
                   }
                   @llo = split('\)',substr($lsp[4],1));
                   if ($llo[0] eq "") {
                      $llo[0]=$ll[-1];
                   }
                   $m_objname{$vaddr} = $llo[0];
                   $lob = "$m_objname{$vaddr}"."@"."$m_libname{$vaddr}";
                   $libobjname{$lob} = $lob;
                   if($lsp[0] eq ".bss"){
                    # print "sec=$m_section{$vaddr}, vd =$m_virt_addr{$vaddr},  size=$m_size{$vaddr}, objname:$libobjname{$lob} \n";
                  }
                #   print "2libobjname:$libobjname{$lob}\n";
               } else {
                  #print "Tier2_n:$secondCount  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
                  $flag_2=0;
               }
               next;
            } 
            if ($second eq " ") {
             
               if (!$flag_2) {#tier 3
                  $thirdCount++;
                  #print "Tier3_f:$thirdCount  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
                  #print "$lineNum  3rd: $thirdCount   first: $first, second: $second, lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n\n ";
                  $flag_2 = 1;
                  $vaddr = substr($lsp[0],2);
                  if(!$vaddr){
                     print "$lineNum wrong\n";
                  }
                  $m_virt_addr{$vaddr} = $vaddr;
                  $m_section{$vaddr} = $sectionname;
                  $m_size{$vaddr} = substr($lsp[1],2);               
                  @ll = split ('/',$lsp[2]);
                  if ($ll[0] eq "tempo") {                      
                        $m_libname{$vaddr} = $ll[1];
                  } else {
                        $m_libname{$vaddr} = $ll[-1]; 
                  }               
                  @llo = split('\)',substr($lsp[3],1));
                  if ($llo[0] eq "") {
                     $llo[0]=$ll[-1];
                  }
                  $m_objname{$vaddr} = $llo[0];
                  $lob = "$m_objname{$vaddr}"."@"."$m_libname{$vaddr}";
                  $libobjname{$lob} = $lob;
                 # print "3libobjname:$libobjname{$lob}\n";
                 # print "******\n";
               }else{
                 # print "Tier3_nf:$thirdCount  lsp[0]=$lsp[0], lsp[1]=$lsp[1], lsp[2]=$lsp[2]\n ";
               }
               next;
            } 
         }
      }
   }

   @m_keys_add = keys %m_virt_addr;
   @m_sorted_keys = sort{ hex($m_virt_addr{$a}) <=> hex($m_virt_addr{$b}) } @m_keys_add;
   &printParse;
  
}
sub printParse {

   $temp_parse = "> $ABS_DIR_SPT_OUTPUT/temp_parse.txt";
   open temp_parse or die "Cannot open temp parse $temp_parse\n";
   print temp_parse "1. libobjname\n";

   while( my ($k, $v) = each %libobjname ) {
        print temp_parse "key: $k, value: $v.\n";
   }

   print temp_parse "2. sections\n";
   print temp_parse " section_virt  section_size   sectionName \n";
   while( my ($k2, $v2) = each %section ) {
        print temp_parse " $section_virt_start{$k2}  $section_size{$k2}  $section{$k2}\n";
   }

   print temp_parse "\n3. ";
   print temp_parse "key  m_virt_addr,   m_section,    m_size,   m_libname,    m_objname\n";
   foreach (@m_sorted_keys) {
      print temp_parse "==$_  ";
       print temp_parse "$m_virt_addr{$_},   $m_section{$_},   $m_size{$_},    $m_libname{$_},    $m_objname{$_}\n";

   }

}
sub getSymbolAndSize {
   print " Retrieve symbol information from image\(qdsp6-nm\)\n";
   $library_in_image = "$ABS_DIR_SPT_OUTPUT/library_in_image.txt";
   system("qdsp6-nm -S ./obj/qdsp6v4_ReleaseG/bootimg.pbn > $library_in_image");
   open library_in_image or die "Cannot read file : $library_in_image\n";
   %nm_addr=0;
   %nm_size=0;
   %nm_name=0;
   while (<library_in_image>) {
      @line = split(' ',$_);
      # print "$_\n";
      if ($line[0] ne 'U') {
            if (length($line[1])>1) {
               $nm_addr{$line[0]}=$line[0];
               $nm_size{$line[0]}=$line[1];
               $nm_char{$line[0]}=$line[2];
               $nm_name{$line[0]}=$line[3];
            }
      }
   }
   &printSymbolsize;
}

sub printSymbolsize {

   $temp_symbolsize = "> $ABS_DIR_SPT_OUTPUT/nm_symbolsize.txt";
   open temp_symbolsize or die "Cannot open temp parse $temp_symbolsize\n";
   print temp_symbolsize "key       nm_addr nm_size  nm_char      nm_name\n";

   while( my ($k, $v) = each %nm_addr ) {
        print temp_symbolsize "$k   $nm_addr{$k}, $nm_size{$k}, $nm_char{$k}, $nm_name{$k}\n";
   }
}



sub PrintObjInfo {

   print " Integrate information from image and map.txt\n";
   $objectinfo = "> $ABS_DIR_SPT_OUTPUT/object_info.txt";
   open objectinfo or die "Cannot open file: $objectinfo\n";
   $flag=1;
   foreach (@m_sorted_keys) {
      $m_size{$_}=sprintf("%.8x",hex($m_size{$_}));
      print objectinfo "$m_size{$_} $m_objname{$_} $m_libname{$_} $m_section{$_} $m_virt_addr{$_}\n";
      $ad = hex($m_virt_addr {$_});
      $si = hex($m_size{$_});
      $i=$ad;
      while ($i<($ad+$si)) { #$si>0
         $hi = sprintf("%x",$i);
         if ($nm_addr{$hi} ne "") {
                  if ($flag==2) {
                     $name = "z_unused"."@"."$staddr";
                     $virt_addr{$name} = $staddr;
                     $size{$name}      = sprintf("%.8x",$stsize);
                     $varname{$name}   = "z_unused";
                     $onechar{$name}   = "u";
                     $libname{$name}   = $m_libname{$_};
                     $objname{$name}   = $m_objname{$_};
                     $section{$name}   = $m_section{$_};
                     $codecomp{$name} = $CodeComponent{$m_libname{$_}};
                     $tempLib=$m_libname{$_};
                     if ($codecomp{$name} eq "") {
                         
                        if($s_component{$tempLib} eq ""){
                           $s_component{$tempLib} = 1;
                           print "\  Found new component $m_libname{$_}\n";
                          system("find . | grep '$m_libname{$_}'");
                          $codecomp{$name}="NEW";
                          $CodeComponent{$m_libname{$_}}="NEW";
                        }
                     }
                  #   print "$phys_addr{$name} $virt_addr{$name} $size{$name} $onechar{$name} $codecomp{$name} $section{$name} $libname{$name} $objname{$name} $varname{$name}\n";
                  }
                  $flag = 1;
                  $name = "$nm_name{$hi}"."@"."$nm_addr{$hi}";
                  $virt_addr{$name} = $nm_addr{$hi};
                  $size{$name}      = $nm_size{$hi};
                  $varname{$name}   = $nm_name{$hi};
                  $onechar{$name}   = $nm_char{$hi};
                  $libname{$name}   = $m_libname{$_};
                  $objname{$name}   = $m_objname{$_};
                  $section{$name}   = $m_section{$_};
                  $codecomp{$name} = $CodeComponent{$m_libname{$_}};
                  $tempLib=$m_libname{$_};
                  if ($codecomp{$name} eq "") {
                     if($s_component{$tempLib} eq ""){
                        $s_component{$tempLib} = 1;
                        print "\n  Found new component $m_libname{$_}\n";
                       system("find . | grep '$m_libname{$_}'");
                       $codecomp{$name}="NEW";
                       $CodeComponent{$m_libname{$_}}="NEW";
                     }
                  }
                  #print "$phys_addr{$name} $virt_addr{$name} $size{$name} $onechar{$name} $codecomp{$name} $section{$name} $libname{$name} $objname{$name} $varname{$name}\n";
                  $i=$i+hex($nm_size{$hi});
         } else {
            #print("Cannot find addr $hi\n");
            if ($flag==1) {
               #print "$staddr\n";
               $staddr = $hi;
               $stsize = 1;
               $flag=2;
            } else {
               $stsize++;
            }
            $i++;
         }
      }
   }

}

#5. Get the section information from dsp image
sub getHeaderInfoFromImage {
   print " Retrieve physical to virtual tables of each sections from image\(hexagon-objdump\)\n";
   system ("hexagon-objdump ./obj/qdsp6v4_ReleaseG/bootimg.pbn -h -w > $ABS_DIR_SPT_OUTPUT/section_info.txt");
   $section_file = "$ABS_DIR_SPT_OUTPUT/section_info.txt";
   open section_file or die "Cannot Open File $section_file\n";
   %vstart_address = 0;
   %pstart_address = 0;
   %size_address = 0;
   $stage=0;
   while(<section_file>)
   {
      if(/Idx/){
         $stage =1;
         next;
      }
      if($stage eq 1){
         @line_array = split(' ',$_);
         $vstart_address{"$line_array[1]"} = $line_array[3];
         $pstart_address{"$line_array[1]"} = $line_array[4];
         $size_address{"$line_array[1]"} = $line_array[2];
      }
         
        # print "$line_array[1] - virt=$line_array[3]  phys=$line_array[4]  size=$line_array[2]\n";
     # }
   }
   
  $namesec{"q6"}="q6";
 
   %btext=0;
   %brodata=0;
   %bsbss=0;
   %bsdata=0;
   %bdata=0;
   %bbss=0;
   %beh_frame=0;
   %bgcc_except_table=0;
   %bctors=0;
   %bdtors=0;
   %bfini=0;
   %binit=0;
   foreach (keys %size_address) {
      #print "key:$_\n";
      if ($_) {
         
         if ($_ eq ".bss") { $bbss{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".sbss") { $bsbss{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".sdata") { $bsdata{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".data") { $bdata{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".init") { $binit{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".text") { $btext{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".rodata") { $brodata{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".fini") { $bfini{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".ctors") { $bctors{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".dtors") { $bdtors{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".eh_frame") { $beh_frame{"q6"} = hex($size_address{$_}); }
         if ($_ eq ".gcc_except_table") { $bgcc_except_table{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".start") { $bstart{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".comment") { $bcomment{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".debug_aranges") { $bdebug_aranges{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".debug_pubnames") { $bdebug_pubnames{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".debug_info") { $bdebug_info{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".debug_abbrev") { $bdebug{"q6"} = hex($size_address{$_}); }
         #if ($_ eq ".bss") { $bbss{"q6"} = hex($size_address{$_}); }
      }
   }

}

#6. calculate physical address 
sub calPhyAddr {
   print " Calculate physical address of each symbol\n";
   foreach (keys %varname) {#every symbol
      $name = $_;
      #print "$_ \n";
      $va = hex($virt_addr{$name});
      
      foreach (keys %vstart_address) {
         $sa = hex($vstart_address{$_});
         $si = hex($size_address{$_});
      #   print "$va $sa $si\n";
         if ( ($va>=$sa) && ($va<($sa+$si))) {
            $paddr = hex($pstart_address{$_}) + $va - $sa;
            $phys_addr{$name} = sprintf("%x",$paddr);
      #print "$phys_addr{$name} $virt_addr{$name} $size{$name} $codecomp{$name} $section{$name} $libname{$name} $objname{$name} $varname{$name}\n";      
         }
      }
   }
}

sub CreateImageDataBase {

   print " Creating image database file\n";
   $outfile = "> $ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_DATABASE";
   open outfile or die "Cannot open out file $outfile\n";
   @keys_add = keys %phys_addr;
   @sorted_keys = sort{ hex($phys_addr{$a}) <=> hex($phys_addr{$b}) } @keys_add;
   print outfile "PA       VA       Size     C Cmp Section         Object	  Library	Name\n";

  # @keys_add = keys %phys_addr;
  # @sorted_keys = sort{ hex($phys_addr{$a}) <=> hex($phys_addr{$b}) } @keys_add;

   foreach (@sorted_keys) {
      if ($_) {
      print outfile "$phys_addr{$_} $virt_addr{$_} $size{$_} $onechar{$_} $codecomp{$_} $section{$_} $objname{$_} $libname{$_} $varname{$_}\n";
      }
   }

  

}
sub CreateObjList {

   $object_list = "> $ABS_DIR_SPT_OUTPUT/object_list.txt";
   open object_list or die "Cannot open out file $object_list\n";

   @obkeys = keys %libobjname;
   @objsort = sort { $libobjname{$a} cmp $libobjname{$b} } @obkeys;

   foreach (@objsort) {
      if ($_) {   
         @ll=split('@',$libobjname{$_});
         @lf=split('\.',$ll[1]);
         if ($lf[-1] eq "o") {
            print object_list "INPUT (tempo/$ll[0])\n";
         } else {
            print object_list "INPUT (tempo/$ll[1]/$ll[0])\n";
         }
      }
   }
}

sub CreateImageSizeOutput {
   %namecomp=0;
   %atext=0;
   %arodata=0;
   %asbss=0;
   %asdata=0;
   %adata=0;
   %abss=0;
   print " Creating image size analysis file\n";
   foreach (@sorted_keys) {
      if ($_) {
      $namecomp{$codecomp{$_}} = $codecomp{$_};
      #print "namecomp: $namecomp{$codecomp{$_}}\n";

      #print "cur:$_, section{$_}:$section{$_} \n";
      #print "$phys_addr{$_} $virt_addr{$_} $size{$_} $onechar{$_} $codecomp{$_} $section{$_} $objname{$_} $libname{$_} $_ \n";
      #print "section{$_}:$section{$_}\n";
      if ($section{$_} eq "q6.text") {
         $atext{$codecomp{$_}} += hex($size{$_});
      }
      if ($section{$_} eq "q6.rodata") {
         $arodata{$codecomp{$_}} += hex($size{$_});
      }
      if ($section{$_} eq "q6.sbss") {
         $asbss{$codecomp{$_}} += hex($size{$_});
      }
      if ($section{$_} eq "q6.bss") {
         $abss{$codecomp{$_}} += hex($size{$_});
      }
      if ($section{$_} eq "q6.sdata") {
         $asdata{$codecomp{$_}} += hex($size{$_});
      }
      if ($section{$_} eq "q6.data") {
         $adata{$codecomp{$_}} += hex($size{$_});
      }
      }
   }
   
   
   
   $outfilesize = "> $ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_SIZE_OUTPUT";
   open outfilesize or die "Cannot open out file $outfilesize\n";
   print outfilesize "Name,text,rodata,sbss,sdata,data,bss,init,fini,eh_frame,gcc_except_table,ctors,dtors\n";
   foreach (keys %namesec) {
         print outfilesize "$_,$btext{$_},$brodata{$_},$bsbss{$_},$bsdata{$_},$bdata{$_},$bbss{$_},$binit{$_},$bfini{$_},$beh_frame{$_},$bgcc_except_table{$_},$bctors{$_},$bdtors{$_}\n";      
   }
   print outfilesize "\n\n\n\nWhole Image size break down\nName,text,rodata,sbss,sdata,data,bss\n";
   @name_keys = keys %namecomp;
   @name_sorted_keys = sort{ uc("$namecomp{$a}") cmp uc("$namecomp{$b}") } @name_keys;
   foreach (@name_sorted_keys) {
      if ($_) {
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_}\n";
      }
   }
   
   %namecomp=0;
   %atext=0;
   %arodata=0;
   %asbss=0;
   %asdata=0;
   %adata=0;
   %abss=0;
   foreach (@sorted_keys) {
      if ($_) {
      $namecomp{$codecomp{$_}} = $codecomp{$_};
      #print "$namecomp{$codecomp{$_}}\n";
      #print outfile "$phys_addr{$_} $virt_addr{$_} $size{$_} $onechar{$_} $codecomp{$_} $section{$_} $objname{$_} $libname{$_} $_ \n";
      $lbname = $libname{$_};
      if ($lbname =~ /Test/) {
         
      } elsif ( $lbname =~ /test/ ){
   
      } elsif ( $lbname =~ /TEST/ ) {
   
      } else {
         #print "$lbname under test section\n"; 
         #print "section{$_}:$section{$_}\n";
         if ($section{$_} eq "q6.text") {      
            $atext{$codecomp{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.rodata") {
           $arodata{$codecomp{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.sbss") {
            $asbss{$codecomp{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.bss") {
            $abss{$codecomp{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.sdata") {
            $asdata{$codecomp{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.data") {
            $adata{$codecomp{$_}} += hex($size{$_});
         }
      }
   
      }
   }
   
   print outfilesize "\n\n\n\nImage size without test lib break down \nName,text,rodata,sbss,sdata,data,bss\n";
   foreach (@name_sorted_keys) {
      if ($_) {
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_}\n";
      }
   }
   
   %namecomp=0;
   %atext=0;
   %arodata=0;
   %asbss=0;
   %asdata=0;
   %adata=0;
   %abss=0;
   foreach (@sorted_keys) {
      if ($_) {
      $namecomp{$libname{$_}} = $codecomp{$_};
      #print "$namecomp{$codecomp{$_}}\n";
      #print outfile "$phys_addr{$_} $virt_addr{$_} $size{$_} $onechar{$_} $codecomp{$_} $section{$_} $objname{$_} $libname{$_} $_ \n";   
         if ($section{$_} eq "q6.text") {      
            $atext{$libname{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.rodata") {
            $arodata{$libname{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.sbss") {
            $asbss{$libname{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.bss") {
            $abss{$libname{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.sdata") {
            $asdata{$libname{$_}} += hex($size{$_});
         }
         if ($section{$_} eq "q6.data") {
            $adata{$libname{$_}} += hex($size{$_});
         }
      }
   }
   
   print outfilesize "\n\n\n\nLibraries without tests\nName,text,rodata,sbss,sdata,data,bss,Component\n";
   @ckeys_add = keys %namecomp;
   @csorted_keys = sort{ $namecomp{$a} cmp $namecomp{$b} } @ckeys_add;
   foreach (@csorted_keys) {
      if ($_) {   
      if ( /Test/) {
         
      } elsif ( /test/ ){
   
      } elsif ( /TEST/ ) {
   
      } else {
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_},$namecomp{$_}\n";
      }
      }
   }
   
   print outfilesize "\n\n\n\nLibraries flagged as tests\nName,text,rodata,sbss,sdata,data,bss,Component\n";
   foreach (@csorted_keys) {
      if ($_) {
      
      if ( /Test/) {
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_},$namecomp{$_}\n";      
      } elsif ( /test/ ){
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_},$namecomp{$_}\n";
      } elsif ( /TEST/ ) {
         if ($atext{$_} eq "") {
            $atext{$_} = 0;
         }
         if ($abss{$_} eq "") {
            $abss{$_} = 0;
         }
         if ($asbss{$_} eq "") {
            $asbss{$_} = 0;
         }
         if ($asdata{$_} eq "") {
            $asdata{$_} = 0;
         }
         if ($adata{$_} eq "") {
            $adata{$_} = 0;
         }
         if ($arodata{$_} eq "") {
            $arodata{$_} = 0;
         }
         print outfilesize "$_,$atext{$_},$arodata{$_},$asbss{$_},$asdata{$_},$adata{$_},$abss{$_},$namecomp{$_}\n";
      } else {
      }
      }
   }
}

sub CleanImageTempFile {

   print " Copying image_breakdown.csv and image_database.txt to root directory\n";

   system("cp -f $ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_SIZE_OUTPUT $WORKING_DIRECTORY/../../../image_breakdown.csv");
   system("cp -f $ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_DATABASE $WORKING_DIRECTORY/../../../image_database.txt");

   print " Cleaning meta files\n";
   system("rm -f $ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_SIZE_OUTPUT");
   
   system("rm -f $ABS_DIR_SPT_OUTPUT/temp_parse.txt");
   system("rm -f $ABS_DIR_SPT_OUTPUT/nm_symbolsize.txt");
   system("rm -f $ABS_DIR_SPT_OUTPUT/section_info.txt");
   system("rm -f $ABS_DIR_SPT_OUTPUT/object_list.txt");
   system("rm -f $ABS_DIR_SPT_OUTPUT/object_info.txt");
   system("rm -f $ABS_DIR_SPT_OUTPUT/library_in_image.txt");

   $outfilesize = "$ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_SIZE_OUTPUT";
   $outfile = "$ABS_DIR_SPT_OUTPUT/$FILE_NAME_IMAGE_DATABASE";
}

