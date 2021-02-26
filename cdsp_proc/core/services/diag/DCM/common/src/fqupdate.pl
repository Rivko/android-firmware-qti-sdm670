# ------------------------------------------------------------------------
#  Feature Query Mask Update 
#
#  This script hunts down all the FEATURES_X in the named custX.h file
#  and adds the appropriate text to the feature query mask in feature.c
#  and the feature query enum in feature.h.  These files must be present
#  in the directory along with the cust file for fqupdate to run.
#
#  If the -checkin option is selected fqupdate.pl will checkout feature.h
#  and feature.c update them with new features.  Then it will check them in
#  label the FEATURE VU in the PVCS with the label specified.
#
#  Copyright 1998,2000,2001,2002 QUALCOMM Inc.                                       
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# Command Line Arguments 
# ------------------------------------------------------------------------
my $fUpdateVersionControl = 0;
my $fVerbose              = 0;

# ------------------------------------------------------------------------
# Search Directories
#    List of directories to search for feature flags 
# ------------------------------------------------------------------------
my @SearchDirs = ();

# ------------------------------------------------------------------------
# Search Patterns 
#    List of patterns to use in file search 
# ------------------------------------------------------------------------
my @SearchPatterns = ();

# ------------------------------------------------------------------------
# File List 
#    List of files found in specified directories with the specified
#    patterns 
# ------------------------------------------------------------------------
my @FileList = ();

# ------------------------------------------------------------------------
# NewFeatures 
#    List of features found in FileList 
# ------------------------------------------------------------------------
my @NewFeatures = ();

# ------------------------------------------------------------------------
# CurrentFeatureList 
#    List of features already accounted for in feature.h/.c
# ------------------------------------------------------------------------
my %CurrentFeatureList = ();

# ------------------------------------------------------------------------
#  FUNCTION  SEARCH_FOR_NEW_FEATURES
#
#  DESCRIPTION
#    This function returns all FEATURES in the given file that 
#    are not in the enum in feature.h
#
#  DEPENDENCIES
#    "typedef enum { /* begin feature_query_enum_type */" denotes the
#      beginning of the enum in feature.h
#    "FEATURE_QUERY_ENUM_LENGTH /* used to determine mask size */" denotes
#      the end of the enum
#
#  RETURN VALUE
#    A list of new features
# ------------------------------------------------------------------------
sub search_for_new_features  {

  local( $file );
  @searchfiles = ( $_[0] );
  @found_features = ();

  foreach $file ( @searchfiles ) {
    $search = 0;
    print "Processing $file...\n";
    open CUST, "<$file" or die "Error: opening file $file";
    while(<CUST>)
    {
      #if( /^#\s*include\s"(\w+\.h)"/ ) 
      #{
      #  push( @searchfiles, $1 );
      #}

      while (/(FEATURE_\w{1,23})/)
      {
        print "   Checking $1 with " if ( $fVerbose == 1 );
        $current_line = $_;
        chop;
        $feature = $1;
        $feature_bit = $1;
        $feature_bit =~ s/FEATURE_//;
        $feature_bit = "F_" . $feature_bit . "_BIT";

        print "[$feature_bit] against [$CurrentFeatureList{$feature_bit}] ... " if ( $fVerbose == 1 );

        if( $CurrentFeatureList{$feature_bit} )
        {
           print "found\n" if ( $fVerbose == 1 );
        }
        else
        {
           print "new\n" if ( $fVerbose == 1 );
           @foundlist = ();
           @foundlist = grep $feature eq $_, @found_features;
           $found = scalar(@foundlist);
           if ($found == 0)
           {
              push(@found_features, $feature);
           }
        }
        
        $_ = $current_line;
        s/$feature//;
      }
    }
    close CUST;
  }
  return @found_features;
}

