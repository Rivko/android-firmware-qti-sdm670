#===============================================================================
#
# CBSP Kernel Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/kernel_builders.py#1 $
#  $DateTime: 2018/02/06 02:57:17 $
#  $Author: pwbldsvc $
#  $Change: 15399902 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import os
import subprocess
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('kernel_builders')

def generate(env):
   if env.has_key('USES_L4_3X'):
      ELFWEAVER_FILE_ARM = "${L4_ROOT}/tools/pyelf/elfweaver.py"
   else:
      ELFWEAVER_FILE_ARM = "${L4_ROOT}/tools/pyelf/elfweaver"

   ELFWEAVER_FILE_QDSP6 = "${L4_ROOT}/../l4-qdsp6/bin/elfweaver"
   
   # set up elfweaver command depeding on tool set
   env.Replace(ELFWEAVER_FILE = ELFWEAVER_FILE_ARM)
   
   if env.has_key('QDSP6_PROC'):
      env.Replace(ELFWEAVER_FILE = ELFWEAVER_FILE_QDSP6)
      
   env.Replace(ELFWEAVER_CMD = "${PYTHONCMD} ${ELFWEAVER_FILE}")

   # load builder into enviroment
   pbn_generate(env)
   l4_scl_hdr_generate(env)
   quartz_xml_generate(env)
   quartz_constant_generate(env)
   debug_constant_generate(env)
   
   # add methods to enviroment
   env.AddMethod(get_symbol_addr, "GetSymbolAddress")   
   env.AddMethod(get_pheader_info, "GetProgramHeaderInfo")
   env.AddMethod(build_kernel_cmm, "BuildKernelCMMs")
   
#------------------------------------------------------------------------------
# Get Symbol Address Info
# params image file, search symbol name
#------------------------------------------------------------------------------
def get_symbol_addr(env, image_file, search_sym_name):

   # make sure this is not  a list, andget real path
   if type(image_file) is list:
      image_file = image_file[0]
      
   image_file = env.RealPath(image_file)
   search_sym_name = str(search_sym_name)

   # search symbol data usually in the form of
   # 2066  mmu_data                   0xf001de64   Gb    9  Data  De   0xc
   cmd = "${BINTOOL} --text -s " + image_file
   sym_data, err, rv = env.ExecCmds(cmd, target="Symbol Address", silent=True)
   
   sym_data_re = re.compile(r'^\s+\S+\s+'+ search_sym_name +'\s+(\S+)\s+', re.M)
   sym_data = sym_data_re.findall(sym_data)

   if len(sym_data) == 0:
      env.PrintError("Cannot find symbol address in image file ["+ image_file +"].")
      raise RuntimeError, 'Cannot find symbol in image file.'

   return eval(sym_data[0])
   
#------------------------------------------------------------------------------
# Get Program Header Info
# params image file, search val_name, search_val, return_val_name
#------------------------------------------------------------------------------
def get_pheader_info(env, image_file, search_val_name, search_val, return_val_name):
   
   # make sure this is not  a list, andget real path
   if type(image_file) is list:
      image_file = image_file[0]
      
   image_file = env.RealPath(image_file)
   search_val_name = str(search_val_name)
   return_val_name = str(return_val_name)

   #----------------------------------------------------------------------------
   # create value name offset dictionary and get offset acording to input
   val_name_offset_dic = {
      "Offset" : 0,
      "VirtAddr" : 1,
      "PhysAddr" : 2,
      "FileSiz" : 3,
      "MemSiz" : 4,
      "Flg" : 5,
      "Align" : 6,
   }
   
   search_offset = val_name_offset_dic.get(search_val_name, 'None')
   return_offset = val_name_offset_dic.get(return_val_name, 'None')
   
   if search_offset is 'None':
      env.PrintError(search_val_name + " is not a valid program header value name.")
      raise RuntimeError, "Bad program header value name."

   if return_offset is 'None':
      env.PrintError(return_val_name + " is not a valid program header value name.")
      raise RuntimeError, "Bad program header value name."
   
   #----------------------------------------------------------------------------
   # use elfweaver to get entry point to l4
   cmd = "${ELFWEAVER_CMD} print --pheaders " + image_file
   data, err, rv = env.ExecCmds(cmd, target="Image program header data")   
   
   #----------------------------------------------------------------------------
   # search for entry points
   #
   # Program Headers:
   # Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
   # LOAD           0x008000 0xf0000000 0x00500000 0x1c290 0x1f958 RWE 0x8000
   # ....
   # ....
   entry_re = re.compile(r'^\s+LOAD\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)', re.M)
   entry = entry_re.findall(data)
   
   if len(entry) == 0:
      env.PrintError("Cannot find any entry points from elfweavers program header dump")
      raise RuntimeError, 'No progaram headers data found.'

   #----------------------------------------------------------------------------
   # find requetsed info
   return_val = 'None'
   for index in range(len(entry)):
      #import pdb; pdb.set_trace()
      if eval(entry[index][search_offset]) == eval(str(search_val)):
         return_val = eval(entry[index][return_offset])
         break
         
   if return_val is 'None':
      env.PrintError("Cannot find "+ search_val_name +" with value of "+ str(search_val) +" in program header data.")
      raise RuntimeError, "Bad search value."

   return return_val
   
#------------------------------------------------------------------------------
# Build CMMs method
#------------------------------------------------------------------------------
def build_kernel_cmm(env, booting_pbn):
   units = []
   
   if env['MSM_ID'] in ['6695','6615']:
      debug_cosntant_cmm = env.DbgConstBuilder('dbg_const_${TARGET_NAME}', [
         '${BUILD_MS_ROOT}/cmm/debug_constant_compile_in.cmm',
         target_pbn
      ])
   
      install_dbg_constant_cmm = env.InstallAs(
         '${BUILD_MS_ROOT}/cmm/debug_constant_compile.cmm',
         debug_cosntant_cmm
      )
   
      units += [debug_cosntant_cmm, install_dbg_constant_cmm]
      
      Kernel_cfg = ""
   else:
      Kernel_cfg = "_${TARGET_BLD_KCFG}"
      
   quartz_constant = env.QuartzConstBuilder("quartz_const_${TARGET_NAME}", [
      "${L4_ROOT}/build"+ Kernel_cfg +"/ms/quartz_constants"+ Kernel_cfg +".cmm",
      booting_pbn,
   ])
   
   units += [quartz_constant]
   
   return units

