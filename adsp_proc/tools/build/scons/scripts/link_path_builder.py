#===========================================================================
#  GENERAL DESCRIPTION:
#    Contains a scons custom build for creating Link Path List
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
#  All Rights Reserved.
#  Qqualcomm Technologies Confidential and Proprietary
#===========================================================================
#
# Link Path Builder
#
# GENERAL DESCRIPTION
#    Plug-in like builder to collect link path information for re-use across image script boundaries
#
#   SaveLinkPathListBuilder|save_link_path_list()--> Save all Link files path added to LINK_PATH_ENV_DESCRIPTOR into a link_path_list.txt file.
#
#   LinkPathMergeBuilder|linkpath_merge_builder()--> Combine the designated link_path_list.txt files (individual libs being built into an image) 
#   into one linkpath_master_list.txt file.
#
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
# 2016-01-25   jrosen  created
# 2016-02-02   kushk   added more functionality 
#============================================================================
import os
import string
from SCons.Script import *
import shutil


#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

LINK_PATH_ENV_DESCRIPTOR = 'LINK_PATH_INFO_COLLECTOR'

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
    rootenv = env.get('IMAGE_ENV')        
    rootenv[LINK_PATH_ENV_DESCRIPTOR] = []

    # init uses flags to show builder is loaded
    env.AddUsesFlags('USES_LINKER_PATH_DATA')
    env.AddMethod(add_linker_file_path, "AddLinkerFilePath")
    
    linkpath_source_scanner = env.Scanner(link_path_scanner, name="LinkPathScanner")
    
    linkpath_list_actn = env.GetBuilderAction(save_link_path_list, action_source=None)
    linkpath_list_scanner = env.Scanner(link_path_list_scanner, name="LinkPathListScanner")
    linkpath_list_bldr = env.Builder(action=linkpath_list_actn, target_scanner = linkpath_list_scanner)
    rootenv.Append(BUILDERS={'SaveLinkPathListBuilder' : linkpath_list_bldr})
    
    linkpath_merge_actn = env.GetBuilderAction(linkpath_merge_builder, action_source=None)
    linkpath_merge_bldr = env.Builder(action=linkpath_merge_actn, emitter = linkpath_emitter, source_scanner = linkpath_source_scanner, multi=True)
    rootenv.Append(BUILDERS={'LinkPathMergeBuilder' : linkpath_merge_bldr})
       
    env.PrintDebugInfo('Link Path Builder',' Inside generate function %s' %str(env['IMAGE_NAME']))


def link_path_scanner(node, env, path):
  '''
  This will be called once all XML files are added to the environment. Put
  dependencies on each of them
  '''
  env.PrintDebugInfo('linkpathbuilder','In scanner function')
  contents = node.get_contents().splitlines()
  nodes = []
  for f in contents:
     if os.path.exists(f):
         nodes.append(env.RealPath(f))
  nodes.extend([node])
  return nodes
  
def add_linker_file_path(env, linker_file_path):
    '''-------------------------------------------------------------------------
       add_linker_file_path
    -------------------------------------------------------------------------'''
    rootenv = env.get('IMAGE_ENV')
    rootenv.PrintDebugInfo('linkpathbuilder', 'Adding LCS file %s path to list ' %str(linker_file_path))
    rootenv[LINK_PATH_ENV_DESCRIPTOR].append(linker_file_path)
    env.PrintDebugInfo('Link Path Builder',' Inside add linker file function %s' %str(rootenv['IMAGE_NAME']))
    
    return

def link_path_list_scanner(node, env, path):
  '''
  This will be called once all LCS files are added to the environment. Put
  dependencies on each of them
  '''
  env.PrintDebugInfo('linkpathbuilder','In scanner function')

  rootenv = env.get('IMAGE_ENV')

  # To work with new way of compiling copied build (not sync through P4) where
  # only changed files will be recompiled instead of compiling from scratch.
  rootenv[LINK_PATH_ENV_DESCRIPTOR].sort()
  env.Depends(node, rootenv.Value(str(rootenv[LINK_PATH_ENV_DESCRIPTOR])))

  nodes = list(rootenv.get(LINK_PATH_ENV_DESCRIPTOR, []))
  nodes.extend([node])
  return nodes

