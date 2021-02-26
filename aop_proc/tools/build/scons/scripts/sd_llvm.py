#===============================================================================
#
# Snapdragon LLVM toolchain rules
#
# Snapdragon LLVM 3.8 and greater Tools definitions
#
# Copyright (c) 2009-2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/scripts/sd_llvm.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
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
import string
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# RDirs
# Makes sure the inc paths are as provided, don't let SCons modify them.
#------------------------------------------------------------------------------
def RDirs (path_list):
   return path_list

#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
   # call default static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
   preprocess = env.GetOption('preprocess')
   mixasm = env.GetOption('mixasm')
   hdeptree = env.GetOption('hdeptree')
   
   if preprocess or hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
      env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
      
   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))
      
   if hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))
      
   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])
   
   return (tgt, src)

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
   # call default static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
   # add cleaners for assembling side-effects, that SCons doesn't auto detect
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
   return env.Detect('sd_llvm')

def generate(env):
   #============================================================================
   # Special init for our tools and special dependency in CUST_H
   #============================================================================
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
   
   # Special dependencies in CUST_H
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
   
   #============================================================================
   #                             TOOL DEFINITIONS
   #============================================================================

   # Check SD_LLVM_ROOT and SD_LLVM_SYSROOT enviroment variables first
   sd_llvm_root = os.environ.get('SD_LLVM_ROOT', None)
   
   if sd_llvm_root is not None:
      env.Replace(SD_LLVM_ROOT = sd_llvm_root)
   else:
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("SD_LLVM_ROOT is not defined in environment")
      env.PrintError("-------------------------------------------------------------------------------")
      raise RuntimeError, "SD_LLVM_ROOT is not defined in environment"
      exit(1)
      
   env.Replace(SD_LLVM_BIN = sd_llvm_root + os.sep + 'bin')
   env.Replace(SD_LLVM_TOOLS = sd_llvm_root + os.sep + 'tools' + os.sep + 'bin')

   env.PrintInfo('SD_LLVM_ROOT    = ${SD_LLVM_ROOT}')
   env.PrintInfo('SD_LLVM_BIN     = ${SD_LLVM_BIN}')
   env.PrintInfo('SD_LLVM_TOOLS   = ${SD_LLVM_TOOLS}')

   if env.PathExists("${SD_LLVM_BIN}"):
      env.Replace(SD_LLVM_BIN = env.subst("${SD_LLVM_BIN}" + os.sep))
      env.Replace(SD_LLVM_TOOLS = env.subst("${SD_LLVM_TOOLS}" + os.sep))
   else:
      env.PrintError("${SD_LLVM_BIN} does not exist")
      raise RuntimeError, "SD_LLVM_BIN does not exist"
      raise RuntimeError, "SD_LLVM_BIN does not exist"
      exit(1)
      
   LLVMFlags = "-fuse-baremetal-sysroot -fuse-ld=qcld -mfloat-abi=softfp -target armv7m-none-eabi -fno-vectorize-loops -fshort-enums -fno-short-wchar -ffunction-sections -fdata-sections -fomit-frame-pointer -DRVDS_LIB_COMPAT -Werror"
   
   # Turn off argument unused during compilation for now"
   LLVMFlags = LLVMFlags + " -Wno-unused-parameter -Wno-sign-compare -Wno-gcc-compat"
   
   LLVMFlags = LLVMFlags + " -D_HAVE_STRING_ARCH_strtok_r -D_HAVE_STRING_ARCH_strpbrk"

   # Add ARMCC predefined macro
   LLVMFlags += " -D__TARGET_CPU_CORTEX_M3"
   
   # SD_LLVM C compiler - Thumb
   env.Replace(TCC = "${SD_LLVM_BIN}clang${EXE_EXT} -mthumb " + LLVMFlags)
 
   # SD_LLVM C compiler - Arm
   env.Replace(ARMCC = "${SD_LLVM_BIN}clang${EXE_EXT} -marm " + LLVMFlags)
   
   if ('BUILD_BOOT_CHAIN' not in env) and ('BUILD_TOOL_CHAIN' not in env):
      env.Replace(CC = env['TCC'])
     
   # SD_LLVM C++ Compiler - Thumb
   env.Replace(TCXX = "${SD_LLVM_BIN}clang++${EXE_EXT} -mthumb -fno-exceptions " + LLVMFlags)
   
   # SD_LLVM C++ Compiler - Arm
   env.Replace(ARMCXX = "${SD_LLVM_BIN}clang++${EXE_EXT} -marm -fno-exceptions " + LLVMFlags)
   
   env.Replace(CXX = env['TCXX'])

   # SD_LLVM Assembler
   env.Replace(AS = "${SD_LLVM_BIN}clang${EXE_EXT} -xassembler -fintegrated-as -c")

   # SD_LLVM ARM Library Archiver
   env.Replace(AR = "${SD_LLVM_TOOLS}arm-ar${EXE_EXT}")

   # SD_LLVM ARM Linker
   env.Replace(LINK = "${SD_LLVM_BIN}clang++${EXE_EXT}")

   env.Replace(HEXTOOL = "")
   env.Replace(BINTOOL = "")

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))   
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE} -g")  
   
   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))   
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")  

   #----------------------------------------------------------------------------
   # Processor architecture options
   #----------------------------------------------------------------------------
   env.Replace(ARM_CPU_CMD = "-mcpu")                   # ARM CPU specification
   env.Replace(ARM_ASM = "-D_ARM_ASM_")                 # ARM assembly language selection
   env.Replace(ARM = "-DT_ARM -D__ARMEL__ -D__RVCT__")  # ARM processor target
   env.Replace(ARM_CPU_ARM7 = "ARM7TDMI-S")
   env.Replace(ARM_CPU_ARM9 = "arm926ej-s")
   env.Replace(ARM_CPU_ARM11 = "ARM1136J-S")
   env.Replace(ARM_CPU_SPARROW = "Cortex-A5")
   env.Replace(ARM_CPU_SCORPION = "7")   
      
   if 'BUILD_BOOT_CHAIN' in env:
      env.Replace(ARM_CPU_SCORPION = "6T2")

   if 'PROC' in env:
      if env['PROC'] in ['arm7']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM7")
      elif env['PROC'] in ['arm9']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM9")
      elif env['PROC'] in ['arm11']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM11")
      elif env['PROC'] in ['scorpion']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_SCORPION")
      elif env['PROC'] in ['sparrow']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_SPARROW")
   else:
      env.Replace(ARM_CPU = "$ARM_CPU_CMD CPU_UKNOWN")

   env.Replace(ARMCC_CPU = "${ARM_CPU} ${ARM}")
   env.Replace(ARMASM_CPU = "${ARM_CPU} ${ARM} ${ARM_ASM}")

   #----------------------------------------------------------------------------
   # NOTE:
   # If software stack checking support is enabled, FEATURE_STACK_CHECK must be
   # #defined in the cust*.h file to enable the required common services support.
   #
   # The following APCS options are obsolete in the ARM SDT 2.50 tools:
   #   PCSZ
   #   FRAMEPTR
   #   REENTRNT
   #----------------------------------------------------------------------------

   #----------------------------------------------------------------------------
   # Compiler/Linker/Tools output options
   #----------------------------------------------------------------------------
   env.Replace(ARMCC_OUTPUT_CMD = "-o")
   env.Replace(ARMCC_COMPILE_CMD = "-c")
   env.Replace(ARMCC_PREPROCESS_CMD = "-E -xc")
   env.Replace(ARMCC_ASMOUT_CMD = "-S")
   env.Replace(ARMCC_ASM_CMD = "--asm")
   env.Replace(ARMCC_INTERLEAVE_CMD = "--interleave")
   env.Replace(ARMCC_STDC99_CMD = "-std=gnu99")
   env.Replace(ARMCC_DOLLAR_CMD = "")
   env.Replace(ARMAS_OUTPUT_CMD = "-o")
   env.Replace(ARMAS_LIST_CMD = "--list")
   env.Replace(ARMAR_LIB_REPLACE = "-r")
   env.Replace(ARMAR_LIB_QUITE = "-c")
   env.Replace(ARMLD_OUTPUT_CMD = "-o")
   env.Replace(ARMLD_BM_FLAGS =     "-target armv7m-none-eabi -fuse-baremetal-sysroot -fuse-ld=qcld -v")
   env.Replace(ARMLD_ELF_CMD =      "-Wl,-static")
   env.Replace(ARMLD_LIST_CMD =     "-Wl,-Map")
   env.Replace(ARMLD_SYMDEFS_CMD =  "-Wl,--symdef-file")
   env.Replace(ARMLD_SCATTER_CMD =  "-Wl,-T")
   env.Replace(ARMLD_REMOVE_CMD =   "-Wl,--gc-sections")
   env.Replace(ARMLD_NOREMOVE_CMD = "-Wl,--no-gc-sections")
   env.Replace(ARMBIN_BIN_CMD = "--bin")
   env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = "-o")
   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")
   env.Replace(ARMHEX_OUTPUT_CMD = "-o")
   env.Replace(ARMHEX_I32_CMD = "--i32")
   

   #----------------------------------------------------------------------------
   # Compiler/assembler debug options
   #----------------------------------------------------------------------------
   if 'USES_NO_DEBUG' in env:
      env.Replace(ARM_DBG = "")
   else:
      env.Replace(ARM_DBG = "-g -gdwarf-2")
  
   env.Replace(ARM_PCSZ = "32")                       # Program counter (PC) size (bits)
   env.Replace(ARM_FRAMEPTR = "nofp")                 # No frame pointer
   env.Replace(ARM_FLOATPNT = "softfp")               # Use software floating point library
   env.Replace(ARM_INTRWORK = "interwork")            # ARM/Thumb interworking support
   env.Replace(ARM_NARROW   = "narrow")               # Use caller parameter narrowing
   env.Replace(ARM_REENTRNT = "nonreentrant")         # No support for re-entrancy
   env.Replace(ARM_NOROPI = "noropi")
   env.Replace(ARM_NORWPI = "norwpi")
   env.Replace(ARMCC_APCS = "")
   env.Replace(ARMCXX_APCS = "")
   
   #----------------------------------------------------------------------------
   # Compiler optimization options
   #----------------------------------------------------------------------------
   env.Replace(ARM_OPT_TIME = " ")
   env.Replace(ARM_OPT_SPACE = "-Ospace")
   env.Replace(ARM_OPT_3 = "-Osize")
   env.Replace(ARM_OPT_2 = "-Osize")
   env.Replace(ARM_OPT_1 = "-O1")
   env.Replace(ARMCC_OPT = "${ARM_OPT_SPACE} ${ARM_OPT_2}")      
   env.Replace(ARMCXX_OPT = "${ARM_OPT_SPACE} ${ARM_OPT_3} ${ARM_OPT_2}")
   env.Replace(ARMASM_OPT = "${ARM_OPT_1}")
  
   #----------------------------------------------------------------------------
   #  Stack Protection
   #----------------------------------------------------------------------------
   if 'USES_STACK_PROTECTOR' in env:
      env.Replace(STKPRO = "-fstack-protector")

   #----------------------------------------------------------------------------
   # Common defines
   #----------------------------------------------------------------------------
   env.Replace(BUILD_CCFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" '\
      '-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 '\
      '-DGPL_ENVIRONMENT=2 -D__FILENAME__=\\"$SOURCE.file\\" ')
      
   env.Replace(BUILD_ASMFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" -D__FILENAME__=\\"$SOURCE.file\\" ')

   env.Replace(INCPREFIX = "-I")
   env.Replace(CPPDEFPREFIX  = "-D")
   env.Replace(OBJSUFFIX = ".o")

   env.Replace(LIBPREFIX = "")
   env.Replace(LIBSUFFIX = ".lib")
   env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
   env.Replace(LIBLIKNPREFIX = "")
   env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
   env.Replace(LIBDIRPREFIX = "-L ")

   env.Replace(PROGPREFIX = "")
   env.Replace(PROGSUFFIX = ".elf")

   # Max cmd line length
   # On a cmd sh for XP or newer max comand line is 8191, on 2000 and NT is 2047
   # Using CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables

   if env['PLATFORM'] in ['windows', 'win32']:
      env.Replace(MAXLINELENGTH = 32000)
   elif env['PLATFORM'] in ['cygwin']:
      dyn_maxlinelength = 32000 - len(str(os.environ))
      if dyn_maxlinelength < 2000:
         dyn_maxlinelength = 2000
      env.Replace(MAXLINELENGTH = dyn_maxlinelength)
   else:
      env.Replace(MAXLINELENGTH = 100000)
      
   # WARNING!! cmd line can't have any shell operation such as "<", ">" or "|"
   # If it does we will default back os shell and its cmd length restrictions 
   # will be in effect.

   # Ecapsulate within quotes so we retain the space after --via
   env.Replace(TEMPFILEPREFIX = "\"--via \"")
   env.Replace(TEMPFILESUFFIX = "")
   
   # Add a way to expand link files from a list...
   env.Replace(LISTFILEPREFIX = "\"--via \"")
   env.Replace(LISTFILESUFFIX = "")
   env.Replace(_LISTFILES = '${_concat(LISTFILEPREFIX, LISTFILES, LISTFILESUFFIX, __env__)}')     
      
   # Init FLAGS      
   env.Replace(CCFLAGS = "$ARMCC_CPU $ARMCC_APCS $ARMCC_CODE $ARM_DBG $ARMCC_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $STKPRO ")
   env.Replace(CXXFLAGS = "$ARMCC_CPU $ARMCXX_APCS $ARMCXX_CODE $ARM_DBG $ARMCXX_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $STKPRO ")
   env.Replace(ASMFLAGS = "$ARMASM_CPU $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS $BUILD_ASMFLAGS $STKPRO ")
   env.Replace(ASFLAGS = "$ARM_CPU $ARMCC_APCS $ARM_DBG ")
   env.Replace(ARFLAGS = "$ARMAR_LIB_REPLACE $ARMAR_LIB_QUITE")
   env.Replace(LINKFLAGS = "$ARMLD_ELF_CMD $ARMLD_MAP_CMD $ARMLD_LIST_CMD " \
      "${TARGET.base}.map $ARMLD_SYMDEFS_CMD ${TARGET.base}.sym $ARMLD_SCATTER_CMD " \
      "${TARGET.base}.scl $ARMLD_NOREMOVE_CMD $LFLAGS")

   CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

   CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])
   
   env.Replace(ARCOM =      "${TEMPFILE('$AR $ARFLAGS $TARGET $SOURCES')}")
   env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS  $_LIBDIRFLAGS  $_LISTFILES -Wl,--start-group $SOURCES $_LIBFLAGS $CRTBEGIN -Wl,--end-group  -Wl,--wrap -Wl,malloc -Wl,--wrap -Wl,free -Wl,--wrap -Wl,realloc -Wl,--wrap -Wl,calloc $ARMLD_OUTPUT_CMD $TARGET')}")

   env.Replace(ASCPPCOM =  "$CC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS $ARMCC_PREPROCESS_CMD $SOURCES $ARMCC_OUTPUT_CMD ${TARGET}.pp")
   env.Replace(ASASCOM =   "$AS $ASFLAGS $_CPPINCFLAGS ${TARGET}.i $ARMAS_OUTPUT_CMD $TARGET")
   
   AsmPreprocessAction = SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
   AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
   AsmAssemblyAction = SCons.Action.Action('${TEMPFILE("$ASASCOM")}', '$ASASCOMSTR')

   env.Replace(ASCOM = [
      AsmPreprocessAction,                   # C pre-process assembly files
      AsmCleanAction,                        # Removed junck generated by C preprocess
      AsmAssemblyAction,                     # Do the actuall assymbly
      #env.GetDeleteAction('${TARGET}.i'),   # Do not delete here needed to debug on target
      #env.GetDeleteAction('${TARGET}.pp'),
      #env.GetDeleteAction('${TARGET}.lst'),
   ])
   
   # avoid having a rallib run for no reason, scons defaults does this, but is maybe casing issues
   # as there is no actual ranlibcom
   env.Replace(RANLIBCOM = "")
   env.Replace(RANLIBCOMSTR = "")

   #============================================================================
   # COMMON CC ADDITIONS
   #============================================================================
   comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

   if env.PathExists(comcc_addt):
      env.LoadToolScript(comcc_addt)
      
   #============================================================================
   # Other options
   #============================================================================
   

   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   sd_llvm_product = None
   
   if env.PathExists("${BUILD_SCRIPTS_ROOT}/sd_llvm_product.py"):
      sd_llvm_product = "${BUILD_SCRIPTS_ROOT}/sd_llvm_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/sd_llvm_product.py"):
      sd_llvm_product = "${BUILD_MS_ROOT}/sd_rvct_product.py"
   
   if sd_llvm_product is not None:
      env.LoadToolScript(sd_llvm_product)
   
   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   sd_llvm_oem = "${BUILD_SCRIPTS_OEM_ROOT}/sd_llvm_oem.py"
   
   if env.PathExists(sd_llvm_oem):
      env.LoadToolScript(sd_llvm_oem)
 
