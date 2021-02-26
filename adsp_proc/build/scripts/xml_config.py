#===============================================================================
#
# XML Target Configuration
#
# GENERAL DESCRIPTION
#     Main entry point for the XML Target Configurator
#
# Copyright (c) 2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0/scripts/xml_config.py#1 $
#  $DateTime: 2017/09/03 21:39:31 $
#  $Change: 14261189 $
#                             EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when         who      what, where, why
# --------    ---      ---------------------------------------------------------
# 01/15/14    sdt      Initial release.
#
#===============================================================================
import sys
import os
import glob
import SCons.Script
# import pdb; pdb.set_trace()
# Define all important paths
build_products_path = "${BUILD_ROOT}/build/ms/"
target_config_path  = "${BUILD_ROOT}/build/config/default/"
target_tools_path   = "${BUILD_ROOT}/tools/build/scons/tcfg/"

# Define file name templates for generated target config files
# Standard cust<bid>.h file
cust_file_name_template = "cust%s.h"
# Standard targ<bid>.h file
targ_file_name_template = "targ%s.h"
# New data file for target config (mostly USES_* flags)
tcfg_file_name_template = "tcfg_%s.py"

# Define file name templates for source target config files
bld_script_name_templates = ["${BUILD_ROOT}/build/ms/tbc*.builds",
                             "${BUILD_ROOT}/*/bsp/build/tbc*.builds",
                             "${BUILD_ROOT}/*/build/tbc*.builds",
                             "${BUILD_ROOT}/*/*/build/tbc*.builds"]
# Used for SU-level components, e.g. <build_root>/modem/hdr/build/...

