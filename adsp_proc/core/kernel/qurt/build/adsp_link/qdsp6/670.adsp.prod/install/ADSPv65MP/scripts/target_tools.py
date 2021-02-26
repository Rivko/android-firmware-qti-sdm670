#=============================================================================
#
#                                    target_tools.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#===============================================================================
#
# Kernel HEXAGON tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#===============================================================================

import sys
import re
import os
import string
import SCons.Action
from SCons.Script import *
from SCons.Platform import TempFileMunge

from qurtutils import version_compare

RDirsSCons = ""
RelPath = ""
NormPath = ""

if os.name == 'posix':
   default_root = '/pkg/qct/software/hexagon/releases/tools'
   earlyaccess_default_root = '/pkg/qct/software/hexagon/earlyaccess/volume2'
elif os.name == 'nt':
   default_root = os.path.realpath('C:/Qualcomm/HEXAGON_Tools')
   earlyaccess_default_root = default_root
else:
   print "Unknown platform!"
   sys.exit(0)

v56default=os.path.join(default_root,"8.0.13")
v60default=os.path.join(default_root,"8.0.13")
v61default=os.path.join(default_root,"8.0.13")
v62default=os.path.join(default_root,"8.0.13")
v65default=os.path.join(default_root,"8.1.04")
v66default=os.path.join(default_root,"8.2")
v56wlandefault=os.path.join(default_root,"8.0.13")

toolsver_whitelist = ["3.1", "5.0", "5.1", "6.2", "6.4", "7.0", "7.1", "7.2", "7.3", "7.4", "8.0", "8.1", "8.2"]
#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()

   # call defualt static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

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

   return (tgt, src)

def build_flags(env):
   if os.path.exists(os.path.realpath(env.subst('${QURT_TOP}/scripts/build_flags.py'))):
     cmd = "python " + env.subst('$QURT_TOP') +"/scripts/build_flags.py " + env.subst('$CONFIG_DIR')
     return os.popen(cmd).read().replace('\n','')
   else:
     return ''

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('target_tools')

