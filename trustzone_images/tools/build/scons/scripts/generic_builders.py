#===============================================================================
#
# Common Builders
#
# GENERAL DESCRIPTION
#    Common builders.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/generic_builders.py#1 $
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
# 07/21/10   wd      Create
#
#===============================================================================
import os
import glob
import re
import hashlib
import subprocess
import string
import fnmatch
import shutil
import textwrap
import imp
import SCons.Action
from SCons.Script import *
from datetime import datetime

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('generic_builders')

def generate(env):
   # init methods
   env.AddMethod(get_builder_action, "GetBuilderAction")
   env.AddMethod(get_builder, "GetBuilder")

   # init builders
   
   # ListFileBuilder
   list_file_builder_generate(env)

   # ListFileExtractor
   list_file_extractor_generate(env)
   
   # ReplaceTextInFile
   # PreprocessAndReplaceTextInFile
   # ReplaceTextInFileBuilder
   replace_text_file_builder_generate(env)
   
   # InstallFiles
   # InstallFilesBuilder
   install_files_builder_generate(env)
    
   # ExBuildSystem
   # ExBuildSystemGetListFileNodes
   ext_bldsys_builder_generate(env)
   
   # BuildProducts
   build_products_file_generate(env)

   return None

#===============================================================================
# get_builder_action
#===============================================================================
def get_builder_action (env, action, action_text = 'Generating ', action_target="TARGET", action_source="SOURCE"):
   verbose = int(env.GetOption('verbose'))
   
   action_target_str = "None"
   action_target_name_str = "None"
   action_source_str = "None"
   action_source_name_str = "None"
   
   if action_target is not None:
      action_target_str = "${%s}" % action_target
      action_target_name_str = "${%s.name}" % action_target
   
   if action_source is not None:   
      action_source_str = "${%s}" % action_source
      action_source_name_str = "${%s.name}" % action_source

   if verbose == 3:
      verbose_str = "%s ARGS: [T:%s, S:%s]" % (str(action), action_target_str, action_source_str)
   elif verbose == 2:
      verbose_str = "<== %s\n" \
         "Source: %s\n" \
         "Target: %s\n" \
         "Command: %s\n==>" % (action_text, action_source_str, action_target_str, str(action))
   elif verbose == 1:
      verbose_str = "=== %s %s ==> %s" % (action_text, action_source_name_str, action_target_str)
   else:
      verbose_str = "=== %s ${BUILDPATH}/%s" % (action_text, action_target_name_str)

   act = SCons.Action.Action(action, verbose_str)
   return act

#===============================================================================
# get_builder
#===============================================================================
def get_builder (env, builder_name, action, action_text='Generating ', action_raw_text=None):

   if action_raw_text is not None:
      blder_act = SCons.Action.Action(action, action_raw_text)
   else:
      blder_act = env.GetBuilderAction(action, action_text)

   blder_bld = env.Builder(action = blder_act)
   env.Append(BUILDERS = {builder_name : blder_bld})
   blder = evaluate("env." + builder_name)
   return blder
   
#===============================================================================
#===============================================================================
# Buildes
#
#===============================================================================
#===============================================================================
   
#===============================================================================
# List file builder
# creates a file with a lst of files passed as sources
#===============================================================================
def list_file_builder_generate(env):
   list_file_act = env.GetBuilderAction(list_file_builder)
   list_file_bld = env.Builder(action = list_file_act)
   env.Append(BUILDERS = {'ListFileBuilder' : list_file_bld})

def list_file_builder(target, source, env):
   list_fname = str(target[0])
   list_file_create(env, list_fname, source, env.get('add_header', False), env.get('relative_path'))
   
def list_file_create(env, list_fname, file_list, add_header=False, rel_path_target=None):
   banner = ""  
   
   if add_header:
      banner = env.CreateFileBanner(os.path.split(list_fname)[1])
   
   list_file = open (list_fname, "w")
   list_file.write(banner)

   if rel_path_target is None:
      for fname in file_list:
         list_file.write(env.RealPath(str(fname)) +'\n')         
   else:
      for fname in file_list:
         list_file.write(env.RelPath(os.path.realpath(str(fname)), rel_path_target) +'\n')
         
   list_file.close()
   

