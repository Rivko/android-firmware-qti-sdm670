#===============================================================================
#
# SCL build rules
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/scl_builder.py#1 $
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
import os
import subprocess
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('scl_builder')

def generate(env):
   scl_generate(env)

#===============================================================================
# SCL builder
#===============================================================================
def scl_generate(env):
   scl_act = env.GetBuilderAction(scl_builder)
   scl_bld = env.Builder(action = scl_act, 
                         source_scanner = SCons.Scanner.C.CScanner(),
                         emitter = scl_emitter,
                         suffix = '.scl', src_suffix = '.scl')

   env.Append(BUILDERS = {'SclBuilder' : scl_bld})

#-------------------------------------------------------------------------------
# SCL build emitter
#-------------------------------------------------------------------------------
def scl_emitter(target, source, env):
   
   # clone env and add defines and incpath
   envpp = env.Clone()
   stack_size = ' -DUSER_DEFINED_STACK_SIZE=%s' % (envpp.get('APP_STACK_SIZE', '0x8000'))
   heap_size = ' -DUSER_DEFINED_HEAP_SIZE=%s' % (envpp.get('APP_HEAP_SIZE', '0x8000'))
   envpp.Append(CCFLAGS = stack_size)
   envpp.Append(CCFLAGS = heap_size)

   scl_source = str(source[0])
   
   if len(source) > 1:
      scl_source_header = str(source[1])
      scl_target_path, scl_target_file = os.path.split(str(target[0]))
   
      envpp.Append(CPPDEFINES = [('CUST_L4_SCL_H','\\\"'+ scl_source_header +'\\\"')]) 
      envpp.Append(CPPPATH = [scl_target_path])
      
   if env.has_key('BUILD_BOOT_CHAIN'):
      envpp.Append(CCFLAGS = " ${ARM_ASM} ")
   

   # builder sets the ext, get target base name
   target_name = os.path.splitext(str(target[0]))[0] + '_scl'
   # use pre-process builder
   # alternatively, we can invoke the compiler command here itself, but for now, prefer to call the 
   # PreProcess builder.
   # eg: '$CC -E $CCFLAGS -DCUST_L4_SCL_H=\\"'+ scl_source_header_str +'\\" -I'+ scl_target_path +' $_CCCOMCOM '+ str(source[0]) +' -o '+ str(target[0]) +'.pp'
   scl_pp = envpp.PreProcess(target_name, source[0])
   
   # add dependency
   if len(source) > 1:
      env.Depends(scl_pp, str(source[1]))
   env.Depends(target, scl_pp)

   return (target, source)


#-------------------------------------------------------------------------------
# SCL build action
#-------------------------------------------------------------------------------
def scl_builder(target, source, env):
   scl_target_pp_name = os.path.splitext(str(target[0]))[0] + '_scl.pp'
   
   cmds = [
      '$PPASM -f '+ scl_target_pp_name +' -o '+ str(target[0])
   ]
   
   env.RunCmds(target, source, cmds)
   return None
