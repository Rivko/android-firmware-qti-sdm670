#===============================================================================
#
# Clean Unused Defines from target.builds
#
# GENERAL DESCRIPTION
# 
#    This script is one of several cleanup scripts used to help identify
#    unused code in the Target Build Configuration (tbc) files.
# 
#    This script reads the results of find_unused_defines.py from stdin, then
#    removes these from the .builds file.  It doesn't actually modify the
#    .builds file in place, instead, it reads in the .builds file, and then
#    writes a new .builds file stdout.
# 
# PARAMETERS:
# 
#    None, input received from stdin
# 
# OUTPUT:
# 
#    Revised .builds file, written to stdout
# 
#    Error messages are written to stderr
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/tcfg/tools/clean_unused_defines.py#1 $
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

#---------------------------------------------------------------------
# Function main()
# 
# Main function for the module.
#---------------------------------------------------------------------

def main (unused_syms_file_name):

   #---------------------------------------------------------------------
   # Find the necessary files.
   #---------------------------------------------------------------------

   # Find paths for the local build.
   tcfg_tools_path = os.path.dirname(__file__)
   build_root = tcfg_tools_path.replace('tools/build/scons/tcfg/tools', '')  # in case we're on linux
   build_root = tcfg_tools_path.replace('tools\\build\\scons\\tcfg\\tools', '')  # in case we're on windows
   build_ms_path = build_root + "build/ms/"

   # Get a list of all symbols defined
   unused_syms = set()
   unused_syms_file = open(unused_syms_file_name)
   for line in unused_syms_file.readlines():
      line = line.strip()
      if line.find(' ') == -1:
         # Ignore lines with spaces
         unused_syms.add(line)

   # Read the .builds file, and echo all line that define an unused symbol
   build_file = open(build_ms_path + "target.builds", 'r')
   for line in build_file.readlines():
      write_line = True
      clean_line = line.lstrip()
      if clean_line.startswith('define'):
         sym = clean_line.split()[1]
         sym = sym.split('(')[0]  # Macros have '(' immediately after the symbol
         if sym in unused_syms:
            write_line = False
      if write_line:
         # Echo the line to stdout
         print line.strip('\n')

#=====================================================================
# Call main
#=====================================================================
main(sys.argv[1])

