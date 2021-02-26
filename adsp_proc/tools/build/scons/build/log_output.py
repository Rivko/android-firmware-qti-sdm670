'''Log Output'''
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
#  $Header: //components/rel/scons.qc/1.1/build/log_output.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Change: 7115037 $
#
#===============================================================================

USAGE = '''log_ouput.py:

Usage:

      log_ouput.py <log_file_name> <command_to_execute>

   Example:

      log_output.py build-log.txt -- %~dp0..\SCons\scons %*
'''

import sys
import subprocess

try:
    LOG_FN = sys.argv[1]
    CMD = sys.argv[3:]
except:
    print "Error, Invalid Command Line:", sys.argv, '\n'
    print USAGE 
    sys.exit(1)

LOG = open(LOG_FN, 'a')

TASK = subprocess.Popen(CMD, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
CHAR = TASK.stdout.read(1)
while CHAR:
    LOG.write(CHAR)
    sys.stdout.write(CHAR)
    CHAR = TASK.stdout.read(1)
TASK.wait()

LOG.close()
sys.exit(TASK.returncode)
