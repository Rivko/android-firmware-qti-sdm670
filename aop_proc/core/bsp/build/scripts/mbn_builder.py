#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create MBN file
#
# Copyright 2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/build/scripts/mbn_builder.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 08/30/12   kedara    Add virtual block suppport
# 06/22/12   kedara    Removed preamble generation dependency on USES_SAHARA_BOOT
# 04/11/12   dh        Only prepend preamble for Flash-type = Nand for secboot as well
# 02/24/12   dh        Add ssd side effect file names
# 11/16/11   plc       Only prepend preamble for Flash-type = Nand
# 10/03/11   dxiang    Clean up 
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
QDST_ROOT = "${BUILD_ROOT}/core/storage/tools/qdst"

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('mbn_builder')

def generate(env):
   # Add MBN Tools to environment
   env.Tool('mbn_tools', toolpath = ['${BUILD_ROOT}/tools/build/scons/sectools/sectools/features/isc/parsegen'])
   mbn_generate(env)
   modify_elf_flags_generate(env)

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
   env.ModifyElfFlags(target_full, source_scl_full)

   return None

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

   enc_mbn_act = env.GetBuilderAction(enc_mbn_builder)
   enc_mbn_bld = env.Builder(action = enc_mbn_act,
                             suffix = '.enc')

   env.Append(BUILDERS = {'EncMbnBuilder' : enc_mbn_bld})


   mbn_dummy_act = env.GetBuilderAction(mbn_dummy_file_gen)
   mbn_dummy_bld = env.Builder(action = mbn_dummy_act, suffix = '.mbn')

   env.Append(BUILDERS = {'MbnDummyBuilder' : mbn_dummy_bld})

