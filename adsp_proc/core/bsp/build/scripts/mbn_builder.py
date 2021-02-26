#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create MBN file
#
# Copyright  2009-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/mbn_builder.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#  $Change: 13907704 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 06/29/16   sm        Updating the version of sectools used.
# 05/29/14   sm        Adding in sectools integration, as requested by Secboot Team.
# 05/26/11   dxiang    Make relocatable modem feature controlled from top level Scons 
# 05/10/11   dh        Support encryption for MBN image type, use USES_ENCRYPT_MBN
#                      flag to control both modem elf and mbn encryption work
# 02/04/11   dxiang    Modified image generation of Q6 images so that the format is similar to AMSS
# 01/12/10   dxiang    Support encryption of ELF image types  
# 01/11/10   dxiang    Fixed image generation for legacy Q6 images using MBN Tools 
# 12/21/10   plc       Change QDST path to core/storage/tools
# 12/17/10   dxiang    Fix for MBN type assignment for builds without MBN Tools 
# 12/16/10   rk        Fixing Secboot building for boot loaders
# 12/16/10   rk        Adding back the 6615 changes missed out in latest revision
# 12/14/10   plc       Make "do_images_exist" check for certificates support 
#                      1-3 certs for mbn_tools.py use, or all 3 certs for 
#                      older .exe versions of mbn tools.  
# 11/29/10   plc       Added support for Secure-Boot with Qualcomm Development 
#                      certificates
# 11/29/10   plc       Added support for 1-3 certs for Secure Boot
# 11/15/10   aus       DBL header for Sahara does not need preamble and it has 
#                      a 40-byte header.
# 11/02/10   dxiang    1. Consolidate code
#                      2. Put in support for Python tools for non-SB3 images 
#                      3. FSB Support for SB3 images
# 10/19/10   dxiang    Support relocatable modem feature and EMMC image generation  
# 10/11/10   dxiang    Clean up and support Python version of modify_elf_flags 
# 10/11/10   dxiang    Verify image signing for SB3 images 
# 10/04/10   niting    Adding FSB support
# 09/24/10   dxiang    Support preamble generation for SBL1 image in SB3 
# 09/09/10   dxiang    1. Add the MBN tools into the internal generate() function.
#                      2. Always generate a global_dict to be used by mbn_builder
#                      3. To enable MBN Tools, add 'mbntools makeonly' into .builds 
# 08/26/10   dxiang    Add support for MBN tools usage 
# 08/13/10   shirleyy  adding 8660 into build system
# 07/09/10   shirleyy  adding apps.mbn into install_mbn_final() for Apps build
# 05/26/10   niting    fixing secureboot for Sahara and non-Sahara builds
# 05/7/10    ddk       Moving the script to the boot source path.
# 05/05/10   vkg       1. adding rules for EMMCHOSTDL image
#                      2. use of kwargs to avoid  any interface change
# 04/23/10   vkg       target_nonsec definiton as per the USES_SECBOOT flag
# 04/14/10   vkg       Splitting of Securebootstage1 and stage2
# 04/06/10   vkg       Added support for QDSP6SW and QDSP6FW image secboot
# 03/10/10   vkg       Added support for QDSP5 image secboot,
#                      Added Unzip_Files function instead of calling extractall.
# 11/01/09   niting    Added support for secure boot authentication
#
#============================================================================
import os
import subprocess
import string
import re
import zipfile
import glob
import shutil

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
BOOT_TOOLS_ROOT = "${BUILD_ROOT}/tools/headergen"
PBOOT_GEN_ELF = BOOT_TOOLS_ROOT + "/pboot_gen_elf${EXE_EXT}"
IMAGE_HEADER = BOOT_TOOLS_ROOT + "/image_header${EXE_EXT}"
PBOOT_ADD_HASH = BOOT_TOOLS_ROOT + "/pboot_add_hash${EXE_EXT}"
MODIFY_ELF_FLAGS = BOOT_TOOLS_ROOT + "/modify_elf_flags${EXE_EXT}"
IMAGE_AUTH = BOOT_TOOLS_ROOT + "/image_auth${EXE_EXT}"
QDST_ROOT = "${BUILD_ROOT}/core/storage/tools/qdst"
RELPATH = "${BUILD_ROOT}/avs"
#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('mbn_builder')

def generate(env):
   # Add MBN Tools to environment
   env.Tool('mbn_tools', toolpath = ['${BUILD_ROOT}/sectools/sectools/features/isc/parsegen'])

   mbn_generate(env)
   modify_elf_flags_generate(env)
   modify_relocatable_flags_generate(env)

#============================================================================
# Modify Elf Flags rules
#============================================================================
# input ELF = source [0]
# input SCL = source [1]

#----------------------------------------------------------------------------
# Modify Elf Flags definition
#----------------------------------------------------------------------------
def modify_elf_flags_generate(env):
   modify_elf_flags_act = env.GetBuilderAction(modify_elf_flags_builder)
   modify_elf_flags_bld = env.Builder(action = modify_elf_flags_act,
                                      emitter = modify_elf_flags_emitter,
                                      suffix = '.elf')

   env.Append(BUILDERS = {'ModifyElfFlagsBuilder' : modify_elf_flags_bld})

#----------------------------------------------------------------------------
# Modify Elf Flags Emitter to add clean actions and other misc
#----------------------------------------------------------------------------
def modify_elf_flags_emitter(target, source, env):
   if 'USES_MBNTOOLS' not in env:
      env.Depends(target, env.SubstRealPath(MODIFY_ELF_FLAGS))
   return (target, source)

