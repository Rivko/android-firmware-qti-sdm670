'''
   Analytic Data Builders rules

   GENERAL DESCRIPTION
      Plug-in like builder to collect build information for later processing
      and data mining for example lint, klocwork, etc.
'''
#============================================================================
#
# Analytic Data Builders rules
#
# GENERAL DESCRIPTION
#    Plug-in like builder to collect build information for later processing
#    and data mining for example lint, klocwork, etc.
#
# Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/analytic_data_builder.py#13 $
#  $DateTime: 2015/12/17 17:47:34 $
#  $Author: pwbldsvc $
#  $Change: 9607741 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
#
#
#============================================================================
import os
#import re
#import socket
#import SCons
#import uuid
#import datetime
#from xml.etree import ElementTree as et
import json
import zipfile
import contextlib

#----------------------------------------------------------------------------
# Global values
# Global definitions used within this script for example tool cmd definitions
# which might be use in different places.
#----------------------------------------------------------------------------
TOOL_CMD = "${BUILD_ROOT}/tools/xyz/foo${EXE_EXT}"

# ============================================================================
# Hooks for SCons
# These are the function entry points SCons calls when loading this script.
# ============================================================================
def exists(env):
    '''
       Used by SCons to make sure scripts are not allowed to be
       loaded multiple times per environment.
    '''
    return env.Detect('version_builder')

def generate(env):
    '''
       This is the entry point called by SCons when loading this script.
       This should call other generate functions as a script might define
       multiple builders or methods.
    '''

    # -------------------------------------------------------------------------
    # Init environment for this group of builders
    # -------------------------------------------------------------------------

    # init image environment
    img_env = env.get('IMAGE_ENV')

    if img_env is None:
        env.PrintError('')
        env.PrintError("----------------------------------------------------" \
            "---------------------------")
        env.PrintError("Analytic data builder, is being loaded too early or " \
            "improperly. This builder")
        env.PrintError("needs to be loaded within the context of a SCons " \
            "image.")
        env.PrintError("----------------------------------------------------" \
            "---------------------------")
        env.PrintError('')
        Exit(1)

    img_env.Replace(ANALYTIC_DATA=[])
    img_env.Replace(SCRIPT_PATH_DATA={})

    # init uses flags to show builder is loaded
    env.AddUsesFlags('USES_ANALYTIC_DATA')

    # register for CC, CXX, and AS actions
    env.PrependCCAction(comcc_data_capture, action_text='Analytics ')
    env.PrependCXXAction(comcxx_data_capture, action_text='Analytics ')
    env.PrependASAction(comas_data_capture, action_text='Analytics ')
    env.PrependASPPAction(comaspp_data_capture, action_text='Analytics ')

    env.AddMethod(script_path_capture, "ScriptPathCapture")

    # init builders generates
    analytic_builder_generate(env)

# ============================================================================
# Utility ACTION fucntions to intercept the CC, CXX, and AS actions
# ============================================================================
def com_data_capture(target, source, env, cmd):
    '''-------------------------------------------------------------------------
       com_data_capture

       common action fucntion to capture CC, CXX, or AS actiosn
    -------------------------------------------------------------------------'''

    target_str = str(target[0])
    source_str = str(source[0])
    #print "WD-src>>>:" + source_str
    #print "WD-cmd>>>:" + cmd

    img_env = env.get('IMAGE_ENV')

    if img_env is not None:
        analytic_data_array = img_env.get('ANALYTIC_DATA')

        if analytic_data_array is not None:
            analytic_data_node = DataNode(env.subst \
                (cmd.replace("${TEMPFILE", "{")), cmd, source_str, target_str)
            analytic_data_array.append(analytic_data_node)

def comcc_data_capture(target, source, env):
    '''-------------------------------------------------------------------------
       comcc_data_capture

       CC action fucntion to capture COMCC actiosn
    -------------------------------------------------------------------------'''
    cmd = env['CCCOMCPY']
    return com_data_capture(target, source, env, cmd)

def comcxx_data_capture(target, source, env):
    '''-------------------------------------------------------------------------
       comcxx_data_capture

       CXX action fucntion to capture COMCXX actiosn
    -------------------------------------------------------------------------'''
    cmd = env['CXXCOMCPY']
    return com_data_capture(target, source, env, cmd)

#------------------------------------------------------------------------------
# AS data capture function.
#------------------------------------------------------------------------------
def comas_data_capture(target, source, env):
    '''-------------------------------------------------------------------------
       comas_data_capture

       AS action fucntion to capture COMAS actiosn
    -------------------------------------------------------------------------'''
    cmd = env['ASCOMCPY']
    return com_data_capture(target, source, env, cmd)

#------------------------------------------------------------------------------
# ASPP data capture function.
#------------------------------------------------------------------------------
def comaspp_data_capture(target, source, env):
    '''-------------------------------------------------------------------------
       comaspp_data_capture

       ASPP action fucntion to capture COMAS actiosn
    -------------------------------------------------------------------------'''
    cmds = env['ASPPCOMCPY']

    if type(cmds) is not list:
        cmds = [cmds]

    for cmd in cmds:
        com_data_capture(target, source, env, cmd)

