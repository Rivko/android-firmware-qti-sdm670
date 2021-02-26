'''
   BUILDSPEC file builder
'''
#===============================================================================
#
# BUILDSPEC file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header:
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
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
import subprocess
import string
import re
import sys
import atexit
import SCons.Action
import utils
from SCons.Script import *

#SADIR = ""
CMDDICT = {}

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('buildspec_builder')

def generate(env):
    '''
       generate

       builder for trace file capture
    '''
    #---------------------------------------------------------------------------
    # def builder for trace file capture
    #

    # builder output overrides from defaults
    buildspec = str(env.GetOption('buildspec'))
    buildspec_env = os.environ.get('BUILDSPEC')
    target_root = str(env['TARGET_ROOT'])
    topofbuild = target_root

    # Precedence:
    #    1) buildspec command-line argument
    #    2) BUILDSPEC environment variable
    if buildspec != 'none':
        topofbuild = buildspec
    elif buildspec_env:
        pass
    else:
        # buildspec has not been enabled, so get out of here
        return

    # If the topofbuild dir doesn't exist, print a warning and get out
    if not os.path.exists(topofbuild):
        env.PrintWarning \
          ("buildspec enabled, but directory doesn't exist: " + str(topofbuild))
        env.PrintWarning("Defaulting to current directory")
        topofbuild = "."

    # Make sure we're not cleaning
    clean = env.GetOption('clean')
    no_exec = env.GetOption('no_exec')
    if not clean and not no_exec:
        # Make sure we're building something
        if len(SCons.Script.COMMAND_LINE_TARGETS) > 0:
            # Make sure that we're an internal only build
            if env.IsBuildInternal():
                set_formatting(env)
                sadir = os.path.normpath(topofbuild+"/"+"static_analysis")

                if os.name is 'nt':
                    satool = r'\\stone\aswcrm\smart\nt\bin\SAGenConfig.exe'
                    sacmd = satool
                elif os.name is 'posix':
                    satool = r'/prj/qct/asw/qctss/linux/bin/SAGenConfig.pl'
                    sacmd = 'perl ' + satool

                if os.path.exists(satool):
                    env.PrintInfo("buildspec enabled--tracefile will be " \
                                  "generated in: " + sadir)
                    env['PRINT_CMD_LINE_FUNC'] = buildspec_print_cmd_line
                    atexit.register(buildspec_complete, env, sadir, sacmd)
                else:
                    env.PrintWarning("sagenconfig tool not found: " + satool)

