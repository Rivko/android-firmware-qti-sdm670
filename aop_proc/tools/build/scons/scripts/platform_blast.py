#===============================================================================
#
# CoreBSP Appication CPU build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/platform_blast.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
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
import glob
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('CoreImage Application CPU')

def generate(env):
   #------------------------------------------------------------------------------
   # Decide build steps
   #------------------------------------------------------------------------------
   # regular build steps (no filter) is build everything, once a user start 
   # using filters we have to make decisions depending on user input.
   # The LoadAUSoftwareUnits function will take care of filtering subsystem, units, 
   # etc. This is to take care of what steps to enable disable from the top level
   # script, such as building files specify in this script i.e. quatz, stubs, etc.
   do_local_files = True
   do_link = True
   do_post_link = True

   # get user input from command line
   filter_opt = env.get('FILTER_OPT')
   elf_file = ARGUMENTS.get('process_elf', None)
   
   # make decisions
   if filter_opt is not None:
      do_link = False
      
      if env.FilterMatch(os.getcwd()):
         if elf_file is None:
            do_post_link = False
         else:
            qcoreimg_elf = File(elf_file)
      else:
         do_local_files = False
         do_post_link = False

   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   qcoreimg_libs_path = env.get('INSTALL_LIBPATH')
   qcoreimg_libs = env.get('QCOREIMG_LIBS')
   qcoreimg_objs = env.get('QCOREIMG_OBJS')      

   qcoreimg_units = [qcoreimg_libs, qcoreimg_objs]
   
   if do_local_files:
      #=========================================================================
      # CBSP Image section
      #=========================================================================
   
      # make our clone so we won't mess the lib rules, it shoun't because they 
      # should of clone, but just to be on the safe side.
      env = env.Clone()
	  
      if 'USES_LCS_FILE' in env:
         # Dynamically load lcs_builder
         env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/lcs_builder.py")
         
         # Location and name of lcs file
         lcs_file = env.LcsBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}.elf.lcs", 
            "${BUILD_MS_ROOT}/${BUILD_ASIC}_QuRT_${TARGET_IMAGE}.lcs")

         qcoreimg_units += lcs_file
		 
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
         'MODEM_API',
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
         'SECUREMSM',
         'SERVICES',
         'STORAGE',
         'SYSTEMDRIVERS',
         'WIREDCONNECTIVITY',
         
         # needs to be last also contains wrong comdef.h
         'KERNEL',
      ]

      env.RequirePublicApi(CBSP_API)
      env.RequireRestrictedApi(CBSP_API)
      
      #=========================================================================
      # Beging building QCOREIMG
      #

      #-------------------------------------------------------------------------
      # quartz config XML files
      #-------------------------------------------------------------------------
      AMSS_RELOC_LC = "M9X00ASCAQMAZQ1234.reloc"
      AMSS_RELOC = "${L4_ROOT}/" + AMSS_RELOC_LC
      TCXO_SHUTDOWN_RELOC_LC="0"
      
      quartz_xml = [File(env.subst("${BUILD_ROOT}/build/ms/qdsp6_surf.xml"))]

      #-------------------------------------------------------------------------
      # Build env scatter load
      #-------------------------------------------------------------------------

      qcoreimg_units += quartz_xml
      
      #-------------------------------------------------------------------------
      # Sources, libraries
      #-------------------------------------------------------------------------

   if do_link:
      #-------------------------------------------------------------------------
      # Build env QCOREIMG
      #-------------------------------------------------------------------------
      qcoreimg_elf = env.Program("${SHORT_BUILDPATH}/${TARGET_NAME}", qcoreimg_objs, LIBS=qcoreimg_libs)
      
      qcoreimg_map = env.subst("${SHORT_BUILDPATH}/${TARGET_NAME}.map")
      qcoreimg_sym = env.subst("${SHORT_BUILDPATH}/${TARGET_NAME}.sym")
      
      env.Clean(qcoreimg_elf, qcoreimg_map)
      env.Clean(qcoreimg_elf, qcoreimg_sym)

   if do_post_link:  
      #-------------------------------------------------------------------------
      # Add a dummy segment for dynamic footprints
      #-------------------------------------------------------------------------
      if 'USES_DUMMY_SEG' in env:
         if 'AddDummySegBuilder' not in env.get('BUILDERS'):
            env.LoadToolScript("${COREBSP_ROOT}/bsp/build/scripts/kernel_builders.py")
            
         qcoreimg_elf = env.AddDummySegBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}_patch",
            [qcoreimg_elf, "${BUILD_MS_ROOT}/cust_config.xml"])
			
      #-------------------------------------------------------------------------
      # Install ELF, reloc files
      #-------------------------------------------------------------------------
      if 'USES_QSHRINK2' in env:
         env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/../../../qshrink/src/Qshrink20.py")

         qshrink20_elf = env.Qshrink20(
            "${SHORT_BUILDPATH}/${TARGET_NAME}_qsr2",
            [qcoreimg_elf, "${BUILD_SCRIPTS_ROOT}/../../../qshrink/msg_hash.txt"])
			
         # copy elf and reloc to needed locations for AMSS tools to load on target
         install_target_elf = env.InstallAs(
            "${BUILD_MS_ROOT}/M${BUILD_ASIC}${BUILD_ID}${BUILD_VER}.elf", qshrink20_elf)
      else:
         # copy elf and reloc to needed locations for AMSS tools to load on target
         install_target_elf = env.InstallAs(
            "${BUILD_MS_ROOT}/M${BUILD_ASIC}${BUILD_ID}${BUILD_VER}.elf", qcoreimg_elf)

      #-------------------------------------------------------------------------
      # Build env reloc files
      #-------------------------------------------------------------------------
      if 'USES_BLAST' in env:
         #-------------------------------------------------------------------------
         # Build env mbn files
         #-------------------------------------------------------------------------
         target_mbn = env.MbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", install_target_elf, IMAGE_TYPE="qdsp6sw",
            FLASH_TYPE=env['TARGET_FLASH_TYPE'])

         if 'USES_PIL' in env:
            env.PrintInfo("Setting up Pil Splitter Builder")
            #-------------------------------------------------------------------------
            # Split mbn file for PIL
            #-------------------------------------------------------------------------
            env.LoadToolScript('pil_splitter_builder', toolpath = ['${BUILD_ROOT}/core/bsp/build/scripts'])
            split_files = env.PilSplitterBuilder('${BUILD_ROOT}/build/ms/bin/PIL_IMAGES/SPLITBINS_${QC_SHORT_BUILDPATH}/${PIL_SPLITTER_PREFIX}.mdt', target_mbn)
            qcoreimg_units += split_files

         #=========================================================================
         # Define targets needed QCOREIMG
         #
         qcoreimg_units += [
            install_target_elf,
            target_mbn,
         ]
         
      else:
         kernel_reloc = env.Q6_KernelRelocBuilder('ig_server', '${QDSP6_LIBDIR}/ig_server')
         install_kernel_reloc = env.Install('${BUILD_MS_ROOT}/', kernel_reloc)
         
         amss_reloc =  env.Q6_AmssRelocBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", qcoreimg_elf)
         
         # *** hack for now, we need to generate the xml file....
         #install_target_reloc = env.InstallAs('${BUILD_MS_ROOT}/'+ AMSS_RELOC_LC, amss_reloc)
         
         #<program name="AMSS" priority="-1" file="M9X00ASCAQMAFQ1010.reloc" >
         quartz_xml_fname = str(quartz_xml[0])
         quartz_xml_file = open(quartz_xml_fname,'r')
         quartz_xml_data = quartz_xml_file.read()
         quartz_xml_file.close()

         #print quartz_xml_data
         TARGET_FILENAME = 'M'+env.get('BUILD_ASIC')+env.get('BUILD_ID')+env.get('BUILD_VER')+'.reloc'
         searchre = re.compile('M9(.*).reloc')
         
         quartz_xml_data = searchre.sub(TARGET_FILENAME, quartz_xml_data)

         quartz_xml_file = open(quartz_xml_fname,'w')
         quartz_xml_file.write(quartz_xml_data)
         quartz_xml_file.close()

         re_search = re.search('<program name\s*=\s*"AMSS".*file\s*=\s*\"(.*)\"\s*>',quartz_xml_data)
         #import pdb; pdb.set_trace()
         if re_search is not None:
            AMSS_RELOC_LC = re_search.group(1)
            install_target_reloc = env.InstallAs('${BUILD_MS_ROOT}/'+ AMSS_RELOC_LC, amss_reloc)
         else:
            env.PrintError("No program AMSS found in [%s]." % quartz_xml_fname)
            raise RuntimeError, 'No program AMSS found in source xml'

         #-------------------------------------------------------------------------
         # Build env pbn files
         #-------------------------------------------------------------------------
         target_pbn = env.PbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", [quartz_xml, install_target_reloc])

         #-------------------------------------------------------------------------
         # Build env mbn files
         #-------------------------------------------------------------------------
         target_mbn = env.MbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", target_pbn, IMAGE_TYPE="qdsp6sw",
            FLASH_TYPE=env['TARGET_FLASH_TYPE'])

         #=========================================================================
         # Define targets needed QCOREIMG
         #
         qcoreimg_units += [
            install_target_elf,
            install_kernel_reloc,
            install_target_reloc,
            target_pbn,
            target_mbn,
         ]

   #print env.Dump()

   # add aliases
   aliases = env.get('IMAGE_ALIASES')
   for alias in aliases:
      env.Alias(alias, qcoreimg_units)
