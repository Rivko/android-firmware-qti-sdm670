#===============================================================================
#
# Hexagon Linker Control Script (LCS) file builder
#
# GENERAL DESCRIPTION
#    Build script
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/lcs_builder.py#1 $
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
import os
import subprocess
import string
import re
import shutil
import stat

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('lcs_builder')

def generate(env):
   lcs_generate(env)

#===============================================================================
# Linker File build rules
#===============================================================================


#-------------------------------------------------------------------------------
# Generate builder
#
def lcs_generate(env):
   lcs_act = env.GetBuilderAction(lcs_builder)
   lcs_bld = env.Builder(action = lcs_act,
                         emitter = lcs_emitter,
                         suffix = '.lcs')

   env.Append(BUILDERS = {'LcsBuilder' : lcs_bld})

#-------------------------------------------------------------------------------
# Emitter builder
#
def lcs_emitter(target, source, env):
   # Can also add dependencies here
   return (target, source)

#-------------------------------------------------------------------------------
# Builder for linker control script file
#
def lcs_builder(target, source, env):
   #import pdb;pdb.set_trace()
   target_full = str(target[0])
   source_full = str(source[0])

   # Only need to copy source lcs file for now.  Allows for adding 
   # required preprocessing steps in the future.
   shutil.copy (source_full, target_full)
   os.chmod(target_full, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
   return None
