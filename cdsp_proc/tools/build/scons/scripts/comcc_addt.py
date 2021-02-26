'''
   Common CC additions

   GENERAL DESCRIPTION
      common CC additions; such as generate preprocess,  mixasm, hdeptree
'''
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
#------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/comcc_addt.py#8 $
#  $DateTime: 2017/05/23 15:42:43 $
#  $Author: pwbldsvc $
#  $Change: 13396059 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#==============================================================================
import string
import re
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('comcc_addt')

def generate(env):
    '''
       generate
    '''
    #==========================================================================
    # Publish APIs
    #==========================================================================
    # Add prepend and append APIs to modify CC, CXX, AS cmds
    env.AddMethod(prepend_action, "PrependActionByName")
    env.AddMethod(append_action, "AppendActionByName")
    env.AddMethod(prepend_cc_action, "PrependCCAction")
    env.AddMethod(append_cc_action, "AppendCCAction")
    env.AddMethod(prepend_cxx_action, "PrependCXXAction")
    env.AddMethod(append_cxx_action, "AppendCXXAction")
    env.AddMethod(prepend_as_action, "PrependASAction")
    env.AddMethod(append_as_action, "AppendASAction")
    env.AddMethod(prepend_aspp_action, "PrependASPPAction")
    env.AddMethod(append_aspp_action, "AppendASPPAction")

    #==========================================================================
    # ALTERNATE CC options
    #==========================================================================
    preprocess = env.GetOption('preprocess')
    mixasm = env.GetOption('mixasm')
    hdeptree = env.GetOption('hdeptree')
    verbose = int(env.GetOption('verbose'))
    analytic = int(env.GetOption('analytic'))
    cleanpack = int(env.GetOption('cleanpack'))

    #import pdb; pdb.set_trace()

    #--------------------------------------------------------------------------
    # set preprocess option rules
    #--------------------------------------------------------------------------
    if preprocess or hdeptree:

        #----------------------------------------------------------------------
        # Updates dependant on verbose levels
        #----------------------------------------------------------------------
        if verbose >= 2:
            env.Append(HDEPTREECOM=" -v")

        #----------------------------------------------------------------------
        # set hdeptree option action
        # Only works if preprocess is ON (and it implied), needs to be done
        #after preprocess is done.
        # Since the options are prepend or append to CC cmd, then we need
        # to queue it up, this action goes in and the preprocess will get
        # prepended thus executing before the hdeptree rule.
        #----------------------------------------------------------------------
        if hdeptree:
            env.PrependCCAction('$HDEPTREECOM', \
                action_raw_text='$HDEPTREECOMSTR')
            env.PrependCXXAction('$HDEPTREECOM', \
                action_raw_text='$HDEPTREECOMSTR')
            env.AppendASAction('$HDEPTREECOM', \
                action_raw_text='$HDEPTREECOMSTR')

        #----------------------------------------------------------------------
        # Prepend CCCOM action with this action that will preprocess each file
        # first take the normal CC preprocess command and change the target
        # since the target to compile the .o is a .o and not the .pp we need
        # also since the TEMP class doesn't like when we muck with the TARGET
        # variable this way, we need to remove TEMPFILE and re add it but
        # passing a variable instead of the original string.
        #----------------------------------------------------------------------
        ppcccom = env['CCPPCOM']
        ppcccom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.pp', ppcccom)
        ppcccom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ppcccom)
        env.Replace(PPCCCOM=ppcccom)

        env.PrependCCAction('$CCPPCL_ACOM', action_raw_text='$CCPPCLCOM_ASTR')
        env.PrependCCAction('${TEMPFILE("$PPCCCOM")}', \
            action_raw_text='$CCPPCOM_ASTR')

        #----------------------------------------------------------------------
        # Prepend CXXCOM action with this action
        # that will preprocess each file first
        #----------------------------------------------------------------------
        ppcxxcom = env['CXXPPCOM']
        ppcxxcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.pp', ppcxxcom)
        ppcxxcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ppcxxcom)
        env.Replace(PPCXXCOM=ppcxxcom)

        env.PrependCXXAction('$CXXPPCL_ACOM', \
            action_raw_text='$CXXPPCLCOM_ASTR')
        env.PrependCXXAction('${TEMPFILE("$PPCXXCOM")}', \
            action_raw_text='$CXXPPCOM_ASTR')

        #----------------------------------------------------------------------
        # Prepend ASASCOM action with this action
        # that will preprocess each file first
        #----------------------------------------------------------------------
        ppasascom = env['ASASCOM']
        ppasascom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.pp', ppasascom)
        ppasascom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ppasascom)
        env.Replace(PPASASCOM=ppasascom)

        #env.PrependASAction('$ASPPCL_ACOM', action_raw_text='$ASPPCLCOM_ASTR')
        env.PrependASAction('${TEMPFILE("$PPASASCOM")}', \
            action_raw_text='$ASCPPCOMSTR')

    #--------------------------------------------------------------------------
    # set mixasm option rules
    #--------------------------------------------------------------------------
    if mixasm:
        # replace normal CCCOM
        # with this one that will preprocess each file first
        ccmixcom = env['CCMIXCOM']
        ccmixcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.mix', ccmixcom)
        ccmixcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', ccmixcom)
        env.Replace(CCMIXCOM=ccmixcom)

        env.PrependCCAction('${TEMPFILE("$CCMIXCOM")}', \
            action_raw_text='$CCMIXCOM_ASTR')

        # replace normal CXXCOM
        # with this one that will preprocess each file first
        cxxmixcom = env['CXXMIXCOM']
        cxxmixcom = re.sub(r'\$(TARGET\.*\w*)', r'${\1}.mix', cxxmixcom)
        cxxmixcom = re.sub(r'\${TEMPFILE\(\'(.*)\'\)}', r'\1', cxxmixcom)
        env.Replace(CXXMIXCOM=cxxmixcom)

        env.PrependCXXAction('${TEMPFILE("$CXXMIXCOM")}', \
            action_raw_text='$CXXMIXCOM_ASTR')

    #--------------------------------------------------------------------------
    # set analytic option rules
    #--------------------------------------------------------------------------
    if analytic:
        env.LoadToolScript('analytic_data_builder', \
            toolpath=["${BUILD_ROOT}/tools/build/scons/scripts"])


    #--------------------------------------------------------------------------
    # Custom Builders
    #--------------------------------------------------------------------------

    # pre process
    pre_process_action = SCons.Action.Action('$CCPPCOM', '$CCPPCOMSTR')
    pre_process_builder = Builder(
        source_scanner=SCons.Scanner.C.CScanner(),
        action=pre_process_action,
        suffix='.pp')
    env.Append(BUILDERS={'PreProcess' : pre_process_builder})

    # pre process clean
    pre_process_clean_action = \
        SCons.Action.Action('$CCPPCLCOM', '$CCPPCLCOMSTR')
    pre_process_clean_builder = Builder(
        action=pre_process_clean_action,
        suffix='.i')
    env.Append(BUILDERS={'PreProcessClean' : pre_process_clean_builder})

    # mix output process
    #AsmInterleaveAction = SCons.Action.Action('$CCMIXCOM', '$CCMIXCOMSTR')
    asm_interleave_builder = Builder(
        source_scanner=SCons.Scanner.C.CScanner(),
        action=pre_process_action,
        suffix='.mix')
    env.Append(BUILDERS={'AsmInterleave' : asm_interleave_builder})

