'''
   CoreBSP Modem CPU build script
'''
#===============================================================================
#
# CoreBSP Modem CPU build script
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
#  $Header: //components/rel/scons.qc/1.1/scripts/platform_l4.py#3 $
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
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('CoreImage Modem CPU')

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
                image_elf = File(elf_file)
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
        # CBSP Image section
        #=======================================================================
   
        # make our clone so we won't mess the lib rules, it shoun't because they
        # should of clone, but just to be on the safe side.
        env = env.Clone()
      
        #-----------------------------------------------------------------------
        # Source PATH
        #-----------------------------------------------------------------------
        qcoreimg_src = "${COREBSP_ROOT}/bsp/coreimg/src"

        env.VariantDir("${SHORT_BUILDPATH}", qcoreimg_src, duplicate=0)

        #-----------------------------------------------------------------------
        # External depends within CoreBSP
        #-----------------------------------------------------------------------
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

        #-----------------------------------------------------------------------
        # Internal depends within CoreBSP
        #-----------------------------------------------------------------------
        cbsp_api = [
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

        env.RequirePublicApi(cbsp_api)
        env.RequireRestrictedApi(cbsp_api)

        #=======================================================================
        # Beging building QCOREIMG
        #

        #-----------------------------------------------------------------------
        # quartz config XML files
        #-----------------------------------------------------------------------
        amss_reloc_lc = "build_${TARGET_BLD_KCFG}/amss/bin/${BUILD_ID}.reloc"
        amss_reloc = "${L4_ROOT}/" + amss_reloc_lc
        # tcxo_shutdown_reloc_lc = "0"

        if env.has_key('USES_L4_3X'):
            quartz_xml = env.QuartzXMLBuilder \
                ("${SHORT_BUILDPATH}/quartz_cfg_${TARGET_NAME}", \
                 "${BUILD_MS_ROOT}/quartz_cfg.xml", AMSS_RELOC_LC=amss_reloc_lc)
        else:
            quartz_machine_xml = env.QuartzXMLBuilder \
                ("${SHORT_BUILDPATH}/quartz_cfg_machine_${TARGET_NAME}", 
                 "${BUILD_MS_ROOT}/quartz_cfg_machine.xml")
            
            quartz_xml = env.QuartzXMLBuilder \
                ("${SHORT_BUILDPATH}/quartz_cfg_${TARGET_NAME}", 
                ["${BUILD_MS_ROOT}/quartz_cfg.xml", quartz_machine_xml],
                AMSS_RELOC_LC=amss_reloc_lc)

        #-----------------------------------------------------------------------
        # Build env scatter load
        #-----------------------------------------------------------------------
        if env.has_key('USES_L4_3X'):
            target_scl = env.SclBuilder \
                ("${SHORT_BUILDPATH}/${TARGET_NAME}", [
             "${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_DDROnly_rom.scl"
                ])
        else:
            l4_scl_hdr_file = env.L4SclHeaderBuilder \
                ("${SHORT_BUILDPATH}/cust_l4_scl_${TARGET_NAME}", quartz_xml)

            #"${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_rom.scl"
            #"${BUILD_MS_ROOT}/${BUILD_ASIC}_L4_${TARGET_IMAGE}_SMIOnly_rom.scl"
            #"${BUILD_MS_ROOT}/7600A_L4_${TARGET_IMAGE}_rom.scl"
            target_scl = env.SclBuilder \
                ("${SHORT_BUILDPATH}/${TARGET_NAME}", [ \
                 "${BUILD_MS_ROOT}/" \
                 "${BUILD_ASIC}_L4_${TARGET_IMAGE}_DDROnly_rom.scl", \
                l4_scl_hdr_file \
                ])

        image_units += target_scl
      
        #-----------------------------------------------------------------------
        # Sources, libraries
        #-----------------------------------------------------------------------

    if do_link:
        # actually doing link, reset image_units
        image_units = []
      
        #-----------------------------------------------------------------------
        # Build env QCOREIMG
        #-----------------------------------------------------------------------
        # '''
        # image_elf = env.Program("${SHORT_BUILDPATH}/${TARGET_NAME}", \
        #                         image_objs, LIBS=image_libs)
        # env.Depends(image_elf, target_scl)
      
        # image_map = env.subst('${SHORT_BUILDPATH}/${TARGET_NAME}.map')
        # image_sym = env.subst('${SHORT_BUILDPATH}/${TARGET_NAME}.sym')
      
        # env.Clean(image_elf, image_map)
        # env.Clean(image_elf, image_sym)
        # '''
        #'''
        image_elf = env.AddProgram("${SHORT_BUILDPATH}/${TARGET_NAME}", \
        	                   source=image_objs, LIBS=image_libs)
        env.Depends(image_elf, target_scl)
        #'''
      
    if do_post_link:      
        #-----------------------------------------------------------------------
        # Modify elf flags to update attributes for shared segments, 
        # DL segments, etc.
        #-----------------------------------------------------------------------
        target_elf = env.ModifyElfFlagsBuilder \
            ("${SHORT_BUILDPATH}/${TARGET_NAME}_modifyelf", \
            [image_elf, target_scl])
      
        #-----------------------------------------------------------------------
        # Install ELF, reloc files
        #-----------------------------------------------------------------------

        # copy elf and reloc to 
        # needed locations for AMSS tools to load on target
        install_target_elf = env.InstallAs \
            ("${BUILD_MS_ROOT}/M${TARGET_FAMILY}${BUILD_ID}${BUILD_VER}.elf", \
            target_elf)

        install_target_reloc = env.InstallAs(amss_reloc, install_target_elf)

        #-----------------------------------------------------------------------
        # Build env pbn files
        #-----------------------------------------------------------------------
        target_pbn = env.PbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", \
        	                    [quartz_xml, install_target_reloc])

        # Copy image_M.pbn to platform\l4\build_M\bootimg.pbn for RUMI */
        install_target_pbn = env.InstallAs(
            "${L4_ROOT}/build_${TARGET_BLD_KCFG}/bootimg.pbn", target_pbn)

        #-----------------------------------------------------------------------
        # Build CMM scripts
        #-----------------------------------------------------------------------
        quartz_constant_cmm = env.QuartzConstBuilder \
            ("${SHORT_BUILDPATH}/quartz_const_${TARGET_NAME}", [ \
            "${L4_ROOT}/build_${TARGET_BLD_KCFG}/ms/" \
            "quartz_constants_${TARGET_BLD_KCFG}.cmm", \
            install_target_pbn, \
            quartz_xml, \
        ])

        # install scripts
        install_quartz_constant_cmm = env.InstallAs( \
            "${BUILD_MS_ROOT}/quartz_constants_${TARGET_BLD_KCFG}.cmm", \
            quartz_constant_cmm \
        )
      
        install_loadsyms_cmm = env.Install("${BUILD_MS_ROOT}", \
            "${L4_ROOT}/build_${TARGET_BLD_KCFG}/ms/" \
            "loadsyms_${TARGET_BLD_KCFG}.cmm", \
        )
      
        install_loadsyms_men = env.Install("${BUILD_MS_ROOT}", \
            "${L4_ROOT}/build_${TARGET_BLD_KCFG}/ms/" \
            "loadsyms_${TARGET_BLD_KCFG}.men", \
        )
      
        install_cmm_scripts = [
            install_quartz_constant_cmm,
            install_loadsyms_cmm,
            install_loadsyms_men,
        ]
      
        #-----------------------------------------------------------------------
        # Build env mbn files
        #-----------------------------------------------------------------------
        target_mbn = env.MbnBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}", \
             target_pbn, IMAGE_TYPE="amss_mbn", \
             FLASH_TYPE=env['TARGET_FLASH_TYPE'])

        #-----------------------------------------------------------------------
        # create FAT image
        #-----------------------------------------------------------------------
        if 'FatfsBuilder' in env.get('BUILDERS', []):
            target_fatfs = env.FatfsBuilder("${SHORT_BUILDPATH}/fat.bin", \
            	    target_mbn, TARGET_PATH='image')
         
            install_fatfs = env.Install("${MBN_ROOT}", target_fatfs)
         
            image_units += install_fatfs

         
        #=======================================================================
        # Define targets needed QCOREIMG
        #
        image_units += [
            install_target_pbn,
            install_cmm_scripts,
            target_mbn,
        ]

    #print env.Dump()

    # add aliases
    aliases = env.get('IMAGE_ALIASES')
    for alias in aliases:
        env.Alias(alias, image_units)
