#===============================================================================
#
# Utils for extracting all unresolved symbols in shared library(ELF SO)
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
# Copyright (c) 2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/sharedlib_symbols.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#  $Change: 13907704 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/15/14   aa      Create
#
#===============================================================================
import struct
import sys
import os
#import shutil
#import string
#import types
#from SCons.Script import *

# Import ELF parser utils
# To do : use env.RealPath(SCONS_ROOT), again it depends on where the elfparser util is hosted.
#sys.path.append('D:/Builds/Badger/MPSS2.1/CORE.MPSS.2.1.DYNLOAD_DLEXPORT_SUPPORT/modem_proc/core/bsp/build/scripts') 
#import elfparserutils

#------------------------------------------------------------------------------
# Hooks for SCons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sharedlib_symbols')
   
def generate(env):
   try:
      globals()["elfparserutils"] = __import__("elfparserutils")
   except (AttributeError, ImportError):
      sys.path.append(env.RealPath("${BUILD_SCRIPTS_ROOT}"))
      globals()["elfparserutils"] = __import__("elfparserutils")

   #env.AddUsesFlags(['ENABLE_DLFWK'])
   sharedlib_symbol_master_auto_generate(env)
   sharedlib_symbol_auto_generate(env)

   return
   
#===============================================================================
# Builder to generate master lst files(2) passed to linker
#=============================================================================== 
def sharedlib_symbol_master_auto_generate(env):
    
   env.AddMethod(sharedlib_sym_import, "SharedLibGenerateList")
   env.AddMethod(sharedlib_consolidate_syms, "ConsolidateSymsList")
   sharedlib_action = env.GetBuilderAction(sharedlib_symlst_generate_builder)
   sharedlib_bld = env.Builder(action = sharedlib_action,
                             #emitter=skeleton_emitter,
                             #target_scanner=skeleton_scanner,
                             #suffix = '.my_extention'
                            )
   env.Append(BUILDERS = {'SharedLibGenerateListBldr' : sharedlib_bld})  
   return
        
#===============================================================================
# Builder to extract all unresolved symbols (UND) from shared library
#===============================================================================
def sharedlib_symbol_auto_generate(env):
    
    sharedlib_symbol_act = env.GetBuilderAction(sharedlib_symbols_generate_builder)
    sharedlib_symbol_bld = env.Builder(action = sharedlib_symbol_act,
                                     #emitter=skeleton_emitter,
                                     #target_scanner=skeleton_scanner,
                                     #suffix = '.my_extention'
                                     )
    env.Append(BUILDERS = {'SharedLibGenerateSymbolFile' : sharedlib_symbol_bld})
    
    return

#===================================================================================
# Method to parse elf and extract symbols that are unresolved(UND) in shared library
#==================================================================================== 
def get_undef_symbol_list(elf_fp, elf_file_name) :

   # Initialize
   elf_fp.seek(0)
   elf_header = elfparserutils.Elf32_Ehdr(elf_fp.read(elfparserutils.Elf32_Ehdr.ELF_HDR_SIZE))
   
   # Verify ELF header information
   if elfparserutils.verify_elf_header(elf_header) is False:
      raise RuntimeError, "ELF file failed verification: " + elf_file_name

   # Get section header details (size, file offset, number of headers)
   shdr_table = []
   shdr_size = elf_header.e_shentsize
   shdr_file_offset = elf_header.e_shoff
   shdr_count = elf_header.e_shnum

   # Read all the section headers into shdr table
   shdr_shstrtab_file_offset = -1
   elf_fp.seek(shdr_file_offset)
   for i in range(shdr_count):
       is_shstrtab = 0
       if i == elf_header.e_shstrndx:
           is_shstrtab = 1    
       data = elfparserutils.Elf32_Shdr(elf_fp.read(shdr_size), i,is_shstrtab)
       shdr_table.append(data)
       if is_shstrtab == 1:
           shdr_shstrtab_file_offset = data.sh_offset
    
  
   for i in range(shdr_count):
       elf_fp.seek(shdr_shstrtab_file_offset + shdr_table[i].sh_name)
       ch = elf_fp.read(1)
       while ('\x00' != ch):
           shdr_table[i].str_name += ch
           ch = elf_fp.read(1)
        
   # Read all the symbols which are unresolved.
   elf_symbols = elfparserutils.ElfSymbols(elf_fp, shdr_table)
   undef_symbol_list = []
   for symbol in elf_symbols.symbol_dict :
      if  elf_symbols.symbol_dict[symbol]['shndx'] == 0 :
         undef_symbol_list.append(symbol)
      
   #import pdb; pdb.set_trace()
   return undef_symbol_list
    