#------------------------------------------------------------------------------
# API to allow custom command additions to all objects before the CC/CXX/AS
# command is executed.  Parameter action can be a string (cmd) or a python
# function.  Basically, the same rules apply to all SCons builders.
#
# WARNING: Since target and source apply to the original target and source,
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original cmd.
#------------------------------------------------------------------------------
def prepend_action(env, target_action_name, target_action_str, action, \
    action_text='Generating ', action_raw_text=None):
    '''
      prepend_action

      API to allow custom command additions to all objects before the CC/CXX/AS
      command is executed.  Parameter action can be a string (cmd) or a python
      function.  Basically, the same rules apply to all SCons builders.

      WARNING: Since target and source apply to the original target and source,
      make sure to use another name other then the RAW TARGET as this is the
      TARGET for the original cmd.
    '''

    # create new action
    if action_raw_text is not None:
        cmd_action = SCons.Action.Action(action, action_raw_text)
    else:
        cmd_action = env.GetBuilderAction(action, action_text)

    # get original action and prepend new action
    org_action = env[target_action_name]

    if type(org_action) is str:
        if target_action_str in env:
            org_action = \
                [SCons.Action.Action(org_action, env[target_action_str])]
        else:
            org_action = [SCons.Action.Action(org_action)]

    env[target_action_name] = [cmd_action] + org_action

    return None