#-------------------------------------------------------------------------------
# Quartz XML Scanner to add other dependencies found from within source files
# It scans quartz xml files when they are used as source to gen pbn or scl l4 
# header file.
def quartz_xml_scanner(node, env, path):
   if not str(node.rfile()).endswith('.xml'):
      return []

   # get content of file
   contents = node.get_contents()
   
   # search for dependencies in file
   files_re = re.compile(r'\s+file\s*=\s*\"(\S+)\"', re.M)
   files = files_re.findall(contents)
   
   # fix paths, etc in files, also don't add reloc
   elfweaver_exec_root = "${L4_ROOT}"
   
   if env.has_key('QDSP6_PROC'):
      elfweaver_exec_root = "${BUILD_MS_ROOT}"
   
   file_list = []
   for file in files:
      if file.find('reloc') == -1:
         file_list.append(env.RealPath(elfweaver_exec_root + "/" + file))
      
   #import pdb; pdb.set_trace()   
   return file_list

#===============================================================================
# PBN build rules
# 1) PBN builder one to get next offset 
# 2) Modify process XML to include new section
# 3) PBN builder two generate final pbn image
#===============================================================================

#-------------------------------------------------------------------------------
# def builder for PBN
#
def pbn_generate(env):
   # register mothod to call multiple builders
   env.AddMethod(pbn_complex_builder, "PbnBuilder")
   env.AddMethod(pbn_get_demand_loading_data, "PbnGetDemandLoadingData")
   
   # register QuartzXMLModifyBuilder
   quartz_xml_sc = env.Scanner(function=quartz_xml_scanner, name='Quartz XML scanner', skeys=['.xml'])
   quartz_xml_act = env.GetBuilderAction(quartz_xml_modify_builder)
   quartz_xml_bld = env.Builder(action = quartz_xml_act,
                         source_scanner = quartz_xml_sc,
                         emitter = quartz_xml_modify_emitter,
                         suffix = '.xml', src_suffix = '.xml')

   env.Append(BUILDERS = {'QuartzXMLModifyBuilder' : quartz_xml_bld})   
   
   # register PbnDataBuilder
   pbn_data_sc = env.Scanner(function=quartz_xml_scanner, name='Quartz XML scanner', skeys=['.xml'])
   pbn_data_act = env.GetBuilderAction(pbn_data_builder)
   pbn_data_bld = env.Builder(action = pbn_data_act,
                         source_scanner = pbn_data_sc,
                         emitter = pbn_data_emitter,
                         suffix = '.tmp.txt', src_suffix = '.xml')

   env.Append(BUILDERS = {'PbnDataBuilder' : pbn_data_bld})
   
   # register PbnFileBuilder
   pbn_file_sc = env.Scanner(function=quartz_xml_scanner, name='Quartz XML scanner', skeys=['.xml'])
   pbn_file_act = env.GetBuilderAction(pbn_file_builder)
   pbn_file_bld = env.Builder(action = pbn_file_act,
                         source_scanner = pbn_file_sc,
                         emitter = pbn_file_emitter,
                         suffix = '.pbn', src_suffix = '.xml')

   env.Append(BUILDERS = {'PbnFileBuilder' : pbn_file_bld})
   
#-------------------------------------------------------------------------------
# PbnBuilder Function calls the complicated process to build the PBN
# source[0] quartz cfg XML
# source[1] ELF file
#
def pbn_complex_builder(env, target, source):
   if type(source) is not list:
      source = [source]
   
   if env['MSM_ID'] in ['6695','6615','8660','8960']:
      pbn_data_txt = env.PbnDataBuilder(target, source)
      env.Replace(PBN_DATA_FILE = os.path.realpath(str(pbn_data_txt[0])))
      
   if env['MSM_ID'] in ['6695']:
      mod_xml_file = env.QuartzXMLModifyBuilder('quartz_mod_cfg_${TARGET_NAME}', [source, pbn_data_txt])
      source[0] = mod_xml_file
      
   pbn_two = env.PbnFileBuilder(target, source)

   if env['MSM_ID'] in ['8660','8960']:
      env.Depends(pbn_two, pbn_data_txt)

   return pbn_two
   
#-------------------------------------------------------------------------------
# PbnGetDemandLoadingData
# Function return the DL data, in the following order
# HASH_SEGMENT_SIZE, HASH_SEGMENT_OFFSET, LAST_PHYS_ADDRESS
#
def pbn_get_demand_loading_data(env):
   # get filename from environment
   pbn_data_txt = env.get('PBN_DATA_FILE')

   if pbn_data_txt is None:
      env.PrintError("No PBN Demand Loading data file name found.")
      raise RuntimeError, 'No enviroemnt KEY(PBN_DATA_FILE).'      

   pbn_data_txt_file = open(pbn_data_txt, 'r')
   data = pbn_data_txt_file.read()
   pbn_data_txt_file.close()
      
   # process data
   # we are loking for something like...
   # hash_segment_size=6880
   # hash_segment_offset=0x616000
   # last_phys_address=0x14d8000
   
   #import pdb; pdb.set_trace()
   rematch = re.search(
      "\s*hash_segment_size=(?P<HASH_SEGMENT_SIZE>\w+)"
      "\s*hash_segment_offset=(?P<HASH_SEGMENT_OFFSET>\w+)"
      "\s*last_phys_address=(?P<LAST_PHYS_ADDRESS>\w+)",
      data
   )

   if rematch is not None:
      HASH_SEGMENT_SIZE = rematch.group('HASH_SEGMENT_SIZE')
      HASH_SEGMENT_OFFSET = rematch.group('HASH_SEGMENT_OFFSET')
      LAST_PHYS_ADDRESS = rematch.group('LAST_PHYS_ADDRESS')
   else:
      env.PrintError("Cannot find hash_segment_size, hash_segment_offset or last_phys_address.")
      raise RuntimeError, 'Cannot find data from elfweaver output.'
      
   return HASH_SEGMENT_SIZE, HASH_SEGMENT_OFFSET, LAST_PHYS_ADDRESS
   
