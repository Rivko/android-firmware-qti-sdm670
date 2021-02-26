#===============================================================================
#
# HEX file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/hex_builder.py#1 $
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

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hex_builder')

def generate(env):
   hex_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def hex_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   hex_act = env.GetBuilderAction(hex_builder)
   hex_bld = env.Builder(action = hex_act,
                         suffix = '.hex')

   env.Append(BUILDERS = {'HexBuilder' : hex_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def hex_emitter(target, source, env):

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def hex_builder(target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   cmds = ["$HEXTOOL ${ARMHEX_I32_CMD} ${ARMHEX_OUTPUT_CMD} " + target_str + " " + source_str]

   data, err, rv = env.RunCmds(target, source, cmds)
   
   print data

   return None