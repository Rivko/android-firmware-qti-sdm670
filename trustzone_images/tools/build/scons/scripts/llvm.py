#===============================================================================
#
# ARM tool rules
#
# GENERAL DESCRIPTION
#    ARM 5.0 and greater Tools definitions
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/llvm.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#03/28/17  pdosi   added support for 3.9 LLVM and minor cleanup.
#06/25/16  pdosi   added the  $LINKFLAGSAN to add flags in between start and end tags
#                  linking line.
#04/19/16  pdosi   added -fno-vectorize-loops for 32 bit images.
#03/04/16  pdosi   added support for QCLD and 3.7 LLVM.
#01/28/14  ygui    add new feature of switch 32bit or 64bit compiler conditionally
#===============================================================================
import os
import string
import re
import SCons.Action
from SCons.Script import *
import SCons

# Assembly compile flags (note first pass is through armcc using -E option then passed to armas, see
# build rule below
# standard compile flags
build_dflags = '-DCUST_H=\\"${CUST_H}\\"' \
      ' -DBUILD_TARGET=\\"${BUILD_ID}\\" -DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\"'
      
#global variable for regex to compute llvm version.
LLVM_VERSION_REGEX = re.compile("(([0-9]+)\\.([0-9])+(\\.([0-9]+))*)[/\\\\]bin")
   
#------------------------------------------------------------------------------
# RDirs
# Makes sure the inc paths are as provided, don't let scons mess with them.
#------------------------------------------------------------------------------
def RDirs (path_list):
   #path_list = [str(path).replace('\\', '/') for path in path_list]
   return path_list

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
   hdeptree = env.GetOption('hdeptree')

   if preprocess or hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
      env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))

   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))

   if hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))



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


   return (tgt, src)

#------------------------------------------------------------------------------
# Get LLVM version
#------------------------------------------------------------------------------  
def get_llvm_version(llvm_bin):
   version = os.environ.get("LLVM_VERSION")
   if version != None:
      version_numbers = version.split(".")
      assert len(version_numbers) >= 2, \
            "Invalid LLVM_VERSION '{}'".format(version)
      return (int(version_numbers[0]), int(version_numbers[1]))

   match = LLVM_VERSION_REGEX.search(llvm_bin)
   if match:
      version = (int(match.group(2)), int(match.group(3)))

   return version

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('arm')