#-------------------------------------------------------------------------------
# QuartzXML Modify emitter to add clean actions
#
def quartz_xml_modify_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# QuartzXML Modify builder for qcoreapp_booting.pbn
#
def quartz_xml_modify_builder(target, source, env):
   # init and fix variebles
   source_full = env.RealPath(str(source[0]))
   target_full = env.RealPath(str(target[0]))
   source_path, source_file = os.path.split(source_full)
   target_path, target_file = os.path.split(target_full)
   target_base = os.path.splitext(target_full)[0]
   
   #import pdb; pdb.set_trace()
   hash_seg_size, hash_seg_offs, last_phys_addr = env.PbnGetDemandLoadingData()
   
   xml_file = open(source_full, 'r')
   xml_data = xml_file.read()
   xml_file.close()
   
   re_search = re.search('<program name\s*=\s*"AMSS".* >',xml_data)
   
   if re_search is not None:
      mod_data = "".join([
         '\n<memsection name="hash_segment" direct="true" ',
         'virt_addr="', last_phys_addr, '" '
         'size="', hash_seg_size, '" '
         'attach="rw" zero="false" />\n'
      ])
      
      xml_mod_data = "".join([xml_data[:re_search.end()], mod_data, xml_data[re_search.end()+1:]])
   
      xml_file = open(target_full, 'w')
      xml_file.write(xml_mod_data)
      xml_file.close()
   else:
      env.PrintError("No program AMSS found in [%s]." % source_full)
      raise RuntimeError, 'No program AMSS found in source xml'

   return None
   
#-------------------------------------------------------------------------------
# PbnOneBuilder emitter builder to add clean actions
#
def pbn_data_emitter(target, source, env):
   # Add dependencies on tools
   env.Depends(target, env.RealPath('${ELFWEAVER_FILE}'))

   # clean up extra files
   target_full = env.RealPath(str(target[0]))
   target_base = os.path.splitext(target_full)[0]
   
   target_tmp_pbn_name = target_base + ".pbn"
   env.Clean(target, target_tmp_pbn_name)
   
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# PbnOneBuilder builder for qcoreapp_booting.pbn
#
def pbn_data_builder(target, source, env):
   # init and fix variebles
   source_full = env.RealPath(str(source[0]))
   target_full = env.RealPath(str(target[0]))
   target_path, target_file = os.path.split(target_full)
   target_base = os.path.splitext(target_full)[0]
   
   target_tmp_pbn_name = target_base + ".pbn"
   
   root_cmd = "${ELFWEAVER_CMD}"

   if env['MSM_ID'] in ['6695','6615','8660','8960']:
      # execute elfweaver command
      exec_dir = "${L4_ROOT}"
      cmds = root_cmd + " merge -o "+ target_tmp_pbn_name +" "+ source_full + " "
      
      data, err, rv = env.ExecCmds(cmds, dir=exec_dir, target=target_file, shell=True)
      
      banner = env.CreateFileBanner(os.path.split(target_full)[1])
      
      pbn_data_txt_file = open(target_full, 'w')
      pbn_data_txt_file.write(banner)
      pbn_data_txt_file.write(data)
      pbn_data_txt_file.close()
         

   return None
   
#-------------------------------------------------------------------------------
# PbnBuilder emitter builder to add clean actions
#
def pbn_file_emitter(target, source, env):
   # Add dependencies on tools
   env.Depends(target, env.RealPath('${ELFWEAVER_FILE}'))

   # add clean dependency fro side-effects
   target_str = os.path.realpath(str(target[0]))
   target_base = os.path.splitext(target_str)[0]
   
   target_elfweaver_name = target_base + ".elfweaver.txt"
   env.Clean(target, target_elfweaver_name)
   
   if env.has_key('QDSP6_PROC'):
      target_pbn_tmp = target_base + ".q6.tmp.pbn"
      env.Clean(target, target_pbn_tmp)

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# PbnBuilder builder for qcoreapp_booting.pbn
#
def pbn_file_builder(target, source, env):
   # init and fix variebles
   source_full = env.RealPath(str(source[0]))
   target_full = env.RealPath(str(target[0]))
   source_path, source_file = os.path.split(source_full)
   target_path, target_file = os.path.split(target_full)
   target_base = os.path.splitext(target_full)[0]

   target_elfweaver_name = target_base + '.elfweaver.txt'

   #cd ../../../../platform/l4; tools/pyelf/elfweaver.exe merge -o build_M/bootimg.pbn ../../products/76XX/build/ms/quartz_cfg_TSNCJBPSM.xml --no-section-headers --physical-entry --map
   #cd ../../../../platform/l4; tools/pyelf/elfweaver.exe merge -o build_A/bootimg.pbn ../../products/76XX/build/ms/quartz_cfg_SDCABAAA.xml --no-section-headers --physical-entry --map</argv>
   #cmds = [elfweaver +' merge -o '+ target_full +' '+ source_full +' --no_section_headers --physical_entry']
   
   root_cmd = "${ELFWEAVER_CMD}"

   if env.has_key('QDSP6_PROC'):
      exec_dir = "${BUILD_MS_ROOT}"
      
      cmds = [
         root_cmd + " merge --map -o " + target_base + ".q6.tmp.pbn " + source_full,
         root_cmd + " modify --physical_entry " + target_base + ".q6.tmp.pbn -o " +  target_full,
      ]
   elif env['MSM_ID'] in ['6695']:
      exec_dir = "${L4_ROOT}"
      cmds = root_cmd + " merge -o "+ target_full +" "+ source_full + " --no-section-headers --map"
   else:
      exec_dir = "${L4_ROOT}"
      if env.has_key('USES_L4_3X'):
         if 'USES_RELOCATABLE_MODEM' in env:  
            cmds = [
               root_cmd + " merge -o " + target_base + ".pbn.tmp " +  \
                     source_full + " --emit-relocs --map --traceback --physically_relocatable_image",
               root_cmd + " modify " + target_base + ".pbn.tmp -o " + target_full + \
                     " --physical_entry --physical --no-section-headers --strip",
            ]
         else:
            cmds = [
                  root_cmd + " merge -o " + target_base + ".pbn.tmp " +  \
                        source_full + " --emit-relocs --map --traceback",
                  root_cmd + " modify " + target_base + ".pbn.tmp -o " + target_full + \
                        " --physical_entry --physical --no-section-headers --strip",
            ]
      else:
         if 'USES_RELOCATABLE_MODEM' in env:  
            cmds = root_cmd + " merge -o "+ target_full +" "+ source_full +" --no-section-headers --physical-entry --map --physically_relocatable_image"
         else:
            cmds = root_cmd + " merge -o "+ target_full +" "+ source_full +" --no-section-headers --physical-entry --map"

   data, err, rv = env.ExecCmds(cmds, dir=exec_dir, target=target_file, shell=True)
   
   elfweaver_output_file = open(target_elfweaver_name, 'w')
   elfweaver_output_file.write(data)
   elfweaver_output_file.close()

   return None   
   
