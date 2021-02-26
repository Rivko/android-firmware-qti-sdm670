#!/usr/bin/env python

#===============================================================================
#
# Stripped ELF builder
#
# GENERAL DESCRIPTION
#    Run the  strip tool on .elf files.
#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#===============================================================================

import os
import SCons
from SCons.Script import *

BUILDER_NAME = 'StrippedElfBuilder'
HELPER_NAME = 'ElfStrip'


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('stripped_elf_builder')


def generate(env):
   stripped_elf_generate(env)


#===============================================================================
# build rules
#===============================================================================
def stripped_elf_generate(env):
   """Sets up SCons environment for the stripped elf builder"""

   # Uncomment the following to declare that the build should USE_ELF_STRIPPER
   # env.DeclareFeature("ELF_STRIPPER", uses_prefix=True)

   stripped_elf_bld = env.Builder(action = stripped_elf_builder)

   env.Append(BUILDERS = {BUILDER_NAME : stripped_elf_bld})

   # expose wrapper method for easy splitting
   env.AddMethod(stripper_helper, HELPER_NAME)


#-------------------------------------------------------------------------------
# def builder for stripped elfs
#-------------------------------------------------------------------------------
def stripped_elf_builder(target, source, env):
   """SCons Action to strip symbols from the given elf"""
   # init and fix variables
   source_str = env.SConsAbsPath(source[0])
   target_str = env.SConsAbsPath(target[0])
   strip_elf_file = env.RealPath("${BUILD_ROOT}/ssg/bsp/build/scripts/strip_elf.py")

   cmds = ["python %s %s %s" % (strip_elf_file, source_str, target_str)]

   data, err, rv = env.RunCmds(target, source, cmds)

   given_targets = [ env.SConsAbsPath(path) for path in target ]

   # validate that the expected target was created
   for expected_bin in given_targets:
      assert os.path.exists(expected_bin), \
          "BUG: Missing expected stripped elf '{}'".format(expected_bin)

   return None


def stripper_helper(env, elfs, deploy = False):
   """Pseudo-builder to strip & optionally deploy the given elf file"""
   elfs = env.Flatten(elfs)
   stripped_elfs = []
   for elf in elfs:
      source_str = env.SConsAbsPath(elf)
      elf_name, elf_ext = os.path.splitext(source_str)
      stripped_elf_name = elf_name + "_stripped" + elf_ext
      stripped_elf = env.StrippedElfBuilder([stripped_elf_name], elf)
      stripped_elfs.extend(stripped_elf)

   assert len(stripped_elfs) > 0, \
            "No valid elfs given for symbol stripping (given {})".format( \
            str(elfs))

   if deploy:
      env.Deploy(stripped_elfs)

   return stripped_elfs

