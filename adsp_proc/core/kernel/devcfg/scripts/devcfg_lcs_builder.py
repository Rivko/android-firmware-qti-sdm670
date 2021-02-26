#===============================================================================
#
# Device Config Builder
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2011,2014-16 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
DEVCFG_SEGMENT_INFO_DICT = 'DEVCFG_SEGMENT_INFO_DICT'
SECTION_NAME_HDR_LIST = 'SECTION_NAME_HDR_LIST'
SECTION_ADDR_MACRO_LIST = 'SECTION_ADDR_MACRO_LIST'
SECTION_BASE_ADDR_LIST = 'SECTION_BASE_ADDR_LIST'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return True

def generate(env):
   """
   Generate function for devcfg lcs builder. 
   Generates devcfg linker script
   """

   rootenv = env.get('IMAGE_ENV')
   rootenv[DEVCFG_SEGMENT_INFO_DICT] = {}
   rootenv[SECTION_NAME_HDR_LIST] = []
   rootenv[SECTION_ADDR_MACRO_LIST] = []
   rootenv[SECTION_BASE_ADDR_LIST] = []
   
   # Add methods to get the above defined lists
   rootenv.AddMethod(get_devcfg_seg_info_dict, 'GetDevcfgSegInfoDict')
   rootenv.AddMethod(get_devcfg_section_name_list, 'GetDevcfgSectionInfoList')
   rootenv.AddMethod(get_devcfg_addr_macro_list, 'GetDevcfgAddrMacroList')
   rootenv.AddMethod(get_devcfg_section_base_addr_list, 'GetDevcfgSectionBaseAddrList')

   # Devcfg linker script generator
   devcfg_lcs_act = env.GetBuilderAction(devcfg_lcs_builder)
   devcfg_lcs_target_scan = env.Scanner(devcfg_lcs_target_scan_func, name='DevCfgTargetScanner')
   devcfg_lcs_bld = env.Builder(action = devcfg_lcs_act,
                                target_scanner = devcfg_lcs_target_scan,
                                suffix = '.lcs')
   rootenv.Append(BUILDERS = {'DevcfgLCSBuilder' : devcfg_lcs_bld})

   return
   
def devcfg_lcs_target_scan_func(node, env, path):
   rootenv = env.get('IMAGE_ENV')
   #rootenv.Depends(node, env.Value(env.get('DEVCFG_ENV_USED_XML_TAGS')))
   env.Depends(node, env.Value(rootenv.get('DEVCFG_ENV_USED_XML_TAGS')))
   
   return []

def get_devcfg_section_base_addr_list(env):
   rootenv = env.get('IMAGE_ENV')
   return rootenv[SECTION_BASE_ADDR_LIST]
   
def get_devcfg_seg_info_dict(env):
   rootenv = env.get('IMAGE_ENV')
   return rootenv[DEVCFG_SEGMENT_INFO_DICT]

def get_devcfg_section_name_list(env):
   rootenv = env.get('IMAGE_ENV')
   return rootenv[SECTION_NAME_HDR_LIST]

def get_devcfg_addr_macro_list(env):
   rootenv = env.get('IMAGE_ENV')
   return rootenv[SECTION_ADDR_MACRO_LIST]
   
def devcfg_lcs_builder(target, source, env):
   """
   source: lcs template file name with location
   """
   rootenv = env.get('IMAGE_ENV')
   
   if rootenv.GetUsesFlag('USES_DEVCFG') is False:
      return None
   
   # The lcs template file 
   tmplt_fn = env.RealPath(str(source[0]))
   
   if 'DEVCFG_DEF_HDR_FILE' not in env.Dictionary().keys():
      err_str = 'DEVCFG_DEF_HDR_FILE' + ' key required'
      raise RuntimeError, err_str
   devcfg_hdr_filename = env['DEVCFG_DEF_HDR_FILE']
   
   # The output linker script filename
   lcs_fn = env.RealPath(str(target[0]))
   
   create_devcfg_lcs_file(rootenv, tmplt_fn, devcfg_hdr_filename, lcs_fn)
   