#default to hexagon version 5, if param and Q6_TOOLS_ROOT not specified
def generate(env, target='staticv5'):
   #===============================================================================
   # figure out what tools set will be using
   #===============================================================================
   # The following environment variables must be defined prior to using this make
   # file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
   # the GCC tools.
   if env.subst('$TRGT') is not '':
      target=env.subst('$TRGT')

   if target=='mpqurt':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/mpqurt'
      hexagon_hw_ver=55
   elif target=='mpsecure':
      env['TEST_OBJS'] = True         # mpsecure is an internal target for advanced testing
                                      #  so we can force TEST_OBJS on for this target.
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/mpsecure'
      hexagon_hw_ver=55
   elif target=='staticv5':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/static'
      hexagon_hw_ver=5
   elif target=='modemv56':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv56'
      hexagon_hw_ver=55
   elif target=='modemv61':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv61'
      hexagon_hw_ver=61
   elif target=='modemv61v1':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv61v1'
      hexagon_hw_ver=61
   elif target=='modemv62':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv62'
      hexagon_hw_ver=62
   elif target=='modemv65':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv65'
      hexagon_hw_ver=65
   elif target=='ADSPv56MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv56MP'
      hexagon_hw_ver=55
   elif target=='ADSPv56z':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv56z'
      hexagon_hw_ver=55
   elif target=='ADSPv60MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv60MP'
      hexagon_hw_ver=60
   elif target=='computev60':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/computev60'
      hexagon_hw_ver=60
   elif target=='ADSPv60MP_ISV':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv60MP_ISV'
      hexagon_hw_ver=60
   elif target=='ADSPv61MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv61MP'
      hexagon_hw_ver=61
   elif target=='ADSPv62MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv62MP'
      hexagon_hw_ver=62
   elif target=='ADSPv62MP_ISV':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv62MP_ISV'
      hexagon_hw_ver=62
   elif target=='ADSPv62MP_SL':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv62MP_SL'
      hexagon_hw_ver=62
   elif target=='ADSPv60z':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv60z'
      hexagon_hw_ver=60
   elif target=='ADSPv65MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv65MP'
      hexagon_hw_ver=65
   elif target=='ADSPv66MP':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/ADSPv66MP'
      hexagon_hw_ver=66
   elif target=='sensorsv60':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sensorsv60'
      hexagon_hw_ver=60
   elif target=='sensorsv60_v1':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sensorsv60_v1'
      hexagon_hw_ver=60
   elif target=='sensorsv62':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sensorsv62'
      hexagon_hw_ver=62
   elif target=='sensorsv65':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sensorsv65'
      hexagon_hw_ver=65
   elif target=='sensorsv66':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/sensorsv66'
      hexagon_hw_ver=66
   elif target=='unitv56':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/unitv56'
      hexagon_hw_ver=55
   elif target=='wlanv56':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/wlanv56'
      hexagon_hw_ver=55
   elif target=='wlanv56_rom':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/wlanv56_rom'
      hexagon_hw_ver=55
   elif target=='computev65':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/computev65'
      hexagon_hw_ver=65
   elif target=='computev66':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/computev66'
      hexagon_hw_ver=66
   elif target=='modemv66':
      env['CONFIG_DIR'] = env.subst('$QURT_TOP')+'/config/modemv66'
      hexagon_hw_ver=66
   elif os.path.exists(target):
      env['CONFIG_DIR'] = os.path.realpath(target)
      hexagon_hw_ver=target[target.rindex("v") + len("v"):]
      target = os.path.basename(target)
   else:
      print target
      print "Unknown TARGET!!!"
      sys.exit(0)
   env.Replace(TRGT = target)

   if hexagon_hw_ver == 55:
      env.Replace(T32_SYS_CPU = "HexagonV55")
      if "v56" in target:
        env.Replace(T32_MCD_ARCH = "v56")
      else:
        env.Replace(T32_MCD_ARCH = "v55")
   if hexagon_hw_ver == 60:
      env.Replace(T32_SYS_CPU = "HexagonV60")
      env.Replace(T32_MCD_ARCH = "v60")
   if hexagon_hw_ver == 61:
      env.Replace(T32_SYS_CPU = "HexagonV61")
      env.Replace(T32_MCD_ARCH = "v61")
   #starting from v62, specify the 192 TLBs rev_id to simulator
   if hexagon_hw_ver == 62:
      env.Replace(T32_SYS_CPU = "HexagonV62")
      env.Replace(T32_MCD_ARCH = "v62h_1536")
   if hexagon_hw_ver == 65:
      env.Replace(T32_SYS_CPU = "HexagonV65")
      if target == 'modemv65':
        env.Replace(T32_MCD_ARCH = "v65h_1536")
      if target == 'computev65':
        env.Replace(T32_MCD_ARCH = "v65a_512")
      if target == 'sensorsv65':
        env.Replace(T32_MCD_ARCH = "v65b_1024")
      if target == 'ADSPv65MP':
        env.Replace(T32_MCD_ARCH = "v65b_512")
   if hexagon_hw_ver == 66:
      env.Replace(T32_SYS_CPU = "HexagonV66")
      if target == 'modemv66':
        env.Replace(T32_MCD_ARCH = "v66h_1536")
      if target == 'ADSPv66MP':
        env.Replace(T32_MCD_ARCH = "v66j_512")
      if target == 'sensorsv66':
        env.Replace(T32_MCD_ARCH = "v66b_1536")
      if target == 'computev66':
        env.Replace(T32_MCD_ARCH = "v66g_1024")

   env.Replace(INSTALL_DIR = env.subst('$INSTALL_DIR') + "/" + target)
   env.Replace(BUILD_DIR = env.subst('$BUILD_DIR') + "/" + target)

   env['BUILD_CONFIG_FLAGS'] = build_flags(env)

   #read toolsroot
   hex_tools_root = env.subst('$Q6_TOOLS_ROOT')
   #read version
   hex_tools_ver = env.subst('$Q6_TOOLS_VER')

   #specifying tools version is only valid if the root is given, ignore it otherwise
   #root value can either be directory where version directory is, or the root of the tools itself
   #
   if (hex_tools_root):
      if (hex_tools_ver):
         if os.path.exists(os.path.join(hex_tools_root,hex_tools_ver)):
            hex_tools_root = os.path.join(hex_tools_root,hex_tools_ver)
   else:
      hex_tools_ver = ""
      if hexagon_hw_ver == 55:
         if (target == 'wlanv56' or target == 'wlanv56_rom'):
            hex_tools_root = v56wlandefault
         else:
            hex_tools_root = v56default
      elif hexagon_hw_ver == 56:
         hex_tools_root = v56default
      elif hexagon_hw_ver == 60:
         hex_tools_root = v60default
      elif hexagon_hw_ver == 61:
         hex_tools_root = v61default
      elif hexagon_hw_ver == 62:
         hex_tools_root = v62default
      elif hexagon_hw_ver == 65:
         hex_tools_root = v65default
      elif hexagon_hw_ver == 66:
         hex_tools_root = v66default
      else:
         hex_tools_root = v56default

   if hexagon_hw_ver == 60:
      hex_tools_root = v60default
      hex_tools_ver = ''

   if not (hex_tools_ver):
      hex_tools_ver = os.path.split(hex_tools_root)[1]

   toolsver_match = re.search('^(\d+)\.(\d+).*', hex_tools_ver)
   if not toolsver_match:
      print("\nHexagon Tools version could not be determined by QuRT")
      Exit(1)
   elif toolsver_match.group(1) + "." + toolsver_match.group(2) not in toolsver_whitelist:
      if env.subst('$NO_WHITELIST') == 'False':
        print('\nError: Tools are untested for QuRT, please contact qurt.support\n')
        Exit(1)

   env.Replace(Q6_TOOLS_ROOT = hex_tools_root)
   hexagon_image_entry = os.environ.get('HEXAGON_IMAGE_ENTRY', '0x1e000000')

   q6version = 'v'+str(hexagon_hw_ver)
   hexagon_tools = "HEXAGON%s" % hexagon_hw_ver

   # init variables to environment
   env.Replace(HEXAGON_IMAGE_ENTRY = hexagon_image_entry)
   env.Replace(HEXAGON_IMAGE_ENTRY_SECTION = '.start')
   env.Replace(HEXAGON_TOOLS = hexagon_tools)
   env.Replace(HEXAGONTOOLS = hexagon_tools)
   env.Replace(Q6VERSION = q6version)

   if not os.path.exists(hex_tools_root):
      print("-------------------------------------------------------------------------------")
      print("*** HEXAGON tools are not present")
      print("*** "+hex_tools_root+" is an invalid path for tools")
      print("-------------------------------------------------------------------------------")
      print('')
      Exit(1)

   # Replace Static Object Emitter
   env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)

   #===============================================================================
   #                             TOOL DEFINITIONS
   #===============================================================================
   if(version_compare(hex_tools_ver, '6.0.0') > 0):
     env.Replace(USE_LLVM='True')
   else:
     env.Replace(USE_LLVM='False')
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
     QDSP6BIN = os.path.abspath(hex_tools_root+"/Tools/bin")
     QDSP6QCBIN = os.path.abspath(hex_tools_root+"/Tools/bin")
     QDSP6ISS = os.path.abspath(hex_tools_root+"/Tools/lib/iss")
     env['QDSP6INCLUDE'] = os.path.abspath(hex_tools_root+"/Tools/target/hexagon/include")
     env.Replace(LIB_DSP_ROOT = hex_tools_root+"/Tools/target/hexagon/lib/${Q6VERSION}")
   else:
     QDSP6BIN = os.path.abspath(hex_tools_root+"/gnu/bin")
     QDSP6QCBIN = os.path.abspath(hex_tools_root+"/qc/bin")
     QDSP6ISS = os.path.abspath(hex_tools_root+"/qc/lib/iss")
     env['QDSP6INCLUDE'] = os.path.abspath(hex_tools_root+"/qc/include/common")
     env.Replace(LIB_DSP_ROOT = hex_tools_root+"/qc/lib/${Q6VERSION}")

   if env['PLATFORM'] in ['windows', 'win32']:
      QDSP6BIN = QDSP6BIN + "\\"
      QDSP6QCBIN = QDSP6QCBIN + "\\"
      QDSP6ISS = QDSP6ISS + "\\"
   else:
      QDSP6BIN = QDSP6BIN + "/"
      QDSP6QCBIN = QDSP6QCBIN + "/"
      QDSP6ISS = QDSP6ISS + "/"
   env.Replace(QDSP6BIN = QDSP6BIN)
   env.Replace(QDSP6QCBIN = QDSP6QCBIN)
   env.Replace(QDSP6ISS = QDSP6ISS)

   if (version_compare(hex_tools_ver, '7.0.0') > 0):
        env.Replace(QDSP6_RELEASE_LIB_DIR = hex_tools_root+"/Tools/target/hexagon/lib/${Q6VERSION}")
   else:
        env.Replace(QDSP6_RELEASE_LIB_DIR = hex_tools_root+"/dinkumware/lib/${Q6VERSION}")

