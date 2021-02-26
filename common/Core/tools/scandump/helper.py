"""
===============================================================================

 helper.py

 GENERAL DESCRIPTION
    Generic helper functions

 Copyright (c) 2015 - 2016 by QUALCOMM Technologies Incorporated.
 All Rights Reserved.

 QUALCOMM Proprietary

-------------------------------------------------------------------------------

  $Header: //components/rel/core.glue/1.2/tools/scandump/helper.py#1 $
  $DateTime: 2016/09/20 11:34:52 $
  $Author: pwbldsvc $
  $Change: 11402994 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who         what, where, why
 --------   ---         ---------------------------------------------------------
 04/27/2015 AJCheriyan  Created for Hawker Automation
==============================================================================
"""




import subprocess, sys
import time

"""
@Description
    Function to check the current time
"""
def get_time():
    return time.time()

"""
@Description
    Function to display time as a neat string
"""
def pretty_time():
    return time.strftime("%A %d %b %Y %H:%M:%S", time.localtime())
"""
@Description
    Function to check the time delta between
    any set of python operations
"""
def timedelta(start_time):
    return (get_time() - start_time)


"""
@Description
 Function to terminate program with a message

@Notes
 Default is to disable print messages. Turn on the messages individually
 with the second arg or turn the global one on.
"""
def debug(msg, force=False):
    debug_enable = False
    if debug_enable or force:
        print msg


"""
@Description
 Exit program immediately with a simple print statement

@Notes
"""

def kill(msg):
    if msg:
        print "Error: " + str(msg)
    sys.exit(1)


"""
@Description
 Simple API to hexify a decimal number and return a string

@Notes
"""
def hexify(num):
    return str(hex(num))


"""
@Description
 Check if executing on a 64 bit windows platform

@Notes
"""

def IsWindows64():
    command = 'WMIC OS GET OSARCHITECTURE /Format:list'
    getosarch = subprocess.Popen(command, stderr = subprocess.PIPE, stdout = subprocess.PIPE)
    (cmdout, cmderr) = getosarch.communicate()

    if cmdout.strip().endswith('64-bit'):
        debug('Detected 64 bit Windows')
        return True
