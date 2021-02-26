#===============================================================================
# GENERAL DESCRIPTION
#   Scons builder to compiler r-init input files and generate initialization
#   code for different hardware accelerators.
#
# Copyright (c) 2016-2017 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#-------------------------------------------------------------------------------

import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
RINIT_ENV_DESCRIPTOR = 'RINIT_FILES'
RINIT_USES_FLAG = 'USES_RINIT_PARSER'
RINIT_FILE_TAG = 'files'

#------------------------------------------------------------------------------
# Utility functions
#------------------------------------------------------------------------------
def flat_path_arg(env):
    '''
    This function returns the argument containing version specific flat files
    '''
    build_root = env['BUILD_ROOT']
    chipset = env['CHIPSET']

    base_path = build_root + '/core/api/systemdrivers/hwio/' + chipset
    default_path =  [(base_path + '/M3_ADDRESS_FILE.FLAT', 1.0)]

    # Dictionary for version specific FLAT files for target where applicable
    #    Key - target id, 
    #    Value - list of (flat_file, version)
    target_flat_files = \
    {
        'sdm845' : [(base_path + '/v1/M3_ADDRESS_FILE.FLAT', 1.0),
                    (base_path + '/v2/M3_ADDRESS_FILE.FLAT', 2.0)],

        # Add new targets with multiple flat files as necessary
    }

    flat_paths = target_flat_files.get(chipset, default_path)

    flat_paths = ['--flat=%s,%s' %(e[0], str(e[1])) for e in flat_paths]

    return ' '.join(flat_paths)


#------------------------------------------------------------------------------
# Scons framework plugins
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    '''
    Sets up an interface of the custom r-init builder for clients to pass
    input files
    '''

    env.PrintDebugInfo('rinit_builder', 'Constructing r-init builder')

    # Adding a usage flag to environment
    env.AddUsesFlags(RINIT_USES_FLAG, from_builds_file = True)

    # List of input files - Empty at the begining
    rootenv = env.get('IMAGE_ENV')
    rootenv[RINIT_ENV_DESCRIPTOR] = []

    # Adding a method for clients to specify input file
    rootenv.AddMethod(rinit_add_files, 'AddRinitInfo')

    # Constructing the custom builder
    rinit_action = env.GetBuilderAction(rinit_compiler, action_source = None)
    rinit_scanner = env.Scanner(rinit_scanner_fn, name = 'RinitScanner')
    rinit_bldr = env.Builder(action = rinit_action, emitter = rinit_emitter,
                             target_scanner = rinit_scanner)

    # Adding the newly created builder to environment
    rootenv.Append(BUILDERS = {'RinitBuilder' : rinit_bldr})

    return


def rinit_add_files(env, target, input_dict):
    '''
    This function gets called when client invokes AddRinitInfo
    '''
    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag(RINIT_USES_FLAG) is False or \
       not env.IsTargetEnable(target):
        env.PrintWarning('Adding rinit file for unsupported environemnt')
        return

    # Input validation
    ip_paths = input_dict.get(RINIT_FILE_TAG, None)
    if ip_paths is None or ip_paths == []:
        rinit_error(env, 'Input field (' + RINIT_FILE_TAG + ') is missing or empty')

    if type(ip_paths) is not list:
        ip_paths = [ip_paths]

    # Adding files at path to the global list
    for ip_path in ip_paths:
        # Path validity
        if not env.PathExists(ip_path):
            rinit_error(env, 'Path ' + str(ip_path) + ' not found')

        # Full path
        full_path = env.RealPath(ip_path)
        if os.path.isdir(full_path):
            # if path is dictionary, find files with rinit extensions
            path_files = env.FindFiles(file_name_patterns = ['*.rinit', '*.RINIT'],
                                       root_dir = full_path)
        else:
            # if path is a file, use it in list
            path_files = [full_path]

        env.Depends(target, path_files)
        for path_file in path_files:
            if path_file not in rootenv[RINIT_ENV_DESCRIPTOR]:
                env.PrintDebugInfo('rinit_builder', 
                                   'Adding RINIT file to list ' + str(path_file))
                print 'Adding RINIT file %s' %(str(path_file))
                rootenv[RINIT_ENV_DESCRIPTOR].append(path_file)
            else:
                env.PrintDebugInfo('rinit_builder', 
                                   'Duplicate input file found ' + str(path_file))

    return


def rinit_emitter(target, source, env):
    '''
    Put dependency of each input file on to the target so that if any of them
    changes, builder gets called
    '''
    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag(RINIT_USES_FLAG) is False:
        return

    source.append(env.Value(rootenv[RINIT_ENV_DESCRIPTOR]))
    return (target, source)


def rinit_scanner_fn(node, env, path):
    '''
    This function will be called once all input files are added to the build
    environment. Put dependency on each of them.
    '''

    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag(RINIT_USES_FLAG) is False:
        return

    file_nodes = list(rootenv.get(RINIT_ENV_DESCRIPTOR, []))
    file_nodes.extend([node])
    return file_nodes


def rinit_compiler(target, source, env):
    '''
    This will launch RINIT compiler.
    '''

    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag(RINIT_USES_FLAG) is False:
        return

    ip_files = rootenv[RINIT_ENV_DESCRIPTOR]
    ip_files.sort()             # Deteministic output in multithreaded builds

    # Constructing cmd and arguments to trigger the R-Init compiler
    build_root = rootenv['BUILD_ROOT']
    rinit_path = build_root + '/core/power/rinit/scripts/rinit_main.py'
    hwio_path = build_root + '/core/power/rinit/scripts/'       # temporary
    
    fmt = 'aop'
    data_sec = 'rinit_data_sec'
    files = ' '.join(ip_files)

    rinit_cmd = 'python ' + rinit_path + ' ' + \
                flat_path_arg(env) + ' --path=' + hwio_path + \
                ' --fmt=' + fmt + ' --sec=' + data_sec + \
                ' --op=' + env.RealPath(target[0].path) + \
                ' ' + files
    env.PrintDebugInfo('rinit_builder', 'Invoking %s' %rinit_cmd)

    std_op, std_err, ret_val = env.ExecCmds(cmds=rinit_cmd, 
                                            target=target, shell=True)
    if ret_val != 0:
        rinit_error(env, 'RINIT compilation failed %s' %(str(std_err)))

    return


def rinit_error(env, err_str):
    env.PrintError('Rinit builder error: ' + err_str)
    raise Exception(err_str)