#----------------------------------------------------------------------
# set_formatting
# 	Use the default verbose level for stdout choose by user force full
# 	command output to buildspec file for use by static analysis tools.
#  	o add full verbose output for logging to buildspec only
#	o combine both messages together so we can seperate them in the custom
#         logging function
#  	o the first part of the message is for stdout
#  	o the second part (full command) is for for XML buildspec
#----------------------------------------------------------------------
def set_formatting(env):
    '''
       set_formatting

 	   Use the default verbose level for stdout choose by user force full
 	   command output to buildspec file for use by static analysis tools.
  	   o add full verbose output for logging to buildspec only
	   o combine both messages together so we can seperate them in the
	     custom logging function
  	   o the first part of the message is for stdout
  	   o the second part (full command) is for for XML buildspec
    '''
    env.Replace(CCCOMSTR= \
        "=== Compiling  ${BUILDPATH}/$SOURCE.name &&& $CCCOMCPY")
    env.Replace(CCPPCOMSTR= \
        "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $CCPPCOM")
    env.Replace(CCPPCOM_ASTR= \
        "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $CCPPCOM")
    env.Replace(CCMIXCOMSTR= \
        "=== Generate   $SOURCE.name ==> $TARGET.posix &&& $CCMIXCOM")
    env.Replace(CCMIXCOM_ASTR= \
        "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CCMIXCOM")
    env.Replace(CCPPCLCOMSTR= \
        "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CCPPCLCOM")
    env.Replace(CCPPCLCOM_ASTR="=== Cleaning   ${TARGET.name}.pp ==> "\
                                "${TARGET.posix}.i &&& $CCPPCL_ACOM")
    env.Replace(HDEPTREECOMSTR="=== Generate   ${TARGET.name}.pp ==> " \
                                "${TARGET.posix}.dep &&& $HDEPTREECOM")
    env.Replace(ASASCOMSTR= \
        "=== Assembling ${TARGET.name}.i ==> $TARGET.posix &&& $ASASCOM")
    env.Replace(ASCOMSTR= \
        "=== Assembling $SOURCE.name ==> $TARGET.posix &&& $ASASCOM")
    env.Replace(ASPPCOMSTR= \
        "=== Assembling $SOURCE.name ==> $TARGET.posix &&& $ASPPCOM")
    env.Replace(ASCPPCOMSTR= \
        "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $ASCPPCOM")
    env.Replace(CXXCOMSTR= \
        "=== Compiling  $SOURCE.name ==> $TARGET.posix &&& $CXXCOMCPY")
    env.Replace(CXXPPCOMSTR= \
        "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $CXXPPCOM")
    env.Replace(CXXPPCOM_ASTR= \
        "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $CXXPPCOM")
    env.Replace(CXXMIXCOMSTR= \
        "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CXXMIXCOM")
    env.Replace(CXXPPCLCOMSTR= \
        "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CXXPPCLCOM")
    env.Replace(CXXPPCLCOM_ASTR="=== Cleaning   ${TARGET.name}.pp ==> " \
                                "${TARGET.posix}.i &&& $CXXPPCL_ACOM")
    env.Replace(SHCCCOMSTR= \
        "=== Compiling  ${BUILDPATH}/$SOURCE.name &&& $SHCCCOMCPY")
    env.Replace(SHCCPPCOMSTR= \
        "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $SHCCPPCOM")
    env.Replace(SHCCPPCOM_ASTR= \
        "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $SHCCPPCOM")
    env.Replace(SHCCMIXCOMSTR= \
        "=== Generate   $SOURCE.name ==> $TARGET.posix &&& $SHCCMIXCOM")
    env.Replace(SHCCMIXCOM_ASTR= \
        "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $SHCCMIXCOM")
    env.Replace(SHCCPPCLCOMSTR= \
        "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $SHCCPPCLCOM")
    env.Replace(SHCCPPCLCOM_ASTR="=== Cleaning   ${TARGET.name}.pp ==> " \
                                 "${TARGET.posix}.i &&& $SHCCPPCL_ACOM")
    env.Replace(SHHDEPTREECOMSTR="=== Generate   ${TARGET.name}.pp ==> " \
                                 "${TARGET.posix}.dep &&& $SHHDEPTREECOM")
    env.Replace(SHCXXCOMSTR= \
        "=== Compiling  $SOURCE.name ==> $TARGET.posix &&& $SHCXXCOMCPY")
    env.Replace(SHCXXPPCOMSTR= \
        "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $SHCXXPPCOM")
    env.Replace(SHCXXPPCOM_ASTR= \
        "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $SHCXXPPCOM")
    env.Replace(SHCXXMIXCOMSTR= \
        "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $SHCXXMIXCOM")
    env.Replace(SHCXXPPCLCOMSTR=  \
        "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $SHCXXPPCLCOM")
    env.Replace(SHCXXPPCLCOM_ASTR="=== Cleaning   ${TARGET.name}.pp ==> " \
                                  "${TARGET.posix}.i &&& $SHCXXPPCL_ACOM")
    env.Replace(ARCOMSTR=  \
        "=== Archiving  $TARGET.posix &&& $ARCOM")
    env.Replace(RANLIBCOMST= \
        "=== Indexing   $TARGET.posix &&& $RANLIBCOM")
    env.Replace(LINKCOMSTR= \
        "=== Linking    $TARGET.posix &&& $LINKCOM")
    env.Replace(SHLINKCOMSTR= \
        "=== Linking Sharedlibrary $TARGET.posix &&& $SHLINKCOM")

