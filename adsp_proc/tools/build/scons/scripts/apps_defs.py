'''
   CoreBSP APPS tool rules
'''
#===============================================================================
#
# CoreBSP APPS tool rules
#
# GENERAL DESCRIPTION
#    APPS Tools build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/scons.qc/1.1/scripts/apps_defs.py#3 $
#
#===============================================================================
#import sys
#import os
#import string

# Assembly APPS compile flags (note first pass is through armcc using -E option
# then passed to armas, see build rule 
asm_dflags = '-DCUST_H=\\"${CUST_H}\\" -D__MSMHW_APPS_PROC__=2 ' \
    '-D__MSMHW_MODEM_PROC__=1 -D__MSMHW_PROC_DEF__=__MSMHW_APPS_PROC__ ' \
    '-DMSMHW_APPS_PROC -DIMAGE_APPS_PROC '

# standard APPS compile flags
apps_dflags = '-DCUST_H=\\"${CUST_H}\\" -D__MSMHW_APPS_PROC__=2 ' \
    '-D__MSMHW_MODEM_PROC__=1 -D__MSMHW_PROC_DEF__=__MSMHW_APPS_PROC__ ' \
    '-DMSMHW_MODEM_PROC -DMSMHW_APPS_PROC -DIMAGE_APPS_PROC ' \
    '-DASSERT=ASSERT_FATAL -DBUILD_TARGET=\\"${BUILD_ID}\\" ' \
    '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" '

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('appstools_defs')

def generate(env):
    '''
       generate
    '''
    # Assembly common flags
    env.Replace(ASM_DFLAGS = asm_dflags)
   
    # CC (apps) common compile flags
    env.Replace(CC_DFLAGS = apps_dflags)
