'''
   CoreBSP GCC tool rules
'''
#===============================================================================
#
# CoreBSP GCC tool rules
#
# GENERAL DESCRIPTION
#    Cygwin GCC Tools definitions
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/gcc_x86.py#4 $
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
import sys
import os
import string
import SCons.Action
import subprocess 
from SCons.Script import *

#------------------------------------------------------------------------------
# ESCAPE
# escape hack to check for temp files since qdsp6 tools don't have a way to
# add temp files, so we use `cat file` to get this working; unfortunatly scons
# will add quotes ("") arounds args with spaces :(. So this hack allows us
# to intercept such changes and fix it before spawn executes the command
#------------------------------------------------------------------------------
class gcc_x86_cyg:
    '''
       class gcc_x86_cyg
    '''
    def __init__(self, env):
        '''
           __init__
        '''
        self.env = env
        self.get_cygwin_cmd = env['GET_CYGWIN_CMD']

    def get_cmd(self, sh, escape, cmd, args, env = None):
        '''
           get_cmd
        '''
        #import pdb; pdb.set_trace()
        cmdline, env = self.get_cygwin_cmd(sh, escape, cmd, args, env)
      
        cmdline = cmdline.replace("\\\"`", "`")
        cmdline = cmdline.replace("`\\\"","`")
      
        return cmdline, env
      
#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
    '''
       C_static_obj_emitter
       
       Chain emitter for C files
       Adds explicit dep
    '''
    #import pdb; pdb.set_trace()
   
    # call defualt static object emitter
    tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
    preprocess = env.GetOption('preprocess')
    mixasm = env.GetOption('mixasm')
   
    if preprocess:
        env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
        env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
      
    if mixasm:
        env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))
      
    if 'EXPLICIT_DEPS' in env:
        env.Depends(tgt, env['EXPLICIT_DEPS'])
   
    return (tgt, src)

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
    '''
       S_static_obj_emitter
       
       Chain emitter for asm files
       Adds explicit dep
    '''
    #import pdb; pdb.set_trace()
   
    # call defualt static object emitter
    tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
    # add cleaners for assembling side-effects, taht scons can't auto detect
    env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
    env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
    env.Clean(tgt, "".join([str(tgt[0]), ".lst"]))
   
    if 'EXPLICIT_DEPS' in env:
        env.Depends(tgt, env['EXPLICIT_DEPS'])
   
    return (tgt, src)

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('gcc_x86')

