
#=================================================================================
#   File Name: buildcmd.py
#
#   Python Script to call the QC-SCons build System for Badger
#
#   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#---------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/ms/build.py#1 $
#  $DateTime: 2018/08/07 05:36:15 $
#  $Change: 16823152 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when     who            what, where, why
# --------   ---        ----------------------------------------------------------
# 09/07/17   sm          Add sdm1000 support.
# 04/02/14   corinc      build\ms\build.cmd port to python version
# 04/16/14   corinc      fixed BUILD_FLAGS comma missing issue
#=================================================================================


import os
import re
import subprocess
import sys
import time
import argparse
import fnmatch
import shutil, errno




print "Entering buildcmd.py............"

#if we have a set environment batch file, call it.
OSPlatform=os.name
print "OSPlatform=", OSPlatform
if OSPlatform != 'posix':
   ENV_CMD = 'setenv.cmd'
   if os.path.exists(ENV_CMD):
      os.system(ENV_CMD)
      print 'Finished ' + ENV_CMD + ' ~~~'


parser = argparse.ArgumentParser()
parser.add_argument('allArgs', nargs=argparse.REMAINDER)
pargs = parser.parse_args()
print pargs.allArgs

# default is Multi-PD
MPFLAG = 'USES_MULTI_PD'
argLen = len(pargs.allArgs)
for i in xrange(argLen):
   print 'Parameter: ', pargs.allArgs[i]
   if pargs.allArgs[i] == 'adsp_core_images':
      MPFLAG = 'CORE_KERNEL_CONFIGURATION,SPD'
   elif pargs.allArgs[i] == 'adsp_mpd_core_images':
      MPFLAG = 'CORE_KERNEL_CONFIGURATION,USES_MULTI_PD'


#CHIPSET passed from top level is CHIPSET
CHIPSET = os.environ.get('CHIPSET', '')
print 'CHIPSET passed from top level is ', CHIPSET
if CHIPSET == 'msm8974':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8974'
   os.environ['MSM_ID'] = MSM_ID = '8974'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8974'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8974'
   os.environ['CHIPSET'] = CHIPSET = 'msm8974'
elif CHIPSET == 'mdm9x25':
   os.environ['BUILD_ASIC'] = BUILD_ASIC='9x25A'
   os.environ['MSM_ID'] = MSM_ID='9x25'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM='9x25'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY='9x25'
   os.environ['CHIPSET'] = CHIPSET='mdm9x25'
elif CHIPSET == 'msm8x26':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8x26'
   os.environ['MSM_ID'] = MSM_ID = '8974'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8974'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8974'
   os.environ['CHIPSET'] = CHIPSET = 'msm8974'
elif CHIPSET == 'msm8x10':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8x10'
   os.environ['MSM_ID'] = MSM_ID = '8x10'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8x10'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8x10'
   os.environ['CHIPSET'] = CHIPSET = 'msm8x10'
elif CHIPSET == 'apq8084':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8084'
   os.environ['MSM_ID'] = MSM_ID = '8084'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8084'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8084'
   os.environ['CHIPSET'] = CHIPSET = 'apq8084'
elif CHIPSET == 'mdm9x35':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '9x35'
   os.environ['MSM_ID'] = MSM_ID = '9x35'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '9x35'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '9x35'
   os.environ['CHIPSET'] = CHIPSET = 'mdm9x35'
elif CHIPSET == "mpq8094":
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8094'
   os.environ['MSM_ID'] = MSM_ID = '8094'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8094'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8094'
   os.environ['CHIPSET'] = CHIPSET = 'mpq8094'
elif CHIPSET == "msm8994":
   os.environ['BUILD_ASIC'] = BUILD_ASIC='8994'
   os.environ['MSM_ID'] = MSM_ID = '8994'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8994'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8994'
   os.environ['CHIPSET'] = CHIPSET = 'msm8994'
elif CHIPSET == "msm8952":
   os.environ['BUILD_ASIC'] = BUILD_ASIC='8952'
   os.environ['MSM_ID'] = MSM_ID = '8952'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8952'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8952'
   os.environ['CHIPSET'] = CHIPSET = 'msm8952'   
elif CHIPSET == "msm8992":
   os.environ['BUILD_ASIC'] = BUILD_ASIC='8992'
   os.environ['MSM_ID'] = MSM_ID = '8992'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8992'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8992'
   os.environ['CHIPSET'] = CHIPSET = 'msm8992'
elif CHIPSET == 'msm8996':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8996'
   os.environ['MSM_ID'] = MSM_ID = '8996'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8996'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8996'
   os.environ['CHIPSET'] = CHIPSET = 'msm8996'
elif CHIPSET == 'msm8998':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '8998'
   os.environ['MSM_ID'] = MSM_ID = '8998'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '8998'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '8998'
   os.environ['CHIPSET'] = CHIPSET = 'msm8998'   
elif CHIPSET == 'sdm845':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '845'
   os.environ['MSM_ID'] = MSM_ID = '845'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '845'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '845'
   os.environ['CHIPSET'] = CHIPSET = 'sdm845'   