#===============================================================================
# L4 SCL header builder
#===============================================================================

#-------------------------------------------------------------------------------
# L4 SCL Header builder defenition
# 
def l4_scl_hdr_generate(env):
   l4_scl_hdr_sc = env.Scanner(function=quartz_xml_scanner, name='Quartz XML scanner', skeys=['.xml'])
   l4_scl_hdr_act = env.GetBuilderAction(l4_scl_hdr_builder)
   l4_scl_hdr_bld = env.Builder(action = l4_scl_hdr_act,
                            source_scanner = l4_scl_hdr_sc,
                            emitter = l4_scl_hdr_emitter,
                            suffix = '.h', src_suffix = '.xml')

   env.Append(BUILDERS = {'L4SclHeaderBuilder' : l4_scl_hdr_bld})

#-------------------------------------------------------------------------------
# L4 SCL Header build emitter
# 
def l4_scl_hdr_emitter(target, source, env):
   # Add dependencies on tools
   env.Depends(target, env.RealPath('${ELFWEAVER_FILE}'))

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------
# L4 SCL Header build action
# 
def l4_scl_hdr_builder(target, source, env):
   # init and fix variebles
   source_str = str(source[0])
   source_str = env.RealPath(source_str)
   target_full = env.RealPath(str(target[0]))
   target_path, target_file = os.path.split(target_full)
   
   if 'USES_RELOCATABLE_MODEM' in env:  
         start_address = '0x90000000\n'
   else:
      exec_dir = "${L4_ROOT}"
      
      cmd = "${ELFWEAVER_CMD}"
         
      cmd += " merge "+ source_str +" --ignore=\"AMSS\" --lastphys=\"physical\""      

      data, err, rv = env.ExecCmds(cmd, dir=exec_dir, target=target_file, shell=True)
         
      if not rv:
         if (data.find('error') is not -1) or (data.find('Err') is not -1):
            raise RuntimeError, 'failed w/ %s' % (data)

         start_address = data
      
   # write l4_scl_hdr_file  
   target_path, target_file = os.path.split(str(target[0]))   
   target_str = str(target[0])
   target_file_uc = target_file.upper()
   target_file_uc = target_file_uc.replace('.','_')
   banner = env.CreateFileBanner(os.path.split(target_str)[1], style='C')
   l4_scl_hdr_file = open(target_str, 'w')
   l4_scl_hdr_file.write (banner)
   l4_scl_hdr_file.write ('#ifndef '+ target_file_uc +'\n')
   l4_scl_hdr_file.write ('#define '+ target_file_uc +'\n')
   l4_scl_hdr_file.write ('#define SCL_L4_START_ADDRESS '+ start_address)
   l4_scl_hdr_file.write ('#endif\n')
   l4_scl_hdr_file.close()
      
   return None

#===============================================================================
# Quartz-XML Machine build rules
#===============================================================================
# XML target = target[0]
# XML source = source[0]
# ---------------------- for non-machine quatz-config-xml
# QUARTZ_MACHINE_XML = source[1]
# AMSS_RELOC_LC = env['AMSS_RELOC_LC'] # since it is not build yet (chiken and egg problem),
#                                      # if included as src, scons will complain that the file dosen't
                                       # exist and it is needed by [target].
#-------------------------------------------------------------------------------
# Quartz-XML builder defenition
#
def quartz_xml_generate(env):
   quartz_xml_sc = env.Scanner(function=quartz_xml_scanner, name='Quartz XML scanner', skeys=['.xml'])
   quartz_xml_act = env.GetBuilderAction(quartz_xml_builder)
   quartz_xml_bld = env.Builder(action = quartz_xml_act,
                                emitter = quartz_xml_emitter,
                                suffix = '.xml')

   env.Append(BUILDERS = {'QuartzXMLBuilder' : quartz_xml_bld})

#-------------------------------------------------------------------------------
# Quartz-XML build Emitter to add clean actions and otehr misc
#
def quartz_xml_emitter(target, source, env):
   envpp = env.Clone()

   # if sources is more then one assume extra source is machine XML
   if len(source) > 1:
      #source_machine_xml_str = os.path.abspath(str(source[1]))
      #source_machine_xml_str = os.path.relpath(str(source[1]), env.subst('${PLAT_ROOT}/l4'))
      source_machine_xml_str = env.RelPath(str(source[1]), '${L4_ROOT}')

      extra_defines = ' -DQUARTZ_MACHINE_XML=\\"'+ source_machine_xml_str +'\\" '
      envpp.Append(CCFLAGS = extra_defines)
      
   if env.has_key('TCXO_SHUTDOWN_RELOC_LC'):
      extra_defines = ' -DTCXO_SHUTDOWN_RELOC_LC=\\"'+ env['TCXO_SHUTDOWN_RELOC_LC'] +'\\" '
      envpp.Append(CCFLAGS = extra_defines)

   if env.has_key('AMSS_RELOC_LC'):
      extra_defines = ' -DAMSS_RELOC_LC=\\"'+ env['AMSS_RELOC_LC'] +'\\" '
      envpp.Append(CCFLAGS = extra_defines)

   # srouce file needs to be preprocess
   target_name = os.path.splitext(str(target[0]))[0]

   xml_pp = envpp.PreProcess(target_name, source[0])
   env.Depends(target, xml_pp)
   
   # add explicit dependencies like CUST_H
   if 'EXPLICIT_DEPS' in env:
      env.Depends(xml_pp, env['EXPLICIT_DEPS'])


   #env.Clean(target, str(target[0]) +'.pp')
   # here one can also add dependencies for say aexe.exe (if it needs to be build)


   return (target, source)

