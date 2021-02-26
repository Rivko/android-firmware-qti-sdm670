# Copyright (c) 2011
# Qualcomm Technologies Incorporated
#!/usr/bin/env python

import sys
import os
import shutil
from SCons.Script import *
from elfparserutil import *

INPUT_ELF = 'input_elf'
SECTION_NAME = 'section_name'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('gendevcfghdr_builder')

def generate(env):
    rootenv = env.get('IMAGE_ENV')   
    gendevcfghdr_act = env.GetBuilderAction(gendevcfghdr_builder)
    devcfg_hdr_file_target_scan = env.Scanner(devcfg_hdr_file_target_scan_func, name='DevCfgHdrFileTargetScanner')
    gendevcfghdr_bld = env.Builder(action = gendevcfghdr_act, 
                                        source_scanner = SCons.Scanner.C.CScanner(),
                                        target_scanner = devcfg_hdr_file_target_scan,
                                    )
    rootenv.Append(BUILDERS = {'GenerateDevCfgHdrs' : gendevcfghdr_bld})
    
    return

def devcfg_hdr_file_target_scan_func(node, env, path):
   rootenv = env.get('IMAGE_ENV')
   rootenv.Depends(node, env.Value(env.get('DEVCFG_ENV_USED_XML_TAGS')))   
   # TODO: try return[node, env.get('DEVCFG_ENV_USED_XML_TAGS')]
   return []

def gendevcfghdr_builder(target, source, env):
    pri_file_name = env.RealPath(str(source[0]))
    target_full = env.RealPath(str(target[0]))
    dir_name, file_name = os.path.split(str(target[0]))
    # Converting devcfg_data.h to DEVCFG_DATA_H
    file_name_def = (file_name.replace(".", "_")).upper()    
    pri_section_name_list = []
    section_addr_macro_list = []
    devcfg_xml_tag_list = env.GetDevcfgRelevantTags(['DAL_DEVCFG_IMG'], {})
    for elem in devcfg_xml_tag_list:
        # .8026_PLATFORM_CDP_DEVCFG_DATA
        segment_name = '.' + (elem.replace('xml', 'DEVCFG_DATA'))  
        # DEVCFG_DATA_ADDR_8026_PLATFORM_CDP: Note the macro name can not start with a number. 
        # Compiler throws the folowing error in that case: macro names must be identifiers
        devcfg_hdr_file_addr_var = 'DEVCFG_DATA_ADDR_' + elem.replace('_xml', '')
        pri_section_name_list.append(segment_name)    
        section_addr_macro_list.append(devcfg_hdr_file_addr_var)

    #Initialize Primary ELF
    pri_elf_fp = OPEN(pri_file_name, "rb")
    pri_elf_dict = preprocess_elf_file(pri_elf_fp, pri_file_name)
    if IS_DEBUG():
        readelf(pri_elf_dict)    

    seg_info = {}
    for pri_section_name, section_addr_macro in zip(pri_section_name_list, section_addr_macro_list):
        elf_sec_list = []
        elf_sec_list = query_section_by_name(pri_elf_dict, pri_section_name, compare_full_sect_str=True)

        if len(elf_sec_list) != 1:
            err_str = 'Error:There has to be only one section in the ELF file {0} ' \
                    'with section name:{1}'.format(pri_file_name, pri_section_name)
            env.PrintError(err_str)
            raise RuntimeError, err_str
   
        if elf_sec_list[0].has_key('addr') is False:
            err_str = 'Error:No \'Addr\' found in the ELF file {0} ' \
                    'with section name:{1}'.format(pri_file_name, pri_section_name)
            env.PrintError(err_str)
            raise RuntimeError, err_str

        devcfg_sec_vaddr = elf_sec_list[0]['addr']
        seg_info[pri_section_name] = [section_addr_macro, devcfg_sec_vaddr]     
    pri_elf_fp.close()

    # Add devcfg_sec_vaddr to  devcfg_def.h
    devcfg_def_hdr_file_loc = target_full
    devcfg_def_hdr_file = open(devcfg_def_hdr_file_loc, 'w')
    devcfg_def_hdr_file.write("#ifndef {0}\n".format(file_name_def))
    devcfg_def_hdr_file.write("#define {0}\n".format(file_name_def))
    for section_addr_macro, devcfg_sec_vaddr in seg_info.values():
        devcfg_def_hdr_file.write("#define {0} {1:#0x}\n".format(section_addr_macro, devcfg_sec_vaddr))
        env.Append(CPPDEFINES = [("{0}={1:#0x}".format(section_addr_macro, devcfg_sec_vaddr))])
    devcfg_def_hdr_file.write("#endif /* ifndef {0} */\n".format(file_name_def))
    devcfg_def_hdr_file.close()
    return

def OPEN(file_name, mode):
    try:
        fp = open(file_name, mode)
    except IOError:
        raise RuntimeError, "The file could not be opened: " + file_name

    # File open has succeeded with the given mode, return the file object
    return fp

def query_section_by_name_util(env, targets, input_dict):
    if INPUT_ELF not in input_dict:
        err_str = INPUT_ELF + ' key required'
        raise RuntimeError, err_str
    pri_file_name = input_dict[INPUT_ELF]
    pri_section_name = input_dict[SECTION_NAME]
    env.Depends(pri_section_name, pri_file_name)
    print "File name is : " + pri_file_name, pri_section_name + "\n"
    DEBUG_PRINT("Querying Section details for: " + pri_section_name + " for ELF: " + pri_file_name)

    #Initialize Primary ELF
    pri_elf_fp = OPEN(pri_file_name, "rb")
    pri_elf_dict = preprocess_elf_file(pri_elf_fp, pri_file_name)

    sec_hdrs = query_section_by_name(pri_elf_dict, pri_section_name)

    pri_elf_fp.close()
    
    return sec_hdrs
    