# ------------------------------------------------------------------------
#  FUNCTION  ADD_TO_FH
#
#  DESCRIPTION
#    This function adds the given features to feature.h.
#
#  DEPENDENCIES
#    "FEATURE_QUERY_ENUM_LENGTH /* used to determine mask size */" denotes
#      the end of the enum
#
#  RETURN VALUE
#    None
# ------------------------------------------------------------------------
sub add_to_fh  {
  my @add_features = @_;
  
  open(OLD_ENUM, "<feature.h") or die "Cannot open feature.h";
  open(NEW_ENUM, ">________.TMP") or die "Cannot open temp file";
  while(<OLD_ENUM>)
  {
    if (/\sFEATURE_QUERY_ENUM_LENGTH \/\* used to determine mask size \*\//)
    {
      foreach $feature (@add_features) 
      {
        $featurebit = $feature;
        $featurebit =~ s/FEATURE_//;
        $featurebit = "F_" . $featurebit . "_BIT";
        print NEW_ENUM "  $featurebit,\n";
      }
      print NEW_ENUM "  FEATURE_QUERY_ENUM_LENGTH \/\* used to determine mask size \*\/\n";
    }
    else 
    {
      print NEW_ENUM $_;
    }
  }
  close OLD_ENUM;
  close NEW_ENUM;
  unlink "feature.h";
  rename "________.TMP", "feature.h";
}

# ------------------------------------------------------------------------
#  FUNCTION  GEN_FH_TO_FC
#
#  DESCRIPTION
#    This takes all the strings directed to it and creates feature.h
#    and feature.c files.  It hunts down all the FEATURES_X
#    and creates the feature query mask and enums in feature.h and feature.c
#
#  DEPENDENCIES
#    All FEATURE_X in the stream are valid features
#
#  RETURN VALUE
#    None
# ------------------------------------------------------------------------
sub gen_fh_and_fc () 
{
  open(MASK, ">feature.c");
  print MASK "byte diag_feature_mask[FEATURE_MASK_LENGTH] = {\n";
  print MASK "  0\n";
  open(TARGET, ">feature.h");
  print TARGET "typedef enum {\n";
  close TARGET;
  $count = 0;
  while (<>)
  {
    while (/(FEATURE_\w{1,23})/)  # we enforce a max length for feature names
    {
      $current_line = $_;
      $feature = $1;
      $featurebit = $1;
      $featurebit =~ s/FEATURE_//;
      $featurebit = "F_" . $featurebit . "_BIT";
     
      open(SCAN, "<feature.h");
      $found = 0;
      while (<SCAN>)
      {
        chomp;
        if (/\s$featurebit/)
        {
          $found = 1;
          last;
        }
      }
      close SCAN;
      
      if ($found == 0)
      {
        open(TARGET, ">>feature.h");
        print TARGET "  $featurebit,\n";
        close TARGET;

        if (++$count == 9)
        {
          $count = 1;
          print MASK "  , 0\n";
        }

        print MASK "  #ifdef $feature\n";
        print MASK "  | ADD_BIT_TO_FEATURE_MASK(" . "$featurebit)\n";
        print MASK "  #endif /* $feature */\n";
      }
      $_ = $current_line;
      s/$feature//; # block out the occurrence that I found
                    # so I can search the same line twice
    }
  }
  open(TARGET, ">>feature.h");
  print TARGET "  FEATURE_QUERY_ENUM_LENGTH\n";
  print TARGET "} feature_query_enum_type;\n";
  close TARGET;
  print MASK "};\n";
  close MASK;
}

# ------------------------------------------------------------------------
#  FUNCTION  FH_TO_FC
#
#  DESCRIPTION
#    This function generates a new mask in feature.c based on 
#    the feature enum in feature.h
#
#  DEPENDENCIES
#    "typedef enum { /* begin feature_query_enum_type */" denotes the
#      beginning of the enum in feature.h
#    "FEATURE_QUERY_ENUM_LENGTH /* used to determine mask size */" denotes
#      the end of the enum
#    "}; /* end diag_feature_mask */" denotes the end of the mask in
#      feature.c
#
#  RETURN VALUE
#    A list of new features
# ------------------------------------------------------------------------
sub fh_to_fc (){
  open(ENUM, "<feature.h") or die "Cannot open feature.h";
  open(OLD_MASK, "<feature.c") or die "Cannot open feature.c";
  open(NEW_MASK, ">________.TMP") or die "Cannot open temp file";
  $count = 0;
  $search = 0;

  while(<OLD_MASK>)
  {
    print NEW_MASK $_;
    if (/byte diag_feature_mask\[FEATURE_MASK_LENGTH\] = \{/)
    {
      if ($count == 0)
      {
        print NEW_MASK "  0\n";
      } 
      while(<ENUM>)
      {
        if (/typedef enum \{ \/\* begin feature_query_enum_type \*\//) {
           $search = 1;
        }
        if (/\sFEATURE_QUERY_ENUM_LENGTH \/\* used to determine mask size \*\//) {
           $search = 0;
           last;
        }
        if ($search == 1) {
          if(/\sF_(\w+)_BIT/)
          {
            if (++$count == 9)
            {
              $count = 1;
              print NEW_MASK "  , 0\n";
            }
            print NEW_MASK "  #ifdef FEATURE_$1\n";
            print NEW_MASK "  | ADD_BIT_TO_FEATURE_MASK(" . "F_$1_BIT)\n";
            print NEW_MASK "  #endif /* FEATURE_$1 */\n";
          }
        }
      }
      # Scroll to the bottom of the old mask
      while(<OLD_MASK>)
      {
        if (/}; \/\* end diag_feature_mask \*\//)
        {
          print NEW_MASK $_;
          last;
        }
      }
    }
  }
  close OLD_MASK;
  close NEW_MASK;
  close ENUM;
  unlink "feature.c";
  rename "________.TMP", "feature.c";
}

# ------------------------------------------------------------------------
#  FUNCTION  GET_FEATURE_VU_NAME
#
#  DESCRIPTION
#    This function returns the current VU number
#
#  DEPENDENCIES
#    Label from vlog is in the form "MSM_DIAG.01.X" where X is an integer.
#
#  RETURN VALUE
#    A string (vu label).
# ------------------------------------------------------------------------
sub get_feature_vu_name ()
{
  system("vlog feature.h > ________.TMP") and die "Cannot vlog feature.h";
  open(VLOG, "<________.TMP");
  $feature_ver1 = 0;
  $feature_ver2 = 0;
  $feature_ver3 = 0;
  
  while(<VLOG>)
  {
    if (/"MSM_DIAG\.(\d+)\.(\d+)\.(\d+)"/) {
    
      if( scalar($1) > $feature_ver1 ) {
        $feature_ver1 = scalar($1);
        $feature_ver2 = scalar($2);
        $feature_ver3 = scalar($3);
      } elsif( scalar($2) > $feature_ver2 ) {
        $feature_ver2 = scalar($2);
        $feature_ver3 = scalar($3);
      } elsif( scalar($3) > $feature_ver3 ) {
        $feature_ver3 = scalar($3);
      }
    }
  }
  close VLOG;
  unlink ("________.TMP");
  $feature_ver = sprintf( "MSM_DIAG.%02d.%02d.%02d", $feature_ver1,
                          $feature_ver2, $feature_ver3 );
  return $feature_ver;
}

# ------------------------------------------------------------------------
# SUBROUTINE: ProcessCommandLine
#    Handles command line arguments
# ------------------------------------------------------------------------
sub ProcessCommandLine
{
   my $i=0;

   for( $i=0; $i<$#ARGV+1; $i++)
   {
      my $arg = $ARGV[$i];

      if( $arg eq "-D" )
      {
         $i++;
         push( @SearchDirs, $ARGV[$i] );
      }
      elsif( $arg eq "-P" )
      {
         $i++;
         push( @SearchPatterns, $ARGV[$i] );
      }
      elsif( $arg eq "-help" )
      {
         Usage();
         exit;
      }
      elsif( $arg eq "-v" )
      {
         $fVerbose = 1;
      }
      elsif( $arg eq "-ci" )
      {
         $fUpdateVersionControl = 1;
         print "Automatic update of version control not supported.\n";
         Usage();
         exit;
      }
      elsif( $arg eq "-I" )
      {
         print "Include paths not supported.\n";
         Usage();
         exit;
      }
      else
      {
         print "Unrecognizzed option: $arg\n";
         Usage();
         exit;
      }
   }
}

# ------------------------------------------------------------------------
# SUBROUTINE: Copyright 
# ------------------------------------------------------------------------
sub Copyright 
{
   print "FQUPDATE Version 1.3\n";
   print "Copyright 1998,2000,2001,2002 QUALCOMM Inc.\n\n";
}

# ------------------------------------------------------------------------
# SUBROUTINE: Usage 
# ------------------------------------------------------------------------
sub Usage 
{
   Copyright();
   print "Usage:    perl fqupdate.pl -D <relative directory> -P <pattern> [-ci,-help,-I]\n"; 
   print "Examples:\n";
   print "   perl fqupdate.pl -D ..\..\build\cust -P cust*.h\n";
   print "   perl -help\n";
   print "\n";
}

# ------------------------------------------------------------------------
# SUBROUTINE: GenerateFileList 
# ------------------------------------------------------------------------
sub GenerateFileList 
{
   # foreach directory in SearchDirectories
   #   foreach pattern in SearchPatterns
   #      Get list of files
   # 
   my $dir     = "";
   my $pattern = "";

   foreach $dir (@SearchDirs)
   {
      print "DIR: $dir\n" if ( $fVerbose == 1 );
      foreach $pattern (@SearchPatterns)
      {
         print "   [$dir/$pattern]\n" if ( $fVerbose == 1 );
         while( glob("$dir/$pattern"))
         {
            print "      $_\n" if ( $fVerbose == 1 );
            push @FileList, $_;
         }
      }
   }
}

# ------------------------------------------------------------------------
# SUBROUTINE: GetCurrentFeatureList 
# ------------------------------------------------------------------------
sub GetCurrentFeatureList 
{
   open(ENUM, "<feature.h") or die "Cannot open feature.h";
   my $found   = 0;
   my $feature = 0;
   while( <ENUM> ) 
   {
      chop;

      if (/typedef enum \{ \/\* begin feature_query_enum_type \*\//) 
      {
         $search = 1;
      }
      elsif (/\sFEATURE_QUERY_ENUM_LENGTH \/\* used to determine mask size \*\//) 
      {
         $search = 0;
         last;
      }
      elsif ($search == 1) 
      {
         chop;
         $feature = $_;
         $feature =~ s/^\s*//;
         $CurrentFeatureList{$feature} = $feature;
         print "Present: [$CurrentFeatureList{$feature}]\n" if ( $fVerbose == 1 );
      }
   }
   close ENUM;
}

# ------------------------------------------------------------------------
# SUBROUTINE: UpdateFeatures
# ------------------------------------------------------------------------
sub UpdateFeatures
{
   GetCurrentFeatureList();

   # foreach file in FileList
   #   @fs = sfnfs(file)
   #   add @fs to @new_features
   #
   my $file         = "";
   my @new_features = ();
   my $feature      = "";

   foreach $file (@FileList)
   {
      @new_features = ();
      @new_features = search_for_new_features($file);
      foreach $feature (@new_features)
      {
         print "Adding $feature...\n" if ( $fVerbose == 1 );
         push @NewFeatures, $feature;
      }
   }

   print "\n";

   if( $#NewFeatures + 1 == 0 ) 
   {
      print "No new features to add.\n"; 
   }
   else
   {
      foreach $feature (@NewFeatures)
      {
         print "Adding: $feature\n";
      }

      add_to_fh(@NewFeatures);

      fh_to_fc();
   }
}

# ------------------------------------------------------------------------
# SUBROUTINE: UpdateVersionControl
# ------------------------------------------------------------------------
sub UpdateVersionControl
{
   if( $fUpdateVersionControl == 1 )
   {
   }
}

# ------------------------------------------------------------------------
# MAIN SCRIPT
# ------------------------------------------------------------------------
ProcessCommandLine();

Copyright();

GenerateFileList();

UpdateFeatures();

UpdateVersionControl();