#-------------------------------------------------------------------------------
# Quartz-XML build Action
#
def quartz_xml_builder(target, source, env):
   target_full = env.RealPath(str(target[0]))
   target_path, target_file = os.path.split(target_full)

   # machine-config xmls
   #C:/Apps/RVDS221/RVCT/Programs/2.2/593/win_32-pentium/tcc -E -c --cpu ARM926EJ-S --apcs /noswst/interwork --littleend  -g --dwarf2   -Otime -O1 -DQCOM_OS_ENV -DDAL_FEATURE_REMOTING -O1 -DT_ARM -D__ARMEL__  -DCUST_H=\"CUSTTSNCJBPSM.H\" -D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 -D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ -DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC             -DASSERT=ASSERT_FATAL -I../../build/cust -I../../drivers/hw/inc -I../../drivers/hw/inc/7627 -I../../drivers/hw/inc/7627/plat/l4/user -I../../drivers/hw/inc/7627/proc/modem -I../../dal/inc -I../../core/api/buses -I../../core/buses/api/mpu -I../../core/api/rapi/buses/inc -I../../build/ms -I../../drivers/hw/src/rpc -I ../../services/mproc/oncrpc -I../../services/mproc/rpc_router -I ../../services/wms/src/CMCNas/inc  -I"C:\Apps\RVDS221\RVCT\Data\2.2\503\include\windows" -DBUILD_ASIC=\"76XXT\" -DBUILD_TARGET=\"TSNCJBPSM\" -DBUILD_VER=\"____\"   -o quartz_cfg_machine_TSNCJBPSM_cpp.xml quartz_cfg_machine.xml
   #cleanup_xml.pl -i quartz_cfg_machine_TSNCJBPSM_cpp.xml -o quartz_cfg_machine_TSNCJBPSM.xml
   #C:/Apps/RVDS221/RVCT/Programs/2.2/593/win_32-pentium/tcc -E -c --cpu ARM1136J-S --apcs /noswst/interwork --littleend --force_new_nothrow  -g --dwarf2   -Otime -O2 -O1 -DT_ARM -D__ARMEL__  -DCUST_H=\"CUSTSDCABAAA.H\" -D__MSMHW_MODEM_PROC__=1 -D__MSMHW_APPS_PROC__=2 -D__MSMHW_PROC_DEF__=__MSMHW_APPS_PROC__ -DMSMHW_MODEM_PROC -DMSMHW_APPS_PROC -DIMAGE_APPS_PROC  -DASSERT=ASSERT_FATAL -I../../apps/uione/sdk/BREWSQLExtension -I../../apps/uione/sdk/TrigPlayer -I../../apps/uione/sdk/BREWUIWidgets -I../../apps/uione/sdk/BREWFontExtensions -I../../apps/uione/hdk/bin/qvga/rvct22 -I../../apps/uione/hpk/src/Extensions/SystemNotification/inc -I../../apps/uione/hpk/src/StaticExtensions/CustomOEMConfigItems/inc -I../../apps/uione/hpk/src/StaticExtensions/DeviceInfoEx/inc -I../../apps/uione/hpk/src/StaticExtensions/SignalTone/inc -I../../apps/uione/hpk/src/StaticExtensions/SignalTone/impl/inc -I../../apps/uione/hdk/src/Trigs/PhoneApp -I../../apps/uione/hpk/src/Lib/ModuleBase/inc -I../../apps/uione/hpk/src/IConfig/CustomConfigItems/inc -I../../apps/uione/hpk/src/IConfig/ConfigExtenders/inc -I../../apps/uione/sdk/Fonts/BREWFontExtension_1.2.0/xmod -I../../apps/uione/sdk/BREWUIWidgets/xmod -I../../apps/uione/sdk/BREWUIWidgets/widgets/inc/bid -I../../apps/test/inc -I../../apps/nonpkbrew/pk/inc/msm -I../../apps/nonpkbrew/pk/inc -I../../apps/nonpkbrew/pk/src -I../../apps/nonpkbrew/pk/src/msm -I../../apps/brew/sdk/inc -I../../apps/brew/inc -I../../apps/brew/pk/inc/msm -I../../apps/brew/pk/inc -I../../apps/brew/pk/src -I../../apps/brew/src/OEM -I../../apps/brew/pk/src/msm -I../../apps/brew/inc/OEM -I../../apps/brew/src/OEM/msm -I../../apps/brew/src/OEM/OEMNotify/inc -I../../apps/brew/src/OEM/OEMConfigItem/inc -I../../apps/brew/src/OEM/OEMSound -I../../../../platform/cs/src/OEM/OEMFS/msm -I../../../../platform/cs/src/OEM/OEMMemGrp/msm -I../../../../platform/cs/src/OEM/OEMSecureSysClock/msm -I../../apps/brew/src/OEM/OEMSysClock/msm -I../../apps/brew/src/OEM/OEMShellBeep -I../../apps/brew/src/OEM/OEMServingNetworks/msm -I../../apps/brew/src/OEM/OEMAnnunciatorControl -I../../apps/uione/sdk/BREWUIWidgets/forms/inc -I../../apps/uione/sdk/BREWUIWidgets/widgets/inc -I../../multimedia/camera/oem/ibitmapfx/inc -I../../multimedia/camera/oem/icamera/inc -I../../multimedia/camera/oem/imagepro/inc -I../../multimedia/camera/staticextensions/iipl/inc -I../../multimedia/camera/staticextensions/iycbcr/inc -I../../apps/brewappmgr_2.0 -I../../apps/staticextensions/inc -I../../apps/staticextensions/oem/inc -I../../apps/staticextensions/oem/src -I-I../../multimedia/camcorder/staticextensions/movieedit/inc -I-I../../multimedia/camcorder/staticextensions/moviefile/inc -I../../apps/DynamicExtensions/IMediaVideo -I../../drivers/rf/inc -I../../drivers/rf/umts/inc -I../../drivers/rf/common -I../../drivers/rf/umts -I../../drivers/rf/1x -I../../drivers/rf/hdr -I../../drivers/rf/gps -I../../drivers/rf/libs/msm -I../../drivers/rf/libs/zif -I../../drivers/rf/libs/zif/RTR6500 -I../../data/multi_proc/remote_apis -I../../data/multi_proc/shim -I../../drivers/hw/inc/7600 -I../../drivers/hw/inc/7600/plat/l4/user -I../../drivers/hw/inc/7600/proc/apps -I../../modem/uim/uimdrv/inc -I../../modem/uim/uimdrv/src -I../../modem/uim/uimdrv/src/rpc -I../../modem/uim/nvruim/inc -I../../modem/uim/nvruim/src -I../../modem/uim/nvruim/src/rpc -I../../modem/uim/mmgsdi/inc -I../../modem/uim/mmgsdi/src -I../../modem/uim/gstk/inc -I../../modem/uim/gstk/src -I../../modem/uim/cat/inc -I../../modem/uim/cat/src -I../../modem/uim/icard/inc -I../../modem/uim/icard/src -I../../modem/uim/icardsession/inc -I../../modem/uim/icardsession/src -I../../modem/uim/icat/inc -I../../modem/uim/icat/src -I../../modem/uim/istk/inc -I../../modem/uim/istk/src -I../../modem/uim/dctstk/src -I../../drivers/touch --via ../../build/ms/M76XXCSDCABAAA/M76XXCSDCABAAA.inc -I../../drivers/clkregim/rpc -I../../drivers/hw/src/rpc -I ../../services/mproc/oncrpc -I../../services/mproc/rpc_router -I../../drivers/tlmm -I../../drivers/tlmm/src/drv/rpc -I../../drivers/tlmm/src/bsp -I ../../services/wms/src/CMCNas/inc  -I"C:\Apps\RVDS221\RVCT\Data\2.2\503\include\windows" -DBUILD_ASIC=\"76XXC\" -DBUILD_TARGET=\"SDCABAAA\" -DBUILD_VER=\"3556\"   -o quartz_cfg_machine_SDCABAAA_cpp.xml quartz_cfg_machine.xml
   #./cleanup_xml.pl -i quartz_cfg_machine_SDCABAAA_cpp.xml -o quartz_cfg_machine_SDCABAAA.xml

   # config-xmls
   #C:/Apps/RVDS221/RVCT/Programs/2.2/593/win_32-pentium/tcc -E -c --cpu ARM926EJ-S --apcs /noswst/interwork --littleend  -g --dwarf2   -Otime -O1 -DQCOM_OS_ENV -DDAL_FEATURE_REMOTING -DAMSS_RELOC_LC=\"build_M/amss/bin/tsncjbpsm.reloc\" -DQUARTZ_MACHINE_XML=\"../../products/76XX/build/ms/quartz_cfg_machine_TSNCJBPSM.xml\" -O1 -DT_ARM -D__ARMEL__  -DCUST_H=\"CUSTTSNCJBPSM.H\" -D__MSMHW_APPS_PROC__=2 -D__MSMHW_MODEM_PROC__=1 -D__MSMHW_PROC_DEF__=__MSMHW_MODEM_PROC__ -DMSMHW_MODEM_PROC -DIMAGE_MODEM_PROC             -DASSERT=ASSERT_FATAL -I../../build/cust -I../../drivers/hw/inc -I../../drivers/hw/inc/7627 -I../../drivers/hw/inc/7627/plat/l4/user -I../../drivers/hw/inc/7627/proc/modem -I../../dal/inc -I../../core/api/buses -I../../core/buses/api/mpu -I../../core/api/rapi/buses/inc -I../../build/ms -I../../drivers/hw/src/rpc -I ../../services/mproc/oncrpc -I../../services/mproc/rpc_router -I ../../services/wms/src/CMCNas/inc  -I"C:\Apps\RVDS221\RVCT\Data\2.2\503\include\windows" -DBUILD_ASIC=\"76XXT\" -DBUILD_TARGET=\"TSNCJBPSM\" -DBUILD_VER=\"____\"   -o quartz_cfg_TSNCJBPSM_cpp.xml quartz_cfg.xml
   #cleanup_xml.pl -i quartz_cfg_TSNCJBPSM_cpp.xml -o quartz_cfg_TSNCJBPSM.xml
   #C:/Apps/RVDS221/RVCT/Programs/2.2/593/win_32-pentium/tcc -E -c --cpu ARM1136J-S --apcs /noswst/interwork --littleend --force_new_nothrow  -g --dwarf2   -Otime -O2 -DAMSS_RELOC_LC=\"build_A/amss/bin/sdcabaaa.reloc\" -DQUARTZ_MACHINE_XML=\"../../products/76XX/build/ms/quartz_cfg_machine_SDCABAAA.xml\" -O1 -DT_ARM -D__ARMEL__  -DCUST_H=\"CUSTSDCABAAA.H\" -D__MSMHW_MODEM_PROC__=1 -D__MSMHW_APPS_PROC__=2 -D__MSMHW_PROC_DEF__=__MSMHW_APPS_PROC__ -DMSMHW_MODEM_PROC -DMSMHW_APPS_PROC -DIMAGE_APPS_PROC  -DASSERT=ASSERT_FATAL -I../../apps/uione/sdk/BREWSQLExtension -I../../apps/uione/sdk/TrigPlayer -I../../apps/uione/sdk/BREWUIWidgets -I../../apps/uione/sdk/BREWFontExtensions -I../../apps/uione/hdk/bin/qvga/rvct22 -I../../apps/uione/hpk/src/Extensions/SystemNotification/inc -I../../apps/uione/hpk/src/StaticExtensions/CustomOEMConfigItems/inc -I../../apps/uione/hpk/src/StaticExtensions/DeviceInfoEx/inc -I../../apps/uione/hpk/src/StaticExtensions/SignalTone/inc -I../../apps/uione/hpk/src/StaticExtensions/SignalTone/impl/inc -I../../apps/uione/hdk/src/Trigs/PhoneApp -I../../apps/uione/hpk/src/Lib/ModuleBase/inc -I../../apps/uione/hpk/src/IConfig/CustomConfigItems/inc -I../../apps/uione/hpk/src/IConfig/ConfigExtenders/inc -I../../apps/uione/sdk/Fonts/BREWFontExtension_1.2.0/xmod -I../../apps/uione/sdk/BREWUIWidgets/xmod -I../../apps/uione/sdk/BREWUIWidgets/widgets/inc/bid -I../../apps/test/inc -I../../apps/nonpkbrew/pk/inc/msm -I../../apps/nonpkbrew/pk/inc -I../../apps/nonpkbrew/pk/src -I../../apps/nonpkbrew/pk/src/msm -I../../apps/brew/sdk/inc -I../../apps/brew/inc -I../../apps/brew/pk/inc/msm -I../../apps/brew/pk/inc -I../../apps/brew/pk/src -I../../apps/brew/src/OEM -I../../apps/brew/pk/src/msm -I../../apps/brew/inc/OEM -I../../apps/brew/src/OEM/msm -I../../apps/brew/src/OEM/OEMNotify/inc -I../../apps/brew/src/OEM/OEMConfigItem/inc -I../../apps/brew/src/OEM/OEMSound -I../../../../platform/cs/src/OEM/OEMFS/msm -I../../../../platform/cs/src/OEM/OEMMemGrp/msm -I../../../../platform/cs/src/OEM/OEMSecureSysClock/msm -I../../apps/brew/src/OEM/OEMSysClock/msm -I../../apps/brew/src/OEM/OEMShellBeep -I../../apps/brew/src/OEM/OEMServingNetworks/msm -I../../apps/brew/src/OEM/OEMAnnunciatorControl -I../../apps/uione/sdk/BREWUIWidgets/forms/inc -I../../apps/uione/sdk/BREWUIWidgets/widgets/inc -I../../multimedia/camera/oem/ibitmapfx/inc -I../../multimedia/camera/oem/icamera/inc -I../../multimedia/camera/oem/imagepro/inc -I../../multimedia/camera/staticextensions/iipl/inc -I../../multimedia/camera/staticextensions/iycbcr/inc -I../../apps/brewappmgr_2.0 -I../../apps/staticextensions/inc -I../../apps/staticextensions/oem/inc -I../../apps/staticextensions/oem/src -I-I../../multimedia/camcorder/staticextensions/movieedit/inc -I-I../../multimedia/camcorder/staticextensions/moviefile/inc -I../../apps/DynamicExtensions/IMediaVideo -I../../drivers/rf/inc -I../../drivers/rf/umts/inc -I../../drivers/rf/common -I../../drivers/rf/umts -I../../drivers/rf/1x -I../../drivers/rf/hdr -I../../drivers/rf/gps -I../../drivers/rf/libs/msm -I../../drivers/rf/libs/zif -I../../drivers/rf/libs/zif/RTR6500 -I../../data/multi_proc/remote_apis -I../../data/multi_proc/shim -I../../drivers/hw/inc/7600 -I../../drivers/hw/inc/7600/plat/l4/user -I../../drivers/hw/inc/7600/proc/apps -I../../modem/uim/uimdrv/inc -I../../modem/uim/uimdrv/src -I../../modem/uim/uimdrv/src/rpc -I../../modem/uim/nvruim/inc -I../../modem/uim/nvruim/src -I../../modem/uim/nvruim/src/rpc -I../../modem/uim/mmgsdi/inc -I../../modem/uim/mmgsdi/src -I../../modem/uim/gstk/inc -I../../modem/uim/gstk/src -I../../modem/uim/cat/inc -I../../modem/uim/cat/src -I../../modem/uim/icard/inc -I../../modem/uim/icard/src -I../../modem/uim/icardsession/inc -I../../modem/uim/icardsession/src -I../../modem/uim/icat/inc -I../../modem/uim/icat/src -I../../modem/uim/istk/inc -I../../modem/uim/istk/src -I../../modem/uim/dctstk/src -I../../drivers/touch --via ../../build/ms/M76XXCSDCABAAA/M76XXCSDCABAAA.inc -I../../drivers/clkregim/rpc -I../../drivers/hw/src/rpc -I ../../services/mproc/oncrpc -I../../services/mproc/rpc_router -I../../drivers/tlmm -I../../drivers/tlmm/src/drv/rpc -I../../drivers/tlmm/src/bsp -I ../../services/wms/src/CMCNas/inc  -I"C:\Apps\RVDS221\RVCT\Data\2.2\503\include\windows" -DBUILD_ASIC=\"76XXC\" -DBUILD_TARGET=\"SDCABAAA\" -DBUILD_VER=\"3556\"   -o quartz_cfg_SDCABAAA_cpp.xml quartz_cfg.xml
   #cleanup_xml.pl -i quartz_cfg_SDCABAAA_cpp.xml -o quartz_cfg_SDCABAAA.xml

   pp_target_name = os.path.splitext(str(target[0]))[0] + '.pp'

   cmds = [
      'perl ${BUILD_MS_ROOT}/cleanup_xml.pl -i '+ pp_target_name +' -o '+ str(target[0])
   ]

   data, err, rv = env.ExecCmds(cmds, target=target_file, shell=True)
   return None