# ============================================================================
# Utility ACTION fucntions to capture publish name(s) and publish paths
# ============================================================================
def script_path_capture(env, publish_file, publish_name, publish_paths):
    '''-------------------------------------------------------------------------
       script_path_capture

       fucntion to capture:
         - Publish Name
         - associated unexpanded publish paths
         - associated expanded publish paths
    -------------------------------------------------------------------------'''
    env_subst = env.subst
    img_env = env.get('IMAGE_ENV')

    if img_env is not None:
        script_path_data_dict = img_env.get('SCRIPT_PATH_DATA')

        if script_path_data_dict is not None:
            script_path_data_paths = {}
            publish_file = publish_file.replace('\\', '/')
            for publish_path in publish_paths:
                script_path_data_paths[str(publish_path)] = \
                    env_subst(str(publish_path))
            script_path_data_publish_name = \
                {publish_name : script_path_data_paths}
            if script_path_data_dict.has_key(publish_file):
                temp_dict = script_path_data_dict[publish_file]
                temp_dict.update(script_path_data_publish_name)
                script_path_data_dict[publish_file] = temp_dict
            else:
                script_path_data_dict[publish_file] = \
                    script_path_data_publish_name

# ============================================================================
# Source List builder
# ============================================================================
def analytic_builder_generate(env):
    '''-------------------------------------------------------------------------
       analytic_builder_generate

       Register builder and methods with SCons.
    -------------------------------------------------------------------------'''

    # use standard action wrapper provided by QCT extensions, provides debug
    # levels and correct text output during build. Not using this will result
    # in function pointers and mangle names displayed instead of a nice string
    # such as "=== Generating  <your target>"
    analytic_act = env.GetBuilderAction(analytic_builder, action_text='')


    analytic_bld = env.Builder(
        action=analytic_act,
        suffix='.zip'
    )

    env.Append(BUILDERS={'AnalyticBuilder' : analytic_bld})

def analytic_builder(target, source, env):
    '''-------------------------------------------------------------------------
       Analytic_builder builder action

       The actual builder action this gets called only if the targets needs to
       be build or rebuild. Otherwise this will not be called. When done
       correctly (tell SCons your exact dependencies with scanners or emitters)
       this is a beautiful thing, it does the right thing when needed.
    -------------------------------------------------------------------------'''

    json_dict = {"BUILD_ASIC": env.get("BUILD_ASIC", "NA"),
                 "BUILD_ID": env.get("BUILD_ID", "NA"),
                 "BUILD_VER": env.get("BUILD_VER", "NA"),
                 "Commands": [],
                 "PublishPaths": {},
                 "LoadedBuilders": []}


    # targets and sources are ALWAYS arrays (is a builder standard interface)
    # this builder needs to know if it deals with multiple targes and sources
    # or singles, or mix (common use case: one target multiple sources).
    target_full = str(target[0])
    #source_full = str(source[0])

    # suppose you need to call an the external tool use the following
    #cmd = "".join([TOOL_CMD, " -o=", target_full, " -i=", source_full])
    #std_out, std_err, return_val = env.ExecCmds(cmds)

    # another possibility for the builder is that does everything here in python
    # and then just use open file and write file to generate the target.
    # <fancy/magical python code here>

    img_env = env.get('IMAGE_ENV')
    usage_env = env.get('LOADED_BUILDER_ENV')

    if img_env is not None:
        analytic_data_array = img_env.get('ANALYTIC_DATA')
        script_path_data_dict = img_env.get('SCRIPT_PATH_DATA')

    builders = []
    if usage_env is not None:
        builders = usage_env.get('LOADED_BUILDERS')
        tgt_root = usage_env.get('TARGET_ROOT')
        builders = [x.replace(os.sep, '/').replace(tgt_root, '').strip('/') \
            for x in builders]
        builders = sorted(list(set(builders)))

    for analytic_data_node in analytic_data_array:
        json_dict_item = {'source': analytic_data_node.data_node['source'],
                          'target': analytic_data_node.data_node['target'],
                          'scons_cmd': \
                            analytic_data_node.data_node['scons_cmd'],
                          'cmd': analytic_data_node.data_node['cmd']}
        json_dict["Commands"].append(json_dict_item)

    json_dict["PublishPaths"].update(script_path_data_dict)

    json_dict["LoadedBuilders"] = builders

    json_file_name = target_full.replace(".zip", ".json")
    with open(json_file_name, 'w') as outfile:
        json.dump(json_dict, outfile, sort_keys=True, indent=4, \
            separators=(',', ': '))

    env.Clean(target, target_full)

    with contextlib.closing(zipfile.ZipFile \
        (target_full, 'w', zipfile.ZIP_DEFLATED)) as myzip:
        myzip.write(json_file_name)
    myzip.close()
    os.remove(json_file_name)

def analytic_emitter(target, source, env):
    '''-------------------------------------------------------------------------
       QC Version builder emitter

       Modify a target dependencies on sources (see SCons documentation) and
       also a good place to add Clean rules, Dependency rules, Side effects,
       etc.
    -------------------------------------------------------------------------'''

    # a dependency to this builder such that if tool is missing builder will
    # fail also if such tool changes (newer tool) builder will be called to
    # regenerate target even if sources didn't change, since we are adding the
    # tool as a dependency.

    #env.Depends(target, env.RealPath(TOOL_CMD))

    return (target, source)

class DataNode(object):
    '''
       Class to hold Analytics Data
    '''

    def __init__(self, cmd, scons_cmd, source, target):
        self.data_node = {}
        self.data_node['target'] = target
        self.data_node['source'] = source
        self.data_node['scons_cmd'] = scons_cmd
        self.data_node['cmd'] = cmd
