'''
   CoreBSP Appication CPU build script
'''
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
#  $Header: //components/rel/scons.qc/1.1/scripts/platform_blast.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
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
    '''
       exists
    '''
    return env.Detect('CoreImage Application CPU')

def generate(env):
    '''
       generate
    '''
    #---------------------------------------------------------------------------
    # Decide build steps
    #---------------------------------------------------------------------------
    # regular build steps (no filter) is build everything, once a user start 
    # using filters we have to make decisions depending on user input.
    # The LoadAUSoftwareUnits function will take care of filtering subsystem, 
    # units, etc. This is to take care of what steps to enable disable from the 
    # top level script, such as building files specify in this script
    # i.e. quatz, stubs, etc.
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
                image_elf = env.File(elf_file)
        else:
            do_local_files = False
            do_post_link = False

    #-------------------------------------------------------------------------
    # Libs/Objs
    #-------------------------------------------------------------------------
    #image_libs_path = env.get('INSTALL_LIBPATH')
    image_libs = env.get('QCOREIMG_LIBS')
    image_objs = env.get('QCOREIMG_OBJS')      

    image_units = [image_libs, image_objs]
   
    if do_local_files:
        #=======================================================================
        # Image section
        #=======================================================================
   
        # make our clone so we won't mess the lib rules, it shoun't because they
        # should of clone, but just to be on the safe side.
        env = env.Clone()
      

        if 'USES_LCS_FILE' in env:
            # Dynamically load lcs_builder
            env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/lcs_builder.py")
         
            # Location and name of lcs file
            lcs_file = env.LcsBuilder \
                ("${SHORT_BUILDPATH}/${TARGET_NAME}.elf.lcs", 
                 "${BUILD_MS_ROOT}/${BUILD_ASIC}_QuRT_${TARGET_IMAGE}.lcs")

            image_units += lcs_file
         
    if do_link:
        #-----------------------------------------------------------------------
        # Build env QCOREIMG
        #-----------------------------------------------------------------------
        image_elf = env.Program("${SHORT_BUILDPATH}/${TARGET_NAME}", \
                                 image_objs, LIBS=image_libs)
      
        image_map = env.subst("${SHORT_BUILDPATH}/${TARGET_NAME}.map")
        image_sym = env.subst("${SHORT_BUILDPATH}/${TARGET_NAME}.sym")
      
        env.Clean(image_elf, image_map)
        env.Clean(image_elf, image_sym)

        if 'USES_LCS_FILE' in env:
            # Add lcs dependency to elf file
            env.Depends(image_elf, lcs_file)

    if do_post_link:
        #-----------------------------------------------------------------------
        # Add a dummy segment for dynamic footprints
        #-----------------------------------------------------------------------
        if 'USES_DUMMY_SEG' in env:
            if 'AddDummySegBuilder' not in env.get('BUILDERS'):
                env.LoadToolScript \
                    ("${COREBSP_ROOT}/bsp/build/scripts/kernel_builders.py")
            
            image_elf = env.AddDummySegBuilder \
                ("${SHORT_BUILDPATH}/${TARGET_NAME}_patch", \
                 [image_elf, "${BUILD_MS_ROOT}/cust_config.xml"])
         
        #-----------------------------------------------------------------------
        # Install ELF, reloc files
        #-----------------------------------------------------------------------
        if 'USES_QSHRINK2' in env:
            env.LoadToolScript \
                ("${BUILD_SCRIPTS_ROOT}/../../../qshrink/src/Qshrink20.py")

            qshrink20_elf = env.Qshrink20 \
                ("${SHORT_BUILDPATH}/${TARGET_NAME}_qsr2", [image_elf, \
                 "${BUILD_SCRIPTS_ROOT}/../../../qshrink/msg_hash.txt"])

            # copy elf and reloc to needed locations
            # for AMSS tools to load on target
            install_target_elf = env.InstallAs \
                ("${BUILD_MS_ROOT}/M${BUILD_ASIC}${BUILD_ID}${BUILD_VER}.elf", \
       	         qshrink20_elf)
        else:
            # copy elf and reloc to needed locations
            # for AMSS tools to load on target
            install_target_elf = env.InstallAs \
                ("${BUILD_MS_ROOT}/M${BUILD_ASIC}${BUILD_ID}${BUILD_VER}.elf", \
                 image_elf)

        #-----------------------------------------------------------------------
        # Build env mbn files
        #-----------------------------------------------------------------------
        target_mbn = env.MbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", \
            install_target_elf, IMAGE_TYPE='qdsp6sw', \
            FLASH_TYPE=env['TARGET_FLASH_TYPE'])

        if 'USES_PIL' in env:
            #-------------------------------------------------------------------
            # Split mbn file for PIL
            #-------------------------------------------------------------------
            env.LoadToolScript \
                ("${BUILD_ROOT}/core/bsp/build/scripts/pil_splitter_builder.py")

            # Commenting out some code until we have a chance to have all tools 
            # pull files from build\bsp\...
            # split_files = env.PilSplitterBuilder \
            #    ("${SHORT_BUILDPATH}/SPLITBINS_${QC_SHORT_BUILDPATH}/" \
            #     "${PIL_SPLITTER_PREFIX}.mdt", target_mbn)
            # install_pil = env.Install \
            #     ("${BUILD_ROOT}/build/ms/bin/PIL_IMAGES", split_files[0].dir)
            # image_units += install_pil

            split_files = env.PilSplitterBuilder('${BUILD_ROOT}/build/ms/bin/' \
                '${PIL_BUILDPATH}/${PIL_SPLITTER_PREFIX}.mdt', target_mbn)
            image_units += split_files

        #=======================================================================
        # Define targets needed QCOREIMG
        #
        image_units += [
            install_target_elf,
            target_mbn,
        ]
         

    #print env.Dump()

    # add aliases
    aliases = env.get('IMAGE_ALIASES')
    for alias in aliases:
        env.Alias(alias, image_units)
