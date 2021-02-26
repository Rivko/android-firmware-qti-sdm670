#===============================================================================
#
# Target Configurator Incpaths Check
#
# GENERAL DESCRIPTION
# 
#    As part of the migration path from a make build using the CoreBSP SCons
#    build system to a target-level SCons build system using the Target
#    Configurator, a single tbc_incpaths.py script is created in the build\ms
#    directory from the many auto-generated incpaths<bid>.py scripts in the
#    core\bsp\build\data directory.  The general procedure is to use one of
#    the incpaths<bid>.py scripts as a base by copying it to the build\ms
#    directory and renaming it as tbc_incpaths.py, and then running this
#    script on that file and each of the remaining incpaths<bid>.py script
#    to identify any additional paths that need to be added to the
#    tbc_incpaths.py script.
#    
#    This tool identifies any new paths that may need to be added to
#    tbc_incpaths.py.
# 
#    This tool is not smart enough to parse the paths themselves to make
#    sure that they're the same.  It only looks at the symbols used to
#    refer to the paths to see if any new symbols are added.
# 
# PARAMETERS:
# 
#    make_build_root:  The path to the build_root directory of the equivalent
#                      make build
# 
# OUTPUT:
# 
#    Messages indicating any new paths that may need to be added.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/tcfg/tools/tbc_check.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/10/10   dlb'    Initial version.
#
#===============================================================================

import sys
import os
from glob import glob
from shlex import shlex

#---------------------------------------------------------------------
# Function main(make_build_path)
# 
# Main function for the module.
#---------------------------------------------------------------------

def main (make_build_root):

   #---------------------------------------------------------------------
   # Find the necessary files.
   #---------------------------------------------------------------------

   # Find paths for the local build.
   tcfg_tools_path = os.getcwd()
   local_build_root = tcfg_tools_path.replace('tools/build/scons/tcfg/tools', '')  # in case we're on linux
   local_build_root = tcfg_tools_path.replace('tools\\build\\scons\\tcfg\\tools', '')  # in case we're on windows
   local_build_ms_path = local_build_root + "build/ms/"
   tcfg_path = local_build_root + "tools/build/scons/tcfg"

   # Load tcfg tools
   sys.path.append(tcfg_tools_path)
   from fake_scons_env import FakeSConsEnv

   # Fine paths for the make build
   make_data_path = make_build_root + "core/bsp/build/data/"
   make_build_ms_path = make_build_root + "build/ms/"

   # Define file name templates for config scripts
   tbc_script_name_template  = "tbc*.py"
   make_script_name_template = "*%s.py"

   # Use the list of incpaths<bid>.py files in the make build
   # to make a list of build IDs to test
   bid_list = list()
   bid_file_filter = make_data_path + "incpaths*.py"
   for bid_file_full_name in glob(bid_file_filter):
      bid_file_name = os.path.basename(bid_file_full_name)
      bid = bid_file_name[8:-3]
      bid_list.append(bid)

   # Parse the .builds file
   sys.path.append(tcfg_path)
   from tcfgparser import TargetConfigurationDb
   # Parse the .builds file
   tcfg = TargetConfigurationDb()
   if not tcfg.ParseBuildFile(local_build_ms_path + "target.builds"):
      raise()

   # Test each build ID
   for bid in bid_list:
      print "===== Checking:", bid.upper(), "=====\n"

      # Generate the config for this build ID
      tcfg_bid_data = tcfg.GenConfigData(bid)
      tbc_env = FakeSConsEnv(local_build_root)
      tbc_env['TCFG_BID_IMAGE'] = tcfg_bid_data.image.upper()
      tbc_env.AddUsesFlags(tcfg_bid_data.uses_flags)

      #------------------------------------------------------------------------
      # Now compare the environments to find any differences
      #------------------------------------------------------------------------

      # Load the tbc*.py scripts for the local build
      for tool_name in glob(local_build_ms_path + tbc_script_name_template):
         tool_name = os.path.basename(tool_name)[0:-3] # drop the ".py"
         tbc_env.LoadToolScript(tool_name, toolpath=[local_build_ms_path])

      # Load the config data from the make build
      make_env = FakeSConsEnv(make_build_root)
      for tool_name in glob(make_data_path + make_script_name_template%bid):
         tool_name = os.path.basename(tool_name)[0:-3] # drop the ".py"
         make_env.LoadToolScript(tool_name, toolpath=[make_data_path])

      print "   Checking: build environment differences (USES flags and incpaths)\n"
      build_env_errors = 0
      expected_list = ['USES_SURF', 'USES_MODEM_PROC', 'USES_APPS_PROC', \
                       'TCFG_BID_IMAGE', 'BUILD_ROOT']
      # These are defined in dmss_8660_flags.min for 7x30, but haven't been
      # cleaned up yet for 8660.
      not_clean_yet = ['USES_ATI_GFX_HW', 'USES_COMPILE_ATI_LIBS', \
                       'USES_ATITESTS', 'USES_GLES_SAMPLES', 'USES_GLES_CL',
                       'ATITESTS_INC', 'ATITESTS_DIR', 'USES_3D', 'USES_R2D']
      check_w_Wilhem = ['USES_COREBSP_BUILD_SYSTEM', 'MAKEFILE_LIST', \
                        'BUILD_FEATURE_WCDMA_DL_ENHANCED', 'BUILD_FEATURE_MBMS', \
                        'USES_FLAGS']
      for symbol in tbc_env:
         if symbol not in (expected_list +
                           not_clean_yet +
                           check_w_Wilhem):
            if symbol not in make_env:
               print "      Error:", symbol, "in local build, but not in make build"
               build_env_errors += 1
            elif tbc_env[symbol] != make_env[symbol]:
               print "      Error:", symbol, "has different values in make and local builds:"
               print "         Make build:"
               print "           ", make_env[symbol]
               print "         Local build:"
               print "           ", tbc_env[symbol]
               build_env_errors += 1

      for symbol in make_env:
         if symbol not in (expected_list +
                           not_clean_yet +
                           check_w_Wilhem):
            if symbol not in tbc_env:
               print "      Error:", symbol, "in make build, but not in local build"
               build_env_errors += 1

      if build_env_errors:
         print "     ", build_env_errors, "build environment errors found.\n"
      else:
         print "         No build environment errors found.\n"

      #------------------------------------------------------------------------
      # Now compare the cust/targ header files to find any differences
      #------------------------------------------------------------------------

      print "   Checking: header file differences (cust/targ<bid>.h)\n"
      for header in glob(make_build_ms_path + '*' + bid + '.h'):
         header = os.path.basename(header)
         print "     ", header, "\n"
         check_headers(make_build_ms_path + header,
                       local_build_ms_path + header)

