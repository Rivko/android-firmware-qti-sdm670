#===============================================================================
#
# Log Ouput
#
# GENERAL DESCRIPTION
#    Split output from a command to both stdout and a log file.
# Copyright (c) 2011-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/build/log_output.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Change: 18261931 $
#
#===============================================================================

usage = '''log_ouput.py:

Usage:

      log_ouput.py <log_file_name> <command_to_execute>

   Example:

      log_output.py build-log.txt -- %~dp0..\SCons\scons %*
'''

import sys
import subprocess

try:
   log_fn = sys.argv[1]
   cmd = sys.argv[3:]
except:
   print "Error, Invalid Command Line:", sys.argv, '\n'
   print usage
   sys.exit(1)

log = open(log_fn, 'a')

task = subprocess.Popen(cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
c = task.stdout.read(1)
while c:
   log.write(c)
   sys.stdout.write(c)
   c = task.stdout.read(1)
task.wait()

log.close()
sys.exit(task.returncode)
