#===============================================================================
#
# CoreBSP HEXAGON tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/hexagon.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
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
from SCons.Script import *

RDirsSCons = ""
RelPath = ""
NormPath = ""

#------------------------------------------------------------------------------
# RDirs_rel
# Create relative paths.
#------------------------------------------------------------------------------
def RDirs_rel (path_list):
   global RDirsSCons
   global RelPath
   #import pdb; pdb.set_trace()
   #path_list = RDirsSCons(path_list)
   path_list = [RelPath(str(p), os.getcwd()) for p in path_list]
   return path_list

#------------------------------------------------------------------------------
# RDirs_posix
# Create relative paths.
#------------------------------------------------------------------------------
def RDirs_posix (path_list):
   global RDirsSCons
   global NormPath
   #import pdb; pdb.set_trace()
   #path_list = RDirsSCons(path_list)
   path_list = [NormPath(str(p), posix=True) for p in path_list]
   return path_list
   
#------------------------------------------------------------------------------
# ESCAPE
# escape hack to check for temp files since qdsp6 tools don't have a way to
# add temp files, so we use `cat file` to get this working; unfortunatly scons
# will add quotes ("") arounds args with spaces :(. So this hack allows us
# to intercept such changes and fix it before spawn executes the command
#------------------------------------------------------------------------------
class hexagon_cyg:
   def __init__(self, env):
      self.env = env
      self.get_cygwin_cmd = env['GET_CYGWIN_CMD']

   def get_cmd(self, sh, escape, cmd, args, env = None):
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
   return env.Detect('hexagon')

