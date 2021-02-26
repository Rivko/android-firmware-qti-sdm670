#===============================================================================
#
# CBSP Kernel Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2009-2012 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#===============================================================================
import os
import subprocess
import string
import re
from SCons.Script import *
import shutil

#------------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('add_dummy_seg_builder')

def generate(env):   
   add_dummy_seg_generate(env)

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
# Add dummy segment Emitter to add clean actions and other misc
#----------------------------------------------------------------------------
def add_dummy_seg_emitter(target, source, env):
   if 'USES_DUMMY_SEG' not in env:
      env.Depends(target, env.RealPath("${IMAGE_PATCH_CMD}"))
   return (target, source)

#----------------------------------------------------------------------------
# Add dummy segment definition
#----------------------------------------------------------------------------
def add_dummy_seg_generate(env):
   env.Replace(IMAGE_PATCH_CMD = "${BUILD_ROOT}/core/bsp/build/scripts/image_patch.py")
   add_dummy_seg_act = env.GetBuilderAction(add_dummy_seg_builder)
   add_dummy_seg_bld = env.Builder(action = add_dummy_seg_act,
                                      emitter = add_dummy_seg_emitter,
                                      suffix = '.elf')

   env.Append(BUILDERS = {'AddDummySegBuilder' : add_dummy_seg_bld})

#----------------------------------------------------------------------------
# Add a dummy segment builder
# source[0]: elf
# source[1]: xml
#----------------------------------------------------------------------------
def add_dummy_seg_builder(target, source, env):
   target_full = str(target[0])
   source_elf_full = str(source[0])
   source_xml_full = str(source[1])
   #-------------------------------------------------------------------------
   # Adding a dummy segment to the file
   #-------------------------------------------------------------------------
   # Copy source elf to target directory   
   shutil.copy(source_elf_full, target_full)
   
   cmd = "".join (["${PYTHONCMD} ${IMAGE_PATCH_CMD} ", target_full, " ", source_xml_full])
   env.ExecCmds(str(cmd))   

   return None

