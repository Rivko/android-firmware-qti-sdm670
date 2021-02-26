'''
   CoreBSP Text Formatting Rules
'''
#===============================================================================
#
# CoreBSP Text Formatting Rules
#
# GENERAL DESCRIPTION
#    Fortmatting text output defenition
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/formatting.py#8 $
#  $DateTime: 2015/10/18 16:30:08 $
#  $Author: pwbldsvc $
#  $Change: 9243724 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import sys
import os
import string
import atexit
import SCons.Action
import utils
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
       exists
    '''
    return env.Detect('compile_text_formatting')

def generate(env):
    '''
       generate
    '''
    #===========================================================================
    # Text formatting defaults
    #===========================================================================
    #Progress(['-\r', '\\\r', '|\r', '/\r'], interval=5)

    # builder output overrides from defaults
    progress = env.GetOption('progress')
    verbose = int(env.GetOption('verbose'))
    #buildspec = str(env.GetOption('buildspec'))
    #BUILDSPEC = os.environ.get('BUILDSPEC')

    if verbose == 3:
        if progress:
            Progress('Evaluating $TARGET\n')

        #VERBOSE = 3
        # don't supress any output show all
    elif verbose == 2:
        if progress:
            Progress('Evaluating $TARGET\n')

        env.Replace(CCCOMSTR="<== Compiling  \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CCCOMCPY \n==>")
        env.Replace(CCPPCOMSTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CCPPCOM \n==>")
        env.Replace(CCPPCOM_ASTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: ${TARGET}.pp \nCommand:\n$CCPPCOM \n==>")
        env.Replace(CCMIXCOMSTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CCMIXCOM \n==>")
        env.Replace(CCMIXCOM_ASTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: ${TARGET}.mix \nCommand:\n$CCMIXCOM \n==>")
        env.Replace(CCPPCLCOMSTR="<== Cleaning   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CCPPCLCOM \n==>")
        env.Replace(CCPPCLCOM_ASTR="<== Cleaning   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.i \nCommand:\n$CCPPCL_ACOM \n==>")
        env.Replace(HDEPTREECOMSTR="<== Generate   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.dep \nCommand:\n$HDEPTREECOM \n==>")
        env.Replace(ASASCOMSTR="<== Assembling \nSource: ${TARGET}." \
            "i  \nTarget: $TARGET \nCommand:\n$ASASCOM \n==>")
        env.Replace(ASCOMSTR="<== Assembling \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$ASASCOM \n==>")
        env.Replace(ASPPCOMSTR="<== Assembling \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$ASPPCOM \n==>")
        env.Replace(ASCPPCOMSTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: ${TARGET}.pp \nCommand:\n$ASCPPCOM \n==>")
        env.Replace(CXXCOMSTR="<== Compiling  \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CXXCOMCPY \n==>")
        env.Replace(CXXPPCOMSTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CXXPPCOM \n==>")
        env.Replace(CXXPPCOM_ASTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: ${TARGET}.pp \nCommand:\n$CXXPPCOM \n==>")
        env.Replace(CXXMIXCOMSTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: ${TARGET}.mix \nCommand:\n$CXXMIXCOM \n==>")
        env.Replace(CXXPPCLCOMSTR="<== Cleaning   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$CXXPPCLCOM \n==>")
        env.Replace(CXXPPCLCOM_ASTR="<== Cleaning   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.i \nCommand:\n$CXXPPCL_ACOM \n==>")
        env.Replace(SHCCCOMSTR="<== Compiling  \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCCCOMCPY \n==>")
        env.Replace(SHCCPPCOMSTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCCPPCOM \n==>")
        env.Replace(SHCCPPCOM_ASTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: ${TARGET}.pp \nCommand:\n$SHCCPPCOM \n==>")
        env.Replace(SHCCMIXCOMSTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCCMIXCOM \n==>")
        env.Replace(SHCCMIXCOM_ASTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: ${TARGET}.mix \nCommand:\n$SHCCMIXCOM \n==>")
        env.Replace(SHCCPPCLCOMSTR="<== Cleaning   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCCPPCLCOM \n==>")
        env.Replace(SHCCPPCLCOM_ASTR="<== Cleaning   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.i \nCommand:\n$SHCCPPCL_ACOM \n==>")
        env.Replace(SHHDEPTREECOMSTR="<== Generate   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.dep \nCommand:\n$SHHDEPTREECOM \n==>")
        env.Replace(SHCXXCOMSTR="<== Compiling  \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCXXCOMCPY \n==>")
        env.Replace(SHCXXPPCOMSTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCXXPPCOM \n==>")
        env.Replace(SHCXXPPCOM_ASTR="<== Preprocess \nSource: $SOURCE \n" \
            "Target: ${TARGET}.pp \nCommand:\n$SHCXXPPCOM \n==>")
        env.Replace(SHCXXMIXCOMSTR="<== Generate   \nSource: $SOURCE \n" \
            "Target: ${TARGET}.mix \nCommand:\n$SHCXXMIXCOM \n==>")
        env.Replace(SHCXXPPCLCOMSTR="<== Cleaning   \nSource: $SOURCE \n" \
            "Target: $TARGET \nCommand:\n$SHCXXPPCLCOM \n==>")
        env.Replace(SHCXXPPCLCOM_ASTR="<== Cleaning   \nSource: ${TARGET}." \
            "pp \nTarget: ${TARGET}.i \nCommand:\n$SHCXXPPCL_ACOM \n==>")
        env.Replace(ARCOMSTR="<== Archiving  \nTarget: $TARGET \n" \
            "Command:\n$ARCOM \n==>")
        env.Replace(RANLIBCOMSTR="<== Indexing   \nTarget: $TARGET \n" \
            "Command:\n$RANLIBCOM \n==>")
        env.Replace(LINKCOMSTR="<== Linking    \nTarget: $TARGET \n" \
            "Command:\n$LINKCOM \n==>")
        env.Replace(SHLINKCOMSTR="<== Linking Sharedlibrary \nTarget:" \
            " $TARGET \nCommand:\n$SHLINKCOM \n==>")

        if 0 and env.has_key('PLATFORM') and env['PLATFORM'] == 'cygwin':
            if progress:
                Progress('Evaluating $TARGET.posix\n')

            env.Replace(CCCOMSTR="<== Compiling  \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CCCOMCPY \n==>")
            env.Replace(CCPPCOMSTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CCPPCOM \n==>")
            env.Replace(CCPPCOM_ASTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n" \
                    "$CCPPCOM \n==>")
            env.Replace(CCMIXCOMSTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CCMIXCOM \n==>")
            env.Replace(CCMIXCOM_ASTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n" \
                    "$CCMIXCOM \n==>")
            env.Replace(CCPPCLCOMSTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$CCPPCLCOM \n==>")
            env.Replace(CCPPCLCOM_ASTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$CCPPCL_ACOM \n==>")
            env.Replace(HDEPTREECOMSTR="<== Generate   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.dep \n" \
                    "Command:\n$HDEPTREECOM \n==>")
            env.Replace(ASASCOMSTR="<== Assembling \nSource: " \
                    "${TARGET.posix}.i  \nTarget: $TARGET.posix \nCommand:\n" \
                    "$ASASCOM \n==>")
            env.Replace(ASCOMSTR="<== Assembling \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$ASASCOM \n==>")
            env.Replace(ASPPCOMSTR="<== Assembling \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$ASPPCOM \n==>")
            env.Replace(ASCPPCOMSTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n" \
                    "$ASCPPCOM \n==>")
            env.Replace(CXXCOMSTR="<== Compiling  \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CXXCOMCPY \n==>")
            env.Replace(CXXPPCOMSTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CXXPPCOM \n==>")
            env.Replace(CXXPPCOM_ASTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n" \
                    "$CXXPPCOM \n==>")
            env.Replace(CXXMIXCOMSTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n" \
                    "$CXXMIXCOM \n==>")
            env.Replace(CXXPPCLCOMSTR="<== Cleaning   \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$CXXPPCLCOM \n==>")
            env.Replace(CXXPPCLCOM_ASTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$CXXPPCL_ACOM \n==>")
            env.Replace(SHCCCOMSTR="<== Compiling  \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCCCOMCPY \n==>")
            env.Replace(SHCCPPCOMSTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCCPPCOM \n==>")
            env.Replace(SHCCPPCOM_ASTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n" \
                    "$SHCCPPCOM \n==>")
            env.Replace(SHCCMIXCOMSTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCCMIXCOM \n==>")
            env.Replace(SHCCMIXCOM_ASTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n" \
                    "$SHCCMIXCOM \n==>")
            env.Replace(SHCCPPCLCOMSTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$SHCCPPCLCOM \n==>")
            env.Replace(SHCCPPCLCOM_ASTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$SHCCPPCL_ACOM \n==>")
            env.Replace(SHHDEPTREECOMSTR="<== Generate   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.dep \n" \
                    "Command:\n$SHHDEPTREECOM \n==>")
            env.Replace(SHCXXCOMSTR="<== Compiling  \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCXXCOMCPY \n==>")
            env.Replace(SHCXXPPCOMSTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCXXPPCOM \n==>")
            env.Replace(SHCXXPPCOM_ASTR="<== Preprocess \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n" \
                    "$SHCXXPPCOM \n==>")
            env.Replace(SHCXXMIXCOMSTR="<== Generate   \nSource: " \
                    "$SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n" \
                    "$SHCXXMIXCOM \n==>")
            env.Replace(SHCXXPPCLCOMSTR="<== Cleaning   \nSource: " \
                    "$SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n" \
                    "$SHCXXPPCLCOM \n==>")
            env.Replace(SHCXXPPCLCOM_ASTR="<== Cleaning   \nSource: " \
                    "${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \n" \
                    "Command:\n$SHCXXPPCL_ACOM \n==>")
            env.Replace(ARCOMSTR="<== Archiving  \nTarget: " \
                    "$TARGET.posix \nCommand:\n$ARCOM \n==>")
            env.Replace(RANLIBCOMSTR="<== Indexing   \nTarget: " \
                    "$TARGET.posix \nCommand:\n$RANLIBCOM \n==>")
            env.Replace(LINKCOMSTR="<== Linking    \nTarget: " \
                    "$TARGET.posix \nCommand:\n$LINKCOM \n==>")
            env.Replace(LINKCOMSTR="<== Linking  Sharedlibrary \n" \
                    "Target: $TARGET.posix \nCommand:\n$SHLINKCOM \n==>")

    elif verbose == 1:
        if progress:
            Progress('Evaluating $TARGET\n')

        env.Replace(CCCOMSTR="=== Compiling  $SOURCE ==> $TARGET")
        env.Replace(CCPPCOMSTR="=== Preprocess $SOURCE ==> $TARGET")
        env.Replace(CCPPCOM_ASTR= \
            "=== Preprocess $SOURCE ==> ${TARGET}.pp")
        env.Replace(CCMIXCOMSTR="=== Generate   $SOURCE ==> $TARGET")
        env.Replace(CCMIXCOM_ASTR= \
            "=== Generate   $SOURCE ==> ${TARGET}.mix")
        env.Replace(CCPPCLCOMSTR="=== Cleaning   $SOURCE ==> $TARGET")
        env.Replace(CCPPCLCOM_ASTR= \
            "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
        env.Replace(HDEPTREECOMSTR= \
            "=== Generate   ${TARGET.name}.pp ==> ${TARGET}.dep")
        env.Replace(ASASCOMSTR="=== Assembling ${TARGET.name}.i ==> $TARGET")
        env.Replace(ASCOMSTR="=== Assembling $SOURCE ==> $TARGET")
        env.Replace(ASPPCOMSTR="=== Assembling $SOURCE ==> $TARGET")
        env.Replace(ASCPPCOMSTR="=== Preprocess $SOURCE ==> ${TARGET}.pp")
        env.Replace(CXXCOMSTR="=== Compiling  $SOURCE ==> $TARGET")
        env.Replace(CXXPPCOMSTR="=== Preprocess $SOURCE ==> $TARGET")
        env.Replace(CXXPPCOM_ASTR= \
            "=== Preprocess $SOURCE ==> ${TARGET}.pp")
        env.Replace(CXXMIXCOMSTR= \
            "=== Generate   $SOURCE ==> ${TARGET}.mix")
        env.Replace(CXXPPCLCOMSTR="=== Cleaning   $SOURCE ==> $TARGET")
        env.Replace(CXXPPCLCOM_ASTR= \
            "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
        env.Replace(SHCCCOMSTR="=== Compiling  $SOURCE ==> $TARGET")
        env.Replace(SHCCPPCOMSTR="=== Preprocess $SOURCE ==> $TARGET")
        env.Replace(SHCCPPCOM_ASTR= \
            "=== Preprocess $SOURCE ==> ${TARGET}.pp")
        env.Replace(SHCCMIXCOMSTR="=== Generate   $SOURCE ==> $TARGET")
        env.Replace(SHCCMIXCOM_ASTR= \
            "=== Generate   $SOURCE ==> ${TARGET}.mix")
        env.Replace(SHCCPPCLCOMSTR="=== Cleaning   $SOURCE ==> $TARGET")
        env.Replace(SHCCPPCLCOM_ASTR= \
            "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
        env.Replace(SHHDEPTREECOMSTR= \
            "=== Generate   ${TARGET.name}.pp ==> ${TARGET}.dep")
        env.Replace(SHCXXCOMSTR="=== Compiling  $SOURCE ==> $TARGET")
        env.Replace(SHCXXPPCOMSTR="=== Preprocess $SOURCE ==> $TARGET")
        env.Replace(SHCXXPPCOM_ASTR= \
            "=== Preprocess $SOURCE ==> ${TARGET}.pp")
        env.Replace(SHCXXMIXCOMSTR= \
            "=== Generate   $SOURCE ==> ${TARGET}.mix")
        env.Replace(SHCXXPPCLCOMSTR="=== Cleaning   $SOURCE ==> $TARGET")
        env.Replace(SHCXXPPCLCOM_ASTR= \
            "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
        env.Replace(ARCOMSTR="=== Archiving  $TARGET")
        env.Replace(RANLIBCOMSTR="=== Indexing   $TARGET")
        env.Replace(LINKCOMSTR="=== Linking    $TARGET")
        env.Replace(SHLINKCOMSTR="=== Linking Sharedlibrary $TARGET")

        if 0 and env.has_key('PLATFORM') and env['PLATFORM'] == 'cygwin':
            if progress:
                Progress('Evaluating $TARGET.posix\n')

            env.Replace(CCCOMSTR= \
                "=== Compiling  $SOURCE ==> $TARGET.posix")
            env.Replace(CCPPCOMSTR= \
                "=== Preprocess $SOURCE ==> $TARGET.posix")
            env.Replace(CCPPCOM_ASTR= \
                "=== Preprocess $SOURCE ==> ${TARGET.posix}.pp")
            env.Replace(CCMIXCOMSTR= \
                "=== Generate   $SOURCE ==> $TARGET.posix")
            env.Replace(CCMIXCOM_ASTR= \
                "=== Generate   $SOURCE ==> ${TARGET.posix}.mix")
            env.Replace(CCPPCLCOMSTR= \
                "=== Cleaning   $SOURCE ==> $TARGET.posix")
            env.Replace(CCPPCLCOM_ASTR= \
                "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
            env.Replace(HDEPTREECOMSTR= \
                "=== Generate   ${TARGET.name}.pp ==> ${TARGET.posix}.dep")
            env.Replace(ASASCOMSTR= \
                "=== Assembling ${TARGET.name}.i ==> $TARGET.posix")
            env.Replace(ASCOMSTR= \
                "=== Assembling $SOURCE ==> $TARGET.posix")
            env.Replace(ASPPCOMSTR= \
                "=== Assembling $SOURCE ==> $TARGET.posix")
            env.Replace(ASCPPCOMSTR= \
                "=== Preprocess $SOURCE ==> ${TARGET.posix}.pp")
            env.Replace(CXXCOMSTR= \
                "=== Compiling  $SOURCE ==> $TARGET.posix")
            env.Replace(CXXPPCOMSTR= \
                "=== Preprocess $SOURCE ==> $TARGET.posix")
            env.Replace(CXXPPCOM_ASTR= \
                "=== Preprocess $SOURCE ==> ${TARGET.posix}.pp")
            env.Replace(CXXMIXCOMSTR= \
                "=== Generate   $SOURCE ==> ${TARGET.posix}.mix")
            env.Replace(CXXPPCLCOMSTR= \
                "=== Cleaning   $SOURCE ==> $TARGET.posix")
            env.Replace(CXXPPCLCOM_ASTR= \
                "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
            env.Replace(SHCCCOMSTR= \
                "=== Compiling  $SOURCE ==> $TARGET.posix")
            env.Replace(SHCCPPCOMSTR= \
                "=== Preprocess $SOURCE ==> $TARGET.posix")
            env.Replace(SHCCPPCOM_ASTR= \
                "=== Preprocess $SOURCE ==> ${TARGET.posix}.pp")
            env.Replace(SHCCMIXCOMSTR= \
                "=== Generate   $SOURCE ==> $TARGET.posix")
            env.Replace(SHCCMIXCOM_ASTR= \
                "=== Generate   $SOURCE ==> ${TARGET.posix}.mix")
            env.Replace(SHCCPPCLCOMSTR= \
                "=== Cleaning   $SOURCE ==> $TARGET.posix")
            env.Replace(SHCCPPCLCOM_ASTR= \
                "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
            env.Replace(SHHDEPTREECOMSTR= \
                "=== Generate   ${TARGET.name}.pp ==> ${TARGET.posix}.dep")
            env.Replace(SHCXXCOMSTR= \
                "=== Compiling  $SOURCE ==> $TARGET.posix")
            env.Replace(SHCXXPPCOMSTR= \
                "=== Preprocess $SOURCE ==> $TARGET.posix")
            env.Replace(SHCXXPPCOM_ASTR= \
                "=== Preprocess $SOURCE ==> ${TARGET.posix}.pp")
            env.Replace(SHCXXMIXCOMSTR= \
                "=== Generate   $SOURCE ==> ${TARGET.posix}.mix")
            env.Replace(SHCXXPPCLCOMSTR= \
                "=== Cleaning   $SOURCE ==> $TARGET.posix")
            env.Replace(SHCXXPPCLCOM_ASTR= \
                "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
            env.Replace(ARCOMSTR="=== Archiving  $TARGET.posix")
            env.Replace(RANLIBCOMSTR="=== Indexing   $TARGET.posix")
            env.Replace(LINKCOMSTR="=== Linking    $TARGET.posix")
            env.Replace(SHLINKCOMSTR="=== Linking Sharedlibrary $TARGET.posix")

    else:
        if progress:
            #Progress('.', interval=10, file=sys.stderr)
            Progress('Evaluating $TARGET\n')

        env.Replace(CCCOMSTR="=== Compiling  ${BUILDPATH}/$SOURCE.name")
        env.Replace(CCPPCOMSTR="=== Preprocess ${BUILDPATH}/$SOURCE.name")
        env.Replace(CCPPCOM_ASTR=env['CCPPCOMSTR'])
        env.Replace(CCMIXCOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")
        env.Replace(CCMIXCOM_ASTR=env['CCMIXCOMSTR'])
        env.Replace(CCPPCLCOMSTR="=== Cleaning   ${BUILDPATH}/$SOURCE.name")
        env.Replace(CCPPCLCOM_ASTR= \
            "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")
        env.Replace(HDEPTREECOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.dep")
        env.Replace(ASASCOMSTR="=== Assembling ${BUILDPATH}/${TARGET.name}.i")
        env.Replace(ASCOMSTR="=== Assembling ${BUILDPATH}/$SOURCE.name")
        env.Replace(ASPPCOMSTR="=== Assembling ${BUILDPATH}/$SOURCE.name")
        env.Replace(ASCPPCOMSTR=env['CCPPCOMSTR'])
        env.Replace(CXXCOMSTR="=== Compiling  ${BUILDPATH}/$SOURCE.name")
        env.Replace(CXXPPCOMSTR="=== Preprocess ${BUILDPATH}/$SOURCE.name")
        env.Replace(CXXPPCOM_ASTR=env['CXXPPCOMSTR'])
        env.Replace(CXXMIXCOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")
        env.Replace(CXXPPCLCOMSTR="=== Cleaning   ${BUILDPATH}/$SOURCE.name")
        env.Replace(CXXPPCLCOM_ASTR= \
            "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")
        env.Replace(SHCCCOMSTR="=== Compiling  ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCCPPCOMSTR="=== Preprocess ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCCPPCOM_ASTR=env['CCPPCOMSTR'])
        env.Replace(SHCCMIXCOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")
        env.Replace(SHCCMIXCOM_ASTR=env['CCMIXCOMSTR'])
        env.Replace(SHCCPPCLCOMSTR="=== Cleaning   ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCCPPCLCOM_ASTR= \
            "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")
        env.Replace(SHHDEPTREECOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.dep")
        env.Replace(SHCXXCOMSTR="=== Compiling  ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCXXPPCOMSTR="=== Preprocess ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCXXPPCOM_ASTR=env['CXXPPCOMSTR'])
        env.Replace(SHCXXMIXCOMSTR= \
            "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")
        env.Replace(SHCXXPPCLCOMSTR="=== Cleaning   ${BUILDPATH}/$SOURCE.name")
        env.Replace(SHCXXPPCLCOM_ASTR= \
            "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")

        env.Replace(ARCOMSTR="=== Archiving  ${BUILDPATH}/$TARGET.name")
        env.Replace(RANLIBCOMSTR="=== Indexing   ${BUILDPATH}/$TARGET.name")
        env.Replace(LINKCOMSTR="=== Linking    ${BUILDPATH}/$TARGET.name")
        env.Replace(SHLINKCOMSTR= \
            "=== Linking Sharedlibrary ${BUILDPATH}/$TARGET.name")

    #print env.Dump()

