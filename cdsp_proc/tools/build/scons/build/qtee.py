'''Pipe input to both STDOUT and a log file'''
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
# $Header: //components/rel/scons.qc/1.1/build/qtee.py#3 $
#
#===============================================================================

USAGE = '''
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

OPEN_MODE = 'w'
OUT_FILE_FN = None

for arg in sys.argv[1:]:
    if arg.startswith('-'):
        OPEN_MODE = arg[1]
    else:
        OUT_FILE_FN = arg

if OUT_FILE_FN is None:
    print USAGE 
    sys.exit(1)

OUT_FILE = open(OUT_FILE_FN, OPEN_MODE)

while True:
    CHAR = sys.stdin.read(1)
    if not CHAR:
        break
    sys.stdout.write(CHAR)
    OUT_FILE.write(CHAR)

OUT_FILE.close()
