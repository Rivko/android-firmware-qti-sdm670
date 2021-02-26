#===============================================================================
#
# CoreBSP MODEM tool rules
#
# GENERAL DESCRIPTION
#    MODEM Tools build script
#
# Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/aop/build/aoptools_defs.py#1 $
#
#===============================================================================
import sys
import os
import string

# AOP assembly flags
#asm_dflags = '-DASSEMBLY -D__ASSEMBLER__ -D_ARM_ASM_'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('aoptools_defs')

def generate(env):
   # Add/process command line arguments
   env.Tool('sd_llvm', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])

   env.Append(CPPPATH = "${BUILD_ROOT}/core/bsp/aop/inc")
   env.Append(CPPPATH = "${BUILD_ROOT}/core/bsp/aop/target/${MSM_ID}")
   # AOP specific overrides
   env.Replace(ARM_FRAME_DIRECTIVES = "--emit_frame_directives")
   env.Replace(ARM_CPU = "$ARM_CPU_CMD Cortex-M3")
   env.Replace(ARMCC_OPT  = "${ARM_OPT_TIME} ${ARM_OPT_3} --protect_stack ")
   env.Replace(ARMCXX_OPT = "${ARM_OPT_TIME} ${ARM_OPT_3} --protect_stack ")
   env.Replace(ARMASM_OPT = "${ARM_OPT_TIME} ${ARM_OPT_3} --protect_stack ")
   env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES}")
   env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_SPLIT_SECTIONS}  ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES} --no_rtti --no_exceptions")
   env.Replace(LINKFLAGS = "$ARMLD_ELF_CMD $ARMLD_MAP_CMD $ARMLD_INFO_CMD architecture,common,stack,unused,unusedsymbols,merge,sizes,summarysizes,totals $ARMLD_LIST_CMD " \
      "${TARGET.base}.map $ARMLD_SYMBOLS_CMD $ARMLD_SYMDEFS_CMD ${TARGET.base}.sym $ARMLD_SCATTER_CMD " \
      "${TARGET.base}.scl --bestdebug --remove --callgraph --cgfile=user --no_merge")

   if 'LLVM' in os.environ['ARMTOOLS'] :
      env.Replace(ARM_CPU = "$ARM_CPU_CMD=cortex-m3")
      env.Replace(ARM_FRAME_DIRECTIVES = "")
      env.Replace(ARMCC_OPT  = "${ARM_OPT_TIME} ${ARM_OPT_3} -fstack-protector")
      env.Replace(ARMCXX_OPT = "${ARM_OPT_TIME} ${ARM_OPT_3} -fstack-protector")
      env.Replace(ARMASM_OPT = "${ARM_OPT_TIME} ${ARM_OPT_3} -fstack-protector")
      env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES}")
      env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_SPLIT_SECTIONS}  ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES}")
      env.Replace(LINKFLAGS = "$ARMLD_BM_FLAGS $ARMLD_ELF_CMD $ARMLD_MAP_CMD $ARMLD_INFO_CMD  $ARMLD_LIST_CMD " \
         "-Wl,${TARGET.base}.map  $ARMLD_SCATTER_CMD " \
         "-Wl,${TARGET.base}.scl ${ARMLD_REMOVE_CMD} " \
         "${ARMLD_SYMDEFS_CMD} -Wl,${TARGET.base}.sym ")