#===============================================================================
# Quartz Constnat CMM build rules
#===============================================================================
# target = target[0]
# CMM source = source[0]
# PBN source = source[1]
# Quartz cfg xml source = source[2]
#

#-------------------------------------------------------------------------------
# Quartz Constnat CMM builder defenition
#
def quartz_constant_generate(env):
   quartz_constant_act = env.GetBuilderAction(quartz_constant_builder)
   quartz_constant_bld = env.Builder(action = quartz_constant_act, 
      emitter = quartz_constant_emitter,
      suffix = '.cmm')
   
   env.Append(BUILDERS = {'QuartzConstBuilder' : quartz_constant_bld})

#-------------------------------------------------------------------------------
# Quartz Constnat CMM build Emitter to add clean actions and otehr misc
#
def quartz_constant_emitter(target, source, env):
   # Add dependencies on tools
   env.Depends(target, env.RealPath('${ELFWEAVER_FILE}'))
   env.Depends(target, env.RealPath('${PPASM_FILE}'))

   # source cmm file needs to be preprocess
   target_base, target_ext = os.path.splitext(str(target[0]))

   cmm_pp = env.PreProcess(target_base, source[0])
   
   # add explicit dependencies like CUST_H
   if 'EXPLICIT_DEPS' in env:
      env.Depends(cmm_pp, env['EXPLICIT_DEPS'])
   
   # add the preprocess file to the source for the builder to use
   # this should be and expected to be source[2] 
   source.append(cmm_pp)
   
   # clean side effects
   env.Clean(target, target_base + ".i")

   return (target, source)