#===============================================================================
# List file extractor
# reads list files and extracts the list of files, uses list_file_builder to 
# create the listfiles consiladating all of the files extracted...
#===============================================================================
def list_file_extractor_get_nodes (env, lf_list):
   #import pdb;pdb.set_trace()

   # will use dictioanry to avoid duplicates.
   nodes = dict()
   
   for lf in lf_list:
      lf_fname = env.RealPath(str(lf))
      lf_path, lf_name = os.path.split(lf_fname)      
      
      if env.PathExists(lf_fname):
         lf_fileh = open(lf_fname, "r")
         lf_data = lf_fileh.read()

         # remove any comments in lf data
         lf_data = re.sub(r"(#|;).*", r"", lf_data, re.M)

         # get list of files
         lf_files_raw = re.findall(r"(\S+)", lf_data, re.M)
         
         # after getting the list of files, we need to get their relative path
         # with respect to where linker will execute. The files in the list could 
         # already be a relative to where the original lf was created.
         # trying to find relative paths is difficutl it is easier to detect non 
         # relative paths as these start with "/" or second char is ":" (windows).
         re_nonrel_match = re.compile(r"(/|.\:)")
         for fname in lf_files_raw:
            if re.match (re_nonrel_match, fname):
               nodes[env.RealPath(fname)] = True
            else:
               nodes[env.RealPath("%s/%s" % (lf_path, fname))] = True
            
         lf_fileh.close()
      
   return nodes.keys()

def list_file_extractor_generate(env):
   list_file_ex_act = env.GetBuilderAction(list_file_extractor)
   list_file_ex_bld = env.Builder(action = list_file_ex_act)
   env.Append(BUILDERS = {'ListFileExtractor' : list_file_ex_bld})

def list_file_extractor(target, source, env):
   exbldsys_list_fname = str(target[0])
   exbldsys_lfs = source
   
   nodes = list_file_extractor_get_nodes(env, exbldsys_lfs)
   
   #import pdb;pdb.set_trace()
   build_dir = env.Dir('#').get_abspath()
   list_file_create(env, exbldsys_list_fname, nodes, add_header=True, 
      rel_path_target=build_dir)


#===============================================================================
# ReplaceTextInFile
# Replaces item on resulting frile from provided replace dictionary
#===============================================================================
def replace_text_file(env, target, source, replace_dict=None, header_style=None):
   targte_final = env.ReplaceTextInFileBuilder(target, source, 
      REPLACE_DICT=replace_dict, HEADER_STYLE=header_style)
   
   return targte_final

#===============================================================================
# PreprocessAndReplaceTextInFile
# preprocess source file and then replaces item on resulting frile from 
# provided replace dictionary
#===============================================================================
def preprocess_replace_text_file(env, target, source, replace_dict=None, header_style=None):
   if type(target) is list:
      target = target[0]
      
   if type(source) is list:
      source = source[0]

   target_full = env.RealPath(str(target))
   (target_base, target_ext) = os.path.splitext(target_full)
      
   source_pp = env.PreProcess(target_base, source)
   source_i = env.PreProcessClean(target_base, source_pp)
   targte_final = env.ReplaceTextInFileBuilder(target, source_i, 
      REPLACE_DICT=replace_dict, HEADER_STYLE=header_style)
   
   return targte_final

#===============================================================================
# Replace file builder
# uses source file as an input replaces all the strings matching keys
# from REPLACE_DICT dictionary and saves this to target[0]
#===============================================================================
def replace_text_file_builder_generate(env):
   env.AddMethod(replace_text_file, "ReplaceTextInFile")
   env.AddMethod(preprocess_replace_text_file, "PreprocessAndReplaceTextInFile")
   
   replace_file_act = env.GetBuilderAction(replace_text_file_builder)
   replace_file_bld = env.Builder(action = replace_file_act)
   env.Append(BUILDERS = {'ReplaceTextInFileBuilder' : replace_file_bld})

def replace_text_file_builder(target, source, env):
   # init file names
   replace_fname = str(target[0])
   src_fname = str(source[0])
   
   # get replace dictionary and comment style
   replace_dict = env.get('REPLACE_DICT')
   header_style = env.get('HEADER_STYLE')
   
   # create file banner
   banner = ""
   if header_style is not None:
      banner = env.CreateFileBanner(os.path.split(replace_fname)[1], style=header_style)
   
   # process source file
   src_file = open (src_fname, "r")
   src_data = src_file.read()
   src_file.close()

   if replace_dict is not None and len(replace_dict.keys()) > 0:
      for key in replace_dict:
         src_data = src_data.replace(key, str(replace_dict.get(key)))
   
   #[list_file.write(os.path.realpath(str(src)) +'\n') for src in source]
   #for src in source:
   #   list_file.write(os.path.realpath(str(src)) +'\n')
   replace_file = open (replace_fname, "w")
   replace_file.write(banner)
   replace_file.write(src_data)
   replace_file.close()

