#===============================================================================
#
# CoreBSP Preprocessor cleaner
#
# GENERAL DESCRIPTION
#    celans junk generated by preprocessor
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/ppasm.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/01/10   wd      Create
#
#===============================================================================
import os
import re
import sys
from optparse import OptionParser

#------------------------------------------------------------------------------
# Parse command line arguments
#------------------------------------------------------------------------------
def parse_args():
   usage = "usage: %prog [options]"
   version = "%prog 1.0"
   
   parser = OptionParser(usage=usage, version=version)
   parser.add_option("-f", "--ppfile", dest="pp_filename",
                  help="Read preprocess data from FILE", metavar="FILE")
   parser.add_option("-o", "--outfile", dest="output_filename",
                  help="Write output to FILE", metavar="FILE")
   parser.add_option("-g", "--gnu-comment", dest="gnu_comment", default=False, action="store_true",
                  help="Convert comment to GNU convention", metavar="FILE")
          
   (options, args) = parser.parse_args()
   
   if options.pp_filename is None:
      parser.error("-f option must be defined")
      sys.exit(2)
                  
                  
   return (options, args)
   
#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
def main():
   # get args from cmd line
   (options, args) = parse_args()
   
   file = open(options.pp_filename, 'r')
   isSCLFile = False
   isXMLFile = False
   if options.output_filename is not None:
     if options.output_filename.find('.scl') > 0:
       isSCLFile = True
     elif options.output_filename is not None and options.output_filename.find('.xml') > 0:
       isXMLFile = True
   data = file.readlines()
   file.close()
   
   if options.output_filename is None:
      file = sys.stdout
   else:
      file = open(options.output_filename, 'w')
   #skip the whole module includes section
   re_module_includes = re.compile('^\s*;\s*module includes.*$', re.IGNORECASE)
   re_module_others = re.compile('^\s*(GET )|(;\s*module.*$)', re.IGNORECASE)
   re_macro = re.compile('^\s*MACRO')
   re_macro_end = re.compile('^\s*MEND\s*$')
   re_bad_line = re.compile('(^\s*#line.*$)|(^\s*#\s\d.*$)|(^\s*$)')
   in_macro = False
   in_includes = False
   for line in data:
     if re_macro.match(line):
       in_macro = True
     if re_module_includes.match(line):
       in_includes = True
     elif re_module_others.match(line):
       in_includes = False

     if in_macro or (not in_includes and re_bad_line.match(line)) is None:
       if isSCLFile:
         file.write(line.replace('ASSERT_FATAL','ASSERT'))
       elif options.gnu_comment and not isXMLFile:
         #convert comment ';' to GNU's "//"
         file.write(line.replace(';', '//'))
       else:
         file.write(line)
     if  in_macro and re_macro_end.match(line):
       in_macro = False

   file.close()
   
main()