def save_link_path_list(target, source, env):
   
   if not os.path.exists(env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}')):
      if os.makedirs(env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}')):
         raise

   #link_path argument MUST be passed
   try:
      link_list_path = str(target[0])
   except:
      env.PrintError("Link Path Builder: link_list_path argument not passed")
      exit()

   try:
      link_path_list_file = open(env.RealPath(link_list_path), 'w')
   except IOError:
      env.PrintError("Link Path Builder: Could not open link path list file for write")
      exit()

   for item in env[LINK_PATH_ENV_DESCRIPTOR]:
      link_path_list_file.write(item)
      link_path_list_file.write('\n')  
   link_path_list_file.close()
   env.PrintDebugInfo('Link Path Builder',' Inside save link path list function %s' %str(env['IMAGE_NAME']))
   return  
    
def linkpath_merge_builder(target, source, env):
   # Consolidate all the source files in one target file while maintaining the sorting 

   if not os.path.exists(env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}')):
      if os.makedirs(env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}')):
         raise
      
   #consolidated_target_file_path argument MUST be passed
   try:
     consolidated_target_file_path = str(target[0])
   except:
     env.PrintError("Link Path Builder: Merge link path file argument not passed")
     exit()

   try:
     consolidated_target_file = open(env.RealPath(consolidated_target_file_path), 'w')
   except IOError:
     env.PrintError("Link Path Builder: Could not open link path master file for write")
     exit()

   linkpath_descriptor=[]

   linkpathlists = map(lambda x: str(x), source)
   for linkpathlist in linkpathlists:
     try :
        with open(env.RealPath(linkpathlist), 'r') as input:
                for line in input:
                    linkpath_descriptor.append(line) # Add item to list
     except IOError:
        env.PrintError("Link Path Builder: Could not open link path intermediate file for read")
        exit()
      
   for item in linkpath_descriptor:
      item = item.rstrip()
      consolidated_target_file.write(str(item))
      lcs_filename = item.split("/")[-1]
      env.Print("Link Path Builder: %s" %str(lcs_filename))
      new_path = env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}/' + lcs_filename, posix = True)
      #env.Depends(consolidated_target_file,new_path)
      #shutil.copy(item, new_path)
      
      dir_name = os.path.dirname(os.path.abspath(item))
      env.Print("Link Path Builder: %s" %str(dir_name))
      env.AppendLinkerPath(dir_name)
      
      env.Print("Link Path Builder: %s" %str(new_path))
      consolidated_target_file.write('\n')
      
   consolidated_target_file.close()
   env.PrintDebugInfo('Link Path Builder',' Inside merger builder function %s' %str(env['IMAGE_NAME']))
   return None

def linkpath_emitter(target, source, env):

   return (target, source)

   
def linkpath_emitter_2(target, source, env):
   linkpath_descriptor=[]

   linkpathlists = map(lambda x: str(x), source)
   for linkpathlist in linkpathlists:
     try :
        with open(env.RealPath(linkpathlist), 'r') as input:
                for line in input:
                    linkpath_descriptor.append(line) # Add item to list
     except IOError:
        env.PrintError("Link Path Builder: Could not open link path intermediate file for read")
        exit()
      
   for item in linkpath_descriptor:
      item = item.rstrip()
      lcs_filename = item.split("/")[-1]
      env.Print("Link Path Builder: %s" %str(lcs_filename))
      new_path = env.RealPath('${IMAGE_ROOT}/${SHORT_BUILDPATH}/' + lcs_filename, posix = True)
      target.append(new_path)
      env.Print("Link Path Builder: %s" %str(new_path))

   return (target, source)
