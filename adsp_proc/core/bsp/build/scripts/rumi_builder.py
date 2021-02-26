# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builders for RUMI related files
# --------------------------------------------------------------------------- #
# $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/rumi_builder.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2015 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import os
import types
import sys
import subprocess

# --------------------------------------------------------------------------- #
#                       GLOBAL VARIABLE DEFINITIONS                           #
# --------------------------------------------------------------------------- #

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    '''Scons function to check if builder exists'''
    return env.Detect('rumi_builder')

def generate(env):
    '''Function to register actions with scons'''

    # Save a reference to the expected coproc image
    if 'USES_EMULATION_PLATFORM' in env:
        env['COPROC_IMG'] = "${BUILD_ROOT}/fw/bin/QRAFA/modem_coproc.QRAFA.sym"
    elif 'USES_MODEM_BRINGUP' in env:
        env['COPROC_IMG'] = "${BUILD_ROOT}/fw/bin/QSAFA/modem_coproc.QSAFA.sym"
    else:
        env.PrintInfo("Neither USES_EMULATION_PLATFORM nor USES_MODEM_BRINGUP defined, defaulting to QRAFA.sym")
        env['COPROC_IMG'] = "${BUILD_ROOT}/fw/bin/QRAFA/modem_coproc.QRAFA.sym"

    # Local import to prevent following import when executing from command line
    # where there is no scons framework
    import SCons.Scanner
    
    #Builder for generating the pruned ELF
    prune_elf_act = env.GetBuilderAction(prune_elf_builder)
    prune_elf_bld = env.Builder(action = prune_elf_act, emitter = prune_elf_emitter,
                                suffix = '.elf', src_suffix = '.elf')
    
    env.Append(BUILDERS = {'PreSilPruneElf' : prune_elf_bld})

    #Builder for generating the crashdump cmm script
    dump_script_act = env.GetBuilderAction(dump_script_builder)
    dump_script_bld = env.Builder(action = dump_script_act, emitter = dump_script_emitter,
                                  suffix = '.cmm', src_suffix = '.elf')

    env.Append(BUILDERS = {'PreSilCreateDumpScript' : dump_script_bld})
    
    #Builder for generating the bin file from the MBN
    bin_file_act = env.GetBuilderAction(bin_file_builder)
    bin_file_bld = env.Builder(action = bin_file_act, emitter = bin_file_emitter,
                               suffix = '.bin', src_suffix = '.mbn')
    
    env.Append(BUILDERS = {'PreSilConvertMbnToBin' : bin_file_bld})

    #Builder for generating the std_debug_mpss.cmm script
    load_script_act = env.GetBuilderAction(load_script_builder)
    load_script_bld = env.Builder(action = load_script_act, emitter = load_script_emitter,
                                  suffix = '.cmm', src_suffix = '.mbn')
    env.Append(BUILDERS = {'PresilCreateLoadScript' : load_script_bld})

def prune_elf_emitter(target, source, env):
    pruned_elf = str(target[0])

    # Dependencies
    env.Depends(pruned_elf, "${BUILD_ROOT}/core/bsp/build/scripts/rumi_builder.py")

    return (target,source)


def prune_elf_builder(target, source, env):
    '''Pruned ELF builder function'''

    verbose    = int(env.GetOption('verbose'))
    orig_elf   = str(source[0])
    pruned_elf = str(target[0])

    # Information
    if verbose >= 2:
        env.PrintInfo('RUMI: Generating pruned (non-RO) elf: {:s} -> {:s}'.format(orig_elf, pruned_elf))
        
    # Run the objcopy to generate the pruned ELF
    cmd  = "${OBJCOPY}"
    cmd += " " + orig_elf
    cmd += " " + pruned_elf
    cmd += " -R .text_fw"
    cmd += " -R .text_sw"
    cmd += " -R .rodata"
    cmd += " --strip-all"
    cmd = env.subst(cmd)
    if verbose >= 2:
        env.PrintInfo("RUMI: objcopy: {:s}".format(cmd))

    proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    line = proc.stdout.readline()
    while line:
        if verbose >= 2:
            env.PrintInfo("RUMI objcopy: " + line.rstrip())
        line = proc.stdout.readline()
    rv = proc.wait()
    if verbose >= 2:
        env.PrintInfo("RUMI objcopy: retval = " + str(rv))

    
