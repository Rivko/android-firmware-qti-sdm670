#===============================================================================
#
# Target Configuration
#
# GENERAL DESCRIPTION
#    Main entry point for the Target Configurator
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/tcfg/target_config.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   dlb'     Created from CoreBSP config.py script.
#
#===============================================================================
import sys
import os
import subprocess
import glob
import cPickle
import SCons.Script

# Define all important paths
build_products_path = "${BUILD_ROOT}/build/ms/"
target_config_path  = "${BUILD_ROOT}/build/ms/"
target_tools_path   = "${BUILD_ROOT}/tools/build/scons/tcfg/"

# Define file name templates for generated target config files
cust_file_name_template = "cust%s.h"        # Standard cust<bid>.h file
targ_file_name_template = "targ%s.h"        # Standard targ<bid>.h file
tcfg_file_name_template = "tcfg_%s.py"  # New data file for target config (mostly USES_* flags)

# Define file name templates for source target config files
bld_script_name_templates = ["${BUILD_ROOT}/build/ms/tbc*.builds",   # Nested .builds files
                             "${BUILD_ROOT}/*/bsp/build/tbc*.builds",
                             "${BUILD_ROOT}/*/build/tbc*.builds"]
tbc_script_name_templates = ["${BUILD_ROOT}/build/ms/tbc*.py",       # Target Build Config files
                             "${BUILD_ROOT}/*/bsp/build/tbc*.py",
                             "${BUILD_ROOT}/*/build/tbc*.py",
                             "${BUILD_SCRIPTS_OEM_ROOT}/tbc*.py"]



#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
#------------------------------------------------------------------------
# Function: exists
# 
# Standard SCons exists() function
# 
#------------------------------------------------------------------------
def exists(env):
   return env.Detect('target_config')

#------------------------------------------------------------------------
# Function: generate
# 
# Standard SCons generate() function
# 
# Initialize global data.
# 
#------------------------------------------------------------------------
def generate(env):
   global build_products_path
   global target_config_path
   global target_tools_path

   build_products_path = env.subst(build_products_path)
   target_config_path  = env.subst(target_config_path)
   target_tools_path   = env.subst(target_tools_path)

   sys.path.append(target_tools_path)

   env['TARGET_INCPATHS'] = dict()
   env['USES_FLAGS'] = list()
   env['BUILDS_USES_FLAGS'] = list()

   env.AddMethod(GenTargetConfig,   "GenTargetConfig")
   env.AddMethod(GetTcfgImage,      "GetTcfgImage")
   env.AddMethod(AddUsesFlags,      "AddUsesFlags")
   env.AddMethod(GetUsesFlag,       "GetUsesFlag")
   env.AddMethod(AddTargetIncpaths, "AddTargetIncpaths")
   env.AddMethod(GetTargetIncpaths, "GetTargetIncpaths")
   return None

#------------------------------------------------------------------------
# Function: GenTargetConfig
# 
# This function reads the target configuration, generates the cust/targ.h
# files, and adds a list of USES_* flags specified by the target scripts
# to the construction environment.
# 
# It also stores target incpath information in the environment, which can
# be retrieved later through the env.GetTargetIncpaths() method.
# 
# Parameters:
#   env:      SCons Construction Environment
#   build_id: String containing full build ID.
# 
# Returns:
#   True if successful, False otherwise.
#------------------------------------------------------------------------
def GenTargetConfig(env, build_id):

   env.PrintInfo("Begin Target Config: " + build_id)

   tcfgf_opt = env.GetOption('tcfgf')

   if tcfgf_opt is None:
      bld_target_file = "target.builds"
   else:
      bld_target_file = str(tcfgf_opt)

   env.PrintInfo("Target configuration file: %s" % bld_target_file)

   # Generate the names of the data files used to check if the .builds file has been
   # updated since the last build.  If they have, we'll need to rebuild the configuration
   # files.
   buildsfile_name = target_config_path + bld_target_file
   buildsfile_md5 = build_products_path + bld_target_file + ".md5"
   
   # nothing to do if clean option is enabled.
