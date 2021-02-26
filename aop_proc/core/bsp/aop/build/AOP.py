#===============================================================================
#
# AOP image build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2016-2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Proprietary and Confidential - Qualcomm Technologies, Inc
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/aop/build/AOP.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#
#===============================================================================
import os
import glob
import imp
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('AOP Image')

def generate(env):
   #------------------------------------------------------------------------------
   # Decide build steps
   #------------------------------------------------------------------------------
   # That is, if the user has asked for a filtered build, don't try to link.
   filter_opt = env.get('FILTER_OPT')
   do_link = False if filter_opt is not None else True
   do_local_files = False if not do_link and not env.FilterMatch(os.getcwd()) else True

   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   aop_libs_path = env.get('INSTALL_LIBPATH')
   aop_libs = env.get('AOP_LIBS')
   aop_objs = env.get('AOP_OBJS')

   aop_units = [aop_libs, aop_objs]

   if do_local_files:
      #---------------------------------------------------------------------------
      # AOP image-level sources
      #---------------------------------------------------------------------------
      env = env.Clone()

      AOP_SRC = "../src"
      env.VariantDir("${BUILD_PATH}", AOP_SRC, duplicate=0)
      env.Append(CFLAGS = '${ARMCC_STDC99_CMD}')
      env.Append(CCFLAGS = '${ARMCC_DOLLAR_CMD}')

      if env['BUILD_VER'].isdigit():
        build_id = str(int(env['BUILD_VER']))
      else:
        build_id = str(zlib.adler32(env['BUILD_VER']) & 0xFFFFFFFF)
      env.Append(CPPDEFINES = [('AOP_BUILD_ID', build_id)])

      if env['MSM_ID'] == '8994':
          env.Append(CPPDEFINES = 'INIT_PRNG_NO_TZ')
          env.Append(CPPDEFINES = 'MSM8994_IMAGE_LAYOUT')
          env.Append(CPPDEFINES = 'MSM8994_PROXY_VOTES')
          env.Append(CPPDEFINES = 'DDR_ABORT_REG_DUMP')
      elif env['MSM_ID'] == '8996':
          env.Append(CPPDEFINES = 'MSM8996_STUBS')
          env.Append(CPPDEFINES = 'MSM8996_IMAGE_LAYOUT')
          env.Append(CPPDEFINES = 'MSM8996_PROXY_VOTES')
          env.Append(CPPDEFINES = 'DISABLE_XPUS')
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
      elif env['MSM_ID'] == '8998':
          env.Append(CPPDEFINES = 'MSM8998_STUBS')
          env.Append(CPPDEFINES = 'MSM8998_IMAGE_LAYOUT')
          env.Append(CPPDEFINES = 'MSM8998_PROXY_VOTES')
          env.Append(CPPDEFINES = 'DISABLE_XPUS')
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
      elif env['MSM_ID'] == '845':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
      elif env['MSM_ID'] == '24':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
          env.Append(CPPDEFINES = 'SDX24_STUBS')
      elif env['MSM_ID'] == '670':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
          env.Append(CPPDEFINES = 'SDM670_STUBS')
      #SDM640_CHANGES
      elif env['MSM_ID'] == '640':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
          env.Append(CPPDEFINES = 'SDM670_STUBS')
      elif env['MSM_ID'] == '855':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
          env.Append(CPPDEFINES = 'SDM855_STUBS')
      elif env['MSM_ID'] == '1000':
          env.Append(CPPDEFINES = 'NO_STACK_CHECK')
          env.Append(CPPDEFINES = 'SDM1000_STUBS')

      #----------------------------------------------------------------------------
      # Dependencies on CoreBSP code
      #----------------------------------------------------------------------------
      CBSP_API = [
         'BOOT',
         'BUSES',
         'DAL',
         'DEBUGTOOLS',
         'MPROC',
         'PMIC',
         'POWER',
         'SYSTEMDRIVERS',
         'DEBUGTRACE',
         'PrngCL',
         'SERVICES',
         'KERNEL',
      ]

      env.RequirePublicApi(CBSP_API)
      env.RequireRestrictedApi(CBSP_API)

      #----------------------------------------------------------------------------
      # Sources, libraries
      #----------------------------------------------------------------------------

      # stubs and other qcore app files
      AOP_SOURCES = [
         'main.c',
         ##'retarget.c',
         ##'exceptions.c',
         ##'coredump.c',
         ##'time_service.c',
         #'xpu_init.c',
         #'image_layout.c',
         ##'stack_protect.c',
         ##'aop_mpu.c',
         #'../target/${MSM_ID}/aop_target_init.c',
         ##'../target/${MSM_ID}/aop_interrupt_table.c',
         ##'../target/${MSM_ID}/exception_isr.c',
         ##'../target/${MSM_ID}/aop_mpu_cfg.c',
      ]
      AOP_SPECIAL_SOURCES = [
         #'datetime.c'
      ]
      AOP_STARTUP_SOURCES = [
         #'startup.s',
      ]
      aop_special_objs = env.Object(AOP_SPECIAL_SOURCES)
      aop_startup_objs = env.Object(AOP_STARTUP_SOURCES)
      for so in aop_special_objs:
         so.set_always_build ()
      aop_objs.extend(env.Object(AOP_SOURCES))
      aop_objs.extend(aop_special_objs)
      if env['MSM_ID'] in ['845', '24', '670', '855', '1000', '640']:   #SDM640_CHANGES
         aop_objs.extend(aop_startup_objs)

   # Add CMM Scripts to the menu
   if env['IMAGE_NAME'] in ['AOP']:
	  SRCPATHSCRIPTS = env['BUILD_ROOT']+'/core/bsp/aop/scripts'
	  env.AddCMMScripts ('AOP', [SRCPATHSCRIPTS], { 'aop_dump.cmm' : 'AOP Core Dump'}, 'Kernel')


   if do_link:
      # Link 855 auto like 855 for now
      if env['MSM_ID'] == '855au':
        env['MSM_ID'] = '855'

     #SDM640_CHANGES
      if env['MSM_ID'] in ['640']:
          target_scl = env.SclBuilder('${TARGET_NAME}', [
          #'../src/aop.scl',
          '${BUILD_ROOT}/core/kernel/rex/target/670/aop.scl',      
          ])
      else:
          target_scl = env.SclBuilder('${TARGET_NAME}', [
          #'../src/aop.scl',
          '${BUILD_ROOT}/core/kernel/rex/target/${MSM_ID}/aop.scl', ])

      aop_elf = env.Program('${TARGET_NAME}', aop_objs, LIBS=aop_libs, LIBPATH=aop_libs_path)
      aop_elf_2 = env.AopElfBuilder ('${TARGET_NAME}' + '_aop', aop_elf)

      env.Append(USE_MBN_TOOLS = 'True')
      env.AddUsesFlags('USES_MBNTOOLS')

      if env['MSM_ID'] in ['8998']:
          env.AddUsesFlags('USES_PLATFORM_IMAGE_INTEGRITY_CHECK')

      if env['MSM_ID'] in ['845', '24', '670', '855', '1000', '640']:   #SDM640_CHANGES
          env.AddUsesFlags('USES_PLATFORM_IMAGE_INTEGRITY_CHECK')

      env.Replace(GLOBAL_DICT = {'IMAGE_KEY_AOP_IMG_DEST_ADDR' : 0xFC100080})
      aop_mbn = env.MbnBuilder(
                            '${TARGET_NAME}',
                            aop_elf_2,
                            IMAGE_TYPE="aop",
                            FLASH_TYPE="sdcc",
                            MBN_TYPE="elf"
                        )

      env.Depends(aop_elf, target_scl)
      env.Clean(aop_elf, env.subst('${TARGET_NAME}.map'))
      env.Clean(aop_elf, env.subst('${TARGET_NAME}.sym'))


      #----------------------------------------------------------------------------
      # Sectools signing
      #----------------------------------------------------------------------------

      dest_msm_id = os.environ['MSM_ID']
      if env['MSM_ID'] in ['845']:
         chipset_version = os.environ['CHIPSET_VERSION']
         vers="0x6000"
         sec_config= '${BUILD_ROOT}/tools/build/scons/sectools/config/integration/secimagev2.xml'

      if env['MSM_ID'] in ['845'] and chipset_version in ['v2']:
         install_base_dir = '${BUILD_ROOT}/build/aop/bf/build/'+dest_msm_id+chipset_version+'/'
         hw_version="0x60000200"

      if env['MSM_ID'] in ['670']:
         chipset_pmic = os.environ['CHIPSET_PMIC']
         install_base_dir = '${BUILD_ROOT}/build/aop/bf/build/'+dest_msm_id+'/'+chipset_pmic+'/'
        
      else:
         install_base_dir = '${BUILD_ROOT}/build/aop/bf/build/'
         hw_version="0x60000100"
         hash_table="sha256"

      if env['MSM_ID'] in ['24']:
         hw_version="0x60020100"
         vers="0x6002"
         sec_config= '${BUILD_ROOT}/tools/build/scons/sectools/config/integration/secimage_eccv3.xml'
         hash_table="sha384"

      if env['MSM_ID'] in ['855','855au']:
         hw_version="0x60030100"
         vers="0x6003"
         sec_config= '${BUILD_ROOT}/tools/build/scons/sectools/config/integration/secimagev3.xml'
         hash_table="sha384"

      if env['MSM_ID'] in ['670', '640']:     #SDM640_CHANGES
         hw_version="0x60040000"
         vers="0x6005 0x6009 0x600A"
         sec_config= '${BUILD_ROOT}/tools/build/scons/sectools/config/integration/secimagev2.xml'
         hash_table="sha256"
      
      if env['MSM_ID'] in ['1000']:
         hw_version="0x60060100"
         vers="0x6006"
         sec_config= '${BUILD_ROOT}/tools/build/scons/sectools/config/integration/secimagev3.xml'
         hash_table="sha384"

      install_unsigned_root = env.SectoolGetUnsignedInstallPath(install_base_dir)
      install_aop_mbn = env.InstallAs(os.path.join(install_unsigned_root, 'aop.mbn'), aop_mbn)

      sectools_signed_mbn = env.SectoolBuilder(
                                    target_base_dir = '${TARGET_NAME}',
                                    source=aop_mbn,
                                    sign_id="aop",
                                    #msmid = env.subst('${MSM_ID}'),
                                    sectools_install_base_dir = install_base_dir,
                                    soc_hw_version=hw_version,
                                    soc_vers=vers,
                                    config=sec_config,
                                    install_file_name = "aop.mbn",
                                    hash_table_algo=hash_table,
                                )

      aop_units = env.Alias('aop_units', [
          aop_elf,
          aop_elf_2,
          aop_mbn,
          install_aop_mbn,
          sectools_signed_mbn,
      ])

   # add aliases
   aliases = env.get('IMAGE_ALIASES')
   for alias in aliases:
      env.Alias(alias, aop_units)