def dump_script_emitter(target, source, env):
    cmm = str(target[0])

    # Dependencies
    env.Depends(cmm, "${BUILD_ROOT}/core/bsp/build/scripts/rumi_builder.py")
    env.Depends(cmm, "${BUILD_ROOT}/core/bsp/build/scripts/crashdump.py")
    if 'USES_TABASCO_MODEM' in env:
        env.Depends(cmm, env['COPROC_IMG'])

    return (target,source)


def dump_script_builder(target, source, env):
    '''Dump Script builder function'''

    verbose = int(env.GetOption('verbose'))
    elf = str(source[0])
    mbn = str(source[1])
    cmm = str(target[0])

    # Information
    if verbose >= 2:
        env.PrintInfo('Generating pruned crashdump script: {:s} -> {:s}'.format(elf, cmm))
    
    # Run the script to generate the crashdump script
    cmd  = "python ${BUILD_ROOT}/core/bsp/build/scripts/crashdump.py"
    cmd += " cmm"
    cmd += " " + elf
    cmd += " " + mbn
    cmd += " -o " + cmm
    cmd += " -e .start"
    cmd += " -e .ukernel.*"
    # Tabasco has a coproc
    if 'USES_TABASCO_MODEM' in env:
        cmd += ' --coproc-info=COPROC_IMAGE'
    cmd = env.subst(cmd)
    if verbose >= 2:
        env.PrintInfo("RUMI: cmm command: {:s}".format(cmd))

    proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    line = proc.stdout.readline()
    while line:
        if verbose >= 2:
            env.PrintInfo("RUMI cmm: " + line.rstrip())
        line = proc.stdout.readline()
    rv = proc.wait()
    if verbose >= 2:
        env.PrintInfo("RUMI cmm: retval = " + str(rv))

def bin_file_emitter(target, source, env):
    bin_file = str(target[0])

    # Dependencies
    env.Depends(bin_file, "${BUILD_ROOT}/core/bsp/build/scripts/rumi_builder.py")

    return (target,source)
 
def bin_file_builder(target, source, env):
    '''Dump Script builder function'''

    verbose = int(env.GetOption('verbose'))
    mbn = str(source[0])
    bin = str(target[0])
    
    # Information
    if verbose >= 2:
        env.PrintInfo('Generating BIN file from MBN for DDR Backload: {:s} -> {:s}'.format(mbn, bin))
    
    # Run the script to generate the crashdump script
    cmd  = "python ${BUILD_ROOT}/core/bsp/build/scripts/mbntobin.py"
    cmd += " " + mbn
    cmd += " " + bin
    cmd = env.subst(cmd)
    if verbose >= 2:
        env.PrintInfo("RUMI: MBN to BIN command: {:s}".format(cmd))

    proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    line = proc.stdout.readline()
    while line:
        if verbose >= 2:
            env.PrintInfo("RUMI MBN to BIN: " + line.rstrip())
        line = proc.stdout.readline()
    rv = proc.wait()
    if verbose >= 2:
        env.PrintInfo("RUMI MBN to BIN: retval = " + str(rv)) 
       
def load_script_emitter(target, source, env):
    cmm = str(target[0])

    # Dependencies
    env.Depends(cmm, "${BUILD_ROOT}/core/bsp/build/scripts/rumi_builder.py")
    
    return (target,source)

def load_script_builder(target, source, env):
    '''Dump Script builder function'''

    verbose = int(env.GetOption('verbose'))
    mbn     = str(source[0])    
    buildid = "${BUILD_ID}"
    cmm     = str(target[0])
    
    # Information
    env.PrintInfo('Generating std_debug_mpss.cmm script: {:s} -> {:s}'.format(mbn, cmm))
    
    # Run the script to generate the crashdump script
    cmd  = "python ${BUILD_ROOT}/core/bsp/build/scripts/std_debug_mpss.py"
    cmd += " " + mbn
    cmd += " " + buildid
    cmd += " " + cmm    
    
    cmd = env.subst(cmd)
    if verbose >= 2:
        env.PrintInfo("RUMI: std_debug_mpss.cmm generate command: {:s}".format(cmd))

    proc = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    line = proc.stdout.readline()
    while line:
        if verbose >= 2:
            env.PrintInfo("RUMI cmm: " + line.rstrip())
        line = proc.stdout.readline()
    rv = proc.wait()
    if verbose >= 2:
        env.PrintInfo("RUMI cmm: retval = " + str(rv))