#---------------------------------------------------------------------
# Function def_split()
# 
# This is used to split the #define statements into component parts,
# including both the symbol and the value, but some define statements
# don't have values, and this causes a problem.  This function will add
# a None object to the return list if it doesn't have enough elements.
#---------------------------------------------------------------------

def def_split (string, count):
   result = string.split(' ', count)
   if len(result) < 3:
      result.append(None)
   return result

#---------------------------------------------------------------------
# Function check_headers(header_1, header_2)
# 
# Compare the specified header files to make sure they're equivalent.
#---------------------------------------------------------------------

def check_headers (header_1, header_2):

   # Initialize data structures

   header_1_defines  = dict()
   header_1_includes = set()
   header_1_undefs   = set()

   header_2_defines  = dict()
   header_2_includes = set()
   header_2_undefs   = set()

   if header_1[-3] == 'a':
      proc = "IMAGE_APPS_PROC"
   else:
      proc = "IMAGE_MODEM_PROC"

   # Parse header file 1

   header_1_file = open(header_1, 'r')
   skip = False

   for line in header_1_file.readlines():
      line = line.strip()

      if not skip:
         if (line.startswith('#ifdef IMAGE_') and
             line.split()[1] != proc):
            skip = True
         elif line.startswith('#define'):
            (define, symbol, value) = def_split(line, 2)
            header_1_defines[symbol] = value
         elif line.startswith('#include'):
            (include, incfile) = line.split(' ', 1)
            header_1_includes.add(incfile)
         elif line.startswith('#undef'):
            (undef, symbol) = line.split(' ', 1)
            header_1_undefs.add(symbol)
      elif line.startswith('#endif'):
         skip = False

   header_1_file.close()

   # Parse header file 2

   header_2_file = open(header_2, 'r')
   skip = False

   for line in header_2_file.readlines():
      line = line.strip()

      if not skip:
         if (line.startswith('#ifdef IMAGE_') and
             line.split()[1] != proc):
            skip = True
         elif line.startswith('#define'):
            (define, symbol, value) = def_split(line, 2)
            header_2_defines[symbol] = value
         elif line.startswith('#include'):
            (include, incfile) = line.split(' ', 1)
            header_2_includes.add(incfile)
         elif line.startswith('#undef'):
            (undef, symbol) = line.split(' ', 1)
            header_2_undefs.add(symbol)
      elif line.startswith('#endif'):
         skip = False

   header_2_file.close()

   # These symbols are known differences between make and tcfg builds
   expected_list = ['FEATURE_SEC']

   # Look for stuff that's in header file 1, and not in header file 2
   #import pdb; pdb.set_trace()
   header_errors = 0
   for symbol in header_1_defines:
      if symbol not in expected_list:
         if symbol not in header_2_defines:
            print symbol, "defined in", header_1, "but not in ", header_2
            header_errors +=1

   for incfile in header_1_includes:
      if symbol not in expected_list:
         if incfile not in header_2_includes:
            print incfile, "included in", header_1, "but not in ", header_2
            header_errors +=1

   for symbol in header_1_undefs:
      if symbol not in expected_list:
         if symbol not in header_2_undefs:
            print symbol, "undef'd in", header_1, "but not in ", header_2
            header_errors +=1

   # Look for stuff that's in header file 2, and not in header file 1

   for symbol in header_2_defines:
      if symbol not in expected_list:
         if symbol not in header_1_defines:
            print symbol, "defined in", header_2, "but not in ", header_1
            header_errors +=1

   for incfile in header_2_includes:
      if symbol not in expected_list:
         if incfile not in header_1_includes:
            print incfile, "included in", header_2, "but not in ", header_1
            header_errors +=1

   for symbol in header_2_undefs:
      if symbol not in expected_list:
         if symbol not in header_1_undefs:
            print symbol, "undef'd in", header_2, "but not in ", header_1
            header_errors +=1

   # Look for differences in define values between header file 1 and header file 2
   for symbol in header_1_defines:
      if symbol not in expected_list:
         if ((symbol in header_2_defines) and
             (header_1_defines[symbol] != header_2_defines[symbol])):
            print symbol, "defined differently:"
            print "  ", header_1, ":", header_1_defines[symbol]
            print "  ", header_2, ":", header_2_defines[symbol]
            header_errors +=1

   if header_errors:
      print "     ", header_errors, "header file errors found.\n"
   else:
      print "         No header file errors found.\n"

#=====================================================================
# Call main
#=====================================================================

if len(sys.argv) == 2:
   main(sys.argv[1])
else:
   print "Usage:"
   print "   tbc_check <make_build_root>"
   print
   print "E.g.:"
   print "   pushd \\humor\builds416\TEST\M8660AAABQNBAA0007"
   print "   cd AMSS\\products\\8660"
   print "   tbc_check z:"

