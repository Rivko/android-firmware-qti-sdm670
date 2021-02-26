#======================================================================================================
#
# CLEANUP_XML.PY
#
# GENERAL DESCRIPTION :
#
# This script creates a well formed xml file. It ensures parameter values are contained within quotes.
# It resolves any arithmetic expressions in the parameter values(limited to +/-/*)
#
# Copyright (c) 2014 by Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#===============================================================================

import sys
import re
import os
from types import *
from optparse import OptionParser

inside_drivermod = 0

#==============================================================================================
# Parse the #include directives to write to DALConfig_xxxx.c file.
#==============================================================================================
def parse_header_info(line, out_xml_fp, prop_cfile_fp):
   include = re.match( r'#(.)*\.h\"', line, re.M|re.I)
   if(include):
      header_path, header_file = os.path.split(include.group()) 
      prop_cfile_fp.write("#include \"" + header_file + "\n")
   
   include = re.match( r'enum_header_path(.)*\.h\"', line, re.M|re.I)
   if(include):
      enum_pattern_include = re.sub( r'enum_header_path', "#include", include.group())
      prop_cfile_fp.write(enum_pattern_include+ "\n")  
   
   return

#==============================================================================================
# Main function that parses the raw config xml file to create well formed xml
#==============================================================================================   
def parse_xml_file(input_xml, out_xml, prop_cfile):
   
   global inside_drivermod
   # Open the base symbol file for reading, symbol and extern file for writing.
   try:
      input_xml_fp = open(input_xml, 'r')
      out_xml_fp = open(out_xml, 'w')
      prop_cfile_fp = open(prop_cfile, 'w')  
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      cleanup_error(errStr)

   # Read input XML in one line at a time. We only care about lines that have XML tags in them.
   # These lines must begin with any combination of spaces or tabs ( ,\t)* followed by < ,
   # any number of characters, > , then more spaces or tabs.
   for line in input_xml_fp:

      #import pdb; pdb.set_trace()
      if(len(line)==1 and re.match(r'\n', line)):
         continue
      
      # Its a comment, skip it.
      if(re.match( r'<!--.*>', line, re.M|re.I)):
         continue
      
      # if its #include of any type
      if(re.match( r'( ,\t)*#.*', line, re.M|re.I) or re.match( r'enum_header_path', line, re.M|re.I)):
         parse_header_info(line, out_xml_fp, prop_cfile_fp)
       
      # This line has an XML tag in it.
      if(re.match( r'(\s)*<.*>( ,\t)*', line, re.M|re.I)):
          # Make sure all parameter values are in quotes. Since parameter values may be arithmetic 
          # expressions, this gets a little complex. An explanation of this regular expression
          # is in order...
          # Each of regex defined within () is a group -
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
          while True:
            pattern = re.search(r'(\w+)=([\w\s()\+\-\*]+?)(\s*\w+=|\s*>|\s*\/>)', line, re.M|re.I)
            if(pattern is None):
               break
            # reconstruct line, pattern.group(2) is parsed expression which needed to be quoted.
            line = line.replace(pattern.group(2) + pattern.group(3), '"' + pattern.group(2) + '"' + pattern.group(3))
            
          # Found a start of driver tag.
          pattern = re.match(r'(\s)*<(\s)*driver', line, re.M|re.I)
          if(pattern):
            inside_drivermod = 1
          
          # Found an end of driver tag.
          pattern = re.match(r'(\s)*<(\s)*/(\s)*driver', line, re.M|re.I)
          if(pattern):
            inside_drivermod = 0          
          
          # Evaluate airthmatic expressions(+, -, *)
          pattern = re.search(r'(<props.*\"\s*>)(([\+\-\*]*[0x\w\s()]*[\+\-\*\,]+[0x\w\s()]*)+)', line, re.M|re.I)
          if(pattern):
             substr = pattern.group(2)
             if(len(substr) > 1):
                eval_substr = ""
                substr_isseq = re.search(r'\,\s*end', substr, re.M|re.I)
                if(substr_isseq):
                  isseq_end_index = substr.find(substr_isseq.group())
                  eval_substr = str(eval(substr[:isseq_end_index])).replace('(','').replace(')','') + substr[isseq_end_index:]
                else:
                  eval_substr = eval(substr)
                line = line.replace(substr, "" + format(eval_substr) + "")
          
          out_xml_fp.write(line)  
       
      else:
         if(inside_drivermod):
           out_xml_fp.write(line)  
      

   input_xml_fp.close()
   out_xml_fp.close()
   prop_cfile_fp.close()
   
   return

   
#===============================================================================
# Error handler function
#=============================================================================== 
def cleanup_error(Info, Info1=None):
    """
    Error handler for linker symbol generate script. Info can be a string that 
    describes the error and Info1 can be any data that needs to be printed along 
    with the error message.
    """
    raise Exception(Info) 
   
#==============================================================================================
# Main functiion
#==============================================================================================     
def main():
   parser = OptionParser("usage: %prog [options]")
   parser.add_option("-p", "--post", dest="xml_input", help="XML Filename input, INPUT, REQUIRED")
   parser.add_option("-o", "--out_xml", dest="xml_output", help="XML Filename output, INPUT, OUTPUT")
   parser.add_option("-c", "--ConfigFile", dest="cfg_dir_filename", help="DALConfig filename, OUTPUT")
   
   (options, args) = parser.parse_args()
   Input_MasterXMLFileName = None
   Output_MasterXMLFileName = None
   ConfigFileName = None
   
   if options.xml_input is None:
      parser.print_help()
      exit()
   else:
      Input_MasterXMLFileName = options.xml_input
      if options.xml_output is None:
         parser.print_help()
         exit()
      else:
         Output_MasterXMLFileName = options.xml_output
      if options.cfg_dir_filename is None:
         parser.print_help()
         exit()
      else:
         ConfigFileName = options.cfg_dir_filename

   parse_xml_file(Input_MasterXMLFileName, Output_MasterXMLFileName, ConfigFileName)

   return   

if __name__ == "__main__":
   main()
   
   