def generate(env):
    '''
       generate
    '''
    #===========================================================================
    # Special init for our tools and special dependency in CUST_H
    #===========================================================================
    # Replace Static Object Emitter
    env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
    env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
    env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)

    #===========================================================================
    # Special init to setup cygwin and change text formatting
    #===========================================================================
    if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
        env.Tool('cygwin', toolpath = \
            ['${BUILD_ROOT}/tools/build/scons/scripts'])
      
        # replace escape hack because of temp files
        gcccyg = gcc_x86_cyg(env)
        env['GET_CYGWIN_CMD'] = gcccyg.get_cmd
      
    else:
        env.Replace(GNU_INCPATH = env.RealPath("/usr/include"))
      
    env.Tool('formatting', toolpath = \
        ['${BUILD_ROOT}/tools/build/scons/scripts']) 

    #===========================================================================
    #                             TOOL DEFINITIONS
    #===========================================================================
    # The following environment variables must be defined prior to using this 
    # make file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
    # the GCC tools.

    # Standard include paths always searched
    env.Append(CPPPATH = [
        ".",
        # don't force the ganu path as we may unclude cygwin headers 
        # as oppose to mingw headers if using --nocygwin
        #"${GNU_INCPATH}", 
        "${INC_ROOT}/build/cust",
        "${INC_ROOT}/build/ms",
    ])

    #---------------------------------------------------------------------------
    # Software tool and environment definitions
    #---------------------------------------------------------------------------
    # if env['platform'] == 'windows':

    # GCC set ANSI C compiler
    env.Replace(CC = env.subst("gcc${EXE_EXT}"))

    # GCC set ANSI C++ compiler
    env.Replace(CXX = env.subst("g++${EXE_EXT}"))

    # GCC assembler
    env.Replace(AS = env.subst("as${EXE_EXT}"))

    # GCC library archiver
    env.Replace(AR = env.subst("ar${EXE_EXT}"))

    # GCC linker
    env.Replace(LINK = env['CC'])
    #env.Replace(LINK = env.subst("ld${EXE_EXT}"))

    # GCC SDT utility to create hex file from image
    env.Replace(HEXTOOL = \
        env.SubstRealPath("${ARMBIN}/fromelf.exe", posix=False))

    # GCC SDT utility to create binary file from image
    env.Replace(BINTOOL = \
        env.SubstRealPath("${ARMBIN}/fromelf.exe", posix=False))

    # Preprocess assembly cleaner
    env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))   
    env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")  
   
    # Header dependency tree generator
    env.Replace(HDEPTREE_FILE = \
        env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))   
    env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")  
   
    #---------------------------------------------------------------------------
    # GCC Procedure Call Standard (APCS) options
    #---------------------------------------------------------------------------

    env.Replace(GCC_PCSZ = "32")              # Program counter (PC) size (bits)

    #---------------------------------------------------------------------------
    # NOTE:
    # If software stack checking support is enabled, FEATURE_STACK_CHECK must be
    # defined in the cust*.h file to enable the required common services support
    #
    # The following APCS options are obsolete in the ARM SDT 2.50 tools:
    #   PCSZ
    #   FRAMEPTR
    #   REENTRNT
    #---------------------------------------------------------------------------

    #---------------------------------------------------------------------------
    # Compiler output options
    #---------------------------------------------------------------------------
    env.Replace(GCC_COMPILE_CMD = "-c")
    env.Replace(GCC_PREPROCESS_CMD = "-E")
    env.Replace(GCC_OUTPUT_CMD = "-o")
    env.Replace(GCC_ASMOUT_CMD = "-S")   
    env.Replace(GCCAR_LIB_REPLACE = "-r")
    env.Replace(GCCAR_LIB_QUITE = "-c")
    env.Replace(GCCLD_MAP_CMD = "-M")

    #---------------------------------------------------------------------------
    # Compiler/assembler debug options
    #---------------------------------------------------------------------------
    if env.has_key('USES_NO_DEBUG'):
        env.Replace(GCC_DBG = "")
    else:
        env.Replace(GCC_DBG = "-g")          # Enable DWARF2 format debug tables

    #---------------------------------------------------------------------------
    # Compiler optimization options
    #---------------------------------------------------------------------------
    env.Replace(GCC_OPT_TIME = "-Otime")
    env.Replace(GCC_OPT_SPACE = "-Ospace")
    env.Replace(GCC_OPT_2 = "-O2")
    env.Replace(GCC_OPT_1 = "-O1")

    # Let's optimize for time on the L4 build
    env.Replace(GCCCC_OPT = "${GCC_OPT_TIME} ${GCC_OPT_2} ${GCC_OPT_2}")

    # the other opts found
    env.Replace(GCCCXX_OPT = "${GCC_OPT_SPACE} ${GCC_OPT_2} ${GCC_OPT_2}")
    env.Replace(GCCASM_OPT = "${GCC_OPT_1}")

    #---------------------------------------------------------------------------
    # Compiler code generation options
    #---------------------------------------------------------------------------
    env.Replace(GCC_NOINLINE = "--no_inline")
    env.Replace(GCC_NOAUTOINLINE = "--no_autoinline")
    env.Replace(GCC_NOPT = "--force_new_nothrow")  # this is added for CR 139116
    env.Replace(GCC_END = "--littleend") 
        # Compile for little endian memory architecture
    env.Replace(GCC_ZA  = "-za1") # LDR may only access 32-bit aligned addresses
    env.Replace(GCC_ZAS = "-zas4")           # Min byte alignment for structures
    env.Replace(GCC_ZAP = "-zap0") 
        # Struct ptrs NOT assumed to be aligned per -zas
    env.Replace(GCC_ZAT = "-zat1") 
        # Min byte alignment for top-level static objects
    env.Replace(GCC_ZZT = "-zzt0") 
        # Force uninitialized variables to ZI data area
    env.Replace(GCC_32 = "-m32")                    # use only i386 instructions
   
    if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
        env.Replace(GCC_NOCYGWIN = "-mno-cygwin")  
        # don't use cygwin libs (no Cygwin.dll)
    else:
        env.Replace(GCC_NOCYGWIN = "")

    env.Replace(GCCTCC_CODE = "${GCC_END} ${GCC_NOPT}")
    env.Replace(GCCCC_CODE = "${GCC_END}")
    env.Replace(GCCCXX_CODE = "--fpmode fast ${GCC_END} ${GCC_NOINLINE} "\
    	                  "${GCC_NOAUTOINLINE} --split_sections --data_reorder")

    #---------------------------------------------------------------------------
    # NOTE:
    # The -zap0 option is required due to a known ARM 2.50 compiler bug with
    # pointers to struct sub-fields within __packed structs
    #---------------------------------------------------------------------------

    env.Replace(BUILD_FLAGS = \
        "-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT "\
        "-DNON_GPL_ENVIRONMENT=1 -DGPL_ENVIRONMENT=2")

    env.Replace(INCPREFIX = "-I")
    env.Replace(CPPDEFPREFIX  = "-D")
    env.Replace(OBJSUFFIX = ".o")

    env.Replace(LIBPREFIX = "")
    env.Replace(LIBSUFFIX = ".lib")
    env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
    env.Replace(LIBLIKNPREFIX = "")
    env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
    env.Replace(LIBDIRPREFIX = "-L")

    env.Replace(PROGPREFIX = "")
    env.Replace(PROGSUFFIX = env['EXE_EXT'])

    # max cmd line lenght
    # on a cmd sh for XP or newer max comand line is 8191, on 2000 and NT is 
    # 2047 using CreateProcess (our internal spawn) 32766.
    # POSIX OSs is no issue command line limit usually in the 128k range.
    # cygwin has the added restriction of max 32k including OS env variables
    # we have seen a lot of issues with OS environment grater then 20k 
    # (what are si going on from MAKE world why is environment so large??)
    if env['PLATFORM'] in ['windows', 'win32']:
        env.Replace(MAXLINELENGTH = 32000)
    elif env['PLATFORM'] in ['cygwin']:
        dyn_maxlinelength = 32000 - len(str(os.environ))
        if dyn_maxlinelength < 2000:
            dyn_maxlinelength = 2000
        env.Replace(MAXLINELENGTH = dyn_maxlinelength)
    else:
        env.Replace(MAXLINELENGTH = 100000)

    # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
    # if it does we will default back os shell and its cmd lenght restrictions 
    # will be in effect.
   
    # Ecapsulate within quotes so we retain the space after --via
    env.Replace(TEMPFILEPREFIX = "`cat ")
    env.Replace(TEMPFILESUFFIX = "`")

    # Init FLAGS      
    #env.Replace(CCFLAGS = "-pass-exit-codes -g -Wall -U_cdecl -m32 -I. ")
    env.Replace(CCFLAGS = "${GCC_DBG} ${GCC_32} ${GCC_NOCYGWIN} ${CFLAGS}")
    env.Replace(CXXFLAGS = "${GCC_DBG} ${GCC_32} ${GCC_NOCYGWIN} ${CFLAGS}")
    env.Replace(ASMFLAGS = "")
    env.Replace(ASFLAGS = "")
    env.Replace(ARFLAGS = "$GCCAR_LIB_REPLACE $GCCAR_LIB_QUITE")
    #env.Replace(LINKFLAGS = "$GCCLD_MAP_CMD ${TARGET.base}.map")
    #env.Replace(LINKFLAGS = "$GCCLD_MAP_CMD")
    env.Replace(LINKFLAGS = "${GCC_32} ${GCC_NOCYGWIN} ${LFLAGS}")

    cccom = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $GCC_COMPILE_CMD " \
            "$GCC_OUTPUT_CMD $TARGET.posix $SOURCES.posix')}"
    env.Replace(CCCOM =       cccom)
    env.Replace(CCCOMCPY =    cccom)
    env.Replace(CCPPCOM =     \
        cccom.replace('$GCC_COMPILE_CMD','$GCC_PREPROCESS_CMD'))
    env.Replace(CCMIXCOM =    \
        cccom.replace('$GCC_COMPILE_CMD','$GCC_ASMOUT_CMD'))
    env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES.posix -o $TARGET.posix")
    env.Replace(CCPPCL_ACOM = \
        "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
    env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")
   
    cxxcom = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS " \
             "$GCC_COMPILE_CMD $GCC_OUTPUT_CMD $TARGET.posix $SOURCES.posix')}"
    env.Replace(CXXCOM =       cxxcom)
    env.Replace(CXXCOMCPY =    cxxcom)
    env.Replace(CXXPPCOM =     \
        cxxcom.replace('$GCC_COMPILE_CMD','$GCC_PREPROCESS_CMD'))
    env.Replace(CXXMIXCOM =    \
        cxxcom.replace('$GCC_COMPILE_CMD','$GCC_ASMOUT_CMD'))
    env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
    env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])
   
    env.Replace(ARCOM =      \
        "${TEMPFILE('$AR $ARFLAGS $TARGET.posix $SOURCES.posix')}")
    env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS $GCC_OUTPUT_CMD "\
    	                     "$TARGET.posix $SOURCES $_LIBFLAGS')}")

    env.Replace(ASCPPCOM =   "$CC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS " \
    	                     "$GCC_PREPROCESS_CMD $GCC_OUTPUT_CMD " \
    	                     "${TARGET.posix}.pp $SOURCES.posix")
    env.Replace(ASASCOM =    "$AS $ASFLAGS $_CPPINCFLAGS $INCPATHS --list " \
    	                     "${TARGET}.lst ${TARGET.posix}.i " \
    	                     "$GCC_OUTPUT_CMD $TARGET.posix")

    asm_preprocess_action = \
        SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
    asm_clean_action = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
    asm_assembly_action = SCons.Action.Action('$ASASCOM', '$ASASCOMSTR')

    env.Replace(ASCOM = [
        asm_preprocess_action,      # C pre-process assembly files
        asm_clean_action,           # Removed junck generated by C preprocess
        asm_assembly_action,        # Do the actuall assymbly
        #Delete('${TARGET}.i'),   # Do not delete here needed to debug on target
        #env.GetDeleteAction('${TARGET}.pp'),
        #env.GetDeleteAction('${TARGET}.lst'),
    ])

    # avoid having a rallib run for no reason, scons defaults does this, but is
    # maybe casing issues as there is no actual ranlibcom
    env.Replace(RANLIBCOM = "")
    env.Replace(RANLIBCOMSTR = "")
   
    #===========================================================================
    # COMMON CC ADDITIONS
    #===========================================================================
    comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

    if env.PathExists(comcc_addt):
        env.LoadToolScript(comcc_addt)
   
    #===========================================================================
    # ALTERNATE PRODUCT OPTIONS
    #===========================================================================
    gcc_x86_product = None
   
    if env.PathExists("${BUILD_SCRIPTS_PRODUCT_ROOT}/gcc_x86_product.py"):
        gcc_x86_product = "${BUILD_SCRIPTS_PRODUCT_ROOT}/gcc_x86_product.py"
    elif env.PathExists("${BUILD_SCRIPTS_ROOT}/gcc_x86_product.py"):
        gcc_x86_product = "${BUILD_SCRIPTS_ROOT}/gcc_x86_product.py"
    elif env.PathExists("${BUILD_MS_ROOT}/gcc_x86_product.py"):
        gcc_x86_product = "${BUILD_MS_ROOT}/gcc_x86_product.py"
   
    if gcc_x86_product is not None:
        env.LoadToolScript(gcc_x86_product)

    #===========================================================================
    # ALTERNATE OEM OPTIONS
    #===========================================================================
    gcc_x86_oem = "${BUILD_SCRIPTS_OEM_ROOT}/gcc_x86_oem.py"
   
    if env.PathExists(gcc_x86_oem):
        env.LoadToolScript(gcc_x86_oem)
      
    #print env.Dump()

def resolve_cygwin_path(env, file):
    '''
       resolve_cygwin_path
    '''
    real_file = ""
   
    if env['PLATFORM'] == 'windows':
        cmd = env.subst("cygpath -d " + file)
      
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=False)
        data, err = proc.communicate()
        #rv = proc.wait()
      
        if not proc.returncode:
            real_file = data.strip()
         
    else:
        real_file = file
      
    return real_file
   
