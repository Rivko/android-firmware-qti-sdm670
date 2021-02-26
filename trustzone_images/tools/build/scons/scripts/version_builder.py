#============================================================================
#
# Version Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to <create, process, etc, insert your description here>.
#    replace all instances of SKELETON or skeleton with your builder name.
#
# Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/version_builder.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Author: pwbldsvc $
#  $Change: 18261931 $
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
import re
import socket
import SCons
from xml.etree import ElementTree as et

#----------------------------------------------------------------------------
# Global values
# Global definitions used within this script for example tool cmd definitions 
# which might be use in different places.
#----------------------------------------------------------------------------
TOOL_CMD = "${BUILD_ROOT}/tools/xyz/foo${EXE_EXT}"

"""============================================================================
Hooks for SCons
These are the function entry points SCons calls when loading this script.
============================================================================"""
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
   qc_version_builder_generate(env)

   oem_version_builder_generate(env)

"""============================================================================
QC Version build rules
============================================================================"""
   
def qc_version_builder_generate(env):
   '''-------------------------------------------------------------------------
   QC Version builder generate

   Register builder and methods with SCons.
   -------------------------------------------------------------------------'''

   env.AddMethod(qc_version_builder_wrapper, "QC_VersionBuilder")

   # use standard action wrapper provided by QCT extensions, provides debug 
   # levels and correct text output during build. Not using this will result 
   # in function pointers and mangle names displayed instead of a nice string 
   # such as "=== Generating  <your target>"
   qc_version_act = env.GetBuilderAction(qc_version_builder)
   
   
   qc_version_bld = env.Builder(
      action = qc_version_act,
      #emitter=qc_version_emitter,
      #source_scanner=qc_version_source_scanner,
      #target_scanner=qc_version_target_scanner,
      #suffix = '.my_extention'
   )

   env.Append(BUILDERS = {'QC_VersionInternalBuilder' : qc_version_bld})

def qc_version_builder_wrapper(env, build_tags, target, source):
   '''-------------------------------------------------------------------------
   QC Version builder wrapper

   This method wraps the builder to perform other operations that are require
   all the time, thus clients don't have to all duplicate this extra steps.

   In this case making sure the build tag it the correct one. 
   -------------------------------------------------------------------------'''
   rt = None
   
   # don't do anything if none of the tags match.
   if env.IsKeyEnable(build_tags):
      rt = env.QC_VersionInternalBuilder(target, source)
   
   return rt
   
def qc_version_builder(target, source, env):
   '''-------------------------------------------------------------------------
   QC Version builder action
   
   The actual builder action this gets called only if the targets needs to 
   be build or rebuild. Otherwise this will not be called. When done 
   correctly (tell SCons your exact dependencies with scanners or emitters)
   this is a beautiful thing, it does the right thing when needed.
   -------------------------------------------------------------------------'''
   
   # targets and sources are ALWAYS arrays (is a builder standard interface)
   # this builder needs to know if it deals with multiple targes and sources
   # or singles, or mix (common use case: one target multiple sources).
   target_full = str(target[0])
   source_full = str(source[0])

   # suppose you need to call an the external tool use the following
   #cmd = "".join([TOOL_CMD, " -o=", target_full, " -i=", source_full])
   #std_out, std_err, return_val = env.ExecCmds(cmds)

   # another possibility for the builder is that does everything here in python 
   # and then just use open file and write file to generate the target.
   # <fancy/magical python code here>
   
   # Get XML data
   XmlCfg = et.ElementTree(file=source_full).getroot()
   
   XmlImageTree = XmlCfg.find('image_tree')
   
   if XmlImageTree is not None:
      XmlImageTreeName = XmlImageTree.find('name')
      XmlImageTreeRevision = XmlImageTree.find('revision')
   else:
      env.PrintError("")
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** The configuration XML file: %s is missing 'image_tree' tag" % os.path.split(source_full)[1])
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("")
      SCons.Script.Exit(1)

      
   # create version string
   qc_version_str = "".join([  
      "char QC_IMAGE_VERSION_STRING_AUTO_UPDATED[]=\"QC_IMAGE_VERSION_STRING=", 
      XmlImageTreeName.text.upper().strip(), "-", XmlImageTreeRevision.text.upper().strip(), "\";\n"])


   # create QC version file
   banner = env.CreateFileBanner(os.path.split(target_full)[1], style='C')
   qc_version_file = open (target_full, "w")
   qc_version_file.write(banner)
   qc_version_file.write(qc_version_str)   
   qc_version_file.close()

   
def qc_version_emitter(target, source, env):
   '''-------------------------------------------------------------------------
   QC Version builder emitter
   
   Modify a target dependencies on sources (see SCons documentation) and also 
   a good place to add Clean rules, Dependency rules, Side effects, etc.
   -------------------------------------------------------------------------'''
   
   # a dependency to this builder such that if tool is missing builder will fail 
   # also if such tool changes (newer tool) builder will be called to regenerate 
   # target even if sources didn't change, since we are adding the tool as a 
   # dependency.

   #env.Depends(target, env.RealPath(TOOL_CMD))
   
   return (target, source)

"""============================================================================
OEM Version build rules
============================================================================"""

