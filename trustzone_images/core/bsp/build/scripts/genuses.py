#===============================================================================
#
# genuses
#
# GENERAL DESCRIPTION
#    Generates USES_FLAGS imformation from DATA file generate from build/ms. 
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/genuses.py#1 $
#  $DateTime: 2018/02/06 02:57:17 $
#  $Author: pwbldsvc $
#  $Change: 15399902 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 04/02/10   sk     Created
#
#===============================================================================
import os
import subprocess 
import string
import sys
import re, string, os
from array import array
from optparse import OptionParser
from datetime import datetime

#===============================================================================
# parse_args
# parse command line arguments
#===============================================================================
def parse_args():
   usage = "usage: %prog [options]"
   version = "%prog 1.0"
   
   parser = OptionParser(usage=usage, version=version)
   parser.add_option("-f", "--datfile", dest="dat_filename",
                  help="Read preprocess data from FILE", metavar="FILE")
   parser.add_option("-o", "--outfile", dest="output_filename",
                  help="Write output to FILE", metavar="FILE")
   parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="print status messages to stdout")
          
   (options, args) = parser.parse_args()
   
   if options.dat_filename is None:
      parser.error("--datfile option must be defined")
      sys.exit(2)

   if options.output_filename is None:
      parser.error("--outfile option must be defined")
      sys.exit(2)

                  
   return (options, args)

#===============================================================================
# create_file_banner
# creates a string that can be use as a banner for auto generated files
#===============================================================================
def create_file_banner(fname, description="None", start_comment="#", 
   end_comment="", start_block="", end_block="", style='none'):
   banner_str = \
'''$SB$SCM============================================================================$ECM
$SCM  Name:                                                                     $ECM
$SCM    $FILE_NAME $ECM
$SCM
$SCM  Description:                                                              $ECM
$SCM    $DESCRIPTION $ECM
$SCM                                                                            $ECM
$SCM Copyright (c) $YEAR by QUALCOMM, Incorporated.  All Rights Reserved.        $ECM
$SCM============================================================================$ECM
$SCM                                                                            $ECM
$SCM *** AUTO GENERATED FILE - DO NOT EDIT                                      $ECM
$SCM                                                                            $ECM
$SCM GENERATED: $DATE $ECM
$SCM============================================================================$ECM$EB
'''   
   if style == 'C':
      start_comment = "#"
      end_comment = ""
      start_block = "/*\n"
      end_block = "\n*/"
   
   elif style == 'C++':
      start_comment = "//"
      end_comment = ""
      start_block = ""
      end_block = ""
      
   elif style == 'asm':
      start_comment = ";"
      end_comment = ""
      start_block = ""
      end_block = ""
    
   elif style == 'make' or style == 'shell':
      start_comment = "#"
      end_comment = ""
      start_block = ""
      end_block = ""
   
   elif style == 'dos':
      start_comment = "REM "
      end_comment = ""
      start_block = ""
      end_block = ""
      
   banner_str = banner_str.replace('$SCM', start_comment)
   banner_str = banner_str.replace('$ECM', end_comment)
   banner_str = banner_str.replace('$SB', start_block)
   banner_str = banner_str.replace('$EB', end_block)
   banner_str = banner_str.replace('$YEAR', str(datetime.now().strftime('%Y')))
   banner_str = banner_str.replace('$DATE', str(datetime.now().ctime()))
   banner_str = banner_str.replace('$FILE_NAME', fname)
   banner_str = banner_str.replace('$DESCRIPTION', description)

   return banner_str

def CleanLine(aLine):
   aLine = aLine.replace('(','{')
   aLine = aLine.replace(')','}')
   aLine = aLine.replace('\n','')
   aLine = aLine.replace(':=','=')
   aLine = aLine.replace('?=','=')
   return aLine

def CleanVarName(aVarname):
   aVarname = aVarname.replace('.', '_')
   aVarname = aVarname.replace('export', '')
   aVarname = aVarname.replace('define', '')
   aVarname = re.sub('\s', '', aVarname) #get rid of whitespaces
   return aVarname

def CleanVarValue(aVarvalue):
   aVarvalue = aVarvalue.strip()
   return aVarvalue
   
def WriteData (options, file_handle, data, new_line="\n"):
   file_handle.write(data + new_line)
   
   if options.verbose:
      print data
   
   
def main():   
   # get args from cmd line
   (options, args) = parse_args()

   uses = "USES"
   lines = open(options.dat_filename, 'r').readlines()
   total = ""

   banner = create_file_banner(os.path.split(options.output_filename)[1])
   
   out_file = open(options.output_filename, 'w')
   
   WriteData(options, out_file, banner, new_line="")
   WriteData(options, out_file, "def exists(env):")
   WriteData(options, out_file, "   return env.Detect('usesflags')")
   WriteData(options, out_file, "")
   WriteData(options, out_file, "def generate(env):")

   VarNameDict = {}
   #count = 0
   for line in lines:
      line = line.lstrip()
      if line.find(uses, 0, 4)>-1:
         line = CleanLine(line)
         tempstr = line.split("=")
         VarName = tempstr[0]
         VarName = CleanVarName(VarName)
         VarValue = tempstr[1]
         VarValue = CleanVarValue(VarValue)
         
         if VarValue == "yes":
            vUsesFlag = True
         else:
            vUsesFlag = False
            
         if vUsesFlag == True:
            VarNameDict[VarName] = True
   
   # sort keys and write file
   #import pdb; pdb.set_trace()
   uses_flags = sorted(VarNameDict.iterkeys())

   for uflag in uses_flags:
      WriteData(options, out_file, "   env.Replace(%s = True)" % uflag)
               
   WriteData(options, out_file, "   env.Replace(USES_FLAGS = %s)" % str(uses_flags))
   WriteData(options, out_file, "   return None")
   out_file.close()

#run
main()