#===============================================================================
# Install Files builder
# Install files, replacement for Install and InsatllAs, as these not always 
# clean correctly after themselves.
#
# Rules:
# - Target can only be a single file or a directory, not multiple targets.
# - To copy from many sources to a directory use a directory as target.
# - Using directory as a target from a single source will cause it to use the 
#   same file name as source
# - Source can be a single or multiple files, not a directory
#===============================================================================
def install_files_func(env, target, source):
   targets = []
   do_multi_source = False
   
   if type(target) is list:
      env.PrintError("InstallFiles can only have single target a file or folder ")
      raise RuntimeError, 'InstallFiles param error.'
      
   if (type(source) is list) or (type(source) is SCons.Node.NodeList):
      if len(source) > 1:
         do_multi_source = True
      else:
         source = source[0]
   
   if do_multi_source:
      #import pdb; pdb.set_trace()
      target_path = target
      for source_file in source:
         target = env.RealPath(target_path + "/" + os.path.split(str(source_file))[1])
         targets += env.InstallFilesBuilder(target, source_file)
   else:
      #import pdb; pdb.set_trace()
      target = env.RealPath(target)
      if os.path.isdir(target):
         target = env.RealPath(target + "/" + os.path.split(str(source))[1])
      targets = env.InstallFilesBuilder(target, source)
      
   return targets

def install_files_builder_generate(env):
   env.AddMethod(install_files_func, "InstallFiles")

   install_files_act = env.GetBuilderAction(install_files_builder, 'Install    ')
   install_files_bld = env.Builder(action = install_files_act)
   env.Append(BUILDERS = {'InstallFilesBuilder' : install_files_bld})

def install_files_builder(target, source, env):
   #import pdb; pdb.set_trace()
   target_full = env.RealPath(str(target[0]))
   source_full = env.RealPath(str(source[0]))
   shutil.copy2(source_full, target_full)

#===============================================================================
# External build system builder
#
# Rules:
# - external builder must produce a list fie (target) contaning all the 
#   files built.
# - pass in build comand with BUILD_CMD.
# - pass in clean list with CLEAN_LIST; these are the files that will 
#    removed during clean
# - pass in clean list with CLEAN_LF; this is a file contaning a list of files 
#    which will removed during clean. Files listed must be relative to the
#    list file.
#
# - NOTE: The list file is used for both linking into an image as well as to clean
#
#===============================================================================
def ext_bldsys_get_lf_nodes(env, list_file):
   #import pdb; pdb.set_trace()
   nodes = list_file_extractor_get_nodes(env, list_file)
   return nodes

def ext_bldsys_builder_generate(env):
   env.AddMethod(ext_bldsys_func, "ExBuildSystem")
   env.AddMethod(ext_bldsys_get_lf_nodes, "ExBuildSystemGetListFileNodes")
   
   ext_bldsys_act = env.GetBuilderAction(ext_bldsys_builder, 'Ext BLDSYS ', action_source=None)
   
   ext_bldsys_bld = env.Builder(
      action = ext_bldsys_act, 
      emitter = ext_bldsys_emitter)
      
   env.Append(BUILDERS = {'ExBuildSystemWithSource' : ext_bldsys_bld})
   
def ext_bldsys_func(env, target, **kwargs):
   #import pdb; pdb.set_trace()

   build_dir = kwargs.get('BUILD_DIR')
   if build_dir is not None:
      kwargs['BUILD_DIR'] = env.RealPath(build_dir)
      
   clean_dir = kwargs.get('CLEAN_DIR')
   if clean_dir is not None:
      kwargs['CLEAN_DIR'] = env.RealPath(clean_dir)
   
   nodes = env.ExBuildSystemWithSource(target=target, source=[], **kwargs)
   
   # since we don't know anything about the dependecies in the external build system
   # always call the build system and let it decide if tinsg are upto date or not.
   env.AlwaysBuild(nodes)
   
   return nodes

