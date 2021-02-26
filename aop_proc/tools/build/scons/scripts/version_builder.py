#============================================================================
#
# Version Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to <create, process, etc, insert your description here>.
#    replace all instances of SKELETON or skeleton with your builder name.
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Proprietary and Confidential - Qualcomm Technologies, Inc/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/version_builder.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
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
import uuid
import datetime
import threading
import json
from xml.etree import ElementTree as et
from SCons.Script import *

#----------------------------------------------------------------------------
# Global values
# Global definitions used within this script for example tool cmd definitions 
# which might be use in different places.
#----------------------------------------------------------------------------
TOOL_CMD = "${BUILD_ROOT}/tools/xyz/foo${EXE_EXT}"

# Locking mechanism to deal with multiple images trying to write to the same file
# form multiple threads; when building we can use SCons primitives to deal with
# the situation but cleaning this is a problem. To simply the design lets use same  
# technique in both building and cleaning.
# NOTE: we usually know that cleaning happens serially but it could change at 
# any time, let's not make any assumptions.
UUID_ELF_VARIANT_FILE_LOCK = threading.Lock()

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

   oem_uuid_builder_generate(env)

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
      XmlImageTreeName.text.upper(), "-", XmlImageTreeRevision.text.upper(), "\";\n"])


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
   # Since this builder has no sources we must specify these with passing type 
   # "None" for param action_source.
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
   # this string can come from multiple sources, in order of priority
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
   
   # a dependency to this builder such that if tool is missing builder will fail
   # also if such tool changes (newer tool) builder will be called to regenerate
   # target even if sources didn't change, sicne we are adding the tool as a 
   # dependency.

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

"""============================================================================
OEM UUID build rules
============================================================================"""

def oem_uuid_builder_generate(env):
   '''-------------------------------------------------------------------------
   OEM UUID builder generate

   Register builder and methods with SCons
   -------------------------------------------------------------------------'''
   env.AddMethod(oem_uuid_builder_wrapper, "OEM_UUID_Builder")

   # use standard action wrapper provided by QCT extensions, provides debug
   # levels and correct text output during build. Not using this will result
   # in function pointers and mangle names displayed instead of a nice string
   # such as "=== Generating  <your target>"
   oem_uuid_act = env.GetBuilderAction(oem_uuid_builder)

   oem_uuid_bld = env.Builder(
      action = oem_uuid_act,
      emitter=oem_uuid_emitter,
      #source_scanner=oem_uuid_source_scanner,
      #target_scanner=oem_uuid_tscr,
      #suffix = '.my_extention'
   )

   env.Append(BUILDERS = {'OEM_UUID_InternalBuilder' : oem_uuid_bld})

   # init information for variant elf info.
   variant_elf_info = SCons.Script.ARGUMENTS.get('BUILD_VARIANT_IMG_INFO') 
   if variant_elf_info is None:
      variant_elf_info = os.environ.get('SCONS_BUILD_VARIANT_IMG_INFO')
      if variant_elf_info is None:
         variant_elf_info = '${TARGET_ROOT}/VariantImgInfo_${BUILD_ID}.json'

   # clean possible extra white space passed via cmd line or os environ
   variant_elf_info = variant_elf_info.strip()
   variant_elf_info = variant_elf_info.strip('"')
   variant_elf_info = variant_elf_info.strip("'")
         
   env.Replace(BUILD_VARIANT_IMG_INFO = variant_elf_info)


def oem_uuid_builder_wrapper(env, build_tags, target, source=[], **kargs):
   '''-------------------------------------------------------------------------
   OEM Version builder wrapper

   This method wraps the builder to perform other operations that are require
   all the time, thus clients don't have to all duplicate this extra steps.

   In this case making sure the build tag it the correct one.
   -------------------------------------------------------------------------'''
   rt = None

   # don't do anything if none of the tags match.
   if env.IsKeyEnable(build_tags):
      rt = env.OEM_UUID_InternalBuilder(target, source, **kargs)

   return rt

def get_oem_uuid_string():
   '''-------------------------------------------------------------------------
   Get OEM uuid string

   utility function to get OEM uuid string from multiple sources, used from
   multiple functions
   -------------------------------------------------------------------------'''

   # create oem uuid string

   oem_uuid =  "".join(["Q_SENTINEL_{", str(uuid.uuid4()).upper(), "}", datetime.datetime.now().strftime("_%Y%m%d_%H%M")])

   return oem_uuid
   