#-------------------------------------------------------------------------------
# Quartz Constnat CMM build Action
#
def quartz_constant_builder(target, source, env):
   #import pdb; pdb.set_trace()      
   source_pbn = env.RealPath(str(source[1]))
   source_xml = env.RealPath(str(source[2]))
   source_cmm_pp = env.RealPath(str(source[3]))
   
   source_pbn_base, source_pbn_file = os.path.split(source_pbn)
   
   target_full = env.RealPath(str(target[0]))
   target_path, target_file = os.path.split(target_full)
   target_base, target_ext = os.path.splitext(target_file)
   target_full_base = os.path.splitext(target_full)[0]
   
   #----------------------------------------------------------------------------
   # clean preprocess file
   target_cmm_i = target_full_base + ".i"
   cmd = "${PPASM} -f " + source_cmm_pp + " -o " + target_cmm_i
   data, err, rv = env.ExecCmds(cmd, target=target_cmm_i)
   
   #----------------------------------------------------------------------------
   # get entry point information
   # virt address expected to always be 0xf0000000 for L4 entry
   if env.has_key('USES_L4_3X'):
      if env['MSM_ID'] in ['6695','6615']:
         virt_l4_entry_point = 0x100000
      else:
         if 'MODEM_PROC' in env:  
            virt_l4_entry_point = 0x42F00000
         else:
            virt_l4_entry_point = 0x40200000
   else:
      virt_l4_entry_point = 0xf0000000
   phys_l4_entry_point = env.GetProgramHeaderInfo(source_pbn, "VirtAddr", virt_l4_entry_point, "PhysAddr")
   
   #----------------------------------------------------------------------------
   # get mmu_data entry point
   # first we need to get the kernel file. which can be found in the quartz cfg xml
   # <kernel file="build_M/pistachio/bin/kernel" xip="false" >
   
   xml_file = open (source_xml, "r")
   xml_data = xml_file.read()
   xml_file.close()

   kernel_re = re.compile(r'^\s+<\s*kernel\s+file\s*=\"(\S+)\"\s*', re.M)
   kernel = kernel_re.findall(xml_data)
   
   if len(kernel) == 0:
      env.PrintError("Cannot find kernel file in ["+ source_xml +"].")
      raise RuntimeError, 'Cannot find kernel file in XML data.'

   kernel_file = "${L4_ROOT}/" + kernel[0]
   
   mmu_data_addr = env.GetSymbolAddress(kernel_file, "mmu_data")

   # calculate mmu_data offset
   mmu_data_offset = int(mmu_data_addr - virt_l4_entry_point)
      
   #----------------------------------------------------------------------------
   # use cleaned up preprocess cmm, and add banner to it.
   input_cmm = open (target_cmm_i, "r")
   data = env.CreateFileBanner(target_file, style='asm') + input_cmm.read()
   data = data.replace('L4_ENTRY_POINT', hex(phys_l4_entry_point))
   data = data.replace('MMU_DATA_OFFSET', hex(mmu_data_offset))
   data = data.replace('BOOTING_PBN_PATH', env.RelPath(source_pbn, "${BUILD_MS_ROOT}", posix=True))
   
   #import pdb; pdb.set_trace()
   
   target_file = open (target_full, "w")
   target_file.write(data)
   target_file.close()
   return None
   
   
