#===============================================================================
# Copyrigha (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/platform.dsp/1.0.c3/build/dlinker_symbols.py#1 $
#  $DateTime: 2018/07/30 03:09:14 $
#  $Author: pwbldsvc $
#  $Change: 16760198 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/15/14   aa      Created History
#
#===============================================================================
import sys
import os
import string
from SCons.Script import *
import types

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
DLINKER_ENV_DESCRIPTOR = 'DLINKER_INFO_COLLECTOR'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('dlinker_symbols')
   
def generate(env):
   rootenv = env.get('IMAGE_ENV')
   rootenv[DLINKER_ENV_DESCRIPTOR] = []
   #env.AddUsesFlags(['ENABLE_DLFWK'])
   dlinker_symbol_master_lst_generate(env)
   dlinker_symbol_master_list_generate(env)
   dlinker_symbol_auto_generate(env)
 
   return

#===============================================================================
# DYN SYMBOL MASTER LST build rules
#=============================================================================== 
def dlinker_symbol_master_lst_generate(env):
    
   dlinker_lst_action = env.GetBuilderAction(dlexpose_generate_lst_builder)
   dlinker_lst_bld = env.Builder(action = dlinker_lst_action,
                             #emitter=skeleton_emitter,
                             #target_scanner=skeleton_scan,
                             #suffix = '.my_extention'
                            )
   env.Append(BUILDERS = {'DLExposeLSTBuilder' : dlinker_lst_bld}) 
   return   

   
#===============================================================================
# DYN SYMBOL LIST FILE(TXT)  build rules
#=============================================================================== 
def dlinker_symbol_master_list_generate(env):
    
   env.AddMethod(dl_expose, "DLExpose")
   env.AddMethod(dl_exposeFile, "DLExposeFile")
   #env.AddMethod(dlexpose_generate, "DLExposeGenerate")

   dlinker_action = env.GetBuilderAction(dlexpose_generate_builder, action_source=None)
   dlinker_target_scan = env.Scanner(dlinker_target_scan_func, name='DLTargetScanner')
   dlinker_bld = env.Builder(action = dlinker_action,
                             #emitter=skeleton_emitter,
                             target_scanner=dlinker_target_scan,
                             #suffix = '.my_extention'
                            )
   env.Append(BUILDERS = {'DLExposeGenerateList' : dlinker_bld}) 
   return
    
def dlinker_target_scan_func(node, env, path):
    """
    Scanner adds DLExposed Symbols from each scons env as a dependency
    to generate final linker script.
    """
    #import pdb;pdb.set_trace()
    rootenv = env.get('IMAGE_ENV')
    for dict in rootenv.get('DLINKER_INFO_COLLECTOR', []):
      # the dictionary always has one key value pair
      file_list = dict.values()[0]
      for file in file_list:
         env.Depends(node, file)
    
    return []   
   
#===============================================================================
# DYN SYMBOL EXPOSED INTERMEDIATES File build rules
#===============================================================================
def dlinker_symbol_auto_generate(env):
    
    # load builder into enviroment
    dlinker_symbol_act = env.GetBuilderAction(dlinker_symbols_generate_builder, action_source=None)
    dlinker_symbol_target_scan = env.Scanner(dlinker_symbol_target_scan_func, name='DLSymsTargetScanner')
    dlinker_symbol_bld = env.Builder(action = dlinker_symbol_act,
                                     #emitter=skeleton_emitter,
                                     target_scanner=dlinker_symbol_target_scan,
                                     #suffix = '.my_extention'
                                     )
    env.Append(BUILDERS = {'GenerateSymbolFile' : dlinker_symbol_bld})
    
    return

def dlinker_symbol_target_scan_func(node, env, path):
    """
    Scanner adds DLExposed Symbols Srings from each scons env as a dependency.
    """
  
    return []
    
#===============================================================================
# Builder function to generate intermediate file for call to DLExpose
#===============================================================================    
def dlinker_symbols_generate_builder(target, source, env):
   target_full = env.RealPath(str(target[0]))
   sym_list = env['SYMBOL_LIST']
   file_handle = open(target_full, "w")
   for symbol in sym_list:
      file_handle.write("%s\n" % symbol)
   file_handle.close()
   
   return

#===============================================================================
# function defintion mapped to Scons DLExpose Method
#===============================================================================
def dl_expose(env, name):
   
   # if dynamic loading not enabled, do nothing.
   #import pdb;pdb.set_trace()
   rootenv = env.get('IMAGE_ENV')
   Dict = {}
   
   # Get the symbol name in a list
   names = [n.strip() for n in name.split(',')]
   if(len(names)==0):
      return
   
   verbose = int(GetOption('verbose'))
   if verbose >= 2:
      print 'DLExpose: Exporting symbols from list  %s' % names
         
   # Generate unique prefix for intermediate file in a multi-threaded scons fwk in a dictionary. 
   # Note : Two different scons can have same index in a list but since the scope of these index
   # is hidden by a unique key in a dictionary it doesnt matter if they collide, another way to 
   # achieve a thread safety .
   
   dict_index = len(rootenv[DLINKER_ENV_DESCRIPTOR])
   sym_file = env.RealPath('${BUILDPATH}/dynexposed_syms' + str(dict_index) + '.isl')
   # unique key per scon scan ( path relative to BUILD_ROOT as key)
   sym_key_file = (env.RealPath('${BUILDPATH}').split(env.RealPath('${BUILD_ROOT}'))[1]).upper()
   output_sym_node = env.GenerateSymbolFile([sym_file], None, SYMBOL_LIST=names)
   env.Depends(output_sym_node[0], env.Value(names))
   Dict.update({sym_key_file : output_sym_node})
   rootenv[DLINKER_ENV_DESCRIPTOR].append(Dict)
   
   return
   