#   if env.get('MSM_ID') not in ['7x30']:
#      if env.GetOption('clean'):
#         return None
#   if env.GetOption('clean'):
#      clean_all_products(env)
#      return True

   # check if builds file exists other wise do nothing; most possibly
   # stip/pack build expect everything to be done.
   gen_result = True
   if env.PathExists(buildsfile_name):
   
      # Check to see if we need to regen the config files 
      tcfg_files_changed = not env.CompareAndUpdateMD5File(buildsfile_name, buildsfile_md5)
      for template in bld_script_name_templates + tbc_script_name_templates:
         for file in glob.glob(env.subst(template)):
            md5_file = file + ".md5"
            tcfg_files_changed |= not env.CompareAndUpdateMD5File(file, md5_file)

      if tcfg_files_changed:
         env.PrintInfo("Target Config files have changed.  Reparsing")
         clean_all_products(env)
   
      if not check_bid_products(env, build_id.lower()):
         # Build new target config product files as needed.
         env.PrintInfo("Generating Target Config Files for " + build_id)
         # Change directory to the build\ms directory for the parser to work
         saved_dir = os.getcwd()
         os.chdir(target_config_path)
         import tcfgparser
         tcfg = tcfgparser.TargetConfigurationDb()
         tcfg_search_paths = [env.subst(path).rsplit('/', 1)[0] + '/' for path in bld_script_name_templates]
         gen_result = tcfg.ParseBuildFile(target_config_path + bld_target_file,
                                          search_paths = tcfg_search_paths)
         if gen_result:
            tcfg_bid_data  = tcfg.GenConfigData(build_id)
            if tcfg_bid_data:
               WriteTcfgBidData(env, tcfg_bid_data, build_id)
            else:
               gen_result = False
         # Restore the saved directory
         os.chdir(saved_dir)

   # Header files should already be generated, but we need to read
   # in the pickled tcfg_bid_data.
   ReadTcfgBidData(env, build_id)

   # Load Target Config SCons Tools
   for template in tbc_script_name_templates:
      for tool_name in glob.glob(env.subst(template)):
         tool_dir  = os.path.dirname(tool_name)
         tool_name = os.path.basename(tool_name)[0:-3] # drop the ".py"
         env.LoadToolScript(tool_name, toolpath=[tool_dir])

   env.PrintInfo("End Target Config: " + build_id)
   return gen_result and check_bid_products(env, build_id.lower())

#------------------------------------------------------------------------
# Function: WriteTcfgBidData
# 
# Write the TCFG Build ID data to a file, to be retrieved later.
# 
# Parameters:
#   tcfg_bid_data: Data to be written
#   build_id:      Build ID
# 
# Returns:
#   None
#------------------------------------------------------------------------
def WriteTcfgBidData(env, tcfg_bid_data, build_id):

   tcfg_bid_data_file = open(build_products_path + tcfg_file_name_template % build_id.lower(), 'w')

   tcfg_bid_data_file.write ("""\
#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_%s_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = '%s'

""" % (build_id, tcfg_bid_data.image.upper()))

   for flag in tcfg_bid_data.uses_flags:
      tcfg_bid_data_file.write("""\
   env.AddUsesFlags('%s', from_builds_file = True)
""" % flag)
   
   tcfg_bid_data_file.close()

#------------------------------------------------------------------------
# Function: ReadTcfgBidData
# 
# Read the TCFG Build ID data from the data file.
# 
# Parameters:
#   build_id:      Build ID
# 
# Returns:
#   None
#
# Side effects:
#   Updates environment with Build ID config data
#------------------------------------------------------------------------
def ReadTcfgBidData(env, build_id):
   tcfg_bid_data_file_name = (tcfg_file_name_template % build_id.lower())[0:-3] # drop the ".py"
   env.LoadToolScript(tcfg_bid_data_file_name, toolpath=[target_config_path])

#===============================================================================
# Methods to extract Target Config info
#===============================================================================

#------------------------------------------------------------------------
# Function: GetTcfgImage
# 
# This function returns the name of the image that was specified in the
# .builds file.  Note that this is different than the build system image
# name.
# 
# Parameters:
#   env: SCons Construction Environment
# 
# Returns:
#   String name of the .build file image
#------------------------------------------------------------------------
def GetTcfgImage (env):
   return env['TCFG_BID_IMAGE']