#===============================================================================
# Debug Constnat CMM build rules
#===============================================================================
# target = target[0]
# CMM source = source[0]
# PBN source = source[1]
#

#-------------------------------------------------------------------------------
# Debug Constnat CMM builder defenition
#
def debug_constant_generate(env):
   debug_constant_act = env.GetBuilderAction(debug_constant_builder)
   debug_constant_bld = env.Builder(action = debug_constant_act, 
      emitter = debug_constant_emitter,
      suffix = '.cmm')
   
   env.Append(BUILDERS = {'DbgConstBuilder' : debug_constant_bld})

#-------------------------------------------------------------------------------
# Debug Constnat CMM build Emitter to add clean actions and otehr misc
#
def debug_constant_emitter(target, source, env):
   # Add dependencies on tools
   env.Depends(target, env.RealPath('${ELFWEAVER_FILE}'))
   env.Depends(target, env.RealPath('${PPASM_FILE}'))

   # source cmm file needs to be preprocess
   target_base, target_ext = os.path.splitext(str(target[0]))

   cmm_pp = env.PreProcess(target_base, source[0])
   
   # add explicit dependencies like CUST_H
   if 'EXPLICIT_DEPS' in env:
      env.Depends(cmm_pp, env['EXPLICIT_DEPS'])
   
   # add the preprocess file to the source for the builder to use
   # this should be and expected to be source[2] 
   source.append(cmm_pp)
   
   # clean side effects
   env.Clean(target, target_base + ".i")

   return (target, source)

#-------------------------------------------------------------------------------
# Debug Constnat CMM build Action
#
def debug_constant_builder(target, source, env):
   #import pdb; pdb.set_trace()
   source_pbn = env.RealPath(str(source[1]))
   source_cmm_pp = env.RealPath(str(source[2]))
   target_full = env.RealPath(str(target[0]))
   target_path, target_file = os.path.split(target_full)
   target_base, target_ext = os.path.splitext(target_file)
   target_full_base = os.path.splitext(target_full)[0]
   
   #----------------------------------------------------------------------------
   # clean preprocess file
   target_cmm_i = target_full_base + ".i"
   cmd = "${PPASM} -f " + source_cmm_pp + " -o " + target_cmm_i
   data, err, rv = env.ExecCmds(cmd, target=target_cmm_i)
      
   #----------------------------------------------------------------------------
   # use cleaned up preprocess cmm, and add banner to it.
   input_cmm = open (target_cmm_i, "r")
   data = env.CreateFileBanner(target_file, style='asm') + input_cmm.read()
   
   target_file = open (target_full, "w")
   target_file.write(data)
   target_file.close()
   return None

   