def generate(env):
   #===============================================================================
   # figure out hwta tools set will be using
   #===============================================================================
   # The following environment variables must be defined prior to using this make
   # file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
   # the GCC tools.

   hexagon_root = os.environ.get('HEXAGON_ROOT', 'C:/Qualcomm/HEXAGON_Tools')   
   hexagon_image_entry = os.environ.get('HEXAGON_IMAGE_ENTRY', '0x00c00000')   
   hexagon_rtos_release = os.environ.get('HEXAGON_RTOS_RELEASE', '1.1')
   q6version = os.environ.get('HEXAGON_Q6VERSION', 'v2')
   hexagon_tools = "HEXAGON%s" % hexagon_rtos_release
   
   if 'HEXAGON1' in hexagon_tools:
      hexagon_ver = 1
   elif 'HEXAGON2' in hexagon_tools:
      hexagon_ver = 2
   elif 'HEXAGON3' in hexagon_tools:
      hexagon_ver = 3
   
   
   #===============================================================================
   # Special init to setup cygwin and change text formatting
   #===============================================================================
   if env['PLATFORM'] in ['windows', 'win32', 'cygwin'] and hexagon_rtos_release == '1.1':
      env.LoadToolScript('cygwin', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])
      
      # replace escape hack because of temp files
      hexcyg = hexagon_cyg(env)
      env['GET_CYGWIN_CMD'] = hexcyg.get_cmd
      
   # since we may have change to cygwin re init formatting
      env.LoadToolScript('formatting', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])       

   #===============================================================================
   # Special setup for Windwos Hexagon Tools not cygwin
   #===============================================================================
   if (env['PLATFORM'] in ['windows', 'win32']):
      global RDirsSCons
      global RelPath
      global NormPath
      RDirsSCons = env.get('RDirs')
      RelPath = env.RelPath
      NormPath = env.NormPath      
      #env.Replace(RDirs = RDirs_rel)
      env.Replace(RDirs = RDirs_posix)
   
      
   # init variables to environment
   env.Replace(HEXAGON_ROOT = hexagon_root)
   env.Replace(HEXAGON_IMAGE_ENTRY = hexagon_image_entry)
   env.Replace(HEXAGON_RTOS_RELEASE = hexagon_rtos_release)
   env.Replace(HEXAGON_TOOLS = hexagon_tools)
   env.Replace(HEXAGONTOOLS = hexagon_tools)   
   env.Replace(QDSP6_RELEASE_DIR = env.RealPath("${HEXAGON_ROOT}/${HEXAGON_RTOS_RELEASE}", posix=True))
   env.Replace(Q6VERSION = q6version)
   env.Replace(L4Q6_ROOT = env.RealPath("${L4_ROOT}/../l4-qdsp6", posix=True))
   
   if not env.PathExists("${QDSP6_RELEASE_DIR}"):
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** HEXAGON tools are not present")
      env.PrintError("*** Please install HEXAGON tools RTOS 1.1, 2.0Beta, etc.")
      env.PrintError("*** Currently setup for RTOS: ${HEXAGON_RTOS_RELEASE}")
      env.PrintError("*** Missing RTOS PATH: ${QDSP6_RELEASE_DIR}")
      env.PrintError("-------------------------------------------------------------------------------")
      raise RuntimeError, "HEXAGON tools are not installed on this environment"
   
   #===============================================================================
   # Special init for our tools and special dependency in CUST_H
   #===============================================================================
   # Replace Static Object Emitter
   env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)
   
   # Standard include paths always searched
   env.Append(CPPPATH = [
      ".",
      "${INC_ROOT}/build/cust",
      "${INC_ROOT}/build/ms",
   ])
   
   # special dependencies in CUST_H
   cust_scanner = SCons.Scanner.C.CScanner()
   cust_path = cust_scanner.path(env)
   
   if 'CUST_H' in env:
      cust_h_file = env.FindFile(env['CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)
      
   if 'AMSS_CUST_H' in env:
      cust_h_file = env.FindFile(env['AMSS_CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)
            
   #===============================================================================
   #                             TOOL DEFINITIONS
   #===============================================================================
   env.Replace(QDSP6BIN = "${QDSP6_RELEASE_DIR}/gnu/bin")
   env.Replace(QDSP6_RELEASE_LIB_DIR = "${QDSP6_RELEASE_DIR}/dinkumware/lib/qdsp6${Q6VERSION}")
   if not env.PathExists("${QDSP6_RELEASE_LIB_DIR}"):
      env.Replace(QDSP6_RELEASE_LIB_DIR = "${QDSP6_RELEASE_DIR}/dinkumware/lib/${Q6VERSION}")
   env.Replace(QDSP6_BINDIR = "${L4Q6_ROOT}/bin")

   if 'USES_BLAST' in env:
      qdsp6_libdir = "${COREBSP_ROOT}/kernel/blast/install/modemSW${Q6VERSION}/lib"
      if not env.PathExists(qdsp6_libdir):
        qdsp6_libdir = "${BUILD_ROOT}/core/kernel/blast/install/lib"
      
      env.Replace(QDSP6_LIBDIR = qdsp6_libdir)
   else:
      env.Replace(QDSP6_LIBDIR = "${L4Q6_ROOT}/lib/qube/qdsp6${Q6VERSION}")

   
   #-------------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #-------------------------------------------------------------------------------
   env.Replace(QDSP6_CPPPATH = [])

   #-------------------------------------------------------------------------------
   # Defines to exapnd paths and libs with posix style even on a Windows env.
   #-------------------------------------------------------------------------------
   env.Replace(LIBFLAGS_NULL = '')
   env.Replace(_LIBFLAGS_POSIX = '$( ${_concat(LIBFLAGS_NULL, _LIBFLAGS, LIBFLAGS_NULL, __env__, RDirs)} $)')
   
   if env['PLATFORM'] in ['cygwin']:
      env.Replace(_QDSP6_CPPINCFLAGS = '$( ${_concat(INCPREFIX, QDSP6_CPPPATH, INCSUFFIX, __env__, PosixRDirs, TARGET, SOURCE)} $)')  
   else:
      env.Replace(_QDSP6_CPPINCFLAGS = '$( ${_concat(INCPREFIX, QDSP6_CPPPATH, INCSUFFIX, __env__, RDirs, TARGET, SOURCE)} $)')
      
   if env.has_key('ENV'):
      env['ENV']['PATH'] = env['ENV']['PATH'] + env.subst(";${QDSP6BIN}")
      #print env.Dump()
      
   
   #-------------------------------------------------------------------------------
   # this is needed for the bintools (fromelf) we probbaly should do soemthing 
   # like a hexagon equivelant different here
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #-------------------------------------------------------------------------------
   if os.environ.get('ARMBIN') is not None:
      env.Replace(ARMBIN = os.environ['ARMBIN'])
      env.Replace(ARMLIB = os.environ['ARMLIB'])
   else:
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** ARMBIN is not defined on this environment")
      env.PrintError("*** Please set up the enviroment correctly where ARMBIN is defined correclty.")
      env.PrintError("-------------------------------------------------------------------------------")
      raise RuntimeError, "ARMBIN is not defined on this environment"
         
   #-------------------------------------------------------------------------------
   # Software tool and environment definitions
   #-------------------------------------------------------------------------------
   # Check QDSP6BIN path other wise clear it to use system environment
   if env.PathExists("${QDSP6BIN}"):
      QDSP6BIN = env.RealPath("${QDSP6BIN}")
      if env['PLATFORM'] in ['windows', 'win32']:
         QDSP6BIN = QDSP6BIN + "\\"
      else:
         QDSP6BIN = QDSP6BIN + "/"
   else:
      env.PrintWarning("${QDSP6BIN} not found using system environment. Using system paths for tools.")
      QDSP6BIN = ""
   
   env.PrintInfo("HEXAGON_TOOLS       = ${HEXAGON_TOOLS}")
   env.PrintInfo("HEXAGON_RTOS_REL    = ${HEXAGON_RTOS_RELEASE}")
   env.PrintInfo("Q6VERSION           = ${Q6VERSION}")      
   env.PrintInfo("HEXAGON_ROOT        = %s" % env.RealPath("${HEXAGON_ROOT}"))
   env.PrintInfo("QDSP6BIN            = %s" % env.RealPath("${QDSP6BIN}"))
   env.PrintInfo("HEXAGON_IMAGE_ENTRY = ${HEXAGON_IMAGE_ENTRY}")   

   #import pdb; pdb.set_trace()      
   if env['PLATFORM'] in ['cygwin']:
      QDSP6BIN = string.replace("/cygdrive/%s" % QDSP6BIN, ":", "")
   
   # if env['platform'] == 'windows':

   # HEXAGON set ANSI C compiler
   #import pdb; pdb.set_trace()
   #env.Replace(CC = env.RealPath("${QDSP6BIN}/qdsp6-gcc"))
   env.Replace(CC = QDSP6BIN + "qdsp6-gcc${EXE_EXT}")

   # HEXAGON set ANSI C++ compiler
   #env.Replace(CXX = env.RealPath("${QDSP6BIN}/qdsp6-g++"))
   env.Replace(CXX = QDSP6BIN + "qdsp6-g++${EXE_EXT}")

   # HEXAGON assembler
   #env.Replace(AS = env.RealPath("${QDSP6BIN}/qdsp6-as"))
   env.Replace(AS = QDSP6BIN + "qdsp6-as${EXE_EXT}")

   # HEXAGON library archiver
   #env.Replace(AR = env.RealPath("${QDSP6BIN}/qdsp6-ar"))
   env.Replace(AR = QDSP6BIN + "qdsp6-ar${EXE_EXT}")

   # HEXAGON linker
   #env.Replace(LINK = env.RealPath("${QDSP6BIN}/qdsp6-ld"))
   env.Replace(LINK = QDSP6BIN + "qdsp6-ld${EXE_EXT}")
   
   # HEXAGON library archiver
   #env.Replace(RANLIB = env.RealPath("${QDSP6BIN}/qdsp6-ranlib"))
   env.Replace(RANLIB = QDSP6BIN + "qdsp6-ranlib${EXE_EXT}")

   # HEXAGON SDT utility to create hex file from image
   env.Replace(HEXTOOL = env.RealPath("${ARMBIN}/fromelf${EXE_EXT}"))

   # HEXAGON SDT utility to create binary file from image
   env.Replace(BINTOOL = env.RealPath("${ARMBIN}/fromelf${EXE_EXT}"))

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))   
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")  
   
   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))   
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")  
   
   #-------------------------------------------------------------------------------
   # Compiler output options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_COMPILE_CMD = "-c")
   env.Replace(HEXAGON_PREPROCESS_CMD = "-E -xc")
   env.Replace(HEXAGON_INHIBIT_WARN_CMD = "-w")
   env.Replace(HEXAGON_OUTPUT_CMD = "-o")
   env.Replace(HEXAGON_ASMOUT_CMD = "-S")   
   env.Replace(HEXAGONAR_LIB_REPLACE = "-r")
   env.Replace(HEXAGONAR_LIB_QUITE = "c")
   env.Replace(HEXAGONLD_MAP_CMD = "-Map")
   
   env.Replace(ARMBIN_BIN_CMD = "--bin")
   env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = "-o")
   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")



   #-------------------------------------------------------------------------------
   # Compiler/assembler debug options
   #-------------------------------------------------------------------------------
   if env.has_key('USES_NO_DEBUG'):
      env.Replace(HEXAGON_DBG = "")
   else:
      env.Replace(HEXAGON_DBG = "-g")            # Enable DWARF2 format debug tables

   #-------------------------------------------------------------------------------
   # Compiler optimization options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_OPT_TIME = "-Otime")
   env.Replace(HEXAGON_OPT_SPACE = "-Ospace")
   env.Replace(HEXAGON_OPT_2 = "-O2")
   env.Replace(HEXAGON_OPT_1 = "-O1")
   env.Replace(HEXAGON_OPT_S = "-Os")

   # Let's optimize for time on the L4 build
   env.Replace(HEXAGONCC_OPT = "${HEXAGON_OPT_S}")

   # the other opts found
   env.Replace(HEXAGONCXX_OPT = "${HEXAGON_OPT_SPACE} ${HEXAGON_OPT_TIME} ${HEXAGON_OPT_S}")
   env.Replace(HEXAGONASM_OPT = "${HEXAGON_OPT_1}")

   #-------------------------------------------------------------------------------
   # HEXAGON Warnings Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_WARN = "-Wall -Wpointer-arith")
   env.Replace(HEXAGONCC_WARN = "${HEXAGON_WARN} -Wstrict-prototypes -Wnested-externs")
   env.Replace(HEXAGONCXX_WARN = "${HEXAGON_WARN}")

   #-------------------------------------------------------------------------------
   # HEXAGON Machine Options
   #-------------------------------------------------------------------------------
   hexagon_machine = "-m${Q6VERSION}"
   if env.subst("$Q6VERSION") == "v1" or env.subst("$Q6VERSION") == "v2":
      hexagon_machine += " -mv1-v2-uncached-data"
   env.Replace(HEXAGON_MACHINE = hexagon_machine)

   #-------------------------------------------------------------------------------
   # HEXAGON Undef Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_UNDEF = "-Uqdsp6 -Uq6sim -Uqdsp6r0")
   
   #-------------------------------------------------------------------------------
   # HEXAGON Lang independent options
   #-------------------------------------------------------------------------------
   if hexagon_ver >= 2:
      env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections " \
         "-mno-literal-pool-addresses -nostdlib -fno-exceptions " \
         "-fno-merge-constants -fno-strict-aliasing ")
         
      # There was a bug in the hexagon tools earlier then 2.0.06 (-mno-extended-crossjumping acted as a temp patch)
      if hexagon_rtos_release < "2.0.06":
         env.Append(HEXAGON_LANIND = "-mno-extended-crossjumping ")
   else:      
      env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections " \
         "-mno-literal-pool-addresses  -nostdlib -fno-exceptions " \
         "-fno-merge-constants")      

   #-------------------------------------------------------------------------------
   # HEXAGON Stack Checking
   #-------------------------------------------------------------------------------
   if hexagon_ver >= 3 and 'USES_STACK_PROTECTOR' in env:
      env.Replace(HEXAGON_STKPRO = "-fstack-protector")
   else:
      env.Replace(HEXAGON_STKPRO = "")

   #----------------------------------------------------------------------------
   # common defines
   #----------------------------------------------------------------------------
   env.Replace(BUILD_CCFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" '\
      '-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 '\
      '-DGPL_ENVIRONMENT=2 -D__FILENAME__=\\"$SOURCE.file\\" ')
      
   env.Replace(BUILD_ASMFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" -D__FILENAME__=\\"$SOURCE.file\\" ')
      
   #-------------------------------------------------------------------------------
   # HEXAGON Flags Def.
   #-------------------------------------------------------------------------------

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
   env.Replace(PROGSUFFIX = '.elf')

   # max cmd line length
   # on a cmd sh for XP or newer max command line is 8191, on 2000 and NT is 2047
   # using CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables
   # we have seen a lot of issues with OS environment grater then 20k 
   # (what is going on from MAKE world why is environment so large??)
   if env['PLATFORM'] in ['windows', 'win32']:
      # hexagon uses gcc code under the hood, gcc uses the OS env extensively 
      # because of this we need to limit the command line as it is then added 
      # to the OS env by gcc. the safe value we have been using is 8k
      env.Replace(MAXLINELENGTH = 32000)
   elif env['PLATFORM'] in ['cygwin']:
      env.Replace(MAXLINELENGTH = 8000)
   else:
      env.Replace(MAXLINELENGTH = 100000)
   
   # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
   # if it does we will default back os shell and its cmd lenght restrictions 
   # will be in effect (8191).

   if env['PLATFORM'] in ['cygwin']:
      env.Replace(TEMPFILEPREFIX = "`cat ")
      env.Replace(TEMPFILESUFFIX = "`")
   else:
      env.Replace(TEMPFILEPREFIX = "@")
      env.Replace(TEMPFILESUFFIX = "")
      
   # Init FLAGS   
   env.Replace(CCFLAGS   = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(CXXFLAGS  = "$HEXAGONCXX_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(ASMFLAGS  = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS ")
   env.Replace(ASFLAGS   = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS ")
   #env.Replace(ARFLAGS   = "$HEXAGONAR_LIB_QUITE $HEXAGONAR_LIB_REPLACE")
   env.Replace(ARFLAGS   = "$HEXAGONAR_LIB_REPLACE$HEXAGONAR_LIB_QUITE")

   CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}"
   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES.posix -o $TARGET.posix")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")      
   
   CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])   
   
   env.Replace(ARCOM     =  "${TEMPFILE('$AR $ARFLAGS $TARGET.posix $SOURCES.posix')}")
   env.Replace(RANLIBCOM =  "${TEMPFILE('$RANLIB $TARGET.posix')}")
   
   env.Replace(ASCPPCOM  =  "$CC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS $HEXAGON_PREPROCESS_CMD $HEXAGON_OUTPUT_CMD ${TARGET.posix}.pp $SOURCES.posix")
   env.Replace(ASASCOM   =  "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}")

   AsmPreprocessAction = SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
   AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
   AsmAssemblyAction = SCons.Action.Action('$ASASCOM', '$ASCOMSTR')

   env.Replace(ASCOM = [
   #   AsmPreprocessAction,          # C pre-process assembly files
   #   AsmCleanAction,               # Removed junck generated by C preprocess
      AsmAssemblyAction,            # Do the actuall assymbly
   #   env.GetDeleteAction('${TARGET.posix}.pp'),
   #   env.GetDeleteAction('${TARGET.posix}.lst'),
   ])

   #-------------------------------------------------------------------------------
   # HEXAGON LINK, very special procedure
   #-------------------------------------------------------------------------------
   if 'USES_BLAST' in env:
      env.Replace(LINKFLAGS = "-m${Q6VERSION} -nostdlib --section-start .start=${HEXAGON_IMAGE_ENTRY} ${HEXAGONLD_MAP_CMD} ${TARGET.posix}.map --entry=${HEXAGON_IMAGE_ENTRY} ${LFLAGS}")
               
      if 'USES_LCS_FILE' in env:
         # The following linker control script (lcs) is generated 
         # by: lcs_builder.py
         env.Append(LINKFLAGS = "-T${TARGET.posix}.lcs ")
   else:
      env.Replace(LINKFLAGS = "--defsym COMPILE_FOR_RTOS=3 -lstdc++ -g -r -m${Q6VERSION} -nostdlib ${LFLAGS}")
   
   env.Replace(LINK_DIRS = "-L${QDSP6_RELEASE_LIB_DIR}")

   # LINK special case in hexagon world
   if 'USES_BLAST' in env:
      env.Replace(QDSP6OBJS1  = [
         '${QDSP6_LIBDIR}/crt0.o',
         '${QDSP6_RELEASE_LIB_DIR}/init.o',
      ])

      env.Replace(QDSP6OBJS3  = [
         '${QDSP6_LIBDIR}/libblast.a',
         '${QDSP6_LIBDIR}/libblastkernel.a',
         '${QDSP6_LIBDIR}/libqube_compat.a',
         '${QDSP6_LIBDIR}/libtimer.a',
         '${QDSP6_LIBDIR}/libposix.a',
         '${QDSP6_RELEASE_LIB_DIR}/libc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
         ])
   else:
      env.Replace(QDSP6OBJS1  = [
         '${QDSP6_RELEASE_LIB_DIR}/crt0.o',
         '${QDSP6_RELEASE_LIB_DIR}/init.o',
      ])

      env.Replace(QDSP6OBJS3  = [
         '${QDSP6_LIBDIR}/libqube.a',
         '${QDSP6_RELEASE_LIB_DIR}/libc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
         ])

   env.Replace(QDSP6OBJS4  = [
      '${QDSP6_RELEASE_LIB_DIR}/fini.o',\
   ])

   #-------------------------------------------------------------------------------------
   # if $SOURCES doesn't have any items in it, python is throwing an error $SOURCES.posix.
   # When linking there should always be at least one obj it is the entry point.
   # SOURCES should never be emty if it is someone added objects to library list instead 
   # of object list. Fix that isntead of trying to get creative here.
   #-------------------------------------------------------------------------------------
   env.Replace(LINKOBJGRP = "--undefined=main ${QDSP6OBJS1} " \
      "--start-group $_LIBFLAGS_POSIX  $SOURCES.posix --end-group " \
      "--start-group ${QDSP6OBJS3}  --end-group " \
      "${QDSP6OBJS4}"
   )
   
   #cd $(TARGET_NAME); $(LD) $(LINKFLAGS) $(LINK_DIRS) $(LINKOBJGRP) $(OBJ_CMD) $(TARGET_NAME).$(EXETYPE) -Map $(TARGET_NAME).map; mv $(TARGET_NAME).$(EXETYPE) ../$(TARGET_NAME).$(EXETYPE); mv $(TARGET_NAME).map ../$(TARGET_NAME).map
   #$(CC) -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -mv2 -moslib=qube -Xlinker -m$(Q6VERSION) -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01200000 $(QDSP6_BINDIR)/lib/qube/qdsp6v2/ig_server -o ig_server.reloc
   #$(CC) -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -mv2 -moslib=qube -Xlinker -m$(Q6VERSION) -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01300000 $(TARGET_NAME).$(EXETYPE) -o $(TARGET_NAME).reloc
   
   env.Replace(LINKCOM = "${TEMPFILE('$LINK $LINKFLAGS $LINK_DIRS $LINKOBJGRP $HEXAGON_OUTPUT_CMD $TARGET.posix $LINKFLAGS_END')}")
   env.Replace(KERNEL_RELOCCOM = "$CC -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -moslib=qube -Xlinker -m${Q6VERSION} -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01200000 $SOURCE.posix -o $TARGET.posix")
   env.Replace(AMSS_RELOCCOM = "$CC -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -moslib=qube -Xlinker -m${Q6VERSION} -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01300000 $SOURCE.posix -o $TARGET.posix")
   
   #-------------------------------------------------------------------------------
   # Reloc Builders
   #-------------------------------------------------------------------------------
   kreloc_act = env.GetBuilderAction('$KERNEL_RELOCCOM')
   kreloc_bld = env.Builder(action = kreloc_act, suffix = '.reloc')
   env.Append(BUILDERS = {'Q6_KernelRelocBuilder' : kreloc_bld})

   amssreloc_act = env.GetBuilderAction('$AMSS_RELOCCOM')
   amssreloc_bld = env.Builder(action = amssreloc_act, suffix = '.reloc')
   env.Append(BUILDERS = {'Q6_AmssRelocBuilder' : amssreloc_bld})

   #============================================================================
   # COMMON CC ADDITIONS
   #============================================================================
   comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

   if env.PathExists(comcc_addt):
      env.LoadToolScript(comcc_addt)
   
   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   hexagon_product = None
   
   if env.PathExists("${BUILD_SCRIPTS_ROOT}/hexagon_product.py"):
      hexagon_product = "${BUILD_SCRIPTS_ROOT}/hexagon_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/hexagon_product.py"):
      hexagon_product = "${BUILD_MS_ROOT}/hexagon_product.py"
   
   if hexagon_product is not None:
      env.LoadToolScript(hexagon_product)

   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   hexagon_oem = "${BUILD_SCRIPTS_OEM_ROOT}/hexagon_oem.py"
   
   if env.PathExists(hexagon_oem):
      env.LoadToolScript(hexagon_oem)

  
   #print env.Dump()

   
   '''Possible TARGET.attribute or SOURCE.attributes
   
   dictSpecialAttrs = { "base"     : __get_base_path,
                        "posix"    : __get_posix_path,
                        "windows"  : __get_windows_path,
                        "win32"    : __get_windows_path,
                        "srcpath"  : __get_srcnode,
                        "srcdir"   : __get_srcdir,
                        "dir"      : __get_dir,
                        "abspath"  : __get_abspath,
                        "filebase" : __get_filebase,
                        "suffix"   : __get_suffix,
                        "file"     : __get_file,
                        "rsrcpath" : __get_rsrcnode,
                        "rsrcdir"  : __get_rsrcdir,
                      }   
   '''