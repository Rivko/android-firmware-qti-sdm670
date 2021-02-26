#===============================================================================
#
# CoreBSP Modem CPU build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009, 2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/platform_rex.py#1 $
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
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('CoreImage Modem CPU')

def generate(env):
   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   qcoreimg_libs_path = env.get('INSTALL_LIBPATH')
   qcoreimg_libs = env.get('QCOREIMG_LIBS')
   qcoreimg_objs = env.get('QCOREIMG_OBJS')      

   qcoreimg_units = [qcoreimg_libs, qcoreimg_objs]
   
   # filter is not define or filter is current target files only, ok to build misc files
   filter_opt = env.get('FILTER_OPT')
   if (filter_opt is None) or (env.FilterMatch(os.getcwd())):
      #=========================================================================
      # CBSP Image section
      #=========================================================================
   
      # make our clone so we won't mess the lib rules, it shoun't because they 
      # should of clone, but just to be on the safe side.
      env = env.Clone()
      
      #-------------------------------------------------------------------------
      # Source PATH
      #-------------------------------------------------------------------------
      QCOREIMG_SRC = "${COREBSP_ROOT}/bsp/coreimg/src"

      env.VariantDir("${BUILDPATH}", QCOREIMG_SRC, duplicate=0)

      #-------------------------------------------------------------------------
      # External depends within CoreBSP
      #-------------------------------------------------------------------------
      env.RequireExternalApi([
         'BREW',
         'CS',
         'DSM',
         'MODEM_API',
         'MODEM_MCS',
         'MODEM_1X',
         'MODEM_RF',
         'MODEM_PMIC',
         'MODEM_SERVICES',
         'MODEM_NAS',
         'MODEM_ADIE',
         'MODEM_FTM',
         'MODEM_UIM',
         'MODEM_HDR',
         'MODEM_DATA',
         'MODEM_MDSP',
         'MODEM_GERAN',
         'MODEM_WCDMA',
         'MULTIMEDIA_AUDIO',
         'MULTIMEDIA',
         'WCONNECT',
      ])

      #-------------------------------------------------------------------------
      # Internal depends within CoreBSP
      #-------------------------------------------------------------------------
      CBSP_API = [
         'BOOT',
         'BUSES',
         'DAL',
         'DEBUGTOOLS',
         'HAL',      
         'HWENGINES',
         'IODEVICES',
         'MPROC',
         'POWER',
         'SERVICES',
         'STORAGE',
         'SYSTEMDRIVERS',
         'WIREDCONNECTIVITY',
         
         # needs to be last also contains wrong comdef.h
         'KERNEL',
      ]

      env.RequirePublicApi(CBSP_API, area = 'core')
      env.RequireRestrictedApi(CBSP_API)


      SSG_API = [
        'SECUREMSM',
      ]
      env.RequirePublicApi(SSG_API, area = 'ssg')

      
      #=========================================================================
      # Beging building QCOREIMG
      #

      #-------------------------------------------------------------------------
      # quartz config XML files
      #-------------------------------------------------------------------------
      AMSS_RELOC_LC = "build_${TARGET_BLD_KCFG}/amss/bin/${BUILD_ID}.reloc"
      AMSS_RELOC = "${L4_ROOT}/" + AMSS_RELOC_LC
      TCXO_SHUTDOWN_RELOC_LC="0"
      
      quartz_xml = env.QuartzXMLBuilder('quartz_cfg_${TARGET_NAME}',
         ['${BUILD_MS_ROOT}/quartz_cfg.xml'],
         AMSS_RELOC_LC=AMSS_RELOC_LC)
         
      #----------------------------------------------------------------------------
      # Build env scatter load
      #----------------------------------------------------------------------------
      l4_scl_hdr_file = env.L4SclHeaderBuilder("cust_l4_scl_${TARGET_NAME}",
         quartz_xml)

      # '${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_rom.scl'
      # '${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_SMIOnly_rom.scl'
      # '${BUILD_MS_ROOT}/7600A_L4_${TARGET_IMAGE}_rom.scl'
      target_scl = env.SclBuilder('${TARGET_NAME}', [
         #'${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_SMIOnly_rom',
		 '${BUILD_MS_ROOT}/8960_REX_RIVA_DDROnly',
         #l4_scl_hdr_file
      ])
      
      qcoreimg_units += target_scl
      
      #----------------------------------------------------------------------------
      # Sources, libraries
      #----------------------------------------------------------------------------

      # stubs and other qcore app files
      QCOREIMG_SOURCES = [
         '${BUILDPATH}/other_stubs.c', 
         #'${BUILDPATH}/securemsm_stubs.c',         
      ]
      
      qcoreimg_objs.extend(env.Object(QCOREIMG_SOURCES))      

   # the following steps can only be performed if there is no filters in effect
   if filter_opt is None:
      #-------------------------------------------------------------------------
      # Build env QCOREIMG
      #-------------------------------------------------------------------------
      qcoreimg_elf = env.Program("${TARGET_NAME}", qcoreimg_objs, LIBS=qcoreimg_libs)
      env.Depends(qcoreimg_elf, target_scl)
      
      qcoreimg_map = env.subst('${TARGET_NAME}.map')
      qcoreimg_sym = env.subst('${TARGET_NAME}.sym')
      
      env.Clean(qcoreimg_elf, qcoreimg_map)
      env.Clean(qcoreimg_elf, qcoreimg_sym)

      # copy elf and reloc to needed locations for AMSS tools to load on target
      install_target_elf = env.InstallAs(
         '${BUILD_MS_ROOT}/8960_RIVA.elf', qcoreimg_elf)
      install_target_reloc = env.InstallAs(AMSS_RELOC, install_target_elf)      
      
      #-------------------------------------------------------------------------
      # Build env pbn files
      #-------------------------------------------------------------------------
      target_pbn = env.PbnBuilder('${TARGET_NAME}', [quartz_xml, install_target_reloc])

      # Copy qcoreimg_M.pbn to platform\l4\build_M\bootimg.pbn for RUMI */
      install_target_pbn = env.InstallAs(
         '${L4_ROOT}/build_${TARGET_BLD_KCFG}/bootimg.pbn', target_pbn)

      #-------------------------------------------------------------------------
      # Build env mbn files
      #-------------------------------------------------------------------------
      target_mbn = env.MbnBuilder('${TARGET_NAME}', qcoreimg_elf, IMAGE_TYPE="amss_mbn",
         FLASH_TYPE=env['TARGET_FLASH_TYPE'])

      install_target_mbn = env.InstallAs('${MBN_ROOT}/amss.mbn', target_mbn)

      #============================================================================
      # Define targets needed QCOREIMG
      #
      qcoreimg_units = env.Alias ('arm11_qcoreimg_units', [
         install_target_elf,
        # install_target_pbn,
         install_target_mbn,
      ])

   #print env.Dump()

   # add aliases
   aliases = env.get('IMAGE_ALIASES')   
   for alias in aliases:
      env.Alias(alias, qcoreimg_units)
