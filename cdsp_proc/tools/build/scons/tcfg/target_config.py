'''
Target Configuration

GENERAL DESCRIPTION
   Main entry point for the Target Configurator
'''
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
#  $Header: //components/rel/scons.qc/1.1/tcfg/target_config.py#9 $
#  $DateTime: 2015/11/19 03:27:05 $
#  $Change: 9446675 $
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
import glob
import SCons.Script
import atexit

# Define all important paths
BUILD_PRODUCTS_PATH = "${BUILD_ROOT}/build/ms/"
TARGET_CONFIG_PATH = "${BUILD_ROOT}/build/ms/"
TARGET_TOOLS_PATH = "${BUILD_ROOT}/tools/build/scons/tcfg/"

# Define file name templates for generated target config files
CUST_FILE_NAME_TEMPLATE = "cust%s.h"        # Standard cust<bid>.h file
TARG_FILE_NAME_TEMPLATE = "targ%s.h"        # Standard targ<bid>.h file
TCFG_FILE_NAME_TEMPLATE = "tcfg_%s.py"      # New data file for target config
                                            # (mostly USES_* flags)

# Define file name templates for source target config files
BLD_SCRIPT_NAME_TEMPLATES = ["${BUILD_ROOT}/build/ms/tbc*.builds",
                             # Nested .builds files
                             "${BUILD_ROOT}/*/bsp/build/tbc*.builds",
                             "${BUILD_ROOT}/*/build/tbc*.builds",
                             "${BUILD_ROOT}/*/*/build/tbc*.builds",
                             # Used for SU-level components,
                             #e.g. <build_root>/modem/hdr/build/...
                            ]
