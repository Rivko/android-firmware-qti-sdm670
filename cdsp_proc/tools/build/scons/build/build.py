'''Image Top-Level Build Script'''
#===============================================================================
#
# Image Top-Level Build Script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/build/build.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Change: 7115037 $
#
#===============================================================================

#================================
# Import Python Libraries
#================================
import sys
import os
import time
from xml.etree import ElementTree as et

import logger

def delta_time(start, end):
    '''Calculate the delta between two times'''
    delta = end - start
    minutes = delta / 60
    seconds = delta % 60
    delta_str = "%d minutes, %d seconds" % (minutes, seconds)
    return delta_str

def build(lg, image_cfg_str, args):
    '''Parent script for QC-SCons Build System'''

    #================================
    # Get our bearings
    #================================

    on_linux          = sys.platform != 'win32'
    build_root        = os.path.realpath(os.path.join( \
                            os.path.dirname(__file__), "../../../..")) 
    targ_root         = os.path.realpath(os.path.join( \
                            os.path.dirname(__file__), "../../../../.."))
    tools_root        = os.path.join(build_root, "tools")
    qcscons_root      = os.path.join(tools_root, "build/scons")
    qcscons_cmd       = os.path.realpath(os.path.join( \
                            qcscons_root, "SCons/scons"))
    if not on_linux:
        qcscons_cmd += '.bat'
    qcscons_script    = os.path.realpath(os.path.join( \
                            qcscons_root, "build/start.scons"))

    #================================
    # Parse command line parameters
    # Multiple Build IDs can be specified with the syntax:
    #      BUILD_ID=bid1,bid2,...
    # Any file that matches *image_cfg*.xml 
    # will be interpreted as the image_cfg file.
    #================================
    image_cfg_fn = None
    build_id_list = None
    cleaned_args  = []
    for arg in args:
        if arg.startswith("BUILD_ID="):
            # Let command line override list from image_cfg.xml
            build_id_list = arg[9:].split(',')
        elif arg.startswith('IMAGE_CFG='):
            image_cfg_fn = arg.split('=')[1]
        elif arg.startswith('-f'):
            qcscons_script = arg.split('=')[-1]
        else:
            # All other args are passed through to QC-SCons
            cleaned_args.append(arg)

    #================================
    # Import Image-Specific Configuration Information
    #================================
    #==========================================
    # Retrieve image values for build parameters
    # (This will be pulled from a config file later)
    #==========================================

    # Read data from image config data:
    if image_cfg_fn:
        # Specifying an image_cfg file on the command line over-rides 
        # image_cfg XML data passed in as a parameter
        xml_image_cfg = et.ElementTree(file = image_cfg_fn).getroot()
    elif image_cfg_str:
        xml_image_cfg = et.fromstring(image_cfg_str)
    else:
        # If nothing is specified, look for file called 'image_cfg.xml' in
        # the current directory.
        xml_image_cfg = et.ElementTree(file = "image_cfg.xml").getroot()


    # Read build flags
    lg.log("Reading Image Config")
    lg.log("  Build Flags:")
    build_flags = dict()
    for child in xml_image_cfg.find('build_flags').getchildren():
        bf_name = child.tag.upper()
        bf_val  = child.text
        lg.log("    " + bf_name + " = " + bf_val)
        if bf_val:
            bf_val = bf_val.strip()
        if bf_name == 'BUILD_ID':
            # Command line build IDs override image_cfg build IDs
            if not build_id_list:
                build_id_list = bf_val.split(',')
        elif bf_name == 'COMPILER_ID':
            compiler_id = bf_val
        else:
            build_flags[bf_name] = bf_val

    # Read environment variable settings
    lg.log("  Environment Variables:")
    for child in xml_image_cfg.find('env_vars').getchildren():
        var_name  = child.find('var_name').text
        if var_name:
            var_name = var_name.strip()
        var_value = child.find('var_value').text
        if var_value:
            var_value = var_value.strip()
        lg.log("    " + var_name + " = " + var_value)
        os.environ[var_name] = var_value

    #================================
    # Setup environment variables that SCons needs
    #================================

    os.environ["BUILD_ROOT"]  = build_root
    os.environ["TARGET_ROOT"] = targ_root

    #================================
    # Hacks for backward-compatibility
    #================================
    # At least one tool accesses MSM_ID from os.environ, rather than the SCons
    # environment, so we need to add this there:
    os.environ['MSM_ID'] = build_flags['MSM_ID']

    #================================
    # Load OEM top-level build script, if available
    #================================
    if 'SCONS_BUILD_SCRIPTS_OEM_ROOT' in os.environ:
        oem_root = os.environ['SCONS_BUILD_SCRIPTS_OEM_ROOT']
        if os.path.exists(os.path.join(oem_root, 'build_oem.py')):
            sys.path.append(oem_root)
            import build_oem
   
    #================================
    # Log the environment settings
    #================================
    lg.log("#-------------------------------------------------------------" \
        "------------------")
    lg.log("# ENVIRONMENT BEGIN")
    lg.log("#-------------------------------------------------------------" \
        "------------------")
    env_vars = os.environ.keys()
    env_vars.sort()
    for var in env_vars:
        lg.log(var + " = " + os.environ[var])
    lg.log("#-------------------------------------------------------------" \
        "------------------")
    lg.log("# ENVIRONMENT END")
    lg.log("#-------------------------------------------------------------" \
        "------------------")

    #================================
    # Call QC-SCons
    #================================
   
    ret = 1  # Fail if no build_ids specified
    build_time_log = dict()
    for bid in build_id_list:
        build_cmd_line = [qcscons_cmd, "-f", qcscons_script, "BUILD_ID=" + bid]
        build_cmd_line += \
           [key + "=" + build_flags[key] for key in sorted(build_flags.keys())]
        build_cmd_line += cleaned_args
        lg.log("#---------------------------------------------------------" \
            "----------------------")
        lg.log("# BUILD BEGIN: " + bid)
        lg.log("#---------------------------------------------------------" \
            "----------------------")
        build_time_log[bid] = [time.time()]  # Remember the start time.
        ret = lg.log_exec(build_cmd_line)
        build_time_log[bid].append(time.time())  # Remember the end time.
        lg.log("#---------------------------------------------------------" \
            "----------------------")
        lg.log("# BUILD END: " + bid)
        lg.log("#---------------------------------------------------------" \
            "----------------------")
        if ret != 0:
            break

    # Report build times
    for bid in build_id_list:
        build_start_time = build_time_log[bid][0]
        build_end_time   = build_time_log[bid][1]
        lg.log("Build " + bid + ": Start Time: " + \
            time.ctime(build_start_time) + \
            ",  End Time: " + time.ctime(build_end_time))
        lg.log("Build " + bid + ": Delta Time: " + \
            delta_time(build_start_time, build_end_time))
        lg.log("#---------------------------------------------------------" \
            "----------------------")

    return ret