#----------------------------------------------------------------------------
# Modify Elf Flags Action
#   Generate modify_elf_flags tool and update attributes in target elf file
#   to reflect segment types in the input SCL
#----------------------------------------------------------------------------
def modify_elf_flags_builder(target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   source_base = os.path.splitext(str(source[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]

   target_full = str(target[0])
   source_elf_full = str(source[0])
   source_scl_full = str(source[1])   

   # Copy source elf to target directory
   shutil.copy(source_elf_full, target_full)

   #-------------------------------------------------------------------------
   # Modify elf flags
   #-------------------------------------------------------------------------
   if ('USES_MBNTOOLS' in env) and env['GLOBAL_DICT']:
      env.ModifyElfFlags(target_full, source_scl_full)
   else:
      cmds = env.subst(MODIFY_ELF_FLAGS + " -d " + target_full + " " + source_scl_full)
      data, err, rv = env.ExecCmds(cmds, target=target_file)

   return None

   ######################################################################
   
#============================================================================
# Modify Relocatable Flags rules
#============================================================================
# input ELF        = source [0]

#----------------------------------------------------------------------------
# Modify Relocatable Flags definition
#----------------------------------------------------------------------------
def modify_relocatable_flags_generate(env):
   modify_relocatable_flags_act = env.GetBuilderAction(modify_relocatable_flags_builder)
   modify_relocatable_flags_bld = env.Builder(action = modify_relocatable_flags_act,
                                      emitter = modify_relocatable_flags_emitter,
                                      suffix = '.elf')

   env.Append(BUILDERS = {'ModifyRelocatableFlagsBuilder' : modify_relocatable_flags_bld})

#----------------------------------------------------------------------------
# Modify Relocatable Flags Emitter to add clean actions and other misc
#----------------------------------------------------------------------------
def modify_relocatable_flags_emitter(target, source, env):
   return (target, source)

#----------------------------------------------------------------------------
# Modify Relocatable Flags Action
#   Generate modify_relocatable_flags tool and update attributes in target elf file
#----------------------------------------------------------------------------
def modify_relocatable_flags_builder(target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   source_base = os.path.splitext(str(source[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]

   target_full = str(target[0])
   source_elf_full = str(source[0])  

   # Copy source elf to target directory
   shutil.copy(source_elf_full, target_full)

   #-------------------------------------------------------------------------
   # Modify Relocatable flags
   #-------------------------------------------------------------------------
   env.ModifyRelocatableFlags(target_full)

   return None
   ######################################################################
#============================================================================
# MBN build rules
#============================================================================
def mbn_generate(env):
   env.AddMethod(mbn_builder, "MbnBuilder")

   #-------------------------------------------------------------------------
   # MBN builder definition
   #-------------------------------------------------------------------------
   stage1_mbn_act = env.GetBuilderAction(stage1_mbn_builder)
   stage1_mbn_bld = env.Builder(action = stage1_mbn_act,
                         emitter = stage1_mbn_emitter,
                         suffix = '.mbn')

   env.Append(BUILDERS = {'Stage1MbnBuilder' : stage1_mbn_bld})
   
   stage2_mbn_act = env.GetBuilderAction(stage2_mbn_builder)
   stage2_mbn_bld = env.Builder(action = stage2_mbn_act,
                         emitter = stage2_mbn_emitter,
                         suffix = '.mbn')

   env.Append(BUILDERS = {'Stage2MbnBuilder' : stage2_mbn_bld})

   #Need to add enc_mbn_builder if encryption desired.
   #enc_mbn_act = env.GetBuilderAction(enc_mbn_builder)
   #enc_mbn_bld = env.Builder(action = enc_mbn_act,
   #                         suffix = '.enc')

   #env.Append(BUILDERS = {'EncMbnBuilder' : enc_mbn_bld})


   mbn_dummy_act = env.GetBuilderAction(mbn_dummy_file_gen)
   mbn_dummy_bld = env.Builder(action = mbn_dummy_act, suffix = '.mbn')

   env.Append(BUILDERS = {'MbnDummyBuilder' : mbn_dummy_bld})


#----------------------------------------------------------------------------
# MBN build Action
#
# for DBL, HOSTDL and EHOSTDL
#  1) create dummy fsbl.mbn file
#  2) create header from dbl.bin and fsbl.bin
#  3) concatonate header + dbl.bin
#
# for OSBL and APPSBL mbn
#  1) create header from target.bin
#  2) concatonate header + target.bin
#
# for AMSS mbn
#  1) create hash from pbn source[0]
#  2) create header for hash
#  3) use pbn, hash, and hash_header to generate MBN
#
# for AMSS header mbn
#  1) create header from amss.mbn
#
# for QDSP6 mbn
#  1) create MBN(for q6 is an elf) using fromelf tool
#
#----------------------------------------------------------------------------
# MBN builder function
#----------------------------------------------------------------------------
def mbn_builder(env, target, source, **kwargs):
   target_base = os.path.splitext(str(target))[0]
   target_full = str(target)

   #----------------------------------------------------------------------------
   # Parse argument list 
   #----------------------------------------------------------------------------
   if not kwargs.has_key('IMAGE_TYPE'):
      raise RuntimeError, "IMAGE_TYPE is not defined"
   if not kwargs.has_key('FLASH_TYPE'):
      raise RuntimeError, "FLASH_TYPE is not defined"
  
   #----------------------------------------------------------------------------
   # MBN Builder supported features
   #----------------------------------------------------------------------------
   if kwargs.has_key('ENABLE_ENCRYPT'):
      env.Replace(ENABLE_ENCRYPT = kwargs.get('ENABLE_ENCRYPT'))
   else:
      env.Replace(ENABLE_ENCRYPT = False) 

   if kwargs.has_key('ENABLE_RELOCATABLE'):
      env.Replace(ENABLE_RELOCATABLE = kwargs.get('ENABLE_RELOCATABLE'))
   else:
      env.Replace(ENABLE_RELOCATABLE = False)

   #----------------------------------------------------------------------------
   # Initialize MBN Tools
   #----------------------------------------------------------------------------
   # MBN Tools Flag must be enabled in the environment and the global dictionary
   # must be populated in order to use MBN Tools
   if ('USES_MBNTOOLS' in env) and env['GLOBAL_DICT']:
      use_mbn_tools = True 
      # Create the generic dictionary for MBN tools to use
      gen_dict = env.FilterDictionary(env['GLOBAL_DICT'], **kwargs)
      # Assign MBN Type to the environment
      env.Replace(MBN_TYPE = gen_dict['IMAGE_KEY_MBN_TYPE'])
   else:
      use_mbn_tools = False
      gen_dict = {} 
      # If MBN Tools is not present, do manual check and assign MBN type
      if is_elf_image(kwargs.get('IMAGE_TYPE')):
         env.Replace(MBN_TYPE = 'elf')
      else:
         env.Replace(MBN_TYPE = 'bin')

   #----------------------------------------------------------------------------
   # Qualcomm Development Signing Tool secure boot
   # - Only enable if USES_SECBOOT is not defined in environment
   # - To enable by default for non secure builds, remove condition check
   #     if 'USES_SECBOOT_QDST' in env
   #   and enable environment variables USES_SECBOOT='yes' and 
   #   USES_SECBOOT_QDST='yes' by default
   # 
   #----------------------------------------------------------------------------
   if not 'USES_SECBOOT' in env:
      ### uncomment the two lines below to enable by default
      #env.Replace(USES_SECBOOT_QDST = 'yes')
      #env.Replace(USES_SECBOOT = 'yes')
      ###

      ### remove the two lines below if enabling by default
      if 'USES_SECBOOT_QDST' in env:
         env.Replace(USES_SECBOOT = 'yes')
      ###

   #----------------------------------------------------------------------------
   # Determine secure boot stage
   #----------------------------------------------------------------------------
   if 'USES_SECBOOT' in env:
      if (not is_hash_required(env)):
         target_nonsec = target_base + "_nonsec.mbn"
      else:
         target_nonsec = target_base + "_hash_nonsec.mbn"
      target = target_base + ".mbn"
      env.Depends(target_nonsec, source)
      mbn_stage1 = env.Stage1MbnBuilder( target_nonsec, source, 
                    IMAGE_TYPE = kwargs.get('IMAGE_TYPE'), 
                    FLASH_TYPE = kwargs.get('FLASH_TYPE'),
                    USE_MBN_TOOLS = use_mbn_tools,
                    GEN_DICT = gen_dict)
      mbn_stage_final = env.Stage2MbnBuilder( target, mbn_stage1,
                    IMAGE_TYPE = kwargs.get('IMAGE_TYPE'),
                    FLASH_TYPE = kwargs.get('FLASH_TYPE'),
                    USE_MBN_TOOLS = use_mbn_tools,
                    GEN_DICT = gen_dict)
      #Delete intermediate dummy target target
      if(os.path.exists(str(env.subst(target)))):
         if (os.path.getsize(str(env.subst(target))) == 0):
             os.remove(str(env.subst(target)))
   else:
      mbn_stage_final = env.Stage1MbnBuilder( target, source,
                    IMAGE_TYPE = kwargs.get('IMAGE_TYPE'), 
                    FLASH_TYPE = kwargs.get('FLASH_TYPE'),
                    USE_MBN_TOOLS = use_mbn_tools,
                    GEN_DICT = gen_dict)

   #----------------------------------------------------------------------------
   # Place the Image to build\ms\bin folder
   #----------------------------------------------------------------------------
   #Note: In case of SecureBuild, stage1 will be placing size zero Image as a
   #sideeffect for all the images and later stage2 it will be updating
   #that to nonzero size signed image.
   PROC_NAME=env.get('PROC_NAME','MODEM')
   install_mbn_final = ""
   if (kwargs.get('IMAGE_TYPE')=="dbl" ):
        install_mbn_final = env.InstallAs('${MBN_ROOT}/dbl.mbn', mbn_stage_final)
   elif (kwargs.get('IMAGE_TYPE')=="osbl"):
        install_mbn_final = env.InstallAs('${MBN_ROOT}/osbl.mbn', mbn_stage_final)	
   elif (kwargs.get('IMAGE_TYPE')=="appsbl"):
        install_mbn_final = env.InstallAs('${MBN_ROOT}/appsboot.mbn', mbn_stage_final)
   elif (kwargs.get('IMAGE_TYPE')=="amss_mbn"):
        if (PROC_NAME=="MODEM" or PROC_NAME=="UNIPROC"):
            install_mbn_final = env.InstallAs('${MBN_ROOT}/amss.mbn', mbn_stage_final)
        elif (PROC_NAME=="APPS"):
            install_mbn_final = env.InstallAs('${MBN_ROOT}/apps.mbn', mbn_stage_final)
   elif (kwargs.get('IMAGE_TYPE')=="qdsp6sw"):
   	
		if env['ENABLE_RELOCATABLE'] is True:  
			install_mbn_final = env.InstallAs('${MBN_ROOT}/dsp2.mbn', mbn_stage_final)
		else:
			install_mbn_final = env.InstallAs('${MBN_ROOT}/non-reloc/dsp2.mbn', mbn_stage_final)
		            
   elif (kwargs.get('IMAGE_TYPE')=="qdsp6fw"):
        install_mbn_final = env.InstallAs('${MBN_ROOT}/dsp1.mbn', mbn_stage_final)
   elif (kwargs.get('IMAGE_TYPE')=="hostdl"):
        install_mbn_final =  mbn_stage_final
   elif (kwargs.get('IMAGE_TYPE')=="ehostdl"):
        install_mbn_final =  mbn_stage_final
   elif (kwargs.get('IMAGE_TYPE')=="emmcbld"):
        install_mbn_final =  mbn_stage_final
   elif (kwargs.get('IMAGE_TYPE')=="qdsp5"):
        install_mbn_final = env.InstallAs('${MBN_ROOT}/adsp.mbn', mbn_stage_final)
   else: 
   
	


		# install encrypted mbn image as .enc file if encryption feature is on
		if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
		      
		      target_full_enc = target_base + ".enc" 
		      enc_stage_final = env.EncMbnBuilder(target_full_enc, mbn_stage_final)
		      if 'MBN_FILE' in env:      
		         install_enc_final = env.InstallAs('${MBN_FILE}.enc', enc_stage_final )
		      else:         
		         if env['ENABLE_RELOCATABLE'] is True:  
		            install_enc_final = env.InstallAs('${MBN_ROOT}/' + kwargs.get('IMAGE_TYPE') + '.enc', enc_stage_final )
		         else:
		            install_enc_final = env.InstallAs('${MBN_ROOT}/non-reloc/' + kwargs.get('IMAGE_TYPE') + '.enc', enc_stage_final )
		elif 'MBN_FILE' in env:
			install_mbn_final = env.InstallAs('${MBN_ROOT}/' + kwargs.get('IMAGE_TYPE') + '.mbn', mbn_stage_final)
		else:
			if env['ENABLE_RELOCATABLE'] is True:  
				install_mbn_final = env.InstallAs('${MBN_ROOT}/' + kwargs.get('IMAGE_TYPE') + '.mbn', mbn_stage_final )
			else:
				install_mbn_final = env.InstallAs('${MBN_ROOT}/non-reloc/' + kwargs.get('IMAGE_TYPE') + '.mbn', mbn_stage_final )	

		

   # Return two builder targets if Encryption Feature is ON
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      return [install_mbn_final, install_enc_final]
   else:
      return  install_mbn_final


#----------------------------------------------------------------------------
# Stage1 MBN builder emitter
#----------------------------------------------------------------------------
def stage1_mbn_emitter(target, source, env):
   source_base_str = os.path.splitext(str(source[0]))[0]
   target_base_str = os.path.splitext(str(target[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]
   source_path, source_file = os.path.split(os.path.abspath(str(source[0])))
   target_bin_base = env.subst("${MBN_ROOT}")
   target_bin_dir = target_bin_dir_name(env)
   
   if 'USES_SECBOOT' in env:
      # clean nonsec files in bin directory
      if (not is_hash_required(env)):
         target_bin_nonsec = os.path.join(target_bin_base, target_bin_dir, \
            str(os.path.split(target_base)[1]) + "_nonsec.mbn")
      else:
         target_bin_nonsec = os.path.join(target_bin_base, target_bin_dir, \
            str(os.path.split(target_base)[1]) + "_hash_nonsec.mbn")
      env.Clean(target, target_bin_nonsec)

      # clean zip files and cert directories in bin directory
      target_zip_cert_dir = os.path.join(target_bin_base, target_bin_dir, \
                                         "cert")
      bin_zipfiles = glob.glob(target_zip_cert_dir + '/*.zip')
      for bin_zip_file in bin_zipfiles:
         env.Clean(target, bin_zip_file)

      env.Clean(target, target_zip_cert_dir)
      env.Clean(target, os.path.join(target_bin_base, target_bin_dir))

   if 'USES_SECBOOT_QDST' in env:
      target_qdst_base = 'QDST_' + target_base_str + '_attest.key'
      env.Clean(target, target_qdst_base)
    
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      #clean the unencrypted mbn file under bin directory
      target_bin_non_enc = glob.glob(target_bin_base + '/'+ target_bin_dir + '.mbn')
      env.Clean(target, target_bin_non_enc)
      env.Clean(target, target_base_str +'_enc_no_xml.mbn')
      env.Clean(target, target_base_str +'_enc_md.xml')
      env.Clean(target, target_base_str +'_non_enc.mbn')
      
   if env.subst("$IMAGE_TYPE") in \
      ["dbl","sbl1"]:
      if 'USES_MBNTOOLS' not in env:
         env.Depends(target, env.SubstRealPath(IMAGE_HEADER))
      env.Clean(target, target_base_str +'.hd')
      # preamble file is a side affect file
      env.SideEffect(target_base_str +'_preamble.mbn', target)

   elif env.subst("$IMAGE_TYPE") in \
      ["osbl","sbl2","sbl3","tz","rpm","appsbl","hostdl","ehostdl","emmcbld","qdsp5"]:
      if 'USES_MBNTOOLS' not in env:
         env.Depends(target, env.SubstRealPath(IMAGE_HEADER))
      env.Clean(target, target_base_str +'.hd')

   elif env.subst("$IMAGE_TYPE") in \
      ["amss_mbn","qdsp6sw","qdsp6fw"]:
      if 'USES_MBNTOOLS' not in env:
         env.Depends(target, env.SubstRealPath(PBOOT_GEN_ELF))
         env.Depends(target, env.SubstRealPath(IMAGE_HEADER))
         env.Depends(target, env.SubstRealPath(PBOOT_ADD_HASH))
         env.Depends(target, env.SubstRealPath(MODIFY_ELF_FLAGS))
      env.Clean(target, target_base_str +'.hash')
      env.Clean(target, target_base_str +'_hash.hd')
      env.Clean(target, target_base_str +'_phdr.pbn')

   return (target, source)

#----------------------------------------------------------------------------
# Stage1 MBN builder
#----------------------------------------------------------------------------
def stage1_mbn_builder( target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   source_base = os.path.splitext(str(source[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]
   source_full = str(source[0])
   target_full = str(target[0])
   if env['ENABLE_RELOCATABLE'] is True:  
      target_bin_base = env.subst("${MBN_ROOT}/")
   else:
      target_bin_base = env.subst("${MBN_ROOT}/non-reloc/")
   target_bin_dir = target_bin_dir_name(env)

   # Check for proper arguments in environment
   if not env.has_key('IMAGE_TYPE'):
      raise RuntimeError, "IMAGE_TYPE is not defined"
   if not env.has_key('FLASH_TYPE'):
      raise RuntimeError, "FLASH_TYPE is not defined"
   if not env.has_key('USE_MBN_TOOLS'):
      raise RuntimeError, "USE_MBN_TOOLS is not defined"
   if not env.has_key('GEN_DICT'):
      raise RuntimeError, "GEN_DICT is not defined"

   use_mbn_tools = env.get('USE_MBN_TOOLS')
   gen_dict = env.get('GEN_DICT') 

   # SECBOOT CASE
   if 'USES_SECBOOT' in env:

      target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
         str(os.path.split(target_base)[1]) + ".mbn"

      target_nonsec = target_base + ".mbn"
      image_header_secflag = "secure"

      # Find zip file in source_path
      target_zip_cert_dir = target_bin_base + target_bin_dir + "/cert"
      if not os.path.exists(target_zip_cert_dir):
         os.makedirs(target_zip_cert_dir)
         print "Created " + target_zip_cert_dir + " Folder\n"
      target_zip_str = target_zip_cert_dir + '/*.zip'

      source_zip = ""
      zipfiles = glob.glob(target_zip_str)

      if (len(zipfiles) > 0):
         source_zip = zipfiles[0]

   # NON_SECBOOT CASE
   else:
      image_header_secflag = "nonsecure"

   # Default initializations
   qdst_xml = "/QDST_"+env.subst("$IMAGE_TYPE")+".xml"
   image_header_cmd = env.subst("$IMAGE_TYPE")  

   #-------------------------------------------------------------------------
   # Binary MBN generation
   #-------------------------------------------------------------------------
   if env.subst("$IMAGE_TYPE") in \
      ["dbl","osbl","sbl1","sbl2","sbl3","rpm","tz","appsbl","qdsp5","hostdl","ehostdl","emmcbld"] or \
      env['MBN_TYPE'] == 'bin':
      
      target_full_non_enc = target_base + "_non_enc.mbn"
      target_full_enc_no_xml = target_base + "_enc_no_xml.mbn"
      target_enc_xml = target_base + "_enc_md.xml"
      target_bin_non_enc = target_bin_base + target_bin_dir + ".mbn"
      
      # Default parameter initializations 
      requires_preamble = False
      preamble_file_name = target_base + "_preamble.mbn"
      header_format = 'reg' 

      # Custom image parameter assignments
      if env.subst("$IMAGE_TYPE") == "sbl1":
         requires_preamble = True
         header_format = 'sbl' 

      elif env.subst("$IMAGE_TYPE") == "emmcbld":
         requires_preamble = False
         header_format = 'sbl' 

      elif env.subst("$IMAGE_TYPE") == "dbl":
         header_format = 'sbl' 
         if ('USES_SAHARA_BOOT' in env):
            header_format = 'reg'
         elif env.subst("$FLASH_TYPE") == "nand" and not env.has_key('USES_512NAND'):
            requires_preamble = True
            image_header_cmd = "dbl_preamble" 

      elif env.subst("$IMAGE_TYPE") == "qdsp5": 
         image_header_cmd = "adspq5"

      elif env.subst("$IMAGE_TYPE") == "hostdl": 
         if env['MSM_ID'] in ['6195','6295','6695','6615']:
            image_header_cmd = "nandprg"
         else:
            image_header_cmd = "hostdl"
         qdst_xml = "/QDST_nandprg.xml"

      elif env.subst("$IMAGE_TYPE") == "ehostdl": 
         image_header_cmd = "ehostdl"
         qdst_xml = "/QDST_enandprg.xml"

      # Generating header for the RAWIMAGE
      if use_mbn_tools is True:
         rv = env.ImageHeader(gen_dict, source_full, target_base + ".hd", 
           image_header_secflag, header_format = header_format, 
           requires_preamble = requires_preamble, preamble_file_name = preamble_file_name)
      else:
         if requires_preamble is False:
            cmds = env.subst(IMAGE_HEADER + " $FLASH_TYPE "+ image_header_cmd + " " + \
              source_full + " " + target_base + ".hd " + image_header_secflag)
         else:
            cmds = env.subst(IMAGE_HEADER + " $FLASH_TYPE "+ image_header_cmd + " " + \
               source_full + " " + target_base + ".hd " + image_header_secflag + " " + \
               preamble_file_name)

         data, err, rv = env.ExecCmds(cmds, target=target_file)

      # If no errors in Generating RAW IMAGE
      if not rv:
         files_to_cat_in_order = [target_base + ".hd", source_full]

         # SECURE BOOT CASE
         if env.has_key('USES_SECBOOT'):
            concat_files (target_nonsec, files_to_cat_in_order)
            env.Depends(target_bin_nonsec, target_nonsec)
            shutil.copy(target_nonsec, target_bin_nonsec)
            if source_zip == "":
               print target_bin_nonsec + " image has not been " + \
                  "signed. Generate certificates and signature "+ \
                  "from CSMS website and place it at " + target_bin_dir_name(env)+\
                  "\\cert folder \n"
            target_full = target_nonsec

            if 'USES_SECBOOT_QDST' in env:
               ##### Run QDST script
               qdst_root = env.subst(QDST_ROOT)
               cmds = env.subst("python " + qdst_root + "/QDST.py image=" + target_bin_nonsec + " xml=" + qdst_root + qdst_xml)

               data, err, rv = env.ExecCmds(cmds, target=target_file)
               ##### End Run QDST script

         # NON SECURE BOOT CASE
         else:
            # If preamble is required, prepend to the final image
            if requires_preamble is True:
              files_to_cat_in_order.insert(0,preamble_file_name)
            
            concat_files (target_full, files_to_cat_in_order)		
  
            if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
              # install the unencrypted file to bin dir
              shutil.copy(target_full, target_full_non_enc)
              shutil.copy(target_full, target_bin_non_enc)
              # generate xml header
              env.GenerateMetaData(target_enc_xml,True)
              # encrypt the full mbn image
              env.EncryptMBN(target_full,target_full_enc_no_xml)
              # concat xml header in front of encrypted image
              concat_files (target_full, [target_enc_xml, target_full_enc_no_xml])		
                
            
   #-------------------------------------------------------------------------
   # ELF MBN generation 
   #-------------------------------------------------------------------------
   elif env.subst("$IMAGE_TYPE") in ["amss_mbn","qdsp6sw","qdsp6fw"] or \
        env['MBN_TYPE'] == 'elf':

      target_hash = target_base + ".hash"
      target_hash_hd = target_base + "_hash.hd"
      target_cert_chain = target_base + "_hash_cert_chain.mbn"
      target_hash_full = target_base + "_hash_sec.mbn"
      target_phdr_elf = target_base + "_phdr.pbn"
      target_add_hash_elf = target_phdr_elf
      target_pre_encrypt_elf = target_base + "_pre_encrypt.pbn"
      target_encrypt_xml = target_base + "_encrypt_md.xml"
    
      # Custom image parameter assignments
      if env.subst("$IMAGE_TYPE") == "amss_mbn":
         image_header_cmd = "amsshash"
         qdst_xml = "/QDST_amsshash.xml"
      elif env.subst("$IMAGE_TYPE") == "qdsp6sw":
         image_header_cmd = "dsp2hash"
         qdst_xml = "/QDST_dsp2hash.xml"
      elif env.subst("$IMAGE_TYPE") == "qdsp6fw":
         image_header_cmd = "dsp1hash"
         qdst_xml = "/QDST_dsp1hash.xml"

      # For relocatable modem, get specific parameters for hash size and address
      if ('USES_RELOCATABLE_MODEM' in env) and (env['ENABLE_RELOCATABLE'] is True):  
          (hash_seg_size, hash_seg_offset, last_phys_addr) = env.PbnGetDemandLoadingData()
          hash_seg_size  = eval(hash_seg_size)
          last_phys_addr = eval(last_phys_addr)
      else: 
          hash_seg_size = None
          last_phys_addr = None

      if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
         encrypted_image = True 
      else:
         encrypted_image = False

      if use_mbn_tools is True:
         rv = env.PBootGenElf(source_full, target_hash, 
                              elf_out_file_name = target_phdr_elf,
                              secure_type = image_header_secflag,
                              hash_seg_max_size = hash_seg_size,
                              last_phys_addr = last_phys_addr,        
                              append_xml_hdr = encrypted_image)        
         if not rv:
            rv = env.ImageHeader(gen_dict, target_hash, target_hash_hd, 
                                 image_header_secflag, elf_file_name = source_full)

      if 'USES_SECBOOT' in env:
         target_nonsec = target_base + ".mbn"
         if use_mbn_tools is False: 
            if ('USES_SAHARA_BOOT' in env):
               cmds = [
                  env.subst(PBOOT_GEN_ELF + " " + source_full + " " + target_hash + \
                     " " + target_phdr_elf + " " + image_header_secflag),
                  env.subst(IMAGE_HEADER + " $FLASH_TYPE " + image_header_cmd + " " + \
                     target_hash + " " + target_hash_hd + " " + image_header_secflag + \
                     " " + source_full),
                      ]
               data, err, rv = env.ExecCmds(cmds, target=target_file)
            else:
               cmds = [
                  env.subst(PBOOT_GEN_ELF + " " + source_full + " " + target_hash),
                  env.subst(IMAGE_HEADER + " $FLASH_TYPE " + image_header_cmd + " " + \
                     target_hash + " " + target_hash_hd + " " + image_header_secflag + \
                     " " + source_full),
                      ]
               data, err, rv = env.ExecCmds(cmds, target=target_file)

         if not rv:
            files_to_cat_in_order = [target_hash_hd, target_hash]

            target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
               str(os.path.split(target_nonsec)[1])
            concat_files (target_nonsec, files_to_cat_in_order)
            env.Depends(target_bin_nonsec, target_nonsec)
            shutil.copy(target_nonsec, target_bin_nonsec)
            if source_zip == "":
               print target_bin_nonsec + " image has not been " + \
                  "signed. Generate certificates and signature "+ \
                  "from CSMS website and place it at " + target_bin_dir_name(env)+\
                  "\\cert folder \n"

            if 'USES_SECBOOT_QDST' in env:
               ##### Run QDST script
               qdst_root = env.subst(QDST_ROOT)
               cmds = env.subst("python " + qdst_root + "/QDST.py image=" + target_bin_nonsec + " xml=" + qdst_root + qdst_xml)

               data, err, rv = env.ExecCmds(cmds, target=target_file)
               ##### End Run QDST script

            target_full = target_nonsec

      else:
         target_nonsec = target_base + "_combined_hash.mbn"
         if use_mbn_tools is False:
            cmds = [
               env.subst(PBOOT_GEN_ELF + " " + source_full + " " + target_hash),
               env.subst(IMAGE_HEADER + " $FLASH_TYPE " + image_header_cmd + " " + \
                 target_hash + " " + target_hash_hd + " " + image_header_secflag + \
                 " " + source_full),]

            data, err, rv = env.ExecCmds(cmds, target=target_file)


         if not rv:
            files_to_cat_in_order = [target_hash_hd, target_hash]

            # If modem encryption is ON, generate XML header and append to hash segment
            if encrypted_image is True: 
               env.GenerateMetaData(target_encrypt_xml)
               files_to_cat_in_order.append(target_encrypt_xml) 

            concat_files (target_nonsec, files_to_cat_in_order)

            if use_mbn_tools is True:
               # Add the hash segment into the ELF and encrypt if necessary 
               if encrypted_image is True: 
                  env.PBootAddHash(target_add_hash_elf, target_nonsec, target_pre_encrypt_elf)

                  env.EncryptElfSegments(target_pre_encrypt_elf, target_full)
                  
               else:
                  env.PBootAddHash(target_add_hash_elf, target_nonsec, target_full)
            else:
               cmds = [ 
                  env.subst(PBOOT_ADD_HASH + " " + source_full + " " + \
                            target_nonsec + " " + target_full + " " + \
                            "nonsecure"),
                      ]
               data, err, rv = env.ExecCmds(cmds, target=target_file)

   return None
#END OF STAGE1_MBN_BUILDER

#----------------------------------------------------------------------------
# Stage2 MBN builder emitter
#----------------------------------------------------------------------------
def stage2_mbn_emitter(target, source, env):

   source_base_str = os.path.splitext(str(source[0]))[0]
   target_base_str = os.path.splitext(str(target[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]
   source_path, source_file = os.path.split(os.path.abspath(str(source[0])))
   target_bin_base = env.subst("${MBN_ROOT}")
   target_bin_dir = target_bin_dir_name(env)
   
   if 'USES_SECBOOT' in env:
      zipfiles = glob.glob(source_path + '/*.zip')
      if len(zipfiles) > 0:
         # add zip_file to source list in order to rebuild stage 2
         source.append(zipfiles)
         env.Clean(target, zipfiles)

      sigfiles = glob.glob(source_path + '/*-signature.bin')
      for sig_file in sigfiles:
         env.Clean(target, sig_file)

      certfiles = glob.glob(source_path + '/*.cer')
      for cert_file in certfiles:
         env.Clean(target, cert_file)

      if (not is_hash_required(env)):
         env.Clean(target, target_base_str +'_cert_chain.mbn')
         
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      #clean the unencrypted mbn file under bin drectory
      target_bin_non_enc = glob.glob(target_bin_base + '/'+ target_bin_dir + '.mbn')
      env.Clean(target, target_bin_non_enc)
      env.Clean(target, target_base_str +'_enc_no_xml.mbn')
      env.Clean(target, target_base_str +'_enc_md.xml')
      env.Clean(target, target_base_str +'_non_enc.mbn')
   
   if env.subst("$IMAGE_TYPE") == "dbl":
      if 'USES_SECBOOT' in env:
         env.Clean(target, target_base_str +'_combined.mbn')
         env.Depends(target, target_base_str +'_nonsec_preamble.mbn')
         if 'USES_MBNTOOLS' not in env:
            env.Depends(target, env.SubstRealPath(IMAGE_AUTH))

   elif env.subst("$IMAGE_TYPE") in \
      ["osbl","appsbl","hostdl","ehostdl","emmcbld","qdsp5"]:
      
      if 'USES_SECBOOT' in env:
         if 'USES_MBNTOOLS' not in env:
            env.Depends(target, env.SubstRealPath(IMAGE_AUTH))

   elif env.subst("$IMAGE_TYPE") in ["amss_mbn","qdsp6sw","qdsp6fw"]:
      if 'USES_SECBOOT' in env:
         env.Clean(target, target_base_str +'_hash_sec.mbn')
         env.Clean(target, target_base_str +'_hash_cert_chain.mbn')
         if 'USES_MBNTOOLS' not in env:
            env.Depends(target, env.SubstRealPath(IMAGE_AUTH))

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#----------------------------------------------------------------------------
# Stage2 MBN builder
#----------------------------------------------------------------------------
def stage2_mbn_builder(target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   target_base = os.path.splitext(str(target[0]))[0]
   source_full = str(source[0])
   target_full = str(target[0])
   if env['ENABLE_RELOCATABLE'] is True:  
      target_bin_base = env.subst("${MBN_ROOT}/")
   else:
      target_bin_base = env.subst("${MBN_ROOT}/non-reloc/")
   target_bin_dir = target_bin_dir_name(env)
   
   # Check for proper arguments in environment
   if not env.has_key('IMAGE_TYPE'):
      raise RuntimeError, "IMAGE_TYPE is not defined"
   if not env.has_key('FLASH_TYPE'):
      raise RuntimeError, "FLASH_TYPE is not defined"
   if not env.has_key('USE_MBN_TOOLS'):
      raise RuntimeError, "USE_MBN_TOOLS is not defined"
   if not env.has_key('GEN_DICT'):
      raise RuntimeError, "GEN_DICT is not defined"

   use_mbn_tools = env.get('USE_MBN_TOOLS')
   gen_dict = env.get('GEN_DICT') 

   if 'USES_SECBOOT' in env:
   
      target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
         str(os.path.split(target_base)[1]) + "_nonsec.mbn"

      if (not is_hash_required(env)):
         target_nonsec = target_base + "_nonsec.mbn"
      else:
         target_nonsec = target_base + "_hash_nonsec.mbn"
      image_header_secflag = "secure"

      # Find zip file in source_path
      target_zip_cert_dir = target_bin_base + target_bin_dir + "/cert"
      target_zip_str = target_zip_cert_dir + '/*.zip'

      source_zip = ""
      zipfiles = glob.glob(target_zip_str)
      if len(zipfiles) > 0:
         source_zip = zipfiles[0]

      if source_zip != "":
         target_zip = os.path.join(str(source_path), \
                                   os.path.split(str(source_zip))[1])
         env.Depends(target_zip, source_zip)
         shutil.copy(source_zip, target_zip)
         #x#target_zipfp = zipfile.ZipFile(target_zip, "r")
         target_zip_base = os.path.splitext(str(target_zip))[0]
         signature = target_zip_base + "-signature.bin"
         attestation_cert = target_zip_base + "-attestation_cert.cer"
         attestation_ca_cert = target_zip_base + "-attestation_ca_cert.cer"
         root_cert = target_zip_base + "-root_cert.cer"
         target_cert_chain = target_base + "_cert_chain.mbn"

   #-------------------------------------------------------------------------
   # Binary MBN generation
   #-------------------------------------------------------------------------				  
   if env.subst("$IMAGE_TYPE") in \
      ["dbl","osbl","sbl1","sbl2","sbl3","tz","rpm","appsbl","hostdl","ehostdl","emmcbld","qdsp5"] or \
      env['MBN_TYPE'] == 'bin':

      # Default parameter initializations 
      prepend_preamble = False
      target_combined = target_base + "_combined.mbn"
      preamble_file = target_base + "_nonsec_preamble.mbn"
      
      target_full_non_enc = target_base + "_non_enc.mbn"
      target_full_enc_no_xml = target_base + "_enc_no_xml.mbn"
      target_enc_xml = target_base + "_enc_md.xml"
      target_bin_non_enc = target_bin_base + target_bin_dir + ".mbn"
      
      files_to_cat_in_order = [target_nonsec]

      # Custom image parameter assignments
      if env.subst("$IMAGE_TYPE") == "sbl1":
         prepend_preamble = True
      if (env.subst("$IMAGE_TYPE") == "dbl") and \
         (env.subst("$FLASH_TYPE") == "nand") and \
         (not ('USES_SAHARA_BOOT' in env)):
         prepend_preamble = True

      if 'USES_SECBOOT' in env:
         if source_zip == "":
             # Once stage1 is completed and no cert is found then create a size zero file
             open(target_full,"wb")
         else:
             # If image requires a preamble, prepend to combined image 
             if prepend_preamble == True:
                files_to_cat_in_order.insert(0,preamble_file)

             concat_files (target_combined, files_to_cat_in_order)

             Unzip_Files(target_zip,source_path)
             if use_mbn_tools is True:
               if (do_files_exist([target_combined, signature])):
                 rv = env.ImageAuth(target_combined, signature, attestation_cert, \
                                    attestation_ca_cert, root_cert, target_cert_chain, \
                                    target_full)
                                    
                 if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
                   # install the unencrypted file to bin dir
                   shutil.copy(target_full, target_full_non_enc)
                   shutil.copy(target_full, target_bin_non_enc)
                   # generate xml header
                   env.GenerateMetaData(target_enc_xml,True)
                   # encrypt the full mbn image
                   env.EncryptMBN(target_full,target_full_enc_no_xml)
                   # concat xml header in front of encrypted image
                   concat_files (target_full, [target_enc_xml, target_full_enc_no_xml])		
               
               else:
                 raise RuntimeError, "Missing file(s) required for signing.\n"	
             else: 
               if (do_files_exist([target_combined, signature, attestation_cert, \
                                  attestation_ca_cert, root_cert])):
                 cmds = env.subst(IMAGE_AUTH + " " + target_combined + " " + signature + " " + \
                    attestation_cert + " " + attestation_ca_cert + " " + root_cert + \
                    " " + target_cert_chain + " " + target_full)
                 data, err, rv = env.ExecCmds(cmds, target=target_file)
               else:
                 raise RuntimeError, "Missing file(s) required for signing.\n"

   #-------------------------------------------------------------------------
   # ELF MBN generation 
   #-------------------------------------------------------------------------
   elif env.subst("$IMAGE_TYPE") in ["amss_mbn", "qdsp6sw", "qdsp6fw"] or \
        env['MBN_TYPE'] == 'elf':

      target_cert_chain = target_base + "_hash_cert_chain.mbn"
      target_hash_full = target_base + "_hash_sec.mbn"
      target_pbn = target_base + ".pbn"
      target_phdr_elf = target_base + "_hash_nonsec_phdr.pbn"
      target_add_hash_elf = target_phdr_elf
      target_pre_encrypt_elf = target_base + "_pre_encrypt.pbn"
      target_encrypt_xml = target_base + "_encrypt_md.xml"
      target_hash_full_xml = target_base + "_xml_hash_sec.mbn"

      if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
         encrypted_image = True 
      else:
         encrypted_image = False

      if 'USES_SECBOOT' in env:
         target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
            str(os.path.split(target_nonsec)[1])
         if source_zip == "":
            # Once stage1 is completed and no cert is found then create a size zero file
            open(target_full,"wb")
         else:
            Unzip_Files(target_zip,source_path)
            if use_mbn_tools is True:
               if (do_files_exist([target_nonsec, signature])):
                  # Appends signature and certificates to the hash segment
                  env.ImageAuth(target_nonsec, signature, attestation_cert, \
                                attestation_ca_cert, root_cert, target_cert_chain, \
                                target_hash_full) 

                  # If modem encryption is ON, generate XML header and append to hash segment
                  if encrypted_image is True: 
                     env.GenerateMetaData(target_encrypt_xml)
                     concat_files(target_hash_full_xml, [target_hash_full, target_encrypt_xml])
                    
                     # Add the hash segment into the ELF and encrypt if necessary 
                     env.PBootAddHash(target_add_hash_elf, target_hash_full_xml, target_pre_encrypt_elf)
                     env.EncryptElfSegments(target_pre_encrypt_elf, target_full)
                  
                  else:
                     env.PBootAddHash(target_add_hash_elf, target_hash_full, target_full)
               else:
                 raise RuntimeError, "Missing file(s) required for signing.\n"
            else:
               if (do_files_exist([target_nonsec, signature, attestation_cert, \
                                   attestation_ca_cert, root_cert])):
                 cmds = env.subst(IMAGE_AUTH + " " + target_nonsec + " " + signature + " " + \
                    attestation_cert + " " + attestation_ca_cert + " " + root_cert + \
                    " " + target_cert_chain + " " + target_hash_full)
                 data, err, rv = env.ExecCmds(cmds, target=target_file)

                 if not rv:
                    if ('USES_SAHARA_BOOT' in env):
                       cmds = [
                          env.subst(PBOOT_ADD_HASH + " " + target_phdr_elf + " " + \
                          target_hash_full + " " + target_full + " " + \
                          image_header_secflag )
                              ]
                    else:
                       cmds = [
                          env.subst(PBOOT_ADD_HASH + " " + target_pbn + " " + \
                          target_hash_full + " " + target_full + " " + \
                          "nonsecure" )
                              ]

                    data, err, rv = env.ExecCmds(cmds, target=target_file)
               else:
                 raise RuntimeError, "Missing file(s) required for signing.\n"

   #-------------------------------------------------------------------------
   # QDSP6 MBN generation
   #-------------------------------------------------------------------------
   elif env.subst("$IMAGE_TYPE") == "qdsp6":
      cmds = env.subst("${BINTOOL} ${ARMBIN_NODEBUG_CMD} ${ARMBIN_NOLINKVIEW_CMD} "\
         "${ARMBIN_ELF_CMD} " + source_full + " ${ARMBIN_OUTPUT_CMD} " + target_full)

      data, err, rv = env.ExecCmds(cmds, target=target_file)

   return None
#END OF Stage2_MBN_builder


def do_files_exist(fileslist):
   for file_name in fileslist:
       if (not os.path.exists(file_name)):
          print "Could not find " + file_name + "\n"
          return 0
       else:
          print "Found " + file_name + "\n"
   return 1

def mbn_dummy_file_gen(target, source, env):
   target_str = str(target[0])
   file = open (target_str, "w")
   file.write("\nDummy file created " + target_str + "\n")
   file.close()
   return None

#----------------------------------------------------------------------------
# Target_bin_dir
#This will return the temporary directory name used for the image signing
#----------------------------------------------------------------------------
def target_bin_dir_name(env):
    target_bin_dir = ""
    if env.subst("$IMAGE_TYPE") == "dbl":
       target_bin_dir = "dbl"
    elif env.subst("$IMAGE_TYPE") == "osbl":
       target_bin_dir = "osbl"
    elif env.subst("$IMAGE_TYPE") == "sbl1":
       target_bin_dir = "sbl1"
    elif env.subst("$IMAGE_TYPE") == "sbl2":
       target_bin_dir = "sbl2"
    elif env.subst("$IMAGE_TYPE") == "sbl3":
       target_bin_dir = "sbl3"
    elif env.subst("$IMAGE_TYPE") == "tz":
       target_bin_dir = "tz"	   
    elif env.subst("$IMAGE_TYPE") == "appsbl":
       target_bin_dir = "appsbl"
    elif env.subst("$IMAGE_TYPE") == "hostdl":
       target_bin_dir = "nandprg"
    elif env.subst("$IMAGE_TYPE") == "ehostdl":
       target_bin_dir = "enandprg"
    elif env.subst("$IMAGE_TYPE") == "emmcbld":
       target_bin_dir = "mprg"
    elif env.subst("$IMAGE_TYPE") == "amss_mbn":
       target_bin_dir = "amsshash"
    elif env.subst("$IMAGE_TYPE") == "qdsp5":
       target_bin_dir = "qdsp5"
    elif env.subst("$IMAGE_TYPE") == "qdsp6":
       target_bin_dir = "qdsp6hash"
    elif env.subst("$IMAGE_TYPE") == "qdsp6fw":
       target_bin_dir = "qdsp6fwhash"
    elif env.subst("$IMAGE_TYPE") == "qdsp6sw":
       target_bin_dir = "qdsp6swhash"
    else:
       target_bin_dir = env.subst("$IMAGE_TYPE") 
    return target_bin_dir

#----------------------------------------------------------------------------
#Unzip_Files
#Get the files list from zip files and read and write a file one by one at
#given source path as that of zip file
#Unzip_Files(<complete_zip_file_path>)
#----------------------------------------------------------------------------
def Unzip_Files (target_zip, Unzipped_path):
   target_zipfp1 = zipfile.ZipFile(target_zip, "r")
   List_Of_Files = target_zipfp1.infolist()
   for files in List_Of_Files:
      Data = target_zipfp1.read(files.filename)
      print "Unzipping " + files.filename + " at " + Unzipped_path+ "\n"
      if Data:
         target_file = open(Unzipped_path+"/"+files.filename,'wb')
         target_file.write(Data)
         target_file.close()
   target_zipfp1.close()

# Returns whether hash is required based on image_type
def is_hash_required(env):
   image_type = env.get('IMAGE_TYPE')
   if (env['MBN_TYPE'] == "elf") or is_elf_image(image_type):
      return 1
   else:
      return 0

# Returns whether image is of ELF type for legacy image types
def is_elf_image(image_type):
   if image_type in ["amss_mbn", "qdsp6sw", "qdsp6fw"]:
      return 1
   else:
      return 0

def concat_files (target, sources):
   if type(sources) is not list:
      sources = [sources]

   target_file = open(target,'wb')

   for fname in sources:
      file = open(fname,'rb')
      while True:
         bin_data = file.read(65536)
         if not bin_data:
            break
         target_file.write(bin_data)
      file.close()
   target_file.close()

