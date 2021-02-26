#===============================================================================
#
# CoreBSP MODEM tool rules
#
# GENERAL DESCRIPTION
#    rules build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/qdsp6_defs.py#1 $
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
#
#===============================================================================
import sys
import os
import string

# Assembly QDSP6 compile flags (note first pass is through armcc using -E option then passed to armas, see 
# build rule below
asm_dflags = '-DCUST_H=\\"${CUST_H}\\" -D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 ' \
   '-D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ -DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC'

# standard QDSP6 compile flags
qdsp6_dflags = '-DCUST_H=\\"${CUST_H}\\" -D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 ' \
      '-D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ -DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC ' \
      '-DIMAGE_QDSP6_PROC -DBUILD_TARGET=\\"${BUILD_ID}\\" -DBUILD_VER=\\"${BUILD_VER}\\" ' \
      '-DBUILD_ASIC=\\"${BUILD_ASIC}\\" -DFEATURE_RUN_ON_SINGLE_HWTHREAD -DARCH_QDSP6 ' \
      '-DTHREAD_SAFE -DCONFIG_RESTRICTED_VM=1 -DMACHINE_Q6SIM -DENDIAN_LITTLE -DMY_GCC_FILE=\\"${SOURCE.name}\\"' 
      
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('qdsp6tools_defs')

def generate(env):
   # Assembly common flags
   env.Replace(ASM_DFLAGS = asm_dflags)
   
   # CC (apps) common compile flags
   env.Replace(CC_DFLAGS = qdsp6_dflags)
   
   if not env.has_key('BUILD_TOOL_CHAIN'):
      env.Append(CC_DFLAGS = ' -DASSERT=ASSERT_FATAL')