#===============================================================================
# function defintion mapped to Scons DLExposeFile Method
#===============================================================================   
def dl_exposeFile(env, filenames):
  #import pdb; pdb.set_trace()
  rootenv = env.get('IMAGE_ENV')
  Dict = {}
  file_list = []

  if(type(filenames) == list) :
   for file in filenames :
      file = env.RealPath(file)
      if not os.path.exists(file):
         errStr = 'DLExposeFile: Error: File %s does not exist. (ignored)' % file
         dlinker_error(env, errStr)
      else:
         file_list.append(file)
  else :
     filenames = env.RealPath(filenames)
     if not os.path.exists(filenames): 
        errStr = 'DLExposeFile: Error: File %s does not exist. (ignored)' % filenames
        dlinker_error(env, errStr)
     else :
        file_list.append(filenames) 
      
  verbose = int(GetOption('verbose'))
  if verbose >= 2:
     print 'DLExposeFile: Exporting symbols from file %s' % file_list
  
  # unique key per scon scan ( path relative to BUILD_ROOT as key)
  sym_key_file = (env.RealPath('${BUILDPATH}').split(env.RealPath('${BUILD_ROOT}'))[1]).upper()
  Dict.update({sym_key_file : file_list})
  rootenv[DLINKER_ENV_DESCRIPTOR].append(Dict)
  
  return

#===============================================================================
# Builder function to generate linker file for symbol exposed to linker
#===============================================================================  
def dlexpose_generate_builder(target, source, env):

   #import pdb;pdb.set_trace()
   dynsymbols_file = str(target[0])
   symbol_list = []
   
   rootenv = env.get('IMAGE_ENV')
   
   # Open the base symbol file for reading, and symbol file for writing.
   try:
      dyn_symbol_handle = open(dynsymbols_file, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      dlinker_error(env, errStr)
         
   verbose = int(GetOption('verbose'))
   if verbose >= 2:
      print 'DLExposeGenerateBuilder: Exporting symbols passed via DLExpose/DLExposeFile Method'
        

   for sym_files_dict in rootenv[DLINKER_ENV_DESCRIPTOR]:
      # Assumption : only one key value pair in dictionary
      sym_files = sym_files_dict.values()[0]
      for sym_file in sym_files : 
         # Open the symbol file for reading
         try:
            # can be scons node or file
            dlexpose_symbol_handle = open(str(sym_file), 'r')
         except IOError, error :
            errStr = error.strerror + ":" + error.filename
            dlinker_error(env, errStr)
         
         for line in dlexpose_symbol_handle :
            line = line.strip()
            if line and (line not in symbol_list) :
               symbol_list.append(line)
               dyn_symbol_handle.write(line + "\n")
         
         dlexpose_symbol_handle.close()
         
   dyn_symbol_handle.close()
   return

#===============================================================================
# Builder function to generate linker file for symbol exposed to linker
#===============================================================================     
def dlexpose_generate_lst_builder(target, source, env):

   #import pdb;pdb.set_trace()
   dynsymbols_file = str(target[0])
   externs_file = str(target[1])
   symbol_list = []

   
   rootenv = env.get('IMAGE_ENV')
   
   # Open the base symbol file for reading, and symbol file for writing.
   try:
      dyn_symbol_handle = open(dynsymbols_file, 'w')
      extern_symbol_handle = open(externs_file, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      dlinker_error(env, errStr)
         
   dyn_symbol_handle.write('{\n')
   extern_symbol_handle .write('EXTERN (')   

   for sym_file in source:
      # Open the symbol file for reading
      try:
         # can be scons node or file
         sym_file = env.RealPath(str(sym_file))
         dlexpose_symbol_handle = open(sym_file, 'r')
      except IOError, error :
         errStr = error.strerror + ":" + error.filename
         dlinker_error(env, errStr)
      
      for line in dlexpose_symbol_handle :
         line = line.strip()
         if line and (line not in symbol_list):
            symbol_list.append(line)
            dyn_symbol_handle.write(line + ";\n")
            extern_symbol_handle.write(line + "\n")
      
      dlexpose_symbol_handle.close()
   
   # write footer
   dyn_symbol_handle.write('};\n')
   extern_symbol_handle .write(');\n')
   
   dyn_symbol_handle.close()
   extern_symbol_handle.close()
   
   return   
   
#===============================================================================
# Error handler function
#=============================================================================== 
def dlinker_error(env, Info, Info1=None):
    """
    Error handler for linker symbol generate script. Info can be a string that
    describes the error and Info1 can be any data that needs to be printed along
    with the error message.
    """
    env.PrintError("Dlinker error found: " + Info)
    env.PrintError(Info1)
    raise Exception(Info)   