def oem_version_builder_generate(env):
   '''-------------------------------------------------------------------------
   OEM Version builder generate

   Register builder and methods with SCons
   -------------------------------------------------------------------------'''
   env.AddMethod(oem_version_builder_wrapper, "OEM_VersionBuilder")

   # use standard action wrapper provided by QCT extensions, provides debug 
   # levels and correct text output during build. Not using this will result 
   # in function pointers and mangle names displayed instead of a nice string 
   # such as "=== Generating  <your target>"
   oem_version_act = env.GetBuilderAction(oem_version_builder, action_source=None)
   
   oem_version_tscr = env.Scanner(oem_version_target_scanner, "OEM_VersionTargetScanner")
   
   oem_version_bld = env.Builder(
      action = oem_version_act,
      #emitter=oem_version_emitter,
      #source_scanner=oem_version_source_scanner,
      target_scanner=oem_version_tscr,
      #suffix = '.my_extention'
   )

   env.Append(BUILDERS = {'OEM_VersionInternalBuilder' : oem_version_bld})
   
def oem_version_builder_wrapper(env, build_tags, target, source=[]):
   '''-------------------------------------------------------------------------
   OEM Version builder wrapper

   This method wraps the builder to perform other operations that are require
   all the time, thus clients don't have to all duplicate this extra steps.

   In this case making sure the build tag it the correct one. 
   -------------------------------------------------------------------------'''
   rt = None
   
   # don't do anything if none of the tags match.
   if env.IsKeyEnable(build_tags):
      rt = env.OEM_VersionInternalBuilder(target, source)
   
   return rt
   
def get_oem_version_string():
   '''-------------------------------------------------------------------------
   Get OEM version string
   
   utility function to get OEM version string from multiple sources, used from
   multiple functions
   -------------------------------------------------------------------------'''
   
   # create oem version string
   # this string can coem from multiple sources, in order of priority
   # cmd line: OEM_BUILD_VER
   # os env:  SCONS_OEM_BUILD_VER
   # if nothing found then use machine hostname
   # if that fails use static string
   oem_ver = SCons.Script.ARGUMENTS.get('OEM_BUILD_VER')
   if oem_ver is None:
      oem_ver = os.environ.get('SCONS_OEM_BUILD_VER')
      if oem_ver is None:
         oem_ver = socket.gethostname()
         if oem_ver is None:
            oem_ver = "Engineering build"
            
   oem_ver = str(oem_ver).strip().strip('"')
            
   return oem_ver
   
def oem_version_builder(target, source, env):
   '''-------------------------------------------------------------------------
   OEM Version builder action
   
   The actual builder action this gets called only if the targets needs to 
   be build or rebuild. Otherwise this will not be called. When done 
   correctly (tell SCons your exact dependencies with scanners or emitters)
   this is a beautiful thing, it does the right thing when needed.
   -------------------------------------------------------------------------'''
   
   # targets and sources are ALWAYS arrays (is a builder standard interface)
   # this builder needs to know if it deals with multiple targets and sources
   # or singles, or mix (common one target multiple sources).
   target_full = str(target[0])
   #source_full = str(source[0])

   # suppose you need to call an the external tool use the following
   #cmd = "".join([TOOL_CMD, " -o=", target_full, " -i=", source_full])
   #std_out, std_err, return_val = env.ExecCmds(cmds)

   # another possibility for the builder is that does everything here in python 
   # and then just use open file and write file to generate the target.
   # <fancy/magical python code here>

   # create variant version string
   variant_version_str = "".join([     
      "char IMAGE_VARIANT_STRING_AUTO_UPDATED[]=\"IMAGE_VARIANT_STRING=", 
      env.get('BUILD_ID', "None"), "\";\n"])
   
   oem_ver = get_oem_version_string()
   
   oem_version_str = "".join([  
      "char OEM_IMAGE_VERSION_STRING_AUTO_UPDATED[]=\"OEM_IMAGE_VERSION_STRING=", 
      oem_ver, "\";\n"])
   
   # create OEM version file
   banner = env.CreateFileBanner(os.path.split(target_full)[1], style='C')
   oem_version_file = open (target_full, "w")
   oem_version_file.write(banner)
   oem_version_file.write(variant_version_str)      
   oem_version_file.write(oem_version_str)   
   oem_version_file.close()
   
   
def oem_version_emitter(target, source, env):
   '''-------------------------------------------------------------------------
   OEM Version builder emitter
   
   Modify a target dependencies on sources (see SCons documentation) and also 
   a good place to add Clean rules, Dependency rules, Side effects, etc.
   -------------------------------------------------------------------------'''
   
   # a dependecy to this builder such that if tool is missing builder will fail 
   # also if such tool changes (newer tool) builder will be claled to regenrate 
   # target even if sources didn't change, sicne we are adding the tool as a 
   # dependecy.

   #env.Depends(target, env.RealPath(TOOL_CMD))
   
   return (target, source)
   
def oem_version_target_scanner(node, env, path):
   '''-------------------------------------------------------------------------
   OEM Version target scanner
   
   Target scanner is run even when the explicit dependencies are up-to-date
   and it gives you an opportunity to define new dependencies, for example 
   data only dependencies that do not have a source file for SCons to scan 
   and MD5
   -------------------------------------------------------------------------'''
   oem_ver = get_oem_version_string()
   env.Depends(node, env.Value(oem_ver))

   retList = []
   
   return retList