#----------------------------------------------------------------------------
# MBN builder function
# go/mbnbuilder
#----------------------------------------------------------------------------
def mbn_builder(env, target, source, **kwargs):
   target_base = os.path.splitext(str(target))[0]
   target_full = str(target)

   #----------------------------------------------------------------------------
   # Parse argument list 
   #----------------------------------------------------------------------------
   if not kwargs.has_key('IMAGE_TYPE'):
      raise RuntimeError, "IMAGE_TYPE is not defined"
 
   env.Replace(IMAGE_TYPE = kwargs.get('IMAGE_TYPE'))

   #----------------------------------------------------------------------------
   # MBN Builder supported features
   #----------------------------------------------------------------------------
   if kwargs.has_key('ENABLE_ENCRYPT'):
      env.Replace(ENABLE_ENCRYPT = kwargs.get('ENABLE_ENCRYPT'))
   elif 'ENABLE_ENCRYPT' in env:
      env.Replace(ENABLE_ENCRYPT = env['ENABLE_ENCRYPT'])
   else:
      env.Replace(ENABLE_ENCRYPT = False) 

   if kwargs.has_key('ENABLE_RELOCATABLE'):
      env.Replace(ENABLE_RELOCATABLE = kwargs.get('ENABLE_RELOCATABLE'))
   elif 'ENABLE_RELOCATABLE' in env:
      env.Replace(ENABLE_RELOCATABLE = env['ENABLE_RELOCATABLE'])
   else:
      env.Replace(ENABLE_RELOCATABLE = False)

   if kwargs.has_key('ENABLE_VIRTUAL_BLK'):
      env.Replace(ENABLE_VIRTUAL_BLK = kwargs.get('ENABLE_VIRTUAL_BLK'))
   elif 'ENABLE_VIRTUAL_BLK' in env:
      env.Replace(ENABLE_VIRTUAL_BLK = env['ENABLE_VIRTUAL_BLK'])
   else:
      env.Replace(ENABLE_VIRTUAL_BLK = False)

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
      # Assign generic dictionary for builders to use
      env.Replace(GEN_DICT = gen_dict)
   else:
      raise RuntimeError, "MBNTools is not enabled"

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
      mbn_stage1 = env.Stage1MbnBuilder( target_nonsec, source ) 
     
      mbn_stage_final = env.Stage2MbnBuilder( target, mbn_stage1 )

      #Delete intermediate dummy target target
      if(os.path.exists(str(env.subst(target)))):
         if (os.path.getsize(str(env.subst(target))) == 0):
             os.remove(str(env.subst(target)))
   else:
      mbn_stage_final = env.Stage1MbnBuilder( target, source )

   #----------------------------------------------------------------------------
   # Place the Image to build\ms\bin folder
   #----------------------------------------------------------------------------
   #Note: In case of SecureBuild, stage1 will be placing size zero Image as a
   #sideeffect for all the images and later stage2 it will be updating
   #that to nonzero size signed image.
   # Always install standard *.mbn target
   
   # This should soon be deprecated and final installation should be done
   # by calling script. This builder(s) should only produce targets in the 
   # image folder the rest should be taken care else where (upper scripts).
   # As a start and for BACKWARDS compatibility allow upper script 
   # to override destination.
   if 'MBN_FILE' in env:
      install_mbn_final = env.InstallAs('${MBN_FILE}.mbn', mbn_stage_final)   
   else:
      install_mbn_final = env.InstallAs('${MBN_ROOT}/' + env['IMAGE_TYPE'] + '.mbn', mbn_stage_final)

   # If Encryption Feature is ON, use Encryption builder to set up the encryped target.
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      target_full_enc = target_base + ".enc" 
      enc_stage_final = env.EncMbnBuilder(target_full_enc, mbn_stage_final)
      if 'MBN_FILE' in env:      
         install_enc_final = env.InstallAs('${MBN_FILE}.enc', enc_stage_final )
      else:         
         install_enc_final = env.InstallAs('${MBN_ROOT}/' + env['IMAGE_TYPE'] + '.enc', enc_stage_final )
         
   # Return two builder targets if Encryption Feature is ON
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      return [install_mbn_final, install_enc_final]
   else:
      return install_mbn_final

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
      env.Clean(target, target_base_str +'_enc_no_xml.mbn')
      env.Clean(target, target_base_str +'_enc_md.xml')
      env.Clean(target, target_base_str +'_enc.mbn')
      env.Clean(target, target_base_str +'_combined_hash_xml.mbn')
      env.Clean(target, target_base_str +'_pre_encrypt.pbn')
      
   if env['MBN_TYPE'] == 'bin':
      env.Clean(target, target_base_str +'.hd')
      env.Clean(target, target_base_str +'_preamble.mbn')

   elif env['MBN_TYPE'] == 'elf':
      env.Clean(target, target_base_str +'.hash')
      env.Clean(target, target_base_str +'_hash.hd')
      env.Clean(target, target_base_str +'_phdr.pbn')
      env.Clean(target, target_base_str +'_combined_hash.mbn')
      
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      print "Getting SSD side effect file"
      env.SideEffect(env.GetSSDSideEffectFileName(), target)

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
   target_bin_base = env.subst("${MBN_ROOT}/")
   target_bin_dir = target_bin_dir_name(env)

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
   target_full_enc = target_base + "_enc.mbn"

   #-------------------------------------------------------------------------
   # Binary MBN generation
   #-------------------------------------------------------------------------
   if env['MBN_TYPE'] == 'bin':
     
      target_full_enc_no_xml = target_base + "_enc_no_xml.mbn"
      target_enc_xml = target_base + "_enc_md.xml"
      
      # Default parameter initializations 
      requires_preamble = False
      preamble_file_name = target_base + "_preamble.mbn"
      header_format = 'reg' 

      # Custom image parameter assignments
      if env.subst("$IMAGE_TYPE") == "sbl1":
         header_format = 'sbl'
         if env['GEN_DICT']['IMAGE_KEY_FLASH_TYPE'] == "nand":   
             requires_preamble = True
         else:
             requires_preamble = False

      if env.subst("$IMAGE_TYPE") == "mba":
         header_format = 'sbl' 

      elif env.subst("$IMAGE_TYPE") == "emmcbld":
         requires_preamble = False
         header_format = 'sbl' 

      # Generating header for the RAWIMAGE
      rv = env.ImageHeader(env['GEN_DICT'], source_full, target_base + ".hd", 
        image_header_secflag, header_format = header_format, 
        requires_preamble = requires_preamble, preamble_file_name = preamble_file_name)

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
  
            if (env.subst("$IMAGE_TYPE") == "sbl1") and (env['GEN_DICT']['IMAGE_KEY_FLASH_TYPE'] == "nand"):
               if ('ENABLE_VIRTUAL_BLK' in env) and (env['ENABLE_VIRTUAL_BLK'] is True):
                  env.InsertSBLMagicCookie(target_full)

            if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
               # generate xml header
               env.GenerateMetaData(target_enc_xml,True)
               # encrypt the full mbn image
               env.EncryptMBN(target_full,target_full_enc_no_xml)
               # concat xml header in front of encrypted image
               concat_files (target_full_enc, [target_enc_xml, target_full_enc_no_xml])		
                
   #-------------------------------------------------------------------------
   # ELF MBN generation 
   #-------------------------------------------------------------------------
   elif env['MBN_TYPE'] == 'elf':

      target_hash = target_base + ".hash"
      target_hash_hd = target_base + "_hash.hd"
      target_cert_chain = target_base + "_hash_cert_chain.mbn"
      target_hash_full = target_base + "_hash_sec.mbn"
      target_phdr_elf = target_base + "_phdr.pbn"
      target_add_hash_elf = target_phdr_elf
      target_pre_encrypt_elf = target_base + "_pre_encrypt.pbn"
      target_encrypt_xml = target_base + "_enc_md.xml"
 
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

      rv = env.PBootGenElf(source_full, target_hash, 
                          elf_out_file_name = target_phdr_elf,
                          secure_type = image_header_secflag,
                          hash_seg_max_size = hash_seg_size,
                          last_phys_addr = last_phys_addr,        
                          append_xml_hdr = encrypted_image)        
      if not rv:
         rv = env.ImageHeader(env['GEN_DICT'], target_hash, target_hash_hd, 
                              image_header_secflag, elf_file_name = source_full)

      if 'USES_SECBOOT' in env:
         target_nonsec = target_base + ".mbn"
       
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
         target_nonsec_xml = target_base + "_combined_hash_xml.mbn"

         if not rv:
            files_to_cat_in_order = [target_hash_hd, target_hash]
            concat_files (target_nonsec, files_to_cat_in_order)

            # If modem encryption is ON, generate XML header and append to hash segment
            if encrypted_image is True: 
               env.GenerateMetaData(target_encrypt_xml)
               files_to_cat_in_order.append(target_encrypt_xml) 
               concat_files (target_nonsec_xml, files_to_cat_in_order)

            # Add the hash segment into the ELF 
            env.PBootAddHash(target_add_hash_elf, target_nonsec, target_full)

            # If Encryption Feature is ON, add the hash segment with XML information into the ELF and encrypt 
            if encrypted_image is True:
               env.PBootAddHash(target_add_hash_elf, target_nonsec_xml, target_pre_encrypt_elf)
               env.EncryptElfSegments(target_pre_encrypt_elf, target_full_enc)
 
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

   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      env.Clean(target, target_base_str +'_enc_no_xml.mbn')
      env.Clean(target, target_base_str +'_enc_md.xml')
      env.Clean(target, target_base_str +'_enc.mbn')
      env.Clean(target, target_base_str +'_pre_encrypt.pbn')
      env.Clean(target, target_base_str +'_xml_hash_sec.mbn')
  
   if env['MBN_TYPE'] == 'bin':
      env.Clean(target, target_base_str +'_cert_chain.mbn')
      env.Clean(target, target_base_str +'_combined.mbn')

   elif env['MBN_TYPE'] == 'elf':
      env.Clean(target, target_base_str +'_hash_sec.mbn')
      env.Clean(target, target_base_str +'_hash_cert_chain.mbn')
      
   if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
      print "Getting SSD side effect file"
      env.SideEffect(env.GetSSDSideEffectFileName(), target)
         
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
   target_bin_base = env.subst("${MBN_ROOT}/")
   target_bin_dir = target_bin_dir_name(env)
   target_full_enc = target_base + "_enc.mbn"  

   target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
      str(os.path.split(target_base)[1]) + "_nonsec.mbn"

   if (not is_hash_required(env)):
      target_nonsec = target_base + "_nonsec.mbn"
   else:
      target_nonsec = target_base + "_hash_nonsec.mbn"

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
      target_zip_base = os.path.splitext(str(target_zip))[0]
      signature = target_zip_base + "-signature.bin"
      attestation_cert = target_zip_base + "-attestation_cert.cer"
      attestation_ca_cert = target_zip_base + "-attestation_ca_cert.cer"
      root_cert = target_zip_base + "-root_cert.cer"
      target_cert_chain = target_base + "_cert_chain.mbn"

   #-------------------------------------------------------------------------
   # Binary MBN generation
   #-------------------------------------------------------------------------				  
   if env['MBN_TYPE'] == 'bin':
      # Default parameter initializations 
      requires_preamble = False
      target_combined = target_base + "_combined.mbn"
      preamble_file = target_base + "_nonsec_preamble.mbn"
      
      target_full_enc_no_xml = target_base + "_enc_no_xml.mbn"
      target_enc_xml = target_base + "_enc_md.xml"
      
      files_to_cat_in_order = [target_nonsec]

      # Custom image parameter assignments
      if env.subst("$IMAGE_TYPE") == "sbl1":
         if (not ('USES_SAHARA_BOOT' in env)):      
            if env['GEN_DICT']['IMAGE_KEY_FLASH_TYPE'] == "nand":   
               requires_preamble = True
            else:
               requires_preamble = False

      if source_zip == "":
         # Once stage1 is completed and no cert is found then create a size zero file
         open(target_full,"wb")

         # If Encryption Feature is ON, accomodate for two targets, create two size zero files 
         if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
            open(target_full_enc,"wb")
      else:
         # If image requires a preamble, prepend to combined image 
         if requires_preamble == True:
            files_to_cat_in_order.insert(0,preamble_file)

         concat_files (target_combined, files_to_cat_in_order)

         Unzip_Files(target_zip,source_path)
          
         if (do_files_exist([target_combined, signature])):
            rv = env.ImageAuth(target_combined, signature, attestation_cert, \
                                attestation_ca_cert, root_cert, target_cert_chain, \
                                target_full)
                            
            if (env.subst("$IMAGE_TYPE") == "sbl1") and (env['GEN_DICT']['IMAGE_KEY_FLASH_TYPE'] == "nand"):
               if ('ENABLE_VIRTUAL_BLK' in env) and (env['ENABLE_VIRTUAL_BLK'] is True):
                  env.InsertSBLMagicCookie(target_full)

            if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
               # generate xml header
               env.GenerateMetaData(target_enc_xml,True)
               # encrypt the full mbn image
               env.EncryptMBN(target_full,target_full_enc_no_xml)
               # concat xml header in front of encrypted image
               concat_files (target_full_enc, [target_enc_xml, target_full_enc_no_xml])		
         else:
            raise RuntimeError, "Missing file(s) required for signing.\n"	
        
  #-------------------------------------------------------------------------
   # ELF MBN generation 
   #-------------------------------------------------------------------------
   elif env['MBN_TYPE'] == 'elf':

      target_cert_chain = target_base + "_hash_cert_chain.mbn"
      target_hash_full = target_base + "_hash_sec.mbn"
      target_pbn = target_base + ".pbn"
      target_phdr_elf = target_base + "_hash_nonsec_phdr.pbn"
      target_add_hash_elf = target_phdr_elf
      target_pre_encrypt_elf = target_base + "_pre_encrypt.pbn"
      target_encrypt_xml = target_base + "_enc_md.xml"
      target_hash_full_xml = target_base + "_xml_hash_sec.mbn"

      if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
         encrypted_image = True 
      else:
         encrypted_image = False

      target_bin_nonsec = target_bin_base + target_bin_dir + "/" + \
         str(os.path.split(target_nonsec)[1])
      
      if source_zip == "":
         # Once stage1 is completed and no cert is found then create a size zero file
         open(target_full,"wb")

         # If Encryption Feature is ON, accomodate for two targets, create two size zero files 
         if ('USES_ENCRYPT_MBN' in env) and (env['ENABLE_ENCRYPT'] is True):
            open(target_full_enc,"wb")

      else:
         Unzip_Files(target_zip,source_path)
      
         if (do_files_exist([target_nonsec, signature])):
            # Appends signature and certificates to the hash segment
            env.ImageAuth(target_nonsec, signature, attestation_cert, \
                        attestation_ca_cert, root_cert, target_cert_chain, \
                        target_hash_full) 

            # Insert full hash segment into the ELF 
            env.PBootAddHash(target_add_hash_elf, target_hash_full, target_full)

            # If modem encryption is ON, generate XML header and append to hash segment
            if encrypted_image is True: 
               env.GenerateMetaData(target_encrypt_xml)
               concat_files(target_hash_full_xml, [target_hash_full, target_encrypt_xml])

               # Add the hash segment into the ELF and encrypt if necessary 
               env.PBootAddHash(target_add_hash_elf, target_hash_full_xml, target_pre_encrypt_elf)
               env.EncryptElfSegments(target_pre_encrypt_elf, target_full_enc)
         else:
            raise RuntimeError, "Missing file(s) required for signing.\n"
          
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

def enc_mbn_builder(target, source, env):
   target_base = os.path.splitext(str(target[0]))[0]
   target_full_enc = target_base + "_enc.mbn" 
   target_install_inc = target_base + ".enc"
   shutil.copy(target_full_enc, target_install_inc)
   return None

#----------------------------------------------------------------------------
# Target_bin_dir
# This will return the temporary directory name used for the image signing
#----------------------------------------------------------------------------
def target_bin_dir_name(env):
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
   if (env['MBN_TYPE'] == "elf"):
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
