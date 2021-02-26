#!/usr/bin/perl
$g_dal_proc_info = {
        name    => $string,
        cfg_file => $string
};
        
@g_proc_info;

sub ReadCfgFile
{
    my($cfg_file_path) = @_; 
    open(cfg_file_handle, $cfg_file_path) || die ("Could not open $cfg_file_path\n");

        @cfg_blob=<cfg_file_handle>;
        foreach $cfg_line (@cfg_blob)
        {
            chop($cfg_line);
                ($cfg_item,$cfg_val)=split("=",$cfg_line);
                #trim leading and trailing whitespace from the cfg_item and cfg_val
                $cfg_item =~ s/^\s+//;
                $cfg_item =~ s/\s+$//;
                $cfg_val  =~ s/^\s+//;
                $cfg_val  =~ s/\s+$//;
                if($cfg_item eq "process")
                {
                        my @proc_list =split(";",$cfg_val);
                        foreach $proc_entry(@proc_list)
                        {
                                ($proc_name,$cfg_file)=split(":",$proc_entry);
                                push @{g_proc_info},{name=>$proc_name,cfg_file=>$cfg_file};                              
                        }                       
                }
        }       
        close(cfg_file_handle);
}

sub CleanUpXMLFile
{
   my($input_xml) = @_[0];
   my($output_xml) = @_[1];

   open (input_xml, $input_xml) || die("Could not open the input XML file $input_xml.");  
   open (output_xml, "+>$output_xml") || die("Could not open the output XML file $output_xml.");

   # Read input XML in one line at a time.
   while ($line = <input_xml>) 
   {
     # We only care about lines that have XML tags in them. These lines
     # must begin with any combination of spaces or tabs ( ,\t)* followed
     # by < , any number of characters, > , then more spaces or tabs.
     if ($line =~ m/( ,\t)*<.*>( ,\t)*/ && $line !~ m/<!--.*>/) {
       # This line has an XML tag in it.
   
       # Make sure all parameter values are in quotes. Since parameter values
       # may be arithmetic expressions, this gets a little complex. An explanation
       # of this regular expression is in order...
       #
       # (\w+)= is the name of the parameter and the equals sign following it.
       #
       # ([\w\s()+-]+?) is the value of the key, which may be an arithmetic expression.
       #   It possibly contains alphanumeric characters, parenthesis, spaces, and + or
       #   - signs. We make it not greedy, because if this parameter is followed by another,
       #   a greedy match would steal all but one character of the following parameter name.
       #
       # (\s*\w+=|\s*>|\s*\/>) Things that can follow the parameter value. There can be
       #   0 or more whitespace characters, then either the name of another parameter or
       #   the end of the tag (specified as either > or />).
       #
       # We must run this over and over until there are no more matches. Doing a single
       # global match doesn't work, probably because a match consumes part of an
       # adjacent parameter (meaning that parameter then won't be matched).
       while($line =~ s/ (\w+)=([\w\s()\+\-\*]+?)(\s*\w+=|\s*>|\s*\/>)/ $1=\"$2\"$3/g) {
       }
   
       # Resolve any arithmetic expressions.
       $line =~ s/ (\w+)=\"\s*\(([\w\s()\+\-\*]+)\)\"/ " " . $1 ."=\"" . sprintf("0x%x",eval($2)) . "\""/ge;
       print output_xml $line;
     }else
     {
        my ($line_copy) = $line;
        #check to see if line is empty
        #remove leading whitespaces, if line is empty do not print it.
        $line_copy =~ s/^\s+//;

        if($line_copy ne "")
        {
           print output_xml $line;
        }
     }
   }
   close input_xml;
   close output_xml;
}

sub GenerateProcessPropsFile
{
   my ($prop_xml_file) = @_[0];
   my ($prop_xml_file_pre) = @_[0];
   my ($prop_xml_file_out) = @_[0];
   my ($prop_code_file_out) = "DALConfig_".@_[1].".c";

   print $prop_code_file_out

   $prop_xml_file_pre =~ s/\.xml/_pre\.xml/;    
   $prop_xml_file_out =~ s/\.xml/_out\.xml/;    
   

   my ($preproc_cmd) = "cl.exe /EP /I../inc ".$prop_xml_file." > ".$prop_xml_file_pre;

   my ($gen_prop_code) = "propgen ".$prop_xml_file_out." . ".$prop_code_file_out; 
   
   #run C pre-processor
   my $output = `$preproc_cmd`;
   print "$output \n";

   #Clean up the preprocessed XML file
   CleanUpXMLFile($prop_xml_file_pre,$prop_xml_file_out);

   $output = `$gen_prop_code`;
   print "$output \n";
}

$numArgs = $#ARGV +1; 

if (($numArgs < 2))
{ 
   print "\n";
   print "Usage: perl props.pl process_name xml_config_file\n";
   die(); 
}
GenerateProcessPropsFile($ARGV[1],$ARGV[0]);