#========================================================================================
# Builder function to generate intermediate file(.isl) for shared library with invocation
# of SharedLibGenerateSymbolFile
#========================================================================================    
def sharedlib_symbols_generate_builder(target, source, env):
   
   #import pdb;pdb.set_trace()
   target_full = str(target[0]) # shared lib(.isl)
   source_full = str(source[0]) # shared lib(.so)
   try:
      elf_fp = open(source_full, 'rb')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      raise RuntimeError, "The file could not be opened:" +  errStr
    
   # parse out all undef symbols    
   sym_list = get_undef_symbol_list(elf_fp, source_full)
   try:
      file_handle = open(target_full, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      raise RuntimeError, "The file could not be opened:" +  errStr
   
   for symbol in sym_list:
      file_handle.write("%s\n" % symbol)
   file_handle.close()
   
   return

#==========================================================================================
# function definition mapped to SCons SharedLibGenerateSymbolFile Builder
# Generate linker lst files
#==========================================================================================
def sharedlib_sym_import(env, target, source):
   
   #import pdb;pdb.set_trace()    
   symbol_node_list = []
   shlib_sym_lst = env.RealPath(str(target[0]))
     
   for shlib in  source :
     path, ext = os.path.splitext(env.RealPath(str(shlib)))
     target_sym = [path + ".isl"]
     source_sym = [shlib]
     output_sym_node = env.SharedLibGenerateSymbolFile(target_sym, source_sym)
     symbol_node_list.append(output_sym_node[0])
   
   shlib_sym_lst_node = env.SharedLibGenerateListBldr([shlib_sym_lst], symbol_node_list)
   
   return shlib_sym_lst_node

#==========================================================================================
# function wrapper mapped to Method ConsolidateSymsList to
# consolidate symbols files
#==========================================================================================
def sharedlib_consolidate_syms(env, target, source):
   return env.SharedLibGenerateListBldr(target, source)

#===============================================================================
# Builder function to generate linker file for symbol exposed to linker
#===============================================================================  
def sharedlib_symlst_generate_builder(target, source, env):

   #import pdb;pdb.set_trace()
   dynsymbols_file = str(target[0])
   symbol_list = []
     
   # Open the base symbol file for reading, and symbol file for writing.
   try:
      dyn_symbol_handle = open(dynsymbols_file, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      sharedlib_error(env, errStr)
                
   for sym_file in source:
      # Open the symbol file for reading
      try:
         # can be scons node or file
         dlexpose_symbol_handle = open(str(sym_file), 'r')
      except IOError, error :
         errStr = error.strerror + ":" + error.filename
         sharedlib_error(env, errStr)
         
      for line in dlexpose_symbol_handle :
            line = line.strip()
            if line and line not in symbol_list : 
               symbol_list.append(line)
               dyn_symbol_handle.write(line + "\n")
         
      dlexpose_symbol_handle.close()
         
   dyn_symbol_handle.close()
   return
   
#===============================================================================
# Builder function to generate linker file for symbol exposed to linker
#===============================================================================  
def sharedlib_linker_lst_generate_builder(target, source, env):

   #import pdb;pdb.set_trace()
   dynsymbols_file = str(target[0])
   externs_file = str(target[1])
   symbol_list = []
    
   # Open the base symbol file for reading, symbol and extern file for writing.
   try:
      dyn_symbol_handle = open(dynsymbols_file, 'w')
      extern_symbol_handle = open(externs_file, 'w')  
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      sharedlib_error(env, errStr)
   
   extern_symbol_handle .write('EXTERN (')
   dyn_symbol_handle.write('{\n')
   
   for sym_file in source:
      # Open the symbol file for reading
      try:
         # can be scons node or file
         dlexpose_symbol_handle = open(str(sym_file), 'r')
      except IOError, error :
         errStr = error.strerror + ":" + error.filename
         sharedlib_error(env, errStr)
      
      for line in dlexpose_symbol_handle :
         line = line.strip()
         if line not in symbol_list : 
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

#======================================================================================
# Error handler function
#====================================================================================== 
def sharedlib_error(env, Info, Info1=None):
    """
    Error handler for linker symbol generate script. Info can be a string that 
    describes the error and Info1 can be any data that needs to be printed along 
    with the error message.
    """
    env.PrintError("sharedlib error found: " + Info)
    env.PrintError(Info1)
    raise Exception(Info)   