def uui_variant_img_info(env, oem_uuid):
   '''-------------------------------------------------------------------------
   UUID variant img info
   
   Create uuid variant info file 
   -------------------------------------------------------------------------'''

   def add_update_info(root_dict):
      #import pdb;pdb.set_trace()
      
      img_dict = dict()
      images_array = root_dict.get('images')
      
      if images_array is None:
         root_dict['images'] = [img_dict]
      else:
         for idict in images_array:
            img_name = idict.get('name', 'None')
            if (img_name == env.subst(env.get('IMAGE_NAME'))):
               img_dict = idict
               break
         if (img_dict not in images_array):
            images_array.extend([img_dict])
               
      img_dict['name'] = env.subst(env.get('IMAGE_NAME'))
      img_dict['bid'] = env.subst(env.get('BUILD_ID'))
      img_dict['uuid'] = oem_uuid
      img_dict['scons'] = env.RelPath(env.subst(env.get('IMAGE_SCONSCRIPT_PATH')), os.path.split(var_elf_info_fname)[0], posix=True)
      img_dict['path'] = env.RelPath(env.subst(env.get('TARGET_IMG_PATH')), os.path.split(var_elf_info_fname)[0], posix=True)

      return root_dict
         
   
   try:
      UUID_ELF_VARIANT_FILE_LOCK.acquire()
      var_elf_info_fname = env.subst("${BUILD_VARIANT_IMG_INFO}")
      if (env.PathExists(var_elf_info_fname)):
         var_elf_info = open(var_elf_info_fname, "r")
         var_elf_info_dict = json.load(var_elf_info)
         var_elf_info.close()
      else:
         var_elf_info_dict = dict()
         
      add_update_info(var_elf_info_dict)

      var_elf_info_dict['_comments'] =  eval(env.CreateFileBanner(os.path.split(var_elf_info_fname)[1], style='json'))
         
      var_elf_info = open(var_elf_info_fname, "w")
      json.dump(var_elf_info_dict, var_elf_info, sort_keys=True, indent=3)
      var_elf_info.close()
      
   finally:
      UUID_ELF_VARIANT_FILE_LOCK.release()

def oem_uuid_emitter(target, source, env):
   '''-------------------------------------------------------------------------
      OEM UUID builder emitter

   clean up, SCons will take care of the normal target this emitter needs to also 
   take care of entries in the BUILD_VARIANT_IMG_INFO file.
   If there are no more entries after removing the portion of this target then 
   clean the file itself.
   -------------------------------------------------------------------------'''

   # only do this work if 1) this is clean operation and two this is not a 
   # preview or non_exec operation
   no_exec = env.GetOption('no_exec')
   clean = env.GetOption('clean')

   if (clean and not no_exec and ('TARGET_IMG_PATH' in env)):
      try:
         UUID_ELF_VARIANT_FILE_LOCK.acquire()
         var_elf_info_fname = env.subst("${BUILD_VARIANT_IMG_INFO}")
         if (env.PathExists(var_elf_info_fname)):
            var_elf_info = open(var_elf_info_fname, "r")
            var_elf_info_dict = json.load(var_elf_info)
            var_elf_info.close()
            
            images_array = var_elf_info_dict.get('images')
            img_dict = dict()
            
            if images_array is not None:
               for idict in images_array:
                  img_name = idict.get('name', 'None')
                  if (img_name == env.subst(env.get('IMAGE_NAME'))):
                     img_dict = idict
                     break
               if (img_dict in images_array):
                  images_array.remove(img_dict)           
            
                  var_elf_info = open(var_elf_info_fname, "w")
                  json.dump(var_elf_info_dict, var_elf_info, sort_keys=True, indent=3)
                  var_elf_info.close()
               
                  # make sure to remove file entirely if there was only one record  left.
                  if len(images_array) == 0:
                     env.Clean(target, var_elf_info_fname) 
         
      finally:
         UUID_ELF_VARIANT_FILE_LOCK.release()


   return (target, source)
  
  
def oem_uuid_builder(target, source, env):
   '''-------------------------------------------------------------------------
   OEM UUID builder action

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
   oem_uuid = get_oem_uuid_string()
   oem_uuid_str = "".join([
      "char OEM_IMAGE_UUID_STRING_AUTO_UPDATED[]=\"OEM_IMAGE_UUID_STRING=",
      oem_uuid, "\";\n"])


   # create OEM version file
   banner = env.CreateFileBanner(os.path.split(target_full)[1], style='C')
   oem_version_file = open (target_full, "w")
   oem_version_file.write(banner)
   oem_version_file.write(oem_uuid_str)
   oem_version_file.close()

   # create/update variant info file
   if 'TARGET_IMG_PATH' in env:
      uui_variant_img_info(env, oem_uuid)