# End of build()

# Allow this script to be called from the OS command shell,
# or from another Python script.
if __name__ == "__main__":
    LG = None
    try:
        #================================
        # Look for --no_stdout parameter, which affects logging
        #================================
        ARGS = sys.argv[:]
        LOG_TO_STDOUT = True
        LOG_FILE_NAME = 'build'
        for ARG in ARGS:
            if ARG.lower() == '--no_stdout':
                LOG_TO_STDOUT = False
                sys.argv.remove(ARG)
            elif ARG.lower().startswith('--log_name'):
                LOG_FILE_NAME = ARG.rsplit('=', 1)[1]
                sys.argv.remove(ARG)

        #================================
        # Initialize logging
        #================================

        LG = logger.Logger(LOG_FILE_NAME, log_to_stdout = LOG_TO_STDOUT)

        LG.log("#---------------------------------------------------------" \
            "----------------------")
        LG.log("# BUILD START")
        LG.log("#---------------------------------------------------------" \
            "----------------------")
        START_TIME = time.time()
        LG.log("Start Time: " + time.ctime(START_TIME))
        LG.log("Running $Header: //components/rel/scons.qc/1.1/build/build.py#3 $")

        # Call the build function
        RET = build(log = LG, image_cfg_str = None, args = sys.argv[1:])

        END_TIME = time.time()
        LG.log("Overall Start Time: " + time.ctime(START_TIME) + \

            ",  Overall End Time: " + time.ctime(END_TIME))
        LG.log("Overall Delta Time: " + delta_time(START_TIME, END_TIME))
        LG.log("#---------------------------------------------------------" \
            "----------------------")
    except:
        # Log any crashes to the log file
        RET = 1
        if LG:
            LG.log_crash()
        raise
    finally:
        if LG:
            del LG 
    sys.exit(RET)
