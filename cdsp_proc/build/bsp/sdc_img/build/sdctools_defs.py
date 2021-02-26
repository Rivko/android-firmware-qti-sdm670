#===============================================================================
#
# CoreBSP MODEM tool rules
#
# GENERAL DESCRIPTION
#    MODEM Tools build script
#
# Copyright (c) 2009-2011,2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/dspbuild.adsp/5.0.c3/bsp/sdc_img/build/sdctools_defs.py#1 $
#
#===============================================================================
import sys
import os
import string
import traceback

# RPM assembly flags
#asm_dflags = '-DASSEMBLY -D__ASSEMBLER__ -D_ARM_ASM_'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sdctools_defs')

def generate(env):
   # Add/process command line arguments
   env.Tool('llvm', toolpath = ['.'])

  

   
   # SDC specific overrides

   env.Replace(ARM_CPU = "-mcpu=cortex-m3")
   env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES}")
   env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_SPLIT_SECTIONS}  ${ARM_DATA_REORDER} ${ARM_FRAME_DIRECTIVES} --no_rtti --no_exceptions")

