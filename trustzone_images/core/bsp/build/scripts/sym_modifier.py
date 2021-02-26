#===============================================================================
#
# CoreBSP Symbol Extract
#
# GENERAL DESCRIPTION
#    Removes __stack_chk_fail symbol from source symbol file if any and copy
#    the result in target file
#
# Copyright (c) 2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/sym_modifier.py#1 $
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
# 08/18/16   ah      Initial version
#===============================================================================
import os
import re
import sys

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sym_modifier')

def generate(env):
   sym_modifier_generate(env)

def sym_modifier_generate(env):
  bld = env.Builder(action = env.GetBuilderAction(sym_modifier),
                    suffix = '.sym')
  env.Append(BUILDERS = {'SymModifierBuilder' : bld})

#------------------------------------------------------------------------------
# SYM Modifier
#------------------------------------------------------------------------------
def sym_modifier(target, source, env):
   sym_src_str = os.path.abspath(str(source[0]))
   sym_dst_str = os.path.abspath(str(target[0]))

   file = open(sym_src_str, 'r')
   data = file.readlines()
   file.close()

   file = open(sym_dst_str, 'w+')
   for line in data:
     fields = line.split()
     if fields[0] == "__stack_chk_fail":
       continue
     if fields[0] == "__stack_chk_guard":
       continue
     file.write(line)
   file.close()

   return None