elif CHIPSET == 'sdm670':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '670'
   os.environ['MSM_ID'] = MSM_ID = '670'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '670'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '670'
   os.environ['CHIPSET'] = CHIPSET = 'sdm670'   
elif CHIPSET == 'sdm660':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '660'
   os.environ['MSM_ID'] = MSM_ID = '660'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '660'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '660'
   os.environ['CHIPSET'] = CHIPSET = 'sdm660'   
elif CHIPSET == 'sdm855':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '855'
   os.environ['MSM_ID'] = MSM_ID = '855'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '855'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '855'
   os.environ['CHIPSET'] = CHIPSET = 'sdm855'   
elif CHIPSET == 'sdm1000':
   os.environ['BUILD_ASIC'] = BUILD_ASIC = '1000'
   os.environ['MSM_ID'] = MSM_ID = '1000'
   os.environ['HAL_PLATFORM'] = HAL_PLATFORM = '1000'
   os.environ['TARGET_FAMILY'] = TARGET_FAMILY = '1000'
   os.environ['CHIPSET'] = CHIPSET = 'sdm1000'   
else:
   print '---- Non-supported CHIPSET to setup BUILD_ASIC/MSM_ID/HAL_PLATFORM/TARGET_FAMILY/CHIPSET value'


# Setup Default Values
os.environ['BUILD_ID']=BUILD_ID= os.environ['VARIANT_NAME']
os.environ['BUILD_VER']=BUILD_VER= '0002'
BUILD_FLAGS = os.environ.get('BUILD_FLAGS', '')
USES_FLAGS = os.environ.get('USES_FLAGS', '')
# set Common Flags
BUILD_CMD = 'BUILD_ID='+BUILD_ID + ' BUILD_VER='+BUILD_VER + ' MSM_ID='+MSM_ID + ' HAL_PLATFORM='+HAL_PLATFORM + ' TARGET_FAMILY='+TARGET_FAMILY + ' BUILD_ASIC='+BUILD_ASIC + ' CHIPSET='+CHIPSET + ' USES_FLAGS='+MPFLAG
#
if BUILD_FLAGS == '':
   BUILD_CMD += ' ' + ' '.join(pargs.allArgs)
else:
   BUILD_CMD += ',' + BUILD_FLAGS + ' ' + ' '.join(pargs.allArgs)

print "scons analytics are disabled"
BUILD_CMD +=  ' --noanalytic '
   
print 'BUILD_CMD = ', BUILD_CMD
#==================================================================
#     Setup Hexagon Environment
#==================================================================

if os.environ.get('HEXAGON_RTOS_RELEASE', '') == '':
   os.environ['HEXAGON_RTOS_RELEASE'] = '5.1.05'

if os.environ.get('HEXAGON_Q6VERSION', '') == '':
   os.environ['HEXAGON_Q6VERSION'] = 'v55'

if os.environ.get('HEXAGON_ROOT', '') == '':
   os.environ['HEXAGON_ROOT'] = 'C:\Qualcomm\HEXAGON_Tools'

if os.environ.get('HEXAGON_REQD_Q6VERSION', '') == '':
   os.environ['HEXAGON_REQD_Q6VERSION'] = 'v55'



# Setup Paths
cwd_dir = os.path.join(os.getcwd(), "../../")
cwd_dir=os.path.realpath(cwd_dir)
#os.getcwd(/../..)
print 'cwd_dir=', cwd_dir
SCONS_BUILD_ROOT = cwd_dir
SCONS_TARGET_ROOT = SCONS_BUILD_ROOT+'/..'
TOOLS_SCONS_ROOT = SCONS_BUILD_ROOT+'/tools/build/scons'

#print 'SCONS_BUILD_ROOT=', SCONS_BUILD_ROOT
#print 'SCONS_TARGET_ROOT=', SCONS_TARGET_ROOT
#print 'TOOLS_SCONS_ROOT=', TOOLS_SCONS_ROOT
print 'Calling rename-log.cmd'
#CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd
#CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f %TOOLS_SCONS_ROOT%\build\start.scons %BUILD_CMD%
if OSPlatform == 'posix':
   SCONS_CMD1=''.join([TOOLS_SCONS_ROOT, '/build/rename-log.sh'])
   SCONS_CMD2=''.join([TOOLS_SCONS_ROOT, '/build/build.sh -f ', TOOLS_SCONS_ROOT,'/build/start.scons ', BUILD_CMD])
else:
   SCONS_CMD1=''.join([TOOLS_SCONS_ROOT, '\\build\\rename-log.cmd'])
   SCONS_CMD2=''.join([TOOLS_SCONS_ROOT, '\\build\\build.cmd -f ', TOOLS_SCONS_ROOT,'\\build\\start.scons ', BUILD_CMD])
#print 'SCONS_CMD1=', SCONS_CMD1
#print 'SCONS_CMD2=', SCONS_CMD2
os.system(SCONS_CMD1)
os.system(SCONS_CMD2)
#@exit /B %ERRORLEVEL%
print 'Finished buildcmd.py ~~~'
exit()

