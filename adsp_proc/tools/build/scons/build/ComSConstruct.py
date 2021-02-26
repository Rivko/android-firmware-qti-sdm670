'''Common SConstruct script'''
#===============================================================================
#
# ComSConstruct
#
# GENERAL DESCRIPTION
#    Common SConstruct script, it is intended to be used as a plug in from other
#    build locations. 
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/build/ComSConstruct.py#5 $
#  $DateTime: 2017/09/27 00:49:50 $
#  $Author: pwbldsvc $
#  $Change: 14486553 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/18/09   wd      Created...
#
#===============================================================================
import SCons
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''exists function'''
    return env.Detect('sconstruct')

def generate(env):
    '''set global variables and load default values and scripts'''
    #---------------------------------------------------------------------------
    # Set global variables and load defaults values and scripts
    #---------------------------------------------------------------------------
    env.Replace(PRINT_BUILD_BANNER = 'yes')
    env.Replace(ROOT_GLOBAL_ENV = env)
    env.Tool('defaults', toolpath = ["${BUILD_ROOT}/tools/build/scons/scripts"])

    #---------------------------------------------------------------------------
    # Load TARGET IMAGE scripts
    #  The images scripts should be located in 
    #  build_root/<AU name>/bps/<image name>/build/Sconscript
    #---------------------------------------------------------------------------
    #import pdb; pdb.set_trace()
   
    print "\n"
    print "================================================================" \
        "=============="
    print "Reading SConscript files..."
    print "\n"      
   
    env.LoadImageUnits(env.get('IMAGE_ROOT_DIR_PATTERNS'))

    print "\n"
    print "----------------------------------------------------------------" \
        "--------------"

    # set default target if none is given, print actual targets
    env.Default('None, are you missing a build target?')
   
    print "COMMAND_LINE_TARGETS are", \
        map(str, SCons.Script.COMMAND_LINE_TARGETS)
    print "BUILD_TARGETS are       ", map(str, BUILD_TARGETS)

    print "\n"
    print "================================================================" \
        "=============="

    # check for command line uknown variables
    vars = Variables()
    unknown_var = vars.UnknownVariables()
    if unknown_var:
        print "Unknown variables:", unknown_var.keys()
        Exit(1)

    no_exec = env.GetOption('no_exec')
    clean = env.GetOption('clean')
    cleanpack = env.GetOption('cleanpack')

    if clean and cleanpack and not no_exec:
        create_clean_pack_file(env)

def create_clean_pack_file(env):
    '''create clean pack file'''
    #import pdb; pdb.set_trace()
    create_file = True
    tbc_cleanpack_fname = "tbc_cleanpack"

    tbc_cleanpack = env.RealPath("${BUILD_MS_ROOT}/%s.py" % tbc_cleanpack_fname)
    tbc_cleanpack_md5 = \
        env.RealPath("${BUILD_MS_ROOT}/%s.md5" % tbc_cleanpack_fname)
   
    if env.PathExists(tbc_cleanpack):
        if env.CompareMD5File(tbc_cleanpack, tbc_cleanpack_md5):
            create_file = False

    if create_file:
        env.PrintInfo("Geneating %s.py..." % tbc_cleanpack_fname)

        data = ("%s%s" % 
            (
                env.CreateFileBanner("%s.py" % tbc_cleanpack_fname), 
                "def exists(env):\n" \
                "   return env.Detect('tcfg_cleanpack')\n" \
                "\n" \
                "def generate(env):\n" \
                "   env.AddUsesFlags('USES_CLEAN_PACK',  True)\n"
            )
        )
      
        tbc_clenapack_fh = open(tbc_cleanpack, "w")      
        tbc_clenapack_fh.write (data)
        tbc_clenapack_fh.close()
        env.CreateMD5SumFile(tbc_cleanpack, tbc_cleanpack_md5)