def generate(env):
   #============================================================================
   # Special init for our tools and special dependency in CUST_H
   #============================================================================
   # Replace Static Object Emitter
   
   # adding a scanner for '.s' files (with lower case .s) breaks the scons standard for what
   # file extension pertains to what file "type". By scons standard, .s files are 
   # NOT preprocessed, whereas .S, .spp, and .sx ARE preprocessed. However, our
   # convention is to preprocess .s files as well. The "real" fix would be to
   # rename all .s files needing preprocessing to .S, but that's more trouble
   # than it's worth. Instead, we're actively breaking the scons convention
   # and documenting it
   
   env.Append(CPPSUFFIXES=['.inc','.s'])
   env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)
   SCons.Tool.SourceFileScanner.add_scanner('.s', SCons.Defaults.CScan)
   env.Replace(CC_DFLAGS = build_dflags)

   # Standard include paths always searched
   env.Append(CPPPATH = [
      ".",
      "${INC_ROOT}/build/cust",
      "${INC_ROOT}/build/ms",
   ])


   #============================================================================
   #                             TOOL DEFINITIONS
   #============================================================================
   # The following environment variables must be defined prior to using this make
   # file: ARMHOME, ARMINC, QCTOOLS. In addition the PATH must be updated for
   # the ARM tools.


   #----------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #----------------------------------------------------------------------------

   if os.environ.has_key('LLVMBIN'):
      env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   else:
      print ("-------------------------------------------------------------------------------")
      print ("*** LLVMBIN is not defined on this environment")
      print ("*** Please set up the LLVM enviroment")
      print ("-------------------------------------------------------------------------------")
      raise RuntimeError, "LLVM is not defined on this environment"

   env.PrintInfo('LLVMBIN   = ${LLVMBIN}')

   if env.PathExists("${LLVMBIN}"):
      LLVMBIN = env.RealPath("${LLVMBIN}", posix=False)
      if env['PLATFORM'] in ['windows', 'win32']:
         LLVMBIN = LLVMBIN + "\\"
      else:
         LLVMBIN = LLVMBIN + "/"
   else:
     raise RuntimeError, "${LLVMBIN} not found using system environment."

   arm_ver = 6
   # Setup the ARM major version number
   env.Replace(ARMTOOLSVERSION = arm_ver)

   LLVMTOOLPATH = env.RealPath("${LLVMTOOLPATH}", posix=False)

   env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   env.Replace(LLVMLIB = os.environ['LLVMLIB'])
   env.Replace(LLVMINC = os.environ['LLVMINC'])
   env.Replace(LLVM32INC = os.environ['LLVM32INC'])
   env.Replace(LLVMTOOLPATH = os.environ['LLVMTOOLPATH'])

   env.PrintInfo('LLVMBIN          = ${LLVMBIN}')
   env.PrintInfo('LLVMLIB          = ${LLVMLIB}')
   env.PrintInfo('LLVMINC          = ${LLVMINC}')
   env.PrintInfo('LLVM32INC        = ${LLVM32INC}')
   env.PrintInfo('LLVMTOOLPATH     = ${LLVMTOOLPATH}')
   #----------------------------------------------------------------------------
   #check for LLVM and QCLD Here
   #---------------------------------------------------------------------------- 
   LinkByQCLD=False
   llvmbin=LLVMBIN
   LinkByGNU=True
   llvmver=''

   if llvmbin.find("Bullseye")!= -1 :
      llvmver = get_llvm_version(os.environ['BULLSEYE_PATH'])
      linkerPath=os.environ['BULLSEYE_PATH']
   else:
      llvmver=get_llvm_version(llvmbin)
      linkerPath=LLVMBIN
    
   #----------------------------------------------------------------------------
   # GNU path definitions - begin
   #----------------------------------------------------------------------------
   if env['PLATFORM'] in ['windows', 'win32']:
      if os.environ.has_key('GNULIBC'):
         env.Replace(GNULIBC = os.environ['GNULIBC'])
      else:
         print ("-------------------------------------------------------------------------------")
         print ("*** GNULIBC is not defined on this environment")
         print ("*** Please set up the GNU enviroment")
         print ("-------------------------------------------------------------------------------")
         raise RuntimeError, "GNULIBC is not defined on this environment"

      if os.environ.has_key('GNUTOOL'):
         env.Replace(GNUTOOL = os.environ['GNUTOOL'])
      else:
         print ("-------------------------------------------------------------------------------")
         print ("*** GNUTOOL is not defined on this environment")
         print ("*** Please set up the GNU enviroment")
         print ("-------------------------------------------------------------------------------")
         raise RuntimeError, "GNUTOOL is not defined on this environment"

   if os.environ.has_key('GNUROOT'):
      env.Replace(GNUROOT = os.environ['GNUROOT'])
   if os.environ.has_key('GNUARM7'):
      env.Replace(GNUARM7 = os.environ['GNUARM7'])
   if os.environ.has_key('MUSLPATH'):
      env.Replace(MUSLPATH = os.environ['MUSLPATH'])
   if os.environ.has_key('MUSL32PATH'):
      env.Replace(MUSL32PATH = os.environ['MUSL32PATH'])


   env.PrintInfo('GNULIBC          = ${GNULIBC}')
   env.PrintInfo('GNUTOOL          = ${GNUTOOL}')
   env.PrintInfo('GNUROOT          = ${GNUROOT}')
   env.PrintInfo('GNUARM7          = ${GNUARM7}')
   env.PrintInfo('MUSLPATH         = ${MUSLPATH}')
   env.PrintInfo('MUSL32PATH       = ${MUSL32PATH}')

   GNULIBC  = env.RealPath("${GNULIBC}", posix=False)
   GNUTOOL  = env.RealPath("${GNUTOOL}", posix=False)
   GNUROOT  = env.RealPath("${GNUROOT}", posix=False)
   GNUARM7  = env.RealPath("${GNUARM7}", posix=False)
   MUSLPATH = env.RealPath("${MUSLPATH}", posix=False)
   MUSLPATH = env.RealPath("${MUSL32PATH}", posix=False)

   #----------------------------------------------------------------------------
   # GNU path definitions - end
   #----------------------------------------------------------------------------

   if env['PROC'] == 'A53_64':
      do_64 = True
   else:
      do_64 = False

   llvm_flags_tz=[]
   
   if ARGUMENTS.get('deterministic',0) == 1:
      llvm_flags_tz.append(' -D__TIME__=\\"ss:mm:hh\\"')
      llvm_flags_tz.append(' -D__DATE__=\\"mmmddyyyy\\"')
      llvm_flags_tz.append(' -D__FILE__=\\"${SOURCE.file}\\"')
      llvm_flags_tz.append('-Wno-builtin-macro-redefined')
   
   LLVMFlags = ""
   
   ASM_DBG = ""
   if 'USES_NO_DEBUG' in env:
      env.Replace(ARM_DBG = "")

   use_clang_assembler = True
   '''Flags for 3.9.x 64 bit and 4.0.x 64 bit
       -target aarch64-linux-gnu , -mcpu=cortex-a53, -nostdinc, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare, -werror
     Flags for 3.7.x 64 bit
       -target aarch64-linux-gnu , -mcpu=cortex-a53, -nostdinc, -mllvm -aarch64-strict-align,-Wno-tautological-constant-out-of-range-compare,-mllvm -enable-global-merge=false,
       -mllvm -inline-threshold-multicaller-size=20
     Flags for 3.5.x 64 bit
       -target aarch64-linux-gnu, -mcpu=cortex-a53, -nostdinc, -mllvm -aarch64-strict-align,-Werror,-mllvm -enable-global-merge=false
     Flags for 4.0.x 32 bit
       -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare,-werror,-mcpu=cortex-a7
     Flags for 3.9.x 32 bit
       -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare,-mllvm -post-RA-scheduler=false,-mcpu=cortex-a7
     Flags for 3.7.x 32 bit
        -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare,-mllvm -post-RA-scheduler=false,-mcpu=cortex-a7
       -mllvm -inline-threshold-multicaller-size=20, -mllvm -enable-android-compat
     Flags for 3.5.x 32 bit
        -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,mllvm -enable-android-compat,-mcpu=krait,-mfpu=none,-fno-vectorize-loops, -Werror
   '''
   if do_64:
      llvm_flags_tz.extend([
           "-target aarch64-linux-gnu",                                # Generate code for the given target.           
           "-mcpu=cortex-a53",                                         # Specify CPU.  
           "-nostdinc",                                                # Prevent the preprocessor from searching any of the default system header directories.
       ])
      if llvmver >= (3, 9):
         llvm_flags_tz.extend([
               "-mno-unaligned-access",                                # Force all memory accesses to be aligned.      
               "-Wno-tautological-constant-out-of-range-compare",      # This is a diagnostic flag by clang.
               "-Werror",
           ])
         LinkByQCLD = True
      elif llvmver >= (3, 7):
         llvm_flags_tz.extend([
               "-mllvm -aarch64-strict-align",                         # Force all memory accesses to be aligned. 
               "-mllvm -inline-threshold-multicaller-size=20",         # This flag limits the aggression of inliner.
               "-Wno-tautological-constant-out-of-range-compare",      # This is a diagnostic flag by clang.
               "-mllvm -enable-global-merge=false",                    # Merge globals into a single linkage to save space.
           ])
         LinkByQCLD = True
      elif llvmver >= (3, 5):
         llvm_flags_tz.extend([
               "-Werror",                                              # Enable warnings as errors.
               "-mllvm -aarch64-strict-align",                         # Force all memory accesses to be aligned.
               "-mllvm -enable-global-merge=false",                    # Merge globals into a single linkage to save space.
           ])
      else:
           assert False, "Unsupported LLVM version: {}".format(llvmver)
      LLVMFlags = " ".join(llvm_flags_tz)
      if use_clang_assembler:
         if env['PLATFORM'] in ['windows', 'win32']:
            env.Replace(AS = LLVMBIN + "clang${EXE_EXT} -no-integrated-as --sysroot=" + GNULIBC + " --gcc-toolchain=" + GNUTOOL + " -target aarch64-linux-gnu -x assembler -c ")
         else:
            env.Replace(AS = GNUROOT + "/bin/aarch64-none-elf-as${EXE_EXT} -c ")
      env.Replace(ARM_CPU = "8-A.64") # replaced again below
      env.Replace(TCC = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
      env.Replace(TCPP = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
      if 'USES_NO_DEBUG' not in env:
       env.Replace(ARM_DBG = "-g ")
       ASM_DBG = "-g "
   else:
      do_32 = True
      llvm_flags_tz.extend([
        "-target arm-linux-gnueabi",                                   # Generate code for the given target.  
        "-mfloat-abi=softfp",                                          # Specifies which floating-point ABI to use.
        "-mno-unaligned-access",                                       # Force all memory accesses to be aligned.       
      ])
      if llvmver >= (4, 0):
         llvm_flags_tz.extend([
           "-mcpu=cortex-a7",                                          # Specify CPU.  
           "-Wno-tautological-constant-out-of-range-compare",          # This is a diagnostic flag by clang.
           "-Werror",
         ])
         LinkByQCLD = True
      elif llvmver >= (3, 9):
         llvm_flags_tz.extend([
           "-mcpu=cortex-a7",                                          # Specify CPU.  
           "-Wno-tautological-constant-out-of-range-compare",          # This is a diagnostic flag by clang.
           "-mllvm -post-RA-scheduler=false",                          # Workaround for  issue in the scheduling algorithm.
           "-Werror",
         ])
         LinkByQCLD = True
      elif llvmver >= (3, 7):
         llvm_flags_tz.extend([
           "-mllvm -enable-android-compat",                            # Use hardware div which is available on QC hardware.
           "-mllvm -post-RA-scheduler=false",                          # Workaround for issue in the scheduling algorithm.
           "-mllvm -inline-threshold-multicaller-size=20",             # This flag limits the aggression of inliner.
           "-mcpu=cortex-a7",                                          # Specify CPU.     
           "-Wno-tautological-constant-out-of-range-compare",
         ])
         LinkByQCLD = True
      elif llvmver >= (3, 5):
         llvm_flags_tz.extend([
           "-mllvm -enable-android-compat",                            # Use hardware div which is available on QC hardware.
           "-mcpu=krait2",                                             # Set krait or scorpion specific optimizations.
           "-mfpu=none",                                               # Specify floating-point hardware (or hardware emulation). 
           "-fno-vectorize-loops",                                     # Disable Qualcomm's loop vectorization.    
           "-Werror",
         ])
      else:
         assert False, "Unsupported LLVM version: {}".format(version)
      LLVMFlags = " ".join(llvm_flags_tz)
      if use_clang_assembler:
         env.Replace(AS = LLVMBIN + "clang${EXE_EXT} -target arm-linux-gnu -mfloat-abi=softfp -mcpu=cortex-a8 -x assembler -allow-integrated-as-for-asm-input -c ")
      env.Replace(ARM_CPU = "8-A.32.no_neon") #replaced again below
      env.Replace(TCC = LLVMBIN + "clang${EXE_EXT} -mthumb " + LLVMFlags)
      env.Replace(TCPP = LLVMBIN + "clang${EXE_EXT} -mthumb " + LLVMFlags)
      if 'USES_NO_DEBUG' not in env:
         env.Replace(ARM_DBG = "-g ")
         ASM_DBG = "-g "

   # ARM SDT ARM 32-bit inst. set ANSI C compiler
   env.Replace(ARMCC = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
   env.Replace(CC = env['TCC'])
   env.Replace(CXX = env['TCPP'])
   env.Replace(ARMCPP = LLVMBIN + "clang${EXE_EXT} " + LLVMFlags)
   env.Replace(ARMCXX = env['ARMCPP'])

   # ARM SDT library archiver
   env.Replace(AR = "${LLVMTOOLPATH}/arm-ar")
   # Expose the NM tool for scripts to use for extracting symbol/variable
   # names/sizes/other details
   env.Replace(NMBIN = "${LLVMTOOLPATH}/arm-nm")
   LinkByGNU = True

   if LinkByGNU:
      if do_64:
         if env['PLATFORM'] in ['windows', 'win32']:
            env.Replace(LINK = GNUROOT + "/bin/aarch64-linux-gnu-ld")
         else:
            env.Replace(LINK = GNUROOT + "/bin/aarch64-none-elf-ld")
      else:
         env.Replace(LINK = GNUARM7 + "/bin/arm-linux-gnueabihf-ld")
   if LinkByQCLD:
      if do_64:
         env.Replace(LINK = linkerPath + "/ld.qcld -march aarch64 -z now ")
      else:
         env.Replace(LINK = linkerPath + "/ld.qcld -march arm -z now ")
   
   env.Replace(NM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/nm.py"))
   env.Replace(NM = "${PYTHONCMD} ${NM_FILE}")
  
   sym_bld1 = Builder(action='${LLVMTOOLPATH}/arm-nm $SOURCE > $TARGET', suffix='.tempsym', src_suffix='.elf')
   sym_bld2 = Builder(action='$NM -f $SOURCE -o $TARGET',suffix='.sym',src_suffix='.tempsym')
   env.Append(BUILDERS = {'_GetSymbol1' : sym_bld1})
   env.Append(BUILDERS = {'_GetSymbol2' : sym_bld2})
   def GetSymbol(env,target, source,*args,**kwargs):
     step1 = env._GetSymbol1(source=source,*args,**kwargs)
     return env._GetSymbol2(source=step1, target=target,*args,**kwargs)
   env.AddMethod(GetSymbol,'GetSymbol')
  
   env.Replace(BINTOOL = "${LLVMTOOLPATH}/arm-elfcopy")

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")
   if use_clang_assembler:
      env.Append(PPASM = " -g")

   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")

   #----------------------------------------------------------------------------
   # Processor architecture options
   #----------------------------------------------------------------------------
   env.Replace(ARM_ASM = "-D_ARM_ASM_")                 # ARM assembly language selection

   env.Replace(ARM = "-DT_ARM -D__ARMEL__")  # ARM processor target

   env.Replace(ARM_ARCH = "-target armv8a-arm")

   if 'PROC' in env:
      if do_64:
         env.Replace(ARM_CPU = "8-A.64")
      elif do_32:
         env.Replace(ARM_CPU = "8-A.32.no_neon")
      elif env['PROC'] in ['arm7']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM7")
      elif env['PROC'] in ['arm9']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM9")
      elif env['PROC'] in ['arm11']:
         env.Replace(ARM_CPU = "$ARM_CPU_ARM11")
      elif env['PROC'] in ['scorpion']:
         env.Replace(ARM_CPU = "$ARM_CPU_SCORPION")
      elif env['PROC'] in ['sparrow']:
         env.Replace(ARM_CPU = "$ARM_CPU_SPARROW")
      elif env['PROC'] in ['cortexA7']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_CORTEXA7")
      elif env['PROC'] in ['armv8-64']:
         env.Replace(ARM_CPU = "8-A.64")
   else:
      env.Replace(ARM_CPU = "$ARM_CPU_CMD=CPU_UKNOWN")

# TODO: ARM_CPU is defined above.  Where is the input for this?  Need
#       to switch between 64 bit and 32 bit for the same CPU.  These
#       arguments should depend on the CPU being A53/A57 for 8994 and
#       8916

   env.Replace(ARMCC_CPU = "-mcpu=cortex-a53") #TODO:translate QSP.no_neon.no_vfp -- no_unaligned_access
   #env.Replace(ARMCC_ARCH = "${ARM_ARCH} ${ARM}")
   #env.Replace(ARMCC_ARCH = "")
   env.Replace(ARMASM_CPU = "--cpu ${ARM_CPU} ${ARM} ${ARM_ASM}")
   if use_clang_assembler:
      env.Replace(ARMASM_CPU = "${ARM} ${ARM_ASM}")

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
   env.Replace(ARMCC_PREPROCESS_CMD = "-E -xc++")
   env.Replace(ARMCC_ASMOUT_CMD = "-S")
   #env.Replace(ARMCC_ASM_CMD = "--asm")
   #env.Replace(ARMCC_INTERLEAVE_CMD = "--interleave")
   env.Replace(ARMAS_OUTPUT_CMD = "-o")
   #env.Replace(ARMAS_LIST_CMD = "--list")
   env.Replace(ARMAS_LIST_CMD = " ")

   env.Replace(ARMAR_LIB_REPLACE = "-r")
   env.Replace(ARMAR_LIB_QUITE = "-c")
   env.Replace(ARMLD_OUTPUT_CMD = "-o")
   env.Replace(ARMLD_ELF_CMD = "--elf")
   env.Replace(ARMLD_MAP_CMD = "--map")
   env.Replace(ARMLD_INFO_CMD = "--info")
   env.Replace(ARMLD_LIST_CMD = "--list")
   env.Replace(ARMLD_SYMBOLS_CMD = "--symbols")
   env.Replace(ARMLD_SYMDEFS_CMD = "--symdefs")
   env.Replace(ARMLD_SCATTER_CMD = "--scatter")
   env.Replace(ARMLD_REMOVE_CMD = "--remove")
   env.Replace(ARMLD_NOREMOVE_CMD = "--noremove")
   env.Replace(GNULD_REMOVE_CMD = "--gc-sections")
   env.Replace(GNULD_NO_REMOVE_CMD = "--no-gc-sections")
   env.Replace(ARMLD_CALLGRAPH_CMD = "--callgraph")
   env.Replace(ARMBIN_BIN_CMD = "-O binary")
   env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = " ")

   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")
   env.Replace(ARMHEX_OUTPUT_CMD = "-o")
   env.Replace(ARMHEX_I32_CMD = "--i32")


   #----------------------------------------------------------------------------
   # Compiler/assembler debug options
   #----------------------------------------------------------------------------
   # see above

   #----------------------------------------------------------------------------
   # ARM Procedure Call Standard (APCS) options
   #----------------------------------------------------------------------------

   env.Replace(ARM_PCSZ = "32")                       # Program counter (PC) size (bits)
   env.Replace(ARM_FRAMEPTR = "-fomit-frame-pointer")                 # No frame pointer
   env.Replace(ARM_FLOATPNT = "softfp")               # Use software floating point library
   env.Replace(ARM_INTRWORK = "-mthumb-interwork")            # ARM/Thumb interworking support

   #----------------------------------------------------------------------------
   # Compiler optimization options
   #----------------------------------------------------------------------------
   env.Replace(ARM_OPT_TIME = "-Otime")
   env.Replace(ARM_OPT_SPACE = "-Os")
   env.Replace(ARM_OPT_3 = "-Os")
   env.Replace(ARM_OPT_2 = "-Os")
   env.Replace(ARM_OPT_1 = "-Os")

   # ARM_OPT_STACK_GUARD must be left uninitalized or it will be clobbered
   env.Replace(ARMCC_OPT = "${ARM_OPT_TIME} ${ARM_OPT_SPACE} ${ARM_OPT_STACK_GUARD}")

   # the other opts found
   env.Replace(ARMCXX_OPT = "${ARM_OPT_SPACE} ${ARM_OPT_3} ${ARM_OPT_2} ${ARM_OPT_STACK_GUARD}")

   env.Replace(ARMASM_OPT = "${ARM_OPT_1}")

   #----------------------------------------------------------------------------
   # Compiler code generation options
   #----------------------------------------------------------------------------
   env.Replace(ARM_NOINLINE = "-fno-inline")
   env.Replace(ARM_SPLIT_SECTIONS = "-ffunction-sections")



   if env['MSM_ID'] in ['6695']:
      # --fpmode fast --littleend --no_inline --no_autoinline --split_sections --data_reorder
      env.Replace(ARMCC_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
         "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER}")
   else:
      env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_NOPT}")

   env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
      "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_LOOSE_IMPLICIT_CAST}")

   #----------------------------------------------------------------------------
   # Linker options
   #----------------------------------------------------------------------------

   #----------------------------------------------------------------------------
   # NOTE:
   # The -zap0 option is required due to a known ARM 2.50 compiler bug with
   # pointers to struct sub-fields within __packed structs
   #----------------------------------------------------------------------------

   env.Replace(BUILD_FLAGS = "-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 -DGPL_ENVIRONMENT=2")

   env.Replace(INCPREFIX = "-I")
   env.Replace(CPPDEFPREFIX  = "-D")
   env.Replace(OBJSUFFIX = ".o")

   env.Replace(LIBPREFIX = "")
   env.Replace(LIBSUFFIX = ".lib")
   env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
   env.Replace(LIBLIKNPREFIX = "")
   env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
   env.Replace(LIBDIRPREFIX = "--userlibpath ")
   if (LinkByGNU or LinkByQCLD) :
     env.Replace(LIBDIRPREFIX = "-L ")

   env.Replace(PROGPREFIX = "")
   env.Replace(PROGSUFFIX = ".elf")

   # max cmd line lenght
   # on a cmd sh for XP or newer max comand line is 8191, on 2000 and NT is 2047
   # usig CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables
   # we have seen a lot of issues with OS environment grater then 20k
   # (What is going on from MAKE world why is environment so large??)
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
   #env.Replace(TEMPFILEPREFIX = "\"--via \"")
   env.Replace(TEMPFILEPREFIX = "@")
   env.Replace(TEMPFILESUFFIX = "")

   # Add a way to expand link files from a list...
   env.Replace(LISTFILEPREFIX = "\"@\"")
   env.Replace(LISTFILESUFFIX = "")
   env.Replace(_LISTFILES = '${_concat(LISTFILEPREFIX, LISTFILES, LISTFILESUFFIX, __env__)}')

   # Init FLAGS
   env.Replace(CCFLAGS = "$ARMCC_ARCH $ARMCC_APCS $ARMCC_CODE $ARM_DBG $ARMCC_OPT $BUILD_FLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(CXXFLAGS = "$ARMCC_ARCH $ARMCXX_APCS $ARMCXX_CODE $ARM_DBG $ARMCXX_OPT $BUILD_FLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(ASCPPFLAGS = "$ARMCC_ARCH $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS")
   env.Replace(ASMFLAGS = "$ARMASM_CPU $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS")
   env.Replace(ASFLAGS = "--cpu $ARM_CPU $ARMCC_APCS " + ASM_DBG)
   if use_clang_assembler:
      env.Replace(ASFLAGS = " ")
   env.Replace(ARFLAGS = "$ARMAR_LIB_REPLACE $ARMAR_LIB_QUITE ")
   if ARGUMENTS.get('deterministic',1) == 1:
     env.Append(ARFLAGS= " -D ")
   env.Replace(LINKFLAGS = "$ARMLD_ELF_CMD $ARMLD_MAP_CMD $ARMLD_INFO_CMD sizes,totals $ARMLD_LIST_CMD " \
      "${TARGET.base}.map $ARMLD_SYMBOLS_CMD $ARMLD_SYMDEFS_CMD ${TARGET.base}.sym $ARMLD_SCATTER_CMD " \
      "${TARGET.base}.scl $ARMLD_NOREMOVE_CMD --any_placement=best_fit --sort=Alignment $LFLAGS")
   if LinkByGNU:
      UseARMLib = True
      env.Replace(LINKFLAGS = "-static -nostdlib -nodefaultlibs -no-undefined -T ${TARGET.base}.scl -Map ${TARGET.base}.map ")
      if UseARMLib:
         if do_64:
            env.Replace(LINKFLAGS2 = " $GNULD_REMOVE_CMD --sort-section=alignment ")
         else:
            env.Replace(LINKFLAGS2 = " $GNULD_REMOVE_CMD --sort-section=alignment ")
      else:
         env.Append(LINKFLAGS =  "-L $GNU_PATH/aarch64-linux-gnu/libc/usr/lib/aarch64-linux-gnu "\
                                 "-L $GNU_PATH/lib/gcc/aarch64-linux-gnu/4.8.3 ")
         env.Replace(LINKFLAGS2 = "--start-group -lc -lgcc -lgcc_eh $LINKFLAGSAN --end-group $GNULD_REMOVE_CMD --sort-section=alignment")
   if LinkByQCLD:
      UseARMLib = True
      env.Replace(LINKFLAGS = "-static -nostdlib -no-undefined -T ${TARGET.base}.scl -Map ${TARGET.base}.map ")
      if UseARMLib:
         if do_64:
            env.Replace(LINKFLAGS2 = " $GNULD_REMOVE_CMD")
         else:
            env.Replace(LINKFLAGS2 = " $GNULD_REMOVE_CMD")
      else:
         env.Append(LINKFLAGS =  "-L $GNU_PATH/aarch64-linux-gnu/libc/usr/lib/aarch64-linux-gnu "\
                                 "-L $GNU_PATH/lib/gcc/aarch64-linux-gnu/4.8.3 ")
         env.Replace(LINKFLAGS2 = "--start-group -lc -lgcc -lgcc_eh  $LINKFLAGSAN --end-group $GNULD_REMOVE_CMD --sort-section=alignment")
   if do_64:
     CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVMINC $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   else:
     CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVM32INC $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"

   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

   # TODO: need to remove CCFALGS from CXXCOM it is couseing a lot of warnings, a lot of files in MULTIMEDIA need to be fixed first
   #CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVMINC $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])

   env.Replace(ARCOM =      "${TEMPFILE('$AR $ARFLAGS $TARGET $SOURCES')}")
   env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS $_LIBDIRFLAGS $_LISTFILES $_LIBFLAGS  $SOURCES $ARMLD_OUTPUT_CMD $TARGET')}")
   if LinkByGNU:
     env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS $_LIBDIRFLAGS --start-group $_LISTFILES $_LIBFLAGS  $SOURCES $LINKFLAGSAN --end-group $LINKFLAGS2 $ARMLD_OUTPUT_CMD $TARGET')}")
     env.Append(CCFLAGS = "-DUSE_GNU_LD")

   env.Replace(ASCPPCOM =  "$CC $ASCPPFLAGS $AFLAGS $_CCCOMCOM $INCPATHS -I${LLVMINC} $ARMCC_PREPROCESS_CMD $SOURCES $ARMCC_OUTPUT_CMD ${TARGET}.pp")
   env.Replace(ASASCOM =   "$AS $ASFLAGS ${TARGET}.i $ARMAS_OUTPUT_CMD $TARGET")

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
   callgraph = env.GetOption('callgraph')

   if not (LinkByGNU or LinkByQCLD):
      env.Append(LINKFLAGS = " --force-scanlib")
   if callgraph:
      env.Append(LINKFLAGS = " $ARMLD_CALLGRAPH_CMD")

   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   llvm_product = None

   if env.PathExists("${BUILD_SCRIPTS_ROOT}/llvm_product.py"):
      llvm_product = "${BUILD_SCRIPTS_ROOT}/llvm_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/llvm_product.py"):
      llvm_product = "${BUILD_MS_ROOT}/llvm_product.py"

   if llvm_product is not None:
      env.LoadToolScript(llvm_product)

   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   llvm_oem = "${BUILD_SCRIPTS_OEM_ROOT}/llvm_oem.py"

   if env.PathExists(llvm_oem):
      env.LoadToolScript(llvm_oem)

   env.Tool('fastcpp')
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
