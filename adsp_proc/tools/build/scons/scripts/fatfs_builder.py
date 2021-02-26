'''
   CBSP Builders build rules

   GENERAL DESCRIPTION
       Contains builder(s) to create FAT file system BIN file
'''
#============================================================================
#============================================================================
#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create FAT file system BIN file
#
# Copyright  2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/fatfs_builder.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
# 
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
# 2010-11-17   rh      Initial creation
# 
#============================================================================
import os
#import subprocess
#import string
#import re
#import glob
import shutil
#import sys

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
CPFATFS_TOOLS_ROOT = "${BUILD_ROOT}/core/storage/tools/fattool"
CPFATFS_TOOLS_CMD = "${BUILD_ROOT}/core/storage/tools/fattool/cpfatfs${EXE_EXT}"
CPFATFS_FAT_CONTAINER = CPFATFS_TOOLS_ROOT + "/fat.bin"

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('fatfs_build')

def generate(env):
    '''
       generate
    '''
    fatfs_generate(env)

#============================================================================
# FATFS build rules
#============================================================================
def fatfs_generate(env):
    '''
       fatfs_generate

       FATFS build rules
    '''
    #env.AddMethod(fatfs_builder, "FatfsBuilder")

    fatfs_act = env.GetBuilderAction(fatfs_builder)
    fatfs_bld = env.Builder(action = fatfs_act,
                            emitter = fatfs_emitter,
                            suffix = '.mbn')

    env.Append(BUILDERS = {'FatfsBuilder' : fatfs_bld})
   
#----------------------------------------------------------------------------
# FATFS builder function
#----------------------------------------------------------------------------
def fatfs_builder(target, source, env):
    '''
       fatfs_builder

       FATFS builder function
    '''
    #target_base = os.path.splitext(str(target))[0]
    target_full = os.path.abspath(str(target[0]))
    source_full = os.path.abspath(str(source[0]))

    if not env.has_key('TARGET_PATH'):
        raise RuntimeError, "TARGET_PATH is not defined"

    # Check if the target container exist or not, 
    # if not, copy the empty container
    if not os.path.isfile(target_full):
        shutil.copy(env.subst(CPFATFS_FAT_CONTAINER), target_full)

    cmds = [CPFATFS_TOOLS_CMD + " " + target_full + " " + \
            env.get('TARGET_PATH') + " " + source_full]

    data, err, rev = env.RunCmds(target, source, cmds)


#----------------------------------------------------------------------------
# FATFS builder emitter
#----------------------------------------------------------------------------
def fatfs_emitter(target, source, env):
    '''
       fatfs_emitter

       FATFS builder emitter
    '''

    env.Depends(target, env.RealPath(CPFATFS_TOOLS_CMD))
    return (target, source)