TBC_SCRIPT_NAME_TEMPLATES = ["${BUILD_ROOT}/build/ms/tbc*.py",
                             # Target Build Config files
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
    '''
       Function: exists

       Standard SCons exists() function
    '''
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
    '''
       Function: generate

       Standard SCons generate() function

       Initialize global data.
    '''
    global BUILD_PRODUCTS_PATH
    global TARGET_CONFIG_PATH
    global TARGET_TOOLS_PATH

    BUILD_PRODUCTS_PATH = env.subst(BUILD_PRODUCTS_PATH)
    TARGET_CONFIG_PATH = env.subst(TARGET_CONFIG_PATH)
    TARGET_TOOLS_PATH = env.subst(TARGET_TOOLS_PATH)

    sys.path.append(TARGET_TOOLS_PATH)

    env['TARGET_INCPATHS'] = dict()

    env.AddMethod(GenTargetConfig, "GenTargetConfig")
    env.AddMethod(GetTcfgImage, "GetTcfgImage")
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
    '''
       Function: GenTargetConfig

       This function reads the target configuration, generates the cust/targ.h
       files, and adds a list of USES_* flags specified by the target scripts
       to the construction environment.

       It also stores target incpath information in the environment, which can
       be retrieved later through the env.GetTargetIncpaths() method.
    '''

    env.PrintInfo("Begin Target Config: " + build_id)

    tcfgf_opt = env.GetOption('tcfgf')

    if tcfgf_opt is None:
        bld_target_path = TARGET_CONFIG_PATH
        bld_target_file = "target.builds"
    else:
        bld_target_path, bld_target_file = os.path.split(str(tcfgf_opt))
        if bld_target_path == '':
            bld_target_path = TARGET_CONFIG_PATH

    # Generate the names of the data files used to check if the .builds file has
    # been updated since the last build.  If they have, we'll need to rebuild
    # the configuration files.
    buildsfile_name = os.path.join(bld_target_path, bld_target_file)
    buildsfile_md5 = os.path.join(BUILD_PRODUCTS_PATH, bld_target_file + \
                        ".${SHORT_BUILDPATH}.md5")

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
        if env.GetOption('clean') and not env.GetOption('cleanpack'):
            atexit.register(clean_all_products, env, build_id.lower())
        env.PrintInfo("Target configuration file: %s" % bld_target_file)

        # Check to see if we need to regen the config files
        tcfg_files_changed = \
            not env.CompareAndUpdateMD5File(buildsfile_name, buildsfile_md5)
        for template in BLD_SCRIPT_NAME_TEMPLATES + TBC_SCRIPT_NAME_TEMPLATES:
            for file_name in glob.glob(env.subst(template)):
                md5_file = file_name + ".${SHORT_BUILDPATH}.md5"
                tcfg_files_changed |= \
                    not env.CompareAndUpdateMD5File(file_name, md5_file)

        if tcfg_files_changed:
            env.PrintInfo("Target Config files have changed.  Reparsing")
            clean_all_products(env, build_id.lower())

        if not check_bid_products(env, build_id.lower()):
            # Build new target config product files as needed.
            env.PrintInfo("Generating Target Config Files for " + build_id)
            # Read the PL from the environment variable
            pl = None
            if 'PRODUCT_LINE' in env:
                pl = env['PRODUCT_LINE']
            # Change directory to the build\ms directory for the parser to work
            saved_dir = os.getcwd()
            os.chdir(TARGET_CONFIG_PATH)
            import tcfgparser
            tcfg = tcfgparser.TargetConfigurationDb()
            tcfg_search_paths = [env.subst(path).rsplit('/', 1)[0] + '/' \
                                 for path in BLD_SCRIPT_NAME_TEMPLATES]
            gen_result = tcfg.ParseBuildFile(buildsfile_name, pl,
                                             search_paths=tcfg_search_paths)
            if gen_result:
                tcfg_bid_data = tcfg.GenConfigData(build_id, pl)
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
    for template in TBC_SCRIPT_NAME_TEMPLATES:
        for tool_name in glob.glob(env.subst(template)):
            tool_dir = os.path.dirname(tool_name)
            tool_name = os.path.basename(tool_name)[0:-3] # drop the ".py"
            env.LoadToolScript(tool_name, toolpath=[tool_dir])

    env.PrintInfo("End Target Config: " + build_id)

    if env.PathExists(buildsfile_name):
        ret_val = gen_result and check_bid_products(env, build_id.lower())
    else:
        ret_val = gen_result

    return ret_val

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
    '''
       Function: WriteTcfgBidData

       Write the TCFG Build ID data to a file, to be retrieved later.
    '''

    tcfg_bid_data_file = \
    open(BUILD_PRODUCTS_PATH + TCFG_FILE_NAME_TEMPLATE % build_id.lower(), 'w')

    tcfg_bid_data_file.write("""\
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

    for flag in sorted(tcfg_bid_data.uses_flags):
        tcfg_bid_data_file.write("""\
   env.AddUsesFlags('%s', from_builds_file = True)
""" % flag)

    # Output the tcfg config lines.
    for flag in sorted(tcfg_bid_data.tcfg_flags):
        tcfg_bid_data_file.write('''\
   env.AddTcfgFlag('%s', '%s', from_builds_file = True)
''' % (flag, tcfg_bid_data.tcfg_flags[flag]))

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
    '''
       Function: ReadTcfgBidData

       Read the TCFG Build ID data from the data file.
    '''
    tcfg_fname = TCFG_FILE_NAME_TEMPLATE % build_id.lower()
    tcfg_path = "".join([TARGET_CONFIG_PATH, "/", tcfg_fname])

    if env.PathExists(tcfg_path):
        env.LoadToolScript(tcfg_path)

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
def GetTcfgImage(env):
    '''
       Function: GetTcfgImage

       This function returns the name of the image that was specified in the
       .builds file.  Note that this is different than the build system image
       name.
    '''
    return env['TCFG_BID_IMAGE']

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
def AddTargetIncpaths(env, **incpaths):
    '''
       Function: AddTargetIncpaths

       Add the specified dictionary of name/path mappings to the environment.

       This is a legacy function that should go away as our include paths get
       published through the proper mechanism.
    '''
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

def GetTargetIncpaths(env):
    '''
       Function: GetTargetIncpaths

       Return a dictionary of name/path mappings
       that were defined by the target.

       This is a legacy function that should go away as our include paths get
       published through the proper mechanism.
    '''
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
def check_bid_products(env, build_id):
    '''
       Function: check_bid_products

       Local function used to check if the build product files files for the
       specified build ID are there.  These files include the cust<bid>.h,
       targ<bid>.h and pickled build ID information from the .builds file.
    '''
    return (env.PathExists(BUILD_PRODUCTS_PATH + \
                           CUST_FILE_NAME_TEMPLATE % build_id) and
            env.PathExists(BUILD_PRODUCTS_PATH + \
                           TARG_FILE_NAME_TEMPLATE % build_id) and
            env.PathExists(BUILD_PRODUCTS_PATH + \
                           TCFG_FILE_NAME_TEMPLATE % build_id))

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
def clean_all_products(env, bid):
    '''
       Function: clean_all_products

       Local function used to remove all build product files.  These files
       include the cust<bid>.h, targ<bid>.h and pickled build ID information
       from the .builds file.

       clean_all_products is different from check_bid_products in that it
       removes all target config products regardless of the build ID in
       the name.  It is used to remove all target config products when
       the .builds file or target config scripts have changed.
    '''
    env.PrintInfo("Cleaning all target config products for %s" % bid)
    for fname in glob.glob(BUILD_PRODUCTS_PATH + CUST_FILE_NAME_TEMPLATE % bid):
        env.PrintInfo("Removed %s" % fname)
        os.remove(fname)
    for fname in glob.glob(BUILD_PRODUCTS_PATH + TARG_FILE_NAME_TEMPLATE % bid):
        env.PrintInfo("Removed %s" % fname)
        os.remove(fname)
    for fname in glob.glob(BUILD_PRODUCTS_PATH + TCFG_FILE_NAME_TEMPLATE % bid):
        env.PrintInfo("Removed %s" % fname)
        os.remove(fname)
