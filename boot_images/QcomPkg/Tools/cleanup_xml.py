#======================================================================================================
#
# CLEANUP_XML.PY
#
# GENERAL DESCRIPTION :
#
# This script creates a well formed xml file. It ensures parameter values are contained within quotes.
# It resolves any arithmetic expressions in the parameter values(limited to +/-/*)
#
# Copyright (c) 2009-2015 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/15/15   bh      Merge changes from ap for settings relocation
# 02/03/15   sg      Fix file permission
# 01/22/15   sg      Initial Version
#===============================================================================

import sys
import re
import os, pdb
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
   if os.path.isfile(out_xml):
      os.chmod(out_xml,0666)
   if os.path.isfile(prop_cfile):
      os.chmod(prop_cfile,0666)

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
                  eval_substr = format(eval(substr[:isseq_end_index]))[1:-1] + substr[isseq_end_index:]
                else:
                  eval_substr = eval(substr)
                line = line.replace(substr, "" + format(eval_substr) + "")
          
          out_xml_fp.write(line)  
       
      else:
         if (re.match( r'( ,\t)*#.*', line, re.M|re.I) or re.match( r'enum_header_path', line, re.M|re.I)):
            parse_header_info(line, out_xml_fp, prop_cfile_fp)
            continue
         if(inside_drivermod):
            out_xml_fp.write(line)  

   input_xml_fp.close()
   out_xml_fp.close()
   prop_cfile_fp.close()
   
   return
   
#==============================================================================================
# Function to validate args to extract XML file-list 
#==============================================================================================   
def validate_args_for_xml_files(args):
    xml_file_list = []
    for arg in args:
        if(os.path.isfile(arg)):
            if ((os.path.splitext(arg)[1]).lower() == '.xml'):
                xml_file_list.append(arg)
    return xml_file_list           
#==============================================================================================
# Main function that preprocesses system xml and other xmls to 
#==============================================================================================   
def preprocess_system_dir(Input_PathDir, Output_ImageXMLFileName, Input_SystemXMLFileName, xml_file_list, Input_ModuleName):
   found_system_xml = None
   if os.path.isfile(Output_ImageXMLFileName):
      os.chmod(Output_ImageXMLFileName,0666)
   if not os.path.isdir(Input_PathDir):
      exit()
   try:
      out_xml_fp = open(Output_ImageXMLFileName, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      cleanup_error(errStr)
   out_xml_fp.write("#include \"DALPropDef.h\"\n")
   out_xml_fp.write("#include \"DALDeviceId.h\"\n")
   out_xml_fp.write("#include \"dalconfig.h\"\n")
   out_xml_fp.write("\n")

   out_xml_fp.write("<?xml version=\"1.0\"?>\n")
   out_xml_fp.write("<dal>\n")
   out_xml_fp.write("<module name=\"" +Input_ModuleName +"\">\n")
   #out_xml_fp.write("   #include \""  + Input_PathDir + "/" +Input_SystemXMLFileName + "\"\n")
   for filename in os.listdir(Input_PathDir):
    if filename.endswith('.xml'):
     if filename == os.path.basename(Input_SystemXMLFileName):
      found_system_xml = 1
    
   #Write all the XML files 
   for file in xml_file_list:
      out_xml_fp.write("   #include \""  + file + "\"\n")
   
   out_xml_fp.write("</module>\n")
   out_xml_fp.write("</dal>\n")
   out_xml_fp.close()
   if found_system_xml is None:
      print "ERROR: Unable to locate system_xml file " + Input_SystemXMLFileName + "\n"
      exit(-1)
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
   parser.add_option("-d", "--pre", dest="path_dir", help="XML File Directory input, INPUT, REQUIRED")   
   parser.add_option("-s", "--system_xml", dest="xml_system", help="XML System File input, INPUT, REQUIRED")
   parser.add_option("-i", "--image_xml", dest="xml_image", help="XML Image File input, INPUT, REQUIRED")   
   parser.add_option("-m", "--module", dest="module_name", help="Module Name input, INPUT, REQUIRED")
   parser.add_option("-o", "--out_xml", dest="xml_output", help="XML Filename output, INPUT, OUTPUT")
   parser.add_option("-c", "--ConfigFile", dest="cfg_dir_filename", help="DALConfig filename, OUTPUT")
   
   (options, args) = parser.parse_args()
   Input_MasterXMLFileName = None
   Input_SystemXMLFileName = None
   Input_ModuleName = None
   Input_PathDir = None
   Output_MasterXMLFileName = None
   Output_ImageXMLFileName = None
   ConfigFileName = None
   
   #post
   if options.xml_input is not None:
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
   #pre
   else:
      if options.path_dir is not None:
         Input_PathDir = options.path_dir
         if options.xml_system is None:
            parser.print_help()
            exit()
         else:
            Input_SystemXMLFileName = options.xml_system
         if options.xml_image is None:
            parser.print_help()
            exit()
         else:
            Output_ImageXMLFileName = options.xml_image
         if options.module_name is None:
            parser.print_help()
            exit()
         else:
            Input_ModuleName = options.module_name
      xml_file_list = validate_args_for_xml_files(args)
      preprocess_system_dir(Input_PathDir, Output_ImageXMLFileName, Input_SystemXMLFileName, xml_file_list, Input_ModuleName)
   return   

if __name__ == "__main__":
   main()
