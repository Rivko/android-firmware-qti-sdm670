#===============================================================================
#
# PIL Splitter builder
#
# GENERAL DESCRIPTION
#    Run the PIL Splitter tool on .mbn files.
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tz/1.0.2/bsp/build/scripts/pil_splitter_builder.py#1 $
#  $DateTime: 2018/02/06 03:00:17 $
#  $Author: pwbldsvc $
#  $Change: 15399933 $
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
import SCons
from SCons.Script import *

BUILDER_NAME = 'PilSplitterBuilder'
HELPER_NAME = 'PilSplit'

DEFAULT_SPLIT_BIN_COUNT = 7

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('pil_splitter_builder')

def generate(env):
   splitter_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def splitter_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   splitter_act = env.GetBuilderAction(splitter_builder)
   splitter_bld = env.Builder(action  = splitter_act,
                              emitter = splitter_emitter)

   env.Append(BUILDERS = {BUILDER_NAME : splitter_bld})

   # expose wrapper method for easy splitting
   env.AddMethod(splitter_helper, HELPER_NAME)

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def splitter_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)

   # The PIL Splitter creates multiple files, and we have no way of knowing at this
   # point how many files there will be, so we can't explicitly add these files
   # to the target list at this time.
   target_str = os.path.abspath(str(target[0]))
   target_prefix = os.path.splitext(target_str)[0]
   target_flist_fn = target_prefix + '.flist'

   return (target, source)

#-------------------------------------------------------------------------------
# def builder for qcoreapp_booting.pbn
#
def splitter_builder(target, source, env):
   # init and fix variebles
   source_str = env.SConsAbsPath(source[0])
   target_str = env.SConsAbsPath(target[0])
   target_prefix = os.path.splitext(target_str)[0]
   target_flist_fn = target_prefix + '.flist'

   cmds = ["python ${BUILD_ROOT}/ssg/bsp/build/scripts/pil_splitter.py %s %s" % (source_str, target_prefix)]

   data, err, rv = env.RunCmds(target, source, cmds)

   # Create a list of files that were created, so that we can clean them
   # later, or detect if one has been removed.
   target_flist = open(target_flist_fn, 'w')
   file_list = env.GlobFiles(target_prefix + ".b*")
   for file in file_list:
      rel_path = env.RelPath(file, env.subst("${BUILD_ROOT}")) + '\n'
      rel_path = rel_path.replace('\\', '/')
      target_flist.write(rel_path)
   target_flist.close()

   # TODO - There is a potential corner-case in this logic. If the number of
   #        splitbins decreases by one due to a code change, incremental builds
   #        may incorrectly pass/fail validation due to the extra splitbin.
   #        While a clean should resolve this issue, it may help to handle this
   #        edge-case explicitly in the builder.

   created_splitbins = [ env.SConsAbsPath(path) for path in file_list ]
   given_targets = [ env.SConsAbsPath(path) for path in target ]

   help_message = \
         "Please ensure that the target list given to {} is correct and/or the correct number of splitbins is given to env.{}()".format( \
         BUILDER_NAME, HELPER_NAME)

   # validate that all expected splitbins were created
   for expected_bin in given_targets:
      assert os.path.exists(expected_bin), \
          "BUG: Missing expected splitbin '{}' after pilsplit\n{}".format( \
                expected_bin, help_message)

   # validate that no additional unexpected splitbins were created
   for created_splitbin in created_splitbins:
      assert created_splitbin in given_targets, \
         "BUG: Found unexpected splitbin '{}'\n{}".format( \
               created_splitbin, help_message)

   return None

def splitter_helper(env, mbns, pil_base_dir, \
        split_bin_count = None):
   """Pseudo-builder to split the given mbns into 'split_bin_count' pieces,
      outputting them to 'pil_base_dir'"""
   mbns = env.Flatten(mbns)
   pil_bin_list = []
   for signed_mbn in mbns:
      mbn_path = signed_mbn.get_abspath()
      if not mbn_path.endswith(".mbn"):
         continue # skip non-mbn returns/targets

      # convert all paths to Unix-style for SCons & split the image & dir names
      mbn_path_parts = mbn_path.replace("\\", "/").split("/")
      pil_dir = "/".join(mbn_path_parts[:-1])
      image = os.path.splitext(mbn_path_parts[-1])[0]

      # Determine which splitbin path type this is (eg signed, unsigned, or
      # signed_encrypted). Any other unsupported type is skipped.
      sign_path_index = pil_dir.find(env.subst("${QC_SHORT_BUILDPATH}")) \
            + len(env.subst("${QC_SHORT_BUILDPATH}")) + 1
      sign_path = pil_base_dir + "/" + pil_dir[sign_path_index:]
      if sign_path.find("sign") >= 0 and sign_path.find("signed") <= 0:
         continue

      pil_bins = generate_split_bin_list(image, path = sign_path, \
            split_bin_count = split_bin_count)

      pil_bins.insert(0, os.path.join(sign_path, image + '.mdt'))
      pil = env.PilSplitterBuilder(pil_bins, signed_mbn)
      pil_bin_list.extend(pil)

   assert len(pil_bin_list) > 0, \
            "No valid mbns given for pil_splitting (given {})".format( \
            str(mbns))

   return pil_bin_list

def generate_split_bin_list(image, path, split_bin_count = None):
   """Helper function to generate a list of splitbins for the given image &
      path. Generally used to build the SCons target list for pilsplitting."""
   if split_bin_count == None:
      split_bin_count = DEFAULT_SPLIT_BIN_COUNT
   # first add the .mdt, then each of the .b* files
   file_path = "{}/{}.mdt".format(path, image)
   return_list = [ file_path ]
   for i in range(0, split_bin_count):
      file_path = "{}/{}.b{:02d}".format(path, image, i)
      return_list.append(file_path)
   return return_list

