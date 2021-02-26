#===============================================================================
# Copyrigha (c) 2011, 2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/elfmergerutil.py#1 $
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
# 10/02/14   aa      Removed Qshrink support, moved to devcfg_img.scons
# 10/02/14   aa      Created History
#
#===============================================================================

import sys
import os
import shutil
from SCons.Script import *
from elfparserutil import *

MERGED_ELFS_LIST = 'merged_elf_list'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('elfmergerutil_builder')

def generate(env):
   rootenv = env.get('IMAGE_ENV')
   #rootenv.AddMethod(query_section_by_name_util, "QuerySectionByName")
   rootenv[MERGED_ELFS_LIST]= []
   
   elfmergerutil_act = env.GetBuilderAction(elfmergerutil_builder)
   elfmergerutil_scan = env.Scanner(elfmergerutil_target_scan_func, name='ElfmergerutilTargetScanner')
   elfmergerutil_bld = env.Builder(action = elfmergerutil_act, 
                         source_scanner = SCons.Scanner.C.CScanner(),
                         target_scanner = elfmergerutil_scan,
                         #emitter = elfmergerutil_emitter
                         )

   rootenv.Append(BUILDERS = {'ElfMergerUtilBuilder' : elfmergerutil_bld})

def elfmergerutil_emitter(target, source, env):
   if len(env[MERGED_ELFS_LIST]) > 0:
     target.append(env[MERGED_ELFS_LIST])
   return (target, source)
    
# Target Scanner: Add all the intermediate elf files that were created while merging multiple sections
def elfmergerutil_target_scan_func(node, env, path):
   # if len(env[MERGED_ELFS_LIST]) > 0:
     # return_list = [node, env[MERGED_ELFS_LIST]]
   # else:
     # reutrn_list = [node]rootenv = env.get('IMAGE_ENV')
   rootenv = env.get('IMAGE_ENV')
   env.Depends(node, env.Value(rootenv.get('SECTION_NAME_HDR_LIST')))
   
   return []
   
def merge_elf_util(pri_file_name, pri_section_name, sec_file_name, 
                    sec_section_name, out_file_name, verbose=False):
    global VERBOSE
    VERBOSE = verbose
    DEBUG_PRINT("Running merge on Primary ELF:[" + pri_file_name + "] at section containing "   \
                "name:[\"" + pri_section_name + "\"] and Secondary ELF:[" + sec_file_name +     \
                "] from section(s) containing name:[\"" + sec_section_name + "\"]. "            \
                "Destination Output ELF:[" + out_file_name + "].")

    sec_elf_fp = OPEN(sec_file_name, "rb")
    sec_elf_dict = preprocess_elf_file(sec_elf_fp, sec_file_name)

    shutil.copyfile(pri_file_name, out_file_name)
    out_elf_fp = OPEN(out_file_name, "r+b")
    out_elf_dict = preprocess_elf_file(out_elf_fp, out_file_name)

    no_error = False
    try:
        merge_sections_by_name(out_elf_dict, sec_elf_dict, pri_file_name,
                            pri_section_name, sec_section_name)
        no_error = True
    except RuntimeError as err_str:
        raise RuntimeError, err_str
    finally:
        sec_elf_fp.close()
        out_elf_fp.close()
        if False == no_error:
            os.remove(out_file_name)

    return

def create_section_name_list(env):
   """
   Create the section name list here. This has to be the same as the
   one created in devcfg_lcs_builder.py.
   """
   devcfg_xml_tag_list = env.GetDevcfgRelevantTags(['DAL_DEVCFG_IMG'], {})
   if len(devcfg_xml_tag_list) == 0:
      elfmergeutil_error('None of the devcfg tags seemed to be used in the build\n')
      return
   env_section_name_list = []
   for elem in devcfg_xml_tag_list:
      # .8026_PLATFORM_CDP_DEVCFG_DATA
      sec_name = '.' + (elem.replace('xml', 'DEVCFG_DATA'))
      env_section_name_list.append(sec_name)
      
   return env_section_name_list
    
   
def elfmergerutil_builder(target, source, env):
   rootenv = env.get('IMAGE_ENV')
   pri_elf = env.RealPath(str(source[0]))
   sec_elf = env.RealPath(str(source[1]))
   target_full = env.RealPath(str(target[0]))

   pri_sec_name = env['PRIMARY_SECTION_NAME']
   sec_sec_name = env['SECONDARY_SECTION_NAME']
   if (len(pri_sec_name) > 0) and (len(sec_sec_name) > 0):
      merge_elf_util(pri_elf, pri_sec_name, sec_elf, sec_sec_name, target_full, True)
   if (len(pri_sec_name) == 0) and (len(sec_sec_name) == 0):
      #section_names_list = rootenv.get('SECTION_NAME_HDR_LIST')
      section_names_list = create_section_name_list(rootenv)
      if len(section_names_list) == 0:
         elfmergeutil_error('Section names list is empty')
      tmp_merged_target = ''
      temp_merged_elf = None
      #import pdb;pdb.set_trace()
      for each_section_name in section_names_list:
         if section_names_list.index(each_section_name) == (len(section_names_list) - 1):
            tmp_merged_target = target_full
         else:
            tmp_merged_target = 'merged_elf_' + each_section_name.replace('.', '') + '.elf'
         temp_merged_elf = merge_elf_util(pri_elf, each_section_name, sec_elf, each_section_name, tmp_merged_target, True)
         env[MERGED_ELFS_LIST].append(temp_merged_elf)
         pri_elf = tmp_merged_target
     
   return
   
def elfmergeutil_error(Info, Info1=None):
   """
   Error handler for elfmergeutil module. Info can be a string that 
   describes the error and Info1 can be any data that needs to be 
   printed along with the error message.
   """
   print Info
   raise Exception(Info)
