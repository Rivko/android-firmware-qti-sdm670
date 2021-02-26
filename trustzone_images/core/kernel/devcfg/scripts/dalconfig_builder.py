#===============================================================================
# DAL config autogen buildder
#
# GENERAL DESCRIPTION
#    DAL config builder
#
# Copyright (c) 2011,2014-16 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================
import os
import shutil
import subprocess
import string
import SCons.Scanner
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('dalconfig_builder')

def generate(env):
   scripts_path = os.path.dirname(os.path.realpath(__file__))
   env.Replace(CLEANUP_XML_CMD = scripts_path+"/cleanup_xml.py")
   env.Replace(PROPGEN_CMD = scripts_path+"/propgen.py")
   
   dal_xml_auto_generate(env)
   dal_moddir_src_auto_generate(env)
   dal_config_src_auto_generate(env)

#===============================================================================
# DAL Config XML build rules
#===============================================================================
def dal_xml_auto_generate(env):
   DALConfig_xml_bld = env.Builder(action = DALConfig_xml_builder, 
                               source_scanner = SCons.Scanner.C.CScanner(),
                               emitter = DALConfig_xml_emitter,
                               suffix = '.xml', src_suffix = '.xml')

   env.Append(BUILDERS = {'DALConfigXmlBuilder' : DALConfig_xml_bld})

