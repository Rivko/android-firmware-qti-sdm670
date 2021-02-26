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
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/modem_defs.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Author: pwbldsvc $
#  $Change: 18261931 $
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

# Assembly MODEM compile flags (note first pass is through armcc using -E option then passed to armas, see 
# build rule below
asm_dflags = '-D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 -D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ '\
   '-DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC'

# standard MODEM compile flags
modem_dflags = '-D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 ' \
      '-D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ -DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC '
      
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('modemtools_defs')

def generate(env):
   # Assembly common flags
   env.Replace(ASM_DFLAGS = asm_dflags)
   
   # CC (apps) common compile flags
   env.Replace(CC_DFLAGS = modem_dflags)
   
   if not env.has_key('BUILD_TOOL_CHAIN'):
      env.Append(CC_DFLAGS = ' -DASSERT=ASSERT_FATAL')