tbc_script_name_templates = ["${BUILD_ROOT}/build/ms/tbc*.py",
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
    return env.Detect('xml_config')

#------------------------------------------------------------------------
# Function: generate
# 
# Standard SCons generate() function
# 
# Initialize global data.
# 
#------------------------------------------------------------------------
def generate(env):
    global xmldicts
    global group_private_list
    global group_prot_list
    global group_public_list
    global build_products_path
    global target_config_path
    global target_tools_path

    build_products_path = env.subst(build_products_path)
    target_config_path  = env.subst(target_config_path)
    target_tools_path   = env.subst(target_tools_path)

    # Add the location of the tcfgparser to the path.
    sys.path.append(target_tools_path)

    # Look for target_variation in the ${BUILD_ROOT}/build/config path.
    xml_variation = env.subst(target_config_path + "target_variation.xml")

    env.PrintDebugInfo('xml', "Looking for XML file " + xml_variation)

    # If the file isn't there then the script should not load.
    if not env.PathExists(xml_variation):
        raise RuntimeError, 'Unable to find ' + xml_variation
        return

    # Set these so that SCONS thinks this is a tcfg configurator.
    env['TARGET_CONFIG'] = xml_variation
    env['TARGET_INCPATHS'] = dict()

    # Setup some frequently used environment variables.
    build_var = env['BUILD_ID']
    pl = env['PRODUCT_LINE']

    # Initialize these global variables if not done.
    if 'xmldicts' not in globals():
        xmldicts = dict()
        group_private_list = dict()
        group_prot_list = dict()
        group_public_list = dict()

    # Check to see if we need to regen the config files
    xml_variation_md5 = xml_variation + ".md5"
    files_changed = not env.CompareAndUpdateMD5File(xml_variation,
                                                    xml_variation_md5)
    env.PrintDebugInfo('xml', "MD5 comparison of  " +
                       xml_variation +
                       " returned " +
                       str(files_changed))
    for template in bld_script_name_templates + tbc_script_name_templates:
        for mfile in glob.glob(env.subst(template)):
            md5_file = mfile + ".md5"
            files_changed |= not env.CompareAndUpdateMD5File(mfile, md5_file)
            env.PrintDebugInfo('xml', "MD5 comparison of  " +
                               mfile +
                               " returned " + str(files_changed))

    # If the XML file or *.builds files have been touched, clean header files.
    if files_changed:
        env.PrintInfo("Target Config files have changed. Reparsing")
        clean_all_products(env)

    # Build new target config product files as needed.
    if not check_bid_products(env, build_var.lower()):
        env.PrintInfo("Generating Target Config Files for " + build_var)
        gen_xml_target_config(env, build_var, pl, xml_variation)

    env.AddMethod(AddTargetIncpaths, "AddTargetIncpaths")
    env.AddMethod(GetTargetIncpaths, "GetTargetIncpaths")

    # Load the tcfg*.py script to add uses flags to environment.
    tcfg_fname = tcfg_file_name_template % build_var.lower()
    tcfg_path = "".join([build_products_path,"/",tcfg_fname])

    if env.PathExists(tcfg_path):
        env.LoadToolScript(tcfg_path)

    # Load Target Config SCons Tools
    for template in tbc_script_name_templates:
        for tool_name in glob.glob(env.subst(template)):
            tool_dir  = os.path.dirname(tool_name)
            tool_name = os.path.basename(tool_name)[0:-3] # drop the ".py"
            env.LoadToolScript(tool_name, toolpath=[tool_dir])

    return None

#------------------------------------------------------------------------
# Import tools for parsing XML files.
#------------------------------------------------------------------------
from xml.etree import ElementTree as et
import logger

#------------------------------------------------------------------------
# Function: xml_config_parse_group
# 
# This function takes a group XML definition and parses the features,
# macros, uses flags and group elements.
# 
# Parameters:
#    component: Dictionary containg component information.
#    var_et:     Element tree object for the group.
# 
# Returns:
#    None.
#------------------------------------------------------------------------
def xml_config_parse_group(component, var_et):
    # Add a new macros dictionary to the component.
    if 'macros' in component:
        macros = component['macros']
    else:
        macros = component['macros'] = dict()
    for macro_et in var_et.findall('.//macro'):
        # Add the macro name and value pair to the dictionary.
        macros[str(macro_et.get('name'))] = macro_et.text
    # Add a new features array to the component.
    if 'features' in component:
        features = component['features']
    else:
        features = component['features'] = list()
    for feature_et in var_et.findall('.//feature'):
        # Add the feature name to the array.
        features.append(str(feature_et.get('name')))
    # Add a new uses array to the component.
    if 'uses' in component:
        uses = component['uses']
    else:
        uses = component['uses'] = list()
    for use_et in var_et.findall('.//uses'):
        # Add the uses_flag name to the array.
        uses.append(str(use_et.get('name')).upper())
    # Add a new groups array to the component.
    if 'groups' in component:
        groups = component['groups']
    else:
        groups = component['groups'] = list()
    for group_et in var_et.findall('.//group'):
        # Add the group to the array.
        groups.append(str(group_et.get('name')))
    # Add a new includes array to the component.
    if 'includes' in component:
        includes = component['includes']
    else:
        includes = component['includes'] = list()
    for include_et in var_et.findall('.//include'):
	    includes.append(str(include_et.get('name')))

#------------------------------------------------------------------------
# Function: gen_xml_target_config
# 
# After API files are scanned, add uses flags to the environemnt.
# 
# Parameters:
#    env:         SCONS environment object
#    build_var: Component being processed
# 
# Returns:
#    None.
#------------------------------------------------------------------------
def gen_xml_target_config(env, build_var, pl, path):
    global xmldicts
    global group_private_list
    global group_prot_list
    global group_public_list
    #import pdb; pdb.set_trace()

    # Parse the config, target_variation.xml.
    tree = et.parse(env.subst(path))
    root = tree.getroot()
    ''' Temporarily disabled to support same config across PLs
    # Verify this XML file if for this PL.
    sw_image = root.get('sw_image')
    if sw_image != pl:
        raise RuntimeError, 'XML files is for ' + sw_image + ' not ' + pl
        return
    '''
    # Look through all of the enable tags for our build ID.
    component = None
    for pl_et in root.findall('.//enable'):
        if pl_et.get('name').lower() == build_var[:-1].lower():
            env.PrintInfo('Found variant ' + build_var + ' for PL ' + pl + '.')
            # Create a dictionary for this component in the Config
            component = xmldicts['target'] = dict()
            # parse the enable section for this variant.
            enable = component['enable'] = dict()
            # Add a new includes array to the target component.
            includes = enable['includes'] = list()
            for include_et in pl_et.findall('.//include'):
                # Add the include file to the array.
                includes.append(str(include_et.get('name')))
            xml_config_parse_group(enable, pl_et)
            break

    # If component not set, then the PL/variant could not be found in the 
    # config file.
    if component == None:
        raise (RuntimeError, 'Unable to find ' + build_var + ' for PL ' + pl
               + '.')
        return

    build_groups = list()
    # Process the groups in the enable group in target_variation.xml.
    for group in enable['groups']:
        group_et = None
        for groups in root.findall('.//group_private'):
            if group == groups.get('name') :
                group_et = groups
                break
        # If this group is in target_variation.xml 
        if group_et is not None:
            # Add the group to the
            xml_config_parse_group(enable, group_et)
        else:
            build_groups.append(group)

    gen_target_builds(env, build_var, pl, enable, build_groups)

    gen_target_config(env, build_var, pl)

#------------------------------------------------------------------------
# Function: gen_target_builds
# 
# Generate target.builds file from XML data.
# 
# Parameters:
#    env:           SCONS environment object
#    build_var:     Component being processed
#    pl:            Product line being used
#    enable:        Dictionary with XML config
#    build_groups:  Array of groups to pull from target team *.builds
#
# Returns:
#    None.
#------------------------------------------------------------------------
def gen_target_builds(env, build_var, pl, enable, build_groups):
    # Create the file in the build/ms directory.
    target_builds_fn = build_products_path + build_var + ".builds"
    target_builds = open(target_builds_fn, 'w')

    # Output the tag map.
    target_builds.write(
'''TAG_MAP {
S0,-: Build Variant
S8: Image Type }
''')

    # Output general aliases
    target_builds.write(
    '''
define 4KB        0x0001000
define 16KB       0x0004000
define 64KB       0x0010000
define 144KB      0x0024000
define 192KB      0x0030000
define 200KB      0x0032000
define 216KB      0x0036000
define 256KB      0x0040000
define 272KB      0x0044000
define 320KB      0x0050000
define 384KB      0x0060000
define 448KB      0x0070000
define 512KB      0x0080000
define 768KB      0x00C0000
define 0MB        0x0000000
define 1MB        0x0100000
define 2MB        0x0200000
define 3MB        0x0300000
define 4MB        0x0400000
define 5MB        0x0500000
define 6MB        0x0600000
define 7MB        0x0700000
define 8MB        0x0800000
define 9MB        0x0900000
define 10MB       0x0A00000
define 11MB       0x0B00000
define 12MB       0x0C00000
define 13MB       0x0D00000
define 14MB       0x0E00000
define 15MB       0x0F00000
define 16MB       0x1000000
define 17MB       0x1100000
define 18MB       0x1200000
define 19MB       0x1300000
define 20MB       0x1400000
define 21MB       0x1500000
define 22MB       0x1600000
define 23MB       0x1700000
define 24MB       0x1800000
define 25MB       0x1900000
define 26MB       0x1A00000
define 27MB       0x1B00000
define 28MB       0x1C00000
define 29MB       0x1D00000
define 30MB       0x1E00000
define 31MB       0x1F00000
define 32MB       0x2000000
define 33MB       0x2100000
define 34MB       0x2200000
define 35MB       0x2300000
define 36MB       0x2400000
define 39MB       0x2700000
define 38MB       0x2600000
define 40MB       0x2800000
define 41MB       0x2900000
define 42MB       0x2A00000
define 43MB       0x2B00000
define 44MB       0x2C00000
define 45MB       0x2D00000
define 46MB       0x2E00000
define 47MB       0x2F00000
define 48MB       0x3000000
define 49MB       0x3100000
define 50MB       0x3200000
define 51MB       0x3300000
define 52MB       0x3400000
define 53MB       0x3500000
define 54MB       0x3600000
define 55MB       0x3700000
define 56MB       0x3800000
define 57MB       0x3900000
define 58MB       0x3A00000
define 59MB       0x3B00000
define 60MB       0x3C00000
define 62MB       0x3E00000
define 63MB       0x3F00000
define 64MB       0x4000000
define 65MB       0x4100000
define 66MB       0x4200000
define 67MB       0x4300000
define 68MB       0x4400000
define 70MB       0x4600000
define 72MB       0x4800000
define 73MB       0x4900000
define 76MB       0x4C00000
define 78MB       0x4E00000
define 79MB       0x4F00000
define 80MB       0x5000000
define 82MB       0x5200000
define 83MB       0x5300000
define 84MB       0x5400000
define 86MB       0x5600000
define 88MB       0x5800000
define 89MB       0x5900000
define 96MB       0x6000000
define 99MB       0x6300000
define 108MB      0x6C00000
define 110MB      0x6E00000
define 112MB      0x7000000
define 114MB      0x7200000
define 116MB      0x7400000
define 117MB      0x7500000
define 118MB      0x7600000
define 119MB      0x7700000
define 120MB      0x7800000
define 123MB      0x7B00000
define 125MB      0x7D00000
define 126MB      0x7E00000
define 128MB      0x8000000
define 131MB      0x8300000
define 134MB      0x8600000
define 139MB      0x8B00000
define 135MB      0x8700000
define 136MB      0x8800000
define 256MB      0x10000000
define 1GB        0x40000000

    '''
    )
    
    # Output the include files.
    for include in enable['includes']:
        target_builds.write(
'''include \"{infile}\"
'''.format(infile = include))

    # Output the build variant header.
    target_builds.write(
'''[Build Variant: {var}]
'''.format(var = build_var[:-1]))

    # Output the groups to use specified in local build files.
    for build_group in build_groups:
        target_builds.write(
'''use {bg}
'''.format(bg = build_group))

    # Output the defines from the XML file.
    for feature in enable['features']:
        target_builds.write(
'''define {define}
'''.format(define = feature))

    # Output the macros from the XML file.
    for macro in enable['macros']:
        target_builds.write(
'''define {define} {value}
'''.format(define = macro, value = enable['macros'][macro]))

    # Output the uses flags from the XML file.
    for use in enable['uses']:
        target_builds.write(
'''{define} makeonly
'''.format(define = use[5:].lower()))

    # Output the closing block.
    target_builds.write(
'''
end_block
[Image Type: M]
image     modem_proc
end_block
[Image Type: Q]
image     modem_proc
end_block
''')

    # Close the builds file.
    target_builds.close()

#------------------------------------------------------------------------
# Function: gen_target_config
# 
# Generate cust*.h, targ*.h, and tcfg_*.py files.
# 
# Parameters:
#    env:             SCONS environment object
#    build_var:     Component being processed
#    pl:              Product line being used
#
# Returns:
#    None.
#------------------------------------------------------------------------
def gen_target_config(env, build_var, pl):
    target_builds_fn = build_products_path + build_var + ".builds"

    # Change directory to the build\ms directory for the parser to work
    saved_dir = os.getcwd()
    os.chdir(build_products_path)

    # Import the legacy tcfgparser code to build the cust*.h, targ*.h, and 
    # tcfg_*.py files.
    import tcfgparser
    tcfg = tcfgparser.TargetConfigurationDb()

    # Setup the search paths for tcfgparser.
    tcfg_search_paths = [env.subst(path).rsplit('/', 1)[0] +
                         '/' for path in bld_script_name_templates]

    # Parse the *.builds files.
    gen_result = tcfg.ParseBuildFile(target_builds_fn,
                                     pl,
                                     search_paths = tcfg_search_paths)
    if not gen_result:
        # Restore the saved directory
        os.chdir(saved_dir)
        raise RuntimeError, 'ParseBuildFile failed.'
        return

    # Generate config header files.
    tcfg_bid_data  = tcfg.GenConfigData(build_var, env['PRODUCT_LINE'])

    # Generate tcfg_*.py file
    tcfg_bid_data_file = open(build_products_path +
                              tcfg_file_name_template % build_var.lower(), 'w')

    # Write header to file.
    tcfg_bid_data_file.write ('''\
#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2014 QUALCOMM Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
    return env.Detect('tcfg_%s_data')

def generate(env):
    # Save the tcfg_bid_data in the env
    env['TCFG_BID_IMAGE'] = '%s'

''' % (build_var, tcfg_bid_data.image.upper()))

    # Output the 
    for flag in sorted(tcfg_bid_data.uses_flags):
        tcfg_bid_data_file.write('''\
    env.AddUsesFlags('%s', from_builds_file = True)
''' % flag)

    # Save the file.
    tcfg_bid_data_file.close()

    # Restore the saved directory
    os.chdir(saved_dir)

#------------------------------------------------------------------------
# Function: AddTargetIncpaths
# 
# Add the specified dictionary of name/path mappings to the environment.
# 
# This is a legacy function that should go away as our include paths get
# published through the proper mechanism.
# 
# Parameters:
#    env:        SCons Construction Environment
#    incpaths: Dictionary of name/path mappings.
#                 I'm not sure of the magic here, but the '**' allows the
#                 caller to use a (keyword=value,[keyword=value]) syntax,
#                 which will get converted to a dictionary in this call.
# 
#                 This has the advantage of not having to put quotes around
#                 the name, but has a disadvantage in that each mapping
#                 appears as a separate argument to the caller, and can run
#                 up against a 250 argument limit for function calls.
# 
#                 I'm mostly using it because it's cool.
# 
# Returns:
#    Nothing
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
#    env:    SCons Construction Environment
# 
# Returns:
#    A dictionary of name/path mappings
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
#    env:         SCons Construction Environment
#    build_id:  Build ID, used as part of the file names for the build products.
# 
# Returns:
#    True if the files exist, False otherwise
#------------------------------------------------------------------------
def check_bid_products (env, build_id):
    return (env.PathExists(build_products_path +
                           cust_file_name_template % build_id) and
            env.PathExists(build_products_path +
                           targ_file_name_template % build_id) and
            env.PathExists(build_products_path +
                           tcfg_file_name_template % build_id))

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
#    None
# 
# Returns:
#    None
#------------------------------------------------------------------------
def clean_all_products (env):
    env.PrintInfo("Cleaning all target config products")
    for cfile in glob.glob(build_products_path + cust_file_name_template % '*'):
        env.PrintInfo("Removing cfile : " + cfile)
        os.remove(cfile)
    for cfile in glob.glob(build_products_path + targ_file_name_template % '*'):
        env.PrintInfo("Removing cfile : " + cfile)
        os.remove(cfile)
    for cfile in glob.glob(build_products_path + tcfg_file_name_template % '*'):
        env.PrintInfo("Removing cfile : " + cfile)
        os.remove(cfile)

