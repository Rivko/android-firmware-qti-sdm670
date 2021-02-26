#===============================================================================
#
# Pipe input to both STDOUT and a log file.
#
# GENERAL DESCRIPTION
#    Pipe input to both STDOUT and a log file.
#
# Copyright (c) 2011-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/scons.tz/2.0.2/build/scons/build/qtee.py#1 $
#
#===============================================================================

usage = '''
qtee.py:

   Pipe input to both STDOUT and the specified log file.

Usage:

   qtee.py <open_mode> <log_file_name>

   open_mode:

      Specifies the mode in which to open the log file.  Valid values are:

         -a: append
         -w: write (default)

   log_file_name:

      Specifies the name of the log file to write to.

Example:

   qtee.py -a build-log.txt

'''

import sys

# Parse command line arguments

open_mode = 'w'
out_file_fn = None

for arg in sys.argv[1:]:
   if arg.startswith('-'):
      open_mode = arg[1]
   else:
      out_file_fn = arg

if out_file_fn is None:
   print usage
   sys.exit(1)

out_file = open(out_file_fn, open_mode)

while True:
   char = sys.stdin.read(1)
   if not char:
      break
   sys.stdout.write(char)
   out_file.write(char)

out_file.close()