def create_devcfg_lcs_file(env, template_file, devcfg_hdr_filename, output_devcfg_lcs_file):
   """
   This creates the output linker script for devcfg image
   INPUTS:
   env: Build environment
   template_file: Template filename that the linker script will be based off of
   devcfg_hdr_filename: The devcfg header file that will contain the addressed
   of the segments. These will be reserved in the primary elf
   OUTPUT:
   output_devcfg_lcs_file: Generated devcfg lcs file   
   """
   devcfg_xml_tag_list = env.GetDevcfgRelevantTags(['DAL_DEVCFG_IMG'], {})
   if len(devcfg_xml_tag_list) == 0:
      devcfg_lcs_error('None of the devcfg tags seemed to be used in the build\n')
      return
   try:
      tf = open(template_file, 'r')
   except IOError:
      err_str = "Couldn't open file: " + template_file
      devcfg_lcs_error(err_str)
   
   try:
      lcsf = open(output_devcfg_lcs_file, 'w')
   except IOError:
      err_str = " Couldn't open file: " + output_devcfg_lcs_file + "for writing\n"
      devcfg_lcs_error(err_str)
       
   line_num = 0
   
   # Create the devcfg hdr file list. These header files will hold the segment addrs that the primary elf has been built at
   devcfg_hdr_file_list = [devcfg_hdr_filename]

   # Create all the information required to write to the devcfg linker script for the segments
   #import pdb;pdb.set_trace()
   seg_info = {}
   env_segment_info = {}
   env_section_name_list = []
   env_addr_macro_list = [] 
   env_sec_base_addr_list = []
   for elem in devcfg_xml_tag_list:
      # .8026_PLATFORM_CDP_DEVCFG_DATA
      segment_name = '.' + (elem.replace('xml', 'DEVCFG_DATA'))
      # __8026_PLATFORM_CDP_devcfg_data_addr_base__
      segment_base_addr = '__' + elem.replace('xml', 'devcfg_data_addr_base__')
      # 8026_PLATFORM_CDP_data.o
      data_fileobj_name = elem.replace('xml', 'data.o')
      # DALConfig_8026_PLATFORM_CDP.o
      dalconfig_filename = 'DALConfig_' + elem.replace('_xml', '.o')
      # 8026_PLATFORM_CDP_devcfg_*.o
      devcfg_c_filename = elem.replace('xml', 'devcfg_*.o')
      # DEVCFG_DATA_ADDR_8026_PLATFORM_CDP
      devcfg_hdr_file_addr_var = 'DEVCFG_DATA_ADDR_' + elem.replace('_xml', '')
      # {'8026_PLATFORM_CDP_xml': ['.8026_PLATFORM_CDP_DEVCFG_DATA', 
      #                            '__8026_PLATFORM_CDP_devcfg_data_addr_base__', 
      #                            '8026_PLATFORM_CDP_data.o', 
      #                            'DALConfig_8026_PLATFORM_CDP.o', 
      #                            '8026_PLATFORM_CDP_devcfg_*.o', 
      #                            'DEVCFG_DATA_ADDR_8026_PLATFORM_CDP'] 
      seg_info[elem] = [segment_name, segment_base_addr, data_fileobj_name, dalconfig_filename, devcfg_c_filename, devcfg_hdr_file_addr_var]
      env_segment_info[elem] = [segment_name, segment_base_addr, devcfg_hdr_file_addr_var]
      env_section_name_list.append(segment_name)
      env_addr_macro_list.append(devcfg_hdr_file_addr_var)
      env_sec_base_addr_list.append(segment_base_addr)
      
   # Add the segment name, base address and the address variable to be used in devcfg_hdr.h file to the build env
   env[DEVCFG_SEGMENT_INFO_DICT] = env_segment_info
   env[SECTION_NAME_HDR_LIST] = env_section_name_list
   env[SECTION_ADDR_MACRO_LIST] =  env_addr_macro_list
   env[SECTION_BASE_ADDR_LIST] = env_sec_base_addr_list
   env.Replace(DEVCFG_SEGMENT_INFO_DICT = env[DEVCFG_SEGMENT_INFO_DICT])
   env.Replace(SECTION_NAME_HDR_LIST = env_section_name_list)
   env.Replace(SECTION_ADDR_MACRO_LIST =  env_addr_macro_list)
   env.Replace(SECTION_BASE_ADDR_LIST =  env[SECTION_BASE_ADDR_LIST])
   #env.Append(CPPDEFINES = ["DEVCFG_SEGMENT_INFO_DICT=$(DEVCFG_SEGMENT_INFO_DICT)"])
   #env.Append(CPPDEFINES = ["SECTION_NAME_HDR_LIST=$(SECTION_NAME_HDR_LIST)"])
   #env.Append(CPPDEFINES = ["SECTION_ADDR_MACRO_LIST=$(SECTION_ADDR_MACRO_LIST)"])
   
   for i in devcfg_hdr_file_list:
      lcsf.write('#include "' + i + '"\n')
   
   for line in tf:
      line_num += 1
      if line_num > 3:
         if line == 'SECTIONS\n':
            for val in seg_info.values():
               lcsf.write(val[1] + ' = ' + val[5] + ';\n')
            lcsf.write('\n')
         lcsf.write(line)
         if line == '/* Constants start. */\n':
            for val in seg_info.values():
               #lcsf.write('  .DEVCFG_DATA(__devcfg_data_addr_base__) :\n  {\n    KEEP (*8974_data.o (.rodata* .data* .sdata*))\n    KEEP (*DALConfig_8974.o (.rodata* .data* .sdata*))\n    KEEP (*8974_devcfg_*.o (.rodata* .data* .sdata*))\n  }')
               lcsf.write('  ' + val[0] + '(' + val[1] + ') :\n  {\n    KEEP (*' + val[2] + ' (.rodata* .data* .sdata*))\n    KEEP (*' + val[3] + ' (.rodata* .data* .sdata*))\n    KEEP (*' + val[4] + ' (.rodata* .data* .sdata*))\n  }\n')
   
   tf.close()
   lcsf.close()
   
   return
    
def devcfg_lcs_error(Info, Info1=None):
   """
   Error handler for devcfg framework. Info can be a string that describes the error and
   Info1 can be any data that needs to be printed along with the error message.
   """
   print Info
   raise Exception(Info)
   