#variables used by qurt_builder.py
   if os.name == 'posix':
          env['TimerLib'] = os.path.abspath(QDSP6ISS+"qtimer.so")
          env['L2vicLib'] = os.path.abspath(QDSP6ISS+"l2vic.so")
   elif os.name == 'nt':
          env['TimerLib'] = os.path.abspath(QDSP6ISS+"qtimer.dll")
          env['L2vicLib'] = os.path.abspath(QDSP6ISS+"l2vic.dll")
#variable used by testcases/Sconscript
   if (version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(OBJDUMPOPT = "-private-headers -section-headers -disassemble -g -t")
   else:
      env.Replace(OBJDUMPOPT = "-dClx")


   env.Replace(QDSP6_BINDIR = "${L4Q6_ROOT}/bin")

   if (version_compare(hex_tools_ver, '7.4.02') >= 0) or (version_compare(hex_tools_ver, '7.4.0') < 0 and version_compare(hex_tools_ver, '7.3.08') >= 0) or (version_compare(hex_tools_ver, '7.3.0' ) < 0 and version_compare(hex_tools_ver, '7.2.12') >= 0) :
     env['QURT_MODEL_DLL']= "/qurt_model_64.dll"
     env['QURT_MODEL_SO']= "/qurt_model_64.so"
   elif version_compare(hex_tools_ver, '7.1.0') > 0 :
     env['QURT_MODEL_DLL']= "/qurt_model_7_1.dll"
     env['QURT_MODEL_SO']= "/qurt_model_7_1.so"
   elif version_compare(hex_tools_ver, '7.0.0') > 0 :
     env['QURT_MODEL_DLL']= "/qurt_model_7_0.dll"
     env['QURT_MODEL_SO']= "/qurt_model_7_0.so"
   else:
     env['QURT_MODEL_DLL']= "/qurt_model.dll"
     env['QURT_MODEL_SO']= "/qurt_model.so"

   if os.name == 'posix':
      env.Replace(QURT_MODEL = env.subst('$Q6_RTOS_ROOT')+"/"+env.subst('$TRGT')+"/debugger/lnx64/qurt_model.so")
   elif os.name == 'nt':
      env['ENV']['TMP'] = os.environ['TMP']
      env.Replace(QURT_MODEL = env.subst('$Q6_RTOS_ROOT')+"/"+env.subst('$TRGT')+"/debugger/Win/qurt_model.dll")

   env.Replace(Q6_RTOS_ROOT = env.subst('$Q6_RTOS_ROOT')+"/"+env.subst('$TRGT'))

   if 'Q6_RTOS_ROOT' in env:
      qdsp6_libdir = env.subst('$Q6_RTOS_ROOT')+"/lib"
      env.Replace(Q6_RTOS_LIBS = qdsp6_libdir)
   else:
      print("-------------------------------------------------------------------------------")
      print("*** Q6_RTOS_ROOT is not defined")
      print("*** install location is required")
      print("-------------------------------------------------------------------------------")
      print('')
      Exit(1)
   env["TEMPFILE"] = TempFileMunge
   #-------------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #-------------------------------------------------------------------------------
   env.Replace(QDSP6_CPPPATH = [])

   env.Replace(CPPPATH = [env.subst('$CONFIG_DIR'), env.subst('$Q6_TOOLS_ROOT')+'/qc/bin',
                          '$COMMON_INCLUDE',
                                     env.subst('$QURT_TOP')+'/libs/qurt/include', env.subst('$QURT_TOP')+'/libs/qurt/include/public',
                                     env.subst('$QURT_TOP')+'/qurtos/include', env.subst('$QURT_TOP')+'/kernel/include'])

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
      env.AppendENVPath('PATH', '${QDSP6BIN}')
      #print env.Dump()

   #-------------------------------------------------------------------------------
   # Software tool and environment definitions
   #-------------------------------------------------------------------------------

   #import pdb; pdb.set_trace()
   if env['PLATFORM'] in ['cygwin']:
      QDSP6BIN = string.replace("/cygdrive/%s" % QDSP6BIN, ":", "")

   if(version_compare(hex_tools_ver, '6.0.0') > 0):
      env.Replace(CC = QDSP6QCBIN + "hexagon-clang${EXE_EXT}")
   else:
      env.Replace(CC = QDSP6BIN + "hexagon-gcc${EXE_EXT}")

   # HEXAGON set ANSI C++ compiler
   if(version_compare(hex_tools_ver, '6.0.0') > 0):
      env.Replace(CXX = QDSP6QCBIN + "hexagon-clang++${EXE_EXT}")
   else:
      env.Replace(CXX = QDSP6BIN + "hexagon-g++${EXE_EXT}")

   # HEXAGON assembler
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(AS = QDSP6BIN + "hexagon-llvm-mc${EXE_EXT}")
   else:
      env.Replace(AS = QDSP6BIN + "hexagon-as${EXE_EXT}")

   # HEXAGON library archiver
   env.Replace(AR = QDSP6BIN + "hexagon-ar${EXE_EXT}")

   # HEXAGON linker
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(LINK = QDSP6BIN + "hexagon-link${EXE_EXT}")
   else:
      env.Replace(LINK = QDSP6BIN + "hexagon-ld${EXE_EXT}")

   # HEXAGON library archiver
   env.Replace(RANLIB = QDSP6BIN + "hexagon-ranlib${EXE_EXT}")

   # HEXAGON Object COPY
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(OBJCOPY = QDSP6BIN + "hexagon-elfcopy${EXE_EXT}")
   else:
      env.Replace(OBJCOPY = QDSP6BIN + "hexagon-objcopy${EXE_EXT}")

   # HEXAGON Object Dump
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(OBJDUMP = QDSP6BIN + "hexagon-llvm-objdump${EXE_EXT}")
   else:
      env.Replace(OBJDUMP = QDSP6BIN + "hexagon-objdump${EXE_EXT}")

   # HEXAGON simulator
   env.Replace(SIM = QDSP6QCBIN + "hexagon-sim${EXE_EXT}")

   # HEXAGON profiler
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Replace(PROF = QDSP6QCBIN + "hexagon-coverage${EXE_EXT}")
   else:
      env.Replace(PROF = QDSP6QCBIN + "hexagon-cov${EXE_EXT}")

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
   if target=='unitv56':
      env.Replace(HEXAGONCC_OPT = "-O0")
   else:
      env.Replace(HEXAGONCC_OPT = "${HEXAGON_OPT_2}")

   # the other opts found
   env.Replace(HEXAGONCXX_OPT = "${HEXAGON_OPT_SPACE} ${HEXAGON_OPT_TIME} ${HEXAGON_OPT_S}")
   env.Replace(HEXAGONASM_OPT = "${HEXAGON_OPT_1}")

   #-------------------------------------------------------------------------------
   # HEXAGON Warnings Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_WARN = "-Werror -Wall -Wno-infinite-recursion")
   env.Replace(HEXAGONCC_WARN = "${HEXAGON_WARN}")
   env.Replace(HEXAGONCXX_WARN = "${HEXAGON_WARN}")
   #-------------------------------------------------------------------------------
   # HEXAGON Machine Options
   #-------------------------------------------------------------------------------
   if (hexagon_hw_ver == 61) and (target=='modemv61v1'):
     if version_compare(hex_tools_ver, '8.0') >= 0 :
      hexagon_machine = "-mcpu=hexagonv61"
     else:
      hexagon_machine = "-mcpu=hexagonv61v1"
   elif (hexagon_hw_ver == 61) and (target=='modemv61'):
      hexagon_machine = "-mcpu=hexagonv61"
   elif(hexagon_hw_ver == 62):
     if version_compare(hex_tools_ver, '8.0') >= 0 :
       hexagon_machine = "-mv62"
     else:
       hexagon_machine = "-mv62v1"

   else:
      hexagon_machine = "-m${Q6VERSION}"
   env.Replace(HEXAGON_MACHINE = hexagon_machine)

   #-------------------------------------------------------------------------------
   # HEXAGON Undef Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_UNDEF = "")

   #-------------------------------------------------------------------------------
   # HEXAGON Lang independent options
   #-------------------------------------------------------------------------------
   #env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections " \
   #      "-mno-literal-pool-addresses -nostdlib -fno-exceptions " \
   #      "-fno-merge-constants -fno-strict-aliasing ")
   if 'CONFIG_SECTIONS' not in env.subst('$BUILD_CONFIG_FLAGS'):
      env.Replace(HEXAGON_LANIND = ['-G0','-fdata-sections','-ffunction-sections'])
   else:
      env.Replace(HEXAGON_LANIND = ['-G0'])

   #-------------------------------------------------------------------------------
   # HEXAGON Stack Checking
   #-------------------------------------------------------------------------------
   if 'USES_STACK_PROTECTOR' in env:
      env.Replace(HEXAGON_STKPRO = "-fstack-protector")
   else:
      env.Replace(HEXAGON_STKPRO = "")

   #----------------------------------------------------------------------------
   # common defines
   #----------------------------------------------------------------------------
   env.Replace(BUILD_CCFLAGS = env.subst('$BUILD_CONFIG_FLAGS').split(' '))
   env.Replace(BUILD_CCFLAGS = ["-D__QURTFILENAME__=\\\"$SOURCE.file\\\""] + env['BUILD_CCFLAGS'])

   if (version_compare(hex_tools_ver, '8.1') >= 0 and hexagon_hw_ver >= 65):
      env.Replace(BUILD_CCFLAGS = ["-mhvx"]+ env['BUILD_CCFLAGS'])

   env.Replace(BUILD_ASMFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" -D__FILENAME__=\\"$SOURCE.file\\" ')

   if env.subst('$H2_REFERENCE_DIR'):
       env.Replace(BUILD_CCFLAGS = ['-I'+env.subst('$BUILD_DIR/h2_import')] + env['BUILD_CCFLAGS'])

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

   if(version_compare(hex_tools_ver, '6.0') > 0) and (version_compare(hex_tools_ver, '8.0') < 0 ):
      env.Replace(LLVMCFLAGS = "-fgnu89-inline")
   else:
      env.Replace(LLVMCFLAGS = "")

   if(version_compare(hex_tools_ver, '6.0.0') > 0):
      env.Replace(ERRATACFLAGS = "")

   env.Replace(PICCFLAGS = "")
   env.Replace(EXPOSE_API = "")
   #hide symbols as default for share objects
   if env.subst('$USE_PIE') == 'True' and env.subst('$USE_QURT_EXPOSE_API') == 'True':
        env.Replace(PICCFLAGS = "-fPIC -fvisibility=hidden -fvisibility-inlines-hidden")
   elif env.subst('$USE_PIE') == 'True':
        env.Replace(PICCFLAGS = "-fPIC")
   if env.subst('$USE_QURT_EXPOSE_API') == 'True':
        env.Replace(EXPOSE_API = '-DCONFIG_LIMIT_DLL_API')
   env.Replace(DEBUGFLAGS = "")
   if env.subst('$USE_DEBUG') == 'True':
        env.Replace(DEBUGFLAGS = "-E")

   # Init FLAGS
   env.Replace(CCFLAGS   = "$HEXAGON_MACHINE $HEXAGONCC_WARN $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $DEBUGFLAGS $CC_DFLAGS $CFLAGS $LLVMCFLAGS $PICCFLAGS $ERRATACFLAGS $EXPOSE_API")
   env.Replace(CXXFLAGS  = "$HEXAGON_MACHINE $HEXAGONCXX_WARN $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $LLVMCFLAGS $ERRATACFLAGS")
   env.Replace(ASMFLAGS  = "$HEXAGON_MACHINE $HEXAGONCC_WARN $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $ERRATACFLAGS $EXPOSE_API")
   env.Replace(ASFLAGS   = "$HEXAGON_MACHINE $HEXAGONCC_WARN $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $PICCFLAGS $ERRATACFLAGS")
   #env.Replace(ARFLAGS   = "$HEXAGONAR_LIB_QUITE $HEXAGONAR_LIB_REPLACE")
   env.Replace(ARFLAGS   = "cru")
   if version_compare(hex_tools_ver, '8.0') >= 0 :
      env.Append(ARFLAGS = 'D ')
   elif version_compare(hex_tools_ver, '7.0') >= 0 :
      env.Append(ARFLAGS = ' -D ')


   CCCOM = "${TEMPFILE('$CC $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $CCFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $SOURCES.posix')}"
   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES.posix -o $TARGET.posix")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

   CXXCOM = "${TEMPFILE('$CXX $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $CXXFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $SOURCES.posix')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])

   env.Replace(ARCOM     =  "${TEMPFILE('$AR $ARFLAGS $TARGET.posix $SOURCES.posix')}")
   env.Replace(RANLIBCOM =  "${TEMPFILE('$RANLIB $TARGET.posix')}")

   env.Replace(ASCPPCOM  =  "$CC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS $HEXAGON_PREPROCESS_CMD $HEXAGON_OUTPUT_CMD ${TARGET.posix}.pp $SOURCES.posix")
   env.Replace(ASASCOM   =  "${TEMPFILE('$CC $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $CCFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $SOURCES.posix')}")

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
   env.Replace(LINKFLAGS = "${HEXAGON_MACHINE} -nostdlib --section-start ${HEXAGON_IMAGE_ENTRY_SECTION}=${HEXAGON_IMAGE_ENTRY} ${HEXAGONLD_MAP_CMD} ${TARGET.posix}.map --entry=${HEXAGON_IMAGE_ENTRY} ${LFLAGS} ")
   env.Replace(LINK_DIRS = "-L${QDSP6_RELEASE_LIB_DIR}")
   env.Replace(CRT = '${Q6_RTOS_LIBS}/crt0.o')

   # LINK special case in hexagon world
   env.Replace(QDSP6OBJS1  = [
      '${CRT}',
      '${QDSP6_RELEASE_LIB_DIR}/init.o',
   ])

   env.Replace(QDSP6OBJS3  = [
      '${QDSP6_RELEASE_LIB_DIR}/libc.a',
      '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
      '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
      '${Q6_RTOS_LIBS}/libqurt.a',
      '${Q6_RTOS_LIBS}/libqurtcfs.a',
      '${Q6_RTOS_LIBS}/libqurtkernel_main.a',
      '${Q6_RTOS_LIBS}/libqurtkernel_island.a',
      '${Q6_RTOS_LIBS}/libqube_compat.a',
      '${Q6_RTOS_LIBS}/libtimer.a',
      '${Q6_RTOS_LIBS}/libposix.a',
      '${Q6_RTOS_LIBS}/libqurtdebugger.a'
      #'${LIB_DSP_ROOT}/libqdsp.a'
   ])
   if(version_compare(hex_tools_ver, '7.0.0') > 0):
      env.Append(QDSP6OBJS3 = ['${LIB_DSP_ROOT}/libhexagon.a'])
   else:
      env.Append(QDSP6OBJS3 = ['${LIB_DSP_ROOT}/libqdsp.a'])

   env.Replace(QDSP6OBJS4  = [
      '${QDSP6_RELEASE_LIB_DIR}/fini.o',\
   ])

   #-------------------------------------------------------------------------------------
   # if $SOURCES doesn't have any items in it, python is throwing an error $SOURCES.posix.
   # When linking there should always be at least one obj it is the entry point.
   # SOURCES should never be emty if it is someone added objects to library list instead
   # of object list. Fix that isntead of trying to get creative here.
   #-------------------------------------------------------------------------------------
   env.Replace(LINKOBJGRP = "--start-group ${QDSP6OBJS1} ${QDSP6OBJS3} " \
      "$_LIBFLAGS_POSIX  $SOURCES.posix ${QDSP6OBJS} --end-group ${QDSP6OBJS4}"
   )

   env.Replace(LINKCOM = "${TEMPFILE('$LINK $LINKFLAGS $LINK_DIRS $LINKOBJGRP $HEXAGON_OUTPUT_CMD $TARGET.posix $LINKFLAGS_END')}")
