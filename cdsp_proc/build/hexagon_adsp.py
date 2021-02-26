#===============================================================================
#
# CoreBSP HEXAGON tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#
# Copyright (c) 2009-2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/hexagon_adsp.py#1 $
#  $DateTime: 2018/08/07 05:36:15 $
#  $Author: pwbldsvc $
#  $Change: 16823152 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who         what, where, why
# --------   ---         ---------------------------------------------------------
# 05/08/14   corinc      Check the tool version for the correct bin path
# 08/16/16   c_narkum    Remove redundant code, which is already present in hexagon.py
#===============================================================================
import sys
import os
import string
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hexagon_adsp')

def generate(env):
   #===============================================================================
   # load hexagon.py to environment
   #===============================================================================
   env.Tool('hexagon', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])

   #===============================================================================
   # figure out what tools set will be using
   #===============================================================================
   # The following environment variables must be defined prior to using this make
   # file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
   # the GCC tools.
   hexagon_rtos_release = env.get('HEXAGON_RTOS_RELEASE')
   qc_ts = os.environ.get('OS_ENV_QCOM_TIME_STAMP', None)
   engg_ts = os.environ.get('OS_ENV_ENGG_TIME_STAMP', None)
   image = os.environ.get('IMAGE', None)
   hexagon_major_tool_ver = int(hexagon_rtos_release[0])
      
   # init variables to environment
   env.Replace(IMAGE = image)
   if 'BUILD_BAREBONE' not in env:
       env.Replace(QCOM_TIME_STAMP = qc_ts)
       env.Replace(ENGG_TIME_STAMP = engg_ts)
   
   #----------------------------------------------------------------------------
   # common defines for all components
   #----------------------------------------------------------------------------

   env.Append(CFLAGS='-D__qdsp6__=1')
   if not env.IsTargetEnable(['SSC_ADSP_USER', 'CORE_Q6_ROOT']) or env['IMAGE'] in ['cdsp'] :
       # env.AddUsesFlags('USES_STACK_PROTECTOR')
       env.Append(CFLAGS='-fstack-protector')
      
   env.Append(CFLAGS = '-G0')
   if env['IMAGE'] in ['cdsp']:
        env.Replace(BUILD_CCFLAGS = " -mhvx "+ env['BUILD_CCFLAGS'])

   
   # This option would make the libraries to use zero for timestamps and uids/gids
   # to combined the 'D' with existing ARFLAGS(defined by hexagon.py), e.g -rcD;  separation of switches will not be supported with tool 8.0 onwards
   env.Replace(HEXAGONAR_ADSP_LIB_UID="D")
   env.Append(ARFLAGS = '$HEXAGONAR_ADSP_LIB_UID')

   if 'BUILD_BAREBONE' in env:
      env.Append(CFLAGS='-DBAREBONE_ADSP=1')
      env.AddUsesFlags('USES_DISABLE_LOC')

   if 'USES_AVS_TEST' in env or env.get('CHIPSET').startswith('mdm') or os.environ.get('BUILD_ACT')=='SIM' or 'BUILD_BAREBONE' in env or 'BUILD_SLPI' in env:
      env.AddUsesFlags('USES_PLATFORM_MINIMAL')
   else:
      env.AddUsesFlags('USES_PLATFORM_FS')
      
   if ARGUMENTS.get('SIM') == "1" or ARGUMENTS.get('SIM') == "TRUE":
      env.AddUsesFlags(['USES_AVS_TEST', 'USES_SENSORS_INTEGRATION_ENABLED'])

   env.AddUsesFlags('GEN_SHARED_LIBS') 
   if not env.GetUsesFlag('USES_CDSP_IMG'):
      env.AddUsesFlags('AUDIO_IN_USERPD')
   if 'BUILD_BAREBONE' not in env and 'USES_CDSP_IMG' not in env:
      env.AddUsesFlags('USES_PD_RESTARTABLE')
   #Flag to generate Internal ELF composed of unstrip libs of AVS
   if 'USES_NO_INTERNAL_ELF' not in env:
       env.AddUsesFlags('USES_INTERNAL_ELF_GEN')

   env.AddTcfgFlag('ROOT_IMG_NAME', 'ROOT_${BUILD_ID}')
   env.AddTcfgFlag('AUDIO_IMG_NAME', 'AUDIO_${BUILD_ID}')
   env.AddTcfgFlag('SENSOR_IMG_NAME', 'SENSOR_${BUILD_ID}')
   
   #Add ADSP Image specific linker inputs here
   env.LoadToolScript('qdsp6_defs_adsp', toolpath = ['${BUILD_ROOT}/build/ms'])
   #G0 flag is required so that it is uniform. kernel libs are compiled with G0
   print 'COMPILER= ', os.environ.get('COMPILER', None)
   if hexagon_major_tool_ver >= 7:
      # temporary adding the hexgon warning flags for LLVM, -Wno-cast-align
      env.Replace(HEXAGON_WARN = "-Wall -Wpointer-arith -Wno-cast-align")
   # builder to support dynamic linking -- 3 component need this builder core_libs.scons/devcfg_img.scons/platform_libs.scons
   env.LoadToolScript('platform_builders', toolpath = ['${BUILD_ROOT}/platform/build'])

