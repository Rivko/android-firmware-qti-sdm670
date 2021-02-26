#===============================================================================
#
# Common CC additions
#
# GENERAL DESCRIPTION
#    common CC additions; such as generate preprocess,  mixasm, hdeptree
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/comcc_addt.py#1 $
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
import string
import re
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('comcc_addt')

def generate(env):
   #============================================================================
   # Publish APIs
   #============================================================================
   # Add prepend and append APIs to modify CC, CXX, AS cmds
   env.AddMethod(prepend_action, "PrependActionByName")
   env.AddMethod(append_action, "AppendActionByName")
   env.AddMethod(prepend_cc_action, "PrependCCAction")
   env.AddMethod(append_cc_action, "AppendCCAction")
   env.AddMethod(prepend_cxx_action, "PrependCXXAction")
   env.AddMethod(append_cxx_action, "AppendCXXAction")
   env.AddMethod(prepend_as_action, "PrependASAction")
   env.AddMethod(append_as_action, "AppendASAction")

   #============================================================================
   # ALTERNATE CC options
   #============================================================================
   preprocess = env.GetOption('preprocess')
   mixasm = env.GetOption('mixasm')
   hdeptree = env.GetOption('hdeptree')
   verbose = int(env.GetOption('verbose'))
   
   #import pdb; pdb.set_trace()
   
   #----------------------------------------------------------------------------
   # set preprocess option rules
   #----------------------------------------------------------------------------
   if preprocess or hdeptree:
   
      #-------------------------------------------------------------------------
      # Updates dependant on verbose levels
      #-------------------------------------------------------------------------
      if verbose >= 2:
         env.Append(HDEPTREECOM = " -v")
         
      #-------------------------------------------------------------------------
      # set hdeptree option rules
      # Only works if preprocess is ON, needs to be done after preprocess is 
      # done. Since the options are prepend or append to CC cmd, then we need 
      # to queue it up, this rules goes in and the preprocess will get prepended
      # thus executing before the hdeptree rule.
      #-------------------------------------------------------------------------
      if hdeptree:
         env.PrependCCAction('$HDEPTREECOM', action_raw_text='$HDEPTREECOMSTR')
         env.PrependCXXAction('$HDEPTREECOM', action_raw_text='$HDEPTREECOMSTR')
         env.AppendASAction('$HDEPTREECOM', action_raw_text='$HDEPTREECOMSTR')

      #-------------------------------------------------------------------------
      # Replace normal CCCOM with this one that will preprocess each file first   
      # take the normal CC preprocess command and change the target since the 
      # target to compile the .o is a .o and not the .pp we need
      # also since the TEMP class doesn't like when we muck with the TARGET 
      # variable this way, we need to remove TEMPFILE and re add it but passing
      # a variable instead of the original string.
      #-------------------------------------------------------------------------
      ppcccom = env['CCPPCOM']
      ppcccom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.pp', ppcccom)
      ppcccom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ppcccom)
      env.Replace(PPCCCOM = ppcccom)
      
      env.PrependCCAction('$CCPPCL_ACOM', action_raw_text='$CCPPCLCOM_ASTR')
      env.PrependCCAction('${TEMPFILE("$PPCCCOM")}', action_raw_text='$CCPPCOM_ASTR')
      
      #-------------------------------------------------------------------------
      # replace normal CXXCOM with this one that will preprocess each file first
      #-------------------------------------------------------------------------
      ppcxxcom = env['CXXPPCOM']
      ppcxxcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.pp', ppcxxcom)
      ppcxxcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ppcxxcom)
      env.Replace(PPCXXCOM = ppcxxcom)
      
      env.PrependCXXAction('$CXXPPCL_ACOM', action_raw_text='$CXXPPCLCOM_ASTR')
      env.PrependCXXAction('${TEMPFILE("$PPCXXCOM")}', action_raw_text='$CXXPPCOM_ASTR')

   #----------------------------------------------------------------------------
   # set mixasm option rules
   #----------------------------------------------------------------------------
   if mixasm:
      # replace normal CCCOM with this one that will preprocess each file first   
      ccmixcom = env['CCMIXCOM']
      ccmixcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.mix', ccmixcom)
      ccmixcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ccmixcom)
      env.Replace(CCMIXCOM = ccmixcom)
      
      env.PrependCCAction('${TEMPFILE("$CCMIXCOM")}', action_raw_text='$CCMIXCOM_ASTR')

      # replace normal CXXCOM with this one that will preprocess each file first      
      cxxmixcom = env['CXXMIXCOM']
      cxxmixcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.mix', cxxmixcom)
      cxxmixcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', cxxmixcom)
      env.Replace(CXXMIXCOM = cxxmixcom)
      
      env.PrependCXXAction('${TEMPFILE("$CXXMIXCOM")}', action_raw_text='$CXXMIXCOM_ASTR')      

   #-------------------------------------------------------------------------------
   # Custom Builders
   #-------------------------------------------------------------------------------
   
   # pre process
   PreProcessAction = SCons.Action.Action('$CCPPCOM', '$CCPPCOMSTR')
   PreProcessBuilder = Builder(
      source_scanner = SCons.Scanner.C.CScanner(),
      action = PreProcessAction, 
      suffix = '.pp')
   env.Append(BUILDERS = {'PreProcess' : PreProcessBuilder})
   
   # pre process clean
   PreProcessCleanAction = SCons.Action.Action('$CCPPCLCOM', '$CCPPCLCOMSTR')
   PreProcessCleanBuilder = Builder(
      action = PreProcessCleanAction, 
      suffix = '.i')
   env.Append(BUILDERS = {'PreProcessClean' : PreProcessCleanBuilder})
   
   # mix output process
   AsmInterleaveAction = SCons.Action.Action('$CCMIXCOM', '$CCMIXCOMSTR')
   AsmInterleaveBuilder = Builder(
      source_scanner = SCons.Scanner.C.CScanner(),
      action = PreProcessAction, 
      suffix = '.mix')
   env.Append(BUILDERS = {'AsmInterleave' : AsmInterleaveBuilder})

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def prepend_action(env, target_action_name, target_action_str, action, action_text='Generating ', action_raw_text=None):
   # create new action 
   if action_raw_text is not None:
      cmd_action = SCons.Action.Action(action, action_raw_text)
   else:
      cmd_action = env.GetBuilderAction(action, action_text)
   
   # get oroginal cc action and prepend new action
   org_action = env[target_action_name]
   
   if type(org_action) is str:
      org_action = [SCons.Action.Action(org_action, env[target_action_str])]

   #import pdb; pdb.set_trace()
   #env.Replace(eval(target_action_name) = [cmd_action] + org_action)
   env[target_action_name] = [cmd_action] + org_action   
   
   return None

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def append_action(env, target_action_name, target_action_str_name, action, action_text='Generating ', action_raw_text=None):
   # create new action 
   if action_raw_text is not None:
      cmd_action = SCons.Action.Action(action, action_raw_text)
   else:
      cmd_action = env.GetBuilderAction(action, action_text)
   
   # get oroginal cc action and prepend new action
   org_action = env[target_action_name]
   
   if type(org_action) is str:
      org_action = [SCons.Action.Action(org_action, env[target_action_str_name])]

   #env.Replace(eval(target_action_name) = org_action + [cmd_action])
   env[target_action_name] = org_action + [cmd_action]

   return None

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def prepend_cc_action(env, action, action_text='Generating ', action_raw_text=None):
   return prepend_action(env, 'CCCOM', 'CCCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def append_cc_action(env, action, action_text='Generating ', action_raw_text=None):
   return append_action(env, 'CCCOM', 'CCCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def prepend_cxx_action(env, action, action_text='Generating ', action_raw_text=None):
   return prepend_action(env, 'CXXCOM', 'CXXCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def append_cxx_action(env, action, action_text='Generating ', action_raw_text=None):
   return append_action(env, 'CXXCOM', 'CXXCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def prepend_as_action(env, action, action_text='Generating ', action_raw_text=None):
   return prepend_action(env, 'ASCOM', 'ASCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CC is executed
# parameter action can be a string (cmd) or a python function, basically 
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the 
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def append_as_action(env, action, action_text='Generating ', action_raw_text=None):
   return append_action(env, 'ASCOM', 'ASCOMSTR', action, action_text, action_raw_text)