#===============================================================================
# buildspec_print_cmd_line
#        - Overwrite default scons command line printer to also log all output
#          to log file in verbose mode
#        - Log commands executed to build spec for static analysis tools
#        - enabled from command line with --buildspec option
#===============================================================================
def buildspec_print_cmd_line(s, target, src, env):
    '''
       buildspec_print_cmd_line

       - Overwrite default scons command line printer to also log all output
         to log file in verbose mode
       - Log commands executed to build spec for static analysis tools
       - enabled from command line with --buildspec option
    '''
    #global CMDDICT

    verbose = int(env.GetOption('verbose'))
    #buildspec = str(env.GetOption('buildspec'))

    #targetstr = string.join([str(x) for x in target])
    #srcstr = string.join([str(x) for x in src])

    #----------------------------------------------------------------------
    # split the command str on our string identifier (&&&)
    # element one is the non-verbose output for stdout
    # element two is the entire command line with no formating
    #----------------------------------------------------------------------
    strlist = s.split("&&&")
    sys.stdout.write(" %s...\n"%strlist[0])

    #----------------------------------------------------------------------
    # if verbose flag is on (not zero)
    # print out the entire command line to stdout
    # currently there is no variation between verbose 1, 2, or 3
    #----------------------------------------------------------------------
    if verbose and len(strlist) > 1:
        sys.stdout.write(" %s...\n"%strlist[1])

    #----------------------------------------------------------------------
    # not all commands have a STR list identified
    # only log those we have setup in the generate function above
    # (i.e.: CXXCOMSTR). Performance Note: We open and close this file alot it
    # would be better to save off the file handle and write to it instead of
    # open/close
    #----------------------------------------------------------------------
    #import pdb; pdb.set_trace()
    if env.has_key('IMAGE_NAME'):
        imagename = str(env['IMAGE_NAME'])
    else:
        imagename = 'none'
    if not CMDDICT.has_key(imagename):
        CMDDICT[imagename] = []
    if len(strlist) > 1:
        # Save information to our global data structure
        CMDDICT[imagename].append(strlist[1])


#===============================================================================
# buildspec_complete:
#        - exit function handler to finish buildspec log, and create kw log
#===============================================================================
def buildspec_complete(env, sadir, sacmd):
    '''
       buildspec_complete

       - exit function handler to finish buildspec log, and create kw log
    '''
    # Complicated mess to determine the path in which to create the trace files
    if env.has_key('IMAGE_NAME'):
        imagename = str(env['IMAGE_NAME'])
    else:
        imagename = 'none'

    # Create SA dir
    buildpath = str(env['BUILDPATH'])
    sa_buildpath_dir = sadir+"/"+buildpath
    if not os.path.exists(sa_buildpath_dir):
        os.makedirs(sa_buildpath_dir)

    # Filenames and paths
    tracefilename = str(imagename+".bspec.trace")
    tracepath = sa_buildpath_dir+"/"+tracefilename

    # Write the tracefile
    counter = 0
    file_item = open(os.path.normpath(tracepath), "w")
    # Iterate through the list of commands
    if CMDDICT.has_key(imagename):
        for cmd in CMDDICT[imagename]:
            pieces = cmd.strip(' ').split()
            cmdname = os.path.normpath(pieces[0])
            line_to_write = ";".join(["exec", str(counter), os.getcwd(), \
                                     cmdname, cmdname] + pieces[1:])
            file_item.write(line_to_write + "\n")
            counter += 1
        file_item.close()

    # Setup the SA command to run
    root = os.path.normpath(str(env['TARGET_ROOT']))
    reltracepath = tracepath[tracepath.find('static_analysis'):]
    sacmd += ' -r ' + root + " -t trace -f " + reltracepath
    # Run the SA command
    try:
        utils.exec_cmds(env, sacmd)
    except NameError:
        print "call to sagenconfig failed"