#-------------------------------------------------------------------------------
# DevCfg XML build Emitter to add clean actions and other misc
#
def DALConfig_xml_emitter(target, source, env):
   image_name = ''
   source_full = str(source[0])
   target_full = str(target[0])
   source_path, source_file = os.path.split(str(source[0]))

   # since the include generation uses fully qualify paths, re generate if 
   # root path has changed; as when moving from local build environment to 
   # ecloud or sanbatch. By adding root_path as a -d in the command line, 
   # SCons will see the change and-re build
   env.Append(CPPDEFINES= ["BUILD_ROOT=${BUILD_ROOT}"])

   if env.has_key('NAND_TOOLS_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_NAND_TOOLS"])
       image_name = '_nand_tools'
   elif env.has_key('HOSTDL_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_HOSTDL"])
       image_name = '_hostdl'
   elif env.has_key('EHOSTDL_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_EHOSTDL"])
       image_name = '_ehostdl'
   elif env.has_key('NOR_TOOLS_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_NOR_TOOLS"])
       image_name = '_nor_tools'
   # Add dependencies on tools
   env.Depends(target, env.SubstRealPath('${CLEANUP_XML_CMD}'))
   
   # Source file needs to be preprocess for boot
   if env.has_key('BUILD_BOOT_CHAIN') or env.has_key('BUILD_TOOL_CHAIN'):
      target_path, target_file = os.path.split(str(target[0]))

      preprocess_xml = os.path.join(target_path, "bootproc_cmd"+image_name+".xml")
      
      pre_xml_env = env.Clone()
      pre_xml_env.Append(CPPDEFINES = ["DETERMINE_BOOT_IMAGE"])
      
      pre_xml = pre_xml_env.PreProcess(preprocess_xml, source[0])
      env.Depends(target, pre_xml)

      # add the preprocess file to the source for the builder to use
      source.append(pre_xml)
      
      # add explicit dependencies added by build system these are dependencies 
      # that SCons cannot detect because they are only known at compile time 
      # such as CUST_H (and all of its include files)
      if 'EXPLICIT_DEPS' in env:
         env.Depends(target, env['EXPLICIT_DEPS'])

   # since this builder depends in changes of files being added or removed from
   # the image folder, add the files to source list this way SCons can deal with it 
   # properly.
   xml_files_path_pattern = source_path + "/*.xml"
   xml_files = env.GlobFiles(xml_files_path_pattern)
   
   source.append(xml_files)
      
   # here one can also add dependencies for say propgen.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)
  
#-------------------------------------------------------------------------------
# DAL XML builder
#
def DALConfig_xml_builder(target, source, env):
   source_full = str(source[0])
   target_full = str(target[0])
   source_path, source_file = os.path.split(str(source[0]))

   image_name = ''

   if env.has_key('NAND_TOOLS_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_NAND_TOOLS"])
       image_name = '_nand_tools'
   elif env.has_key('HOSTDL_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_HOSTDL"])
       image_name = '_hostdl'
   elif env.has_key('EHOSTDL_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_EHOSTDL"])
       image_name = '_ehostdl'
   elif env.has_key('NOR_TOOLS_IMAGE'):
       env.Append(CPPDEFINES= ["DAL_IMAGE_FLASH_NOR_TOOLS"])
       image_name = '_nor_tools'

   # set root cmds depending on paltform
   if env['PLATFORM'] == 'linux':
      cleanup_root_cmd = "${PYTHONCMD} ${CLEANUP_XML_CMD}"
   else:
      #cleanup_root_cmd = "${CLEANUP_XML_CMD}"
      cleanup_root_cmd = "${PYTHONCMD} ${CLEANUP_XML_CMD}"
   
   # build command
   if env.has_key('BUILD_BOOT_CHAIN') or env.has_key('BUILD_TOOL_CHAIN'):
      bootproc_cmd = str(source[1]) # added by emitter
      bootproc_cmd.replace(".xml",image_name+".xml")
      
      #${PYTHONCMD} $(DALROOT)/tools/cleanup_xml.py -pre_cmd_file $(DALROOT)/config $(DALROOT)/config/bootproc_cmd.xml
      cmds = cleanup_root_cmd +' -pre_cmd_file '+ source_path +' '+ bootproc_cmd
   else:
      dal_image_name = os.path.basename(source_path)
   
      #${PYTHONCMD} $(DALROOT)/tools/cleanup_xml.py -pre $(DALROOT)/config/apps $(DALROOT)/config/dal_mod_apps.xml dalsystem_apps.xml apps
      cmds = cleanup_root_cmd +' -pre '+ source_path +' '+ target_full +' '+ source_file +' '+ dal_image_name
      
   # run command(s)
   #data, err, rv = env.RunCmds(target, source, cmds)

   subprocess.call(cmds.split())
   
   if env.has_key('BUILD_BOOT_CHAIN') or env.has_key('BUILD_TOOL_CHAIN'):
      shutil.copyfile(source_full, target_full)
   
   return None
   
#===============================================================================
# Common emitter function for auto generated DALModDir & DALConfig c files
#===============================================================================
def dal_src_emitter(target, source, env):
   
   # Add dependencies on tools
   env.Depends(target, env.SubstRealPath('${CLEANUP_XML_CMD}'))
   env.Depends(target, env.SubstRealPath('${PROPGEN_CMD}'))

   # Source file needs to be preprocess
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))

   preprocess_xml = os.path.join(target_path, source_file)
   preprocess_xml = os.path.splitext(preprocess_xml)[0]
   pre_xml = env.PreProcess(preprocess_xml, source[0])
   env.Depends(target, pre_xml)
   
   # add clean for builder generated files
   env.Clean(target, preprocess_xml + '_out.xml')
   
   # add the preprocess file to the source for the builder to use
   source.append(pre_xml)

   # add explicit dependencies added by build system these are dependencies 
   # that SCons cannot detect because they are only known at compile time 
   # such as CUST_H (and all of its include files)
   if 'EXPLICIT_DEPS' in env:
      env.Depends(target, env['EXPLICIT_DEPS'])
   
   # here one can also add dependencies for say propgen.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#===============================================================================
# Common action function for auto generated DALModDir & DALConfig c files
#===============================================================================
def dal_cmd_setup(env):
   
   # set root cmds depending on paltform
   if env['PLATFORM'] == 'linux':
      cleanup_root_cmd = "${PYTHONCMD} ${CLEANUP_XML_CMD}"
      propgen_root_cmd = "${PYTHONCMD} ${PROPGEN_CMD}"
   else:
      #cleanup_root_cmd = "${CLEANUP_XML_CMD}"
      cleanup_root_cmd = "python ${CLEANUP_XML_CMD}"
      propgen_root_cmd = "python ${PROPGEN_CMD}"
      
   return (cleanup_root_cmd, propgen_root_cmd)

   
#===============================================================================
# DAL ModDir Source build rules
#===============================================================================
def dal_moddir_src_auto_generate(env):
   DALModDir_src_bld = env.Builder(action = DALModDir_src_builder, 
                               source_scanner = SCons.Scanner.C.CScanner(),
                               emitter = DALModDir_src_emitter,
                               suffix = '.c', src_suffix = '.xml')

   env.Append(BUILDERS = {'DALModDirSrcBuilder' : DALModDir_src_bld})
   
#-------------------------------------------------------------------------------
# DALModDir Emitter to add clean actions and other misc
#-------------------------------------------------------------------------------
def DALModDir_src_emitter(target, source, env):
   target, source = dal_src_emitter(target, source, env)
   return (target, source)

#-------------------------------------------------------------------------------
# DALModDir Source builder
#-------------------------------------------------------------------------------
def DALModDir_src_builder(target, source, env):
   
   target_path, target_file = os.path.split(str(target[0]))
   target_full = str(target[0])
   
   #cpath, cfile = os.path.split(str(source[1]))
   preprocess_xml = str(source[1]) # added by emitter
   clean_xml = os.path.splitext(preprocess_xml)[0] + '_out.xml'
   
   #Get env parameters
   memory_optimise  = env.get('DEVCFG').getFeature('propsMemoryOptimise')
   struct_size      = env.get('DEVCFG').getFeature('structSize')
   config           = env.get('CONFIG')
   if not config:
       config = ""
   
   cleanup_root_cmd, propgen_root_cmd = dal_cmd_setup(env)
   
   cmd1 = cleanup_root_cmd +' -p '+ preprocess_xml +' -o  '+ clean_xml +' -c  '+ target_full
   cmd2 = propgen_root_cmd + ' --XmlFile=' + clean_xml + ' --DirName=' + target_path + ' --ModDirFile=' + str(target[0]) + ' --RemoveDups=True' + \
          ' --ConfigType=' + config + ' --MemoryOptimiseProps=' + memory_optimise + ' --structSize=' + struct_size
  
   subprocess.call(env.subst(cmd1).split())
   subprocess.call(env.subst(cmd2).split())

   return None
   
   
#===============================================================================
# DAL Config Source build rules
#===============================================================================
def dal_config_src_auto_generate(env):
   DALConfig_src_bld = env.Builder(action = DALConfig_src_builder, 
                               source_scanner = SCons.Scanner.C.CScanner(),
                               emitter = DALConfig_src_emitter,
                               src_suffix = '.xml')

   env.Append(BUILDERS = {'DALConfigSrcBuilder' : DALConfig_src_bld})

#-------------------------------------------------------------------------------
# DAL build Emitter to add clean actions and otheh misc
#-------------------------------------------------------------------------------
def DALConfig_src_emitter(target, source, env):
  target, source = dal_src_emitter(target, source, env)
  return (target, source)

#-------------------------------------------------------------------------------
# DAL Source builder
#-------------------------------------------------------------------------------
def DALConfig_src_builder(target, source, env):
   
    target_path, target_cfg_file = os.path.split(str(target[0]))
    target_config_full = str(target[0])
    target_devcfg_path, target_devcfgdata = os.path.split(str(target[1]))
    target_devcfgdata_full = str(target[1])

    preprocess_xml = str(source[1]) # added by emitter
    clean_xml = os.path.splitext(preprocess_xml)[0] + '_out.xml'

    #Get env parameters
    memory_optimise  = env.get('DEVCFG').getFeature('propsMemoryOptimise')
    struct_size      = env.get('DEVCFG').getFeature('structSize')
    config           = env.get('CONFIG')
    
    if not config:
       config = ""

    cleanup_root_cmd, propgen_root_cmd = dal_cmd_setup(env)
    #$(CC) -E $(CFLAGS) $(DMSS_CFLAGS) $(ALL_INCLUDES) $(DIRNAME) -I"$(ARMINC)" $(BUILD_VERSION) $(APPFLAGS) $(MSG_BT_SSID) $(OBJ_CMD) $(DALROOT)/config/dal_mod_modem_pre.xml $(DALROOT)/config/dal_mod_modem.xml
    #${PYTHONCMD} $(DALROOT)/tools/cleanup_xml.py -p $(DALROOT)/config/dal_mod_modem_pre.xml -o $(DALROOT)/config/dal_mod_modem_out.xml -c $(DALROOT)/config/DALConfig_modem.c
    #$(DALPROPGEN) $(DALROOT)/config/dal_mod_modem_out.xml $(DALROOT)/config/ <DALModDir.c> <DALConfig.c> <devcfg_data.c>
    cmd1 = cleanup_root_cmd +' -p '+ preprocess_xml +' -o '+ clean_xml +'  -c '+ str(target[0])
    print env.subst(cmd1)
    cmd2 =  propgen_root_cmd + ' --XmlFile=' + clean_xml + ' --DirName=' + target_path + ' --ConfigFile=' + target_config_full + \
           ' --DevcfgDataFile=' + target_devcfgdata_full + ' --ConfigType=' + config + ' --MemoryOptimiseProps=' + \
           memory_optimise + ' --structSize=' + struct_size 
    print env.subst(cmd2)

    # run command(s)   
    #data, err, rv = env.RunCmds(target, source, cmds)

    subprocess.call(env.subst(cmd1).split())
    subprocess.call(env.subst(cmd2).split())
    
    return None