#------------------------------------------------------------------------------
# API to allow custom command additions to all objects after the CC/CXX/AS
# command is executed.  Parameter action can be a string (cmd) or a python
# function.  Basically, the same rules apply to all SCons builders.
#
# WARNING: Since target and source apply to the original target and source,
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original cmd.
#------------------------------------------------------------------------------
def append_action(env, target_action_name, target_action_str_name, action, \
    action_text='Generating ', action_raw_text=None):
    '''
      append_action

      API to allow custom command additions to all objects after the CC/CXX/AS
      command is executed.  Parameter action can be a string (cmd) or a python
      function.  Basically, the same rules apply to all SCons builders.

      WARNING: Since target and source apply to the original target and source,
      make sure to use another name other then the RAW TARGET as this is the
      TARGET for the original cmd.
    '''
    # create new action
    if action_raw_text is not None:
        cmd_action = SCons.Action.Action(action, action_raw_text)
    else:
        cmd_action = env.GetBuilderAction(action, action_text)

    # get original action and prepend new action
    org_action = env[target_action_name]

    if type(org_action) is str:
        if target_action_str_name in env:
            org_action = \
                [SCons.Action.Action(org_action, env[target_action_str_name])]
        else:
            org_action = [SCons.Action.Action(org_action)]

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
def prepend_cc_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       prepend_cc_action

       API to allow custom command additions to all obj before CC is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original CC cmd.
    '''
    return prepend_action \
        (env, 'CCCOM', 'CCCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CC is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original CC cmd.
#------------------------------------------------------------------------------
def append_cc_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       append_cc_action

       API to allow custom command additions to all obj after CC is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original CC cmd.
    '''
    return append_action \
        (env, 'CCCOM', 'CCCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before CXX is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original CXX cmd.
#------------------------------------------------------------------------------
def prepend_cxx_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       prepend_cxx_action

       API to allow custom command additions to all obj before CXX is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original CXX cmd.
    '''
    return prepend_action \
        (env, 'CXXCOM', 'CXXCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after CXX is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original CXX cmd.
#------------------------------------------------------------------------------
def append_cxx_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       append_cxx_action

       API to allow custom command additions to all obj after CXX is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original CXX cmd.
    '''
    return append_action \
        (env, 'CXXCOM', 'CXXCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before AS is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original AS cmd.
#------------------------------------------------------------------------------
def prepend_as_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       prepend_as_action

       API to allow custom command additions to all obj before AS is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original AS cmd.
    '''
    return prepend_action \
        (env, 'ASCOM', 'ASCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after AS is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original AS cmd.
#------------------------------------------------------------------------------
def append_as_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       append_as_action

       API to allow custom command additions to all obj after AS is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original AS cmd.
    '''
    return append_action \
        (env, 'ASCOM', 'ASCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj before ASPP is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original ASPP cmd.
#------------------------------------------------------------------------------
def prepend_aspp_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       prepend_aspp_action

       API to allow custom command additions to all obj before ASPP is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original AS cmd.
    '''
    return prepend_action \
        (env, 'ASPPCOM', 'ASPPCOMSTR', action, action_text, action_raw_text)

#------------------------------------------------------------------------------
# API to allow custom command additions to all obj after ASPP is executed
# parameter action can be a string (cmd) or a python function, basically
# the same rules applies as in any SCons builder.
#
# WARNING: since target and sources apply to the actual CC target and source
# make sure to use another name other then the RAW TARGET as this is the
# TARGET for the original AS cmd.
#------------------------------------------------------------------------------
def append_aspp_action(env, action, action_text='Generating ', \
    action_raw_text=None):
    '''
       append_aspp_action

       API to allow custom command additions to all obj after ASPP is executed
       parameter action can be a string (cmd) or a python function, basically
       the same rules applies as in any SCons builder.

       WARNING: since target and sources apply to the actual CC target and
       source make sure to use another name other then the RAW TARGET as this
       is the TARGET for the original AS cmd.
    '''
    return append_action \
        (env, 'ASPPCOM', 'ASPPCOMSTR', action, action_text, action_raw_text)