def ext_bldsys_emitter(target, source, env):
   if env.GetOption('clean'):
      #import pdb; pdb.set_trace()
      
      # a hack we are not too happy with...
      clean_cmd = env.get('CLEAN_CMD')
      if clean_cmd is not None:
         clean_dir = env.get('CLEAN_DIR')
         data, err, rv = env.ExecCmds(clean_cmd, dir=clean_dir, shell=True, buffer=False)

      clean_list = env.get('CLEAN_LIST')
      if clean_list is not None:
         env.Clean(target, clean_list)
      
      clean_lf_full = env.get('CLEAN_LF')
      if clean_lf_full is not None:
         nodes = ext_bldsys_get_lf_nodes(env, clean_lf_full)
         if len(nodes) > 0 :
            env.Clean(target, nodes)
      
   return (target, source)

def ext_bldsys_builder(target, source, env):
   #import pdb; pdb.set_trace()
   target_full = env.RealPath(str(target[0]))
   
   # get build cmd
   build_cmd = env.get('BUILD_CMD')
   
   if build_cmd is None:
      env.PrintError("Missing build command [EXBLDSYS_BLDCMD] don't know what to do!!")
      raise RuntimeError, 'External Build System, called with no command'

   build_dir = env.get('BUILD_DIR')
   
   data, err, rv = env.ExecCmds(build_cmd, dir=build_dir, shell=True, buffer=False)
   


#===============================================================================
# Build Products file builder
# creates or updates build products file edded for CRM infrastructure, 
# requires $TARGET_ROOT variable.
#===============================================================================
def build_products_file_generate(env):
   #import pdb; pdb.set_trace()
   build_producst_file_act = env.GetBuilderAction(build_producst_file_builder)

   build_producst_file_bld = env.Builder(
      action = build_producst_file_act, 
      emitter = build_producst_file_emitter,
      suffix = '_bldpro.txt')
      
   env.Append(BUILDERS = {'BuildProductsFile' : build_producst_file_bld})
   env.Replace(BUILD_PRODUCT_FILE = "${TARGET_ROOT}/BuildProducts.txt")

def build_producst_file_emitter(target, source, env):
   # Well this is all very hacky since we do everything in the emitter, 
   # what does this mean? It means that SCons has no clue about what
   # dependencies are needed for creating or updating this file and/or 
   # explain why this got build or updated with options like 
   # --debug=explain. Also it has the bad habit to update or create files
   # even if this rule is not needed. Another unbecoming of SCons behavior.
   # we should really change how the CRM infrastructure works instead 
   # of hacking this to adapt to it!!

   target_full_str = str(target[0])
   source_full_str = str(source[0])
   
   build_products_file_path  = env.RealPath("${BUILD_PRODUCT_FILE}")
   image_file = "./" + env.RelPath(source_full_str, "${TARGET_ROOT}", posix=True)
   
   #env.SideEffect(target, build_products_file_path)
   
   if env.PathExists(build_products_file_path):
      build_prod_file = open(build_products_file_path, "r")
      build_prod_data = build_prod_file.read()
      build_prod_file.close()
   else:
      build_prod_data = ""

   
   if env.GetOption('clean'):
      # clean our lines or the file if empy...
      #import pdb; pdb.set_trace()   
      if build_prod_data.find(image_file) != -1:
         build_prod_data = build_prod_data.strip().replace(image_file, "")
         
         if len(build_prod_data) == 0:
            env.Clean(target, build_products_file_path)
         else:
            build_prod_file = open (build_products_file_path, "w")
            build_prod_file.write(build_prod_data)
            build_prod_file.close()
   else:
      # add lines for this image.
      if build_prod_data.find(image_file) == -1:
         build_prod_file = open (build_products_file_path, "w")
         build_prod_data = "".join([build_prod_data, image_file, "\n"])
         build_prod_file.write(build_prod_data)
         build_prod_file.close()
      
   return (target, source)

def build_producst_file_builder(target, source, env):
   target_full_str = str(target[0])
   source_full_str = str(source[0])
   build_products_file_path  = env.RealPath("${BUILD_PRODUCT_FILE}")

   env.PrintInfo(source_full_str)
   
   # create local file
   targte_file = open (target_full_str, "w")
   
   banner = env.CreateFileBanner(os.path.split(target_full_str)[1])
   targte_file.write(banner)
   image_file = "./" + env.RelPath(source_full_str, "${TARGET_ROOT}", posix=True)
   targte_file.write(image_file)
   targte_file.close()
   

