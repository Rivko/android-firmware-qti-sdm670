#===============================================================================
#
# Find Unused Defines from target.builds
#
# GENERAL DESCRIPTION
# 
#    This script is one of several cleanup scripts used to help identify
#    unused code in the Target Build Configuration (tbc) files.  This script
#    identifies symbols that are defined in target.builds, but are not used
#    elsewhere in the build.
# 
#    In theory, symbols that are not used anywhere in the build can safely be
#    removed from target.builds.  However, care should be taken to ensure the
#    following:
# 
#       1) The symbols aren't referenced by code that hasn't yet been brought
#          into the build.
# 
#       2) The symbols aren't referenced by files that this script doesn't
#          check.  This script is currently checking the following files:
# 
#                *.c, *.cpp, *.h, *.scl *.xml
# 
#          These are defined in the file_types_to_check variable below.  Please
#          double check this to see what files are checked.
# 
#          Please contact the Build Systems Team (qct.scons.arch) if new file
#          types need to be added to this list.
# 
# PARAMETERS:
# 
#    None.
# 
# OUTPUT:
# 
#    A list of symbol names.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/tcfg/tools/find_unused_defines.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/10/10   dlb'    Initial version.
#
#===============================================================================

import sys
import os
from shlex import shlex

#---------------------------------------------------------------------
# File Types to Check:  Modify this list to add/remove new file types
#---------------------------------------------------------------------
file_types_to_check = ['.c', '.cpp', '.h', '.scl', '.xml']

#---------------------------------------------------------------------
# Function main()
# 
# Main function for the module.
#---------------------------------------------------------------------

def main ():

   #---------------------------------------------------------------------
   # Find the necessary files.
   #---------------------------------------------------------------------

   # Find paths for the local build.
   tcfg_tools_path = os.path.dirname(__file__)
   build_root = tcfg_tools_path.replace('tools/build/scons/tcfg/tools', '')  # in case we're on linux
   build_root = tcfg_tools_path.replace('tools\\build\\scons\\tcfg\\tools', '')  # in case we're on windows
   build_ms_path = build_root + "build/ms/"
   tcfg_path = build_root + "tools/build/scons/tcfg"

   # Add search paths for our modules
   sys.path.append(tcfg_path)

   # Import needed libraries
   from tcfgparser import TargetConfigurationDb
   
   # Parse the .builds file
   tcfg = TargetConfigurationDb()
   if not tcfg.ParseBuildFile(build_ms_path + "target.builds"):
      raise()

   # Get a list of all symbols defined
   sym_list = set()
   for be in tcfg.AllBuildElements.values():
      for attr in be.attributes:
         if attr.type == 'define':
            sym_list.add(attr.name)

   # Now parse all files in the build to see if they use this symbol
   file_count = 0
   for path, dirs, files in os.walk(build_root):
      for file in files:
         file_ext = os.path.splitext(file)[1]
         if file_ext in file_types_to_check:
            file_count += 1
            new_syms = get_syms(os.path.join(path, file), file_count)
            # Remove all the new_syms from our sym_list
            sym_list -= new_syms
            # if sym_list is empty, we're done
            if len(sym_list) == 0:
               print file_count, "files processed"
               print "No unused symbols could be found"
               return

   print file_count, "files processed"
   print "The following symbols could not be found in the build:"
   for sym in sym_list:
      print "   ", sym

#---------------------------------------------------------------------
# Function get_syms()
# 
# Parse a file and return a list of symbols used in the file.
#---------------------------------------------------------------------

def get_syms (file_name, file_num):

   print >> sys.stderr, "Checking file", file_num, ":", file_name
   syms = set()
   input_file = open(file_name, 'r')
   input = shlex(input_file, file_name)
   input.debug = 0
   input.commenters = ''   # We need to do our own comment processing for 'c' style comments
   input.quotes = ''  # Strings cause too many problems, just ignore
   input.whitespace = ' \t\r'
   prev_token = ''
   in_c1_comment = False
   in_c2_comment = False
   try:
      i = 0                                     #zombie
      find_string = ['PRINTSIG"ULogAdversarial"']    #zombie
      while(1):
         token = input.get_token()
         if token == input.eof:
            break
         if token == find_string[i]:            #zombie
            i += 1                              #zombie
            if i >= len(find_string):           #zombie
               i = 0                            #zombie
               import pdb; pdb.set_trace()      #zombie
         else:                                  #zombie
            i = 0                               #zombie
         if in_c1_comment:
            if token == '/' and prev_token == '*':
               in_c1_comment = False
         elif in_c2_comment:
            if token == '\n':
               in_c2_comment = False
         else:
            if token[0] in input.wordchars:
               syms.add(token)
            elif token == '*' and prev_token == '/':
               in_c1_comment = True
            elif token == '/' and prev_token == '/':
               in_c2_comment = True
         prev_token = token

      input_file.close()

   except ValueError:
      print >> sys.stderr, input.error_leader(), "Error while parsing file.  Token is <"+token+">"
      print >> sys.stderr, sys.exc_info()[1]
      import pdb; pdb.set_trace()

   return syms

#=====================================================================
# Call main
#=====================================================================
#get_syms ("C:\\work\\8660_rim_l\\a0007a\\AMSS\\products\\8660\\core\\apttests\\power\\src\\power_daltf_ULog_test.c", 1)
main()