#------------------------------------------------------------------------
# Function: AddUsesFlags
# 
# Add the uses flags to the SCons construction environment
# 
# Parameters:
#   env:   SCons Construction Environment
#   flags: A list of string names of USES flag
#          This may also be a single string.
#   add:   An optional parameter.  If False, the specified flag will
#          be removed from the Construction Environment.
#   from_builds_file:  The USES_* flag comes from the .builds file,
#          and not from within SCons.  The issue here is that these
#          flags need to be specified on the Make command line for
#          builds that are still using Make for some modules.  This
#          flag should only be set to True when adding flags that
#          were set in the .builds file.
# 
# Returns:
#   None
#------------------------------------------------------------------------
def AddUsesFlags (env, flags, add = True, from_builds_file = False):
   if type(flags) != list:
      flags = [flags]
   if add:
      for flag in flags:
         env[flag] = True
         env['USES_FLAGS'].append(flag)
         if from_builds_file == True:
            env['BUILDS_USES_FLAGS'].append(flag)
   else:
      try:
         for flag in flags:
            del env[flag]
            if env['USES_FLAGS'].count(flag) > 0:
               env['USES_FLAGS'].remove(flag)
            if env['BUILDS_USES_FLAGS'].count(flag) > 0:
               env['BUILDS_USES_FLAGS'].remove(flag)
      except:
         pass

#------------------------------------------------------------------------
# Function: GetUsesFlag
# 
# This function returns a boolean indicating whether or not the specified
# flag is defined.
# 
# Parameters:
#   env:  SCons Construction Environment
#   flag: String name of USES flag
# 
# Returns:
#   True if the flag is defined, false otherwise.
#------------------------------------------------------------------------
def GetUsesFlag (env, flag):
   return flag in env

#------------------------------------------------------------------------
# Function: AddTargetIncpaths
# 
# Add the specified dictionary of name/path mappings to the environment.
# 
# This is a legacy function that should go away as our include paths get
# published through the proper mechanism.
# 
# Parameters:
#   env:      SCons Construction Environment
#   incpaths: Dictionary of name/path mappings.
#             I'm not sure of the magic here, but the '**' allows the
#             caller to use a (keyword=value,[keyword=value]) syntax,
#             which will get converted to a dictionary in this call.
# 
#             This has the advantage of not having to put quotes around
#             the name, but has a disadvantage in that each mapping
#             appears as a separate argument to the caller, and can run
#             up against a 250 argument limit for function calls.
# 
#             I'm mostly using it because it's cool.
# 
# Returns:
#   Nothing
#------------------------------------------------------------------------
def AddTargetIncpaths (env, **incpaths):
   env['TARGET_INCPATHS'].update(incpaths)

#------------------------------------------------------------------------
# Function: GetTargetIncpaths
# 
# Return a dictionary of name/path mappings that were defined by the target.
# 
# This is a legacy function that should go away as our include paths get
# published through the proper mechanism.
# 
# Parameters:
#   env:   SCons Construction Environment
# 
# Returns:
#   A dictionary of name/path mappings
#------------------------------------------------------------------------

def GetTargetIncpaths (env):
   return env['TARGET_INCPATHS']

#------------------------------------------------------------------------
# Function: check_bid_products
# 
# Local function used to check if the build product files files for the 
# specified build ID are there.  These files include the cust<bid>.h,
# targ<bid>.h and pickled build ID information from the .builds file.
# 
# Parameters:
#   env:       SCons Construction Environment
#   build_id:  Build ID, used as part of the file names for the build products.
# 
# Returns:
#   True if the files exist, False otherwise
#------------------------------------------------------------------------
def check_bid_products (env, build_id):
   return (env.PathExists(build_products_path + cust_file_name_template % build_id) and
           env.PathExists(build_products_path + targ_file_name_template % build_id) and
           env.PathExists(build_products_path + tcfg_file_name_template % build_id))

#------------------------------------------------------------------------
# Function: clean_all_products
# 
# Local function used to remove all build product files.  These files include
# the cust<bid>.h, targ<bid>.h and pickled build ID information from the
# .builds file.
# 
# clean_all_products is different from check_bid_products in that it
# removes all target config products regardless of the build ID in
# the name.  It is used to remove all target config products when
# the .builds file or target config scripts have changed.
# 
# Parameters:
#   None
# 
# Returns:
#   None
#------------------------------------------------------------------------
def clean_all_products (env):
   env.PrintInfo("Cleaning all target config products")
   for file in glob.glob(build_products_path + cust_file_name_template % '*'):
      os.remove(file)
   for file in glob.glob(build_products_path + targ_file_name_template % '*'):
      os.remove(file)
   for file in glob.glob(build_products_path + tcfg_file_name_template % '*'):
      os.remove(file)


