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
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/formatting.py#1 $
#  $DateTime: 2018/02/06 02:57:17 $
#  $Author: pwbldsvc $
#  $Change: 15399902 $
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
   return env.Detect('compile_text_formatting')

def generate(env):
   #===============================================================================
   # Text formatting defaults
   #===============================================================================
   #Progress(['-\r', '\\\r', '|\r', '/\r'], interval=5)

   # builder output overrides from defaults
   progress = env.GetOption('progress')
   verbose = int(env.GetOption('verbose'))
   buildspec = str(env.GetOption('buildspec'))
   BUILDSPEC = os.environ.get('BUILDSPEC')

   if verbose == 3:
      if progress:
         Progress('Evaluating $TARGET\n')

      VERBOSE = 3
      # don't supress any output show all
   elif verbose == 2:
      if progress:
         Progress('Evaluating $TARGET\n')

      env.Replace(CCCOMSTR =        "<== Compiling  \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CCCOMCPY \n==>")
      env.Replace(CCPPCOMSTR =      "<== Preprocess \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CCPPCOM \n==>")
      env.Replace(CCPPCOM_ASTR =    "<== Preprocess \nSource: $SOURCE \nTarget: ${TARGET}.pp \nCommand:\n$CCPPCOM \n==>")
      env.Replace(CCMIXCOMSTR =     "<== Generate   \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CCMIXCOM \n==>")
      env.Replace(CCMIXCOM_ASTR =   "<== Generate   \nSource: $SOURCE \nTarget: ${TARGET}.mix \nCommand:\n$CCMIXCOM \n==>")      
      env.Replace(CCPPCLCOMSTR =    "<== Cleaning   \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CCPPCLCOM \n==>")
      env.Replace(CCPPCLCOM_ASTR =  "<== Cleaning   \nSource: ${TARGET}.pp \nTarget: ${TARGET}.i \nCommand:\n$CCPPCL_ACOM \n==>")
      env.Replace(HDEPTREECOMSTR =  "<== Generate   \nSource: ${TARGET}.pp \nTarget: ${TARGET}.dep \nCommand:\n$HDEPTREECOM \n==>")      
      env.Replace(ASASCOMSTR =      "<== Assembling \nSource: ${TARGET}.i  \nTarget: $TARGET \nCommand:\n$ASASCOM \n==>")
      env.Replace(ASCOMSTR =        "<== Assembling \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$ASASCOM \n==>")
      env.Replace(ASCPPCOMSTR =     "<== Preprocess \nSource: $SOURCE \nTarget: ${TARGET}.pp \nCommand:\n$ASCPPCOM \n==>")
      env.Replace(CXXCOMSTR =       "<== Compiling  \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CXXCOMCPY \n==>")
      env.Replace(CXXPPCOMSTR =     "<== Preprocess \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CXXPPCOM \n==>")
      env.Replace(CXXPPCOM_ASTR =   "<== Preprocess \nSource: $SOURCE \nTarget: ${TARGET}.pp \nCommand:\n$CXXPPCOM \n==>")
      env.Replace(CXXMIXCOMSTR =    "<== Generate   \nSource: $SOURCE \nTarget: ${TARGET}.mix \nCommand:\n$CXXMIXCOM \n==>")
      env.Replace(CXXPPCLCOMSTR =   "<== Cleaning   \nSource: $SOURCE \nTarget: $TARGET \nCommand:\n$CXXPPCLCOM \n==>")
      env.Replace(CXXPPCLCOM_ASTR = "<== Cleaning   \nSource: ${TARGET}.pp \nTarget: ${TARGET}.i \nCommand:\n$CXXPPCL_ACOM \n==>")
      env.Replace(ARCOMSTR =        "<== Archiving  \nTarget: $TARGET \nCommand:\n$ARCOM \n==>")
      env.Replace(RANLIBCOMSTR =    "<== Indexing   \nTarget: $TARGET \nCommand:\n$RANLIBCOM \n==>")
      env.Replace(LINKCOMSTR =      "<== Linking    \nTarget: $TARGET \nCommand:\n$LINKCOM \n==>")
      
      if 0 and env.has_key('PLATFORM') and env['PLATFORM'] == 'cygwin':      
         if progress:
            Progress('Evaluating $TARGET.posix\n')

         env.Replace(CCCOMSTR =        "<== Compiling  \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CCCOMCPY \n==>")
         env.Replace(CCPPCOMSTR =      "<== Preprocess \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CCPPCOM \n==>")
         env.Replace(CCPPCOM_ASTR =    "<== Preprocess \nSource: $SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n$CCPPCOM \n==>")
         env.Replace(CCMIXCOMSTR =     "<== Generate   \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CCMIXCOM \n==>")                  
         env.Replace(CCMIXCOM_ASTR =   "<== Generate   \nSource: $SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n$CCMIXCOM \n==>")         
         env.Replace(CCPPCLCOMSTR =    "<== Cleaning   \nSource: ${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \nCommand:\n$CCPPCLCOM \n==>")
         env.Replace(CCPPCLCOM_ASTR =  "<== Cleaning   \nSource: ${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \nCommand:\n$CCPPCL_ACOM \n==>")         
         env.Replace(HDEPTREECOMSTR =  "<== Generate   \nSource: ${TARGET.posix}.pp \nTarget: ${TARGET.posix}.dep \nCommand:\n$HDEPTREECOM \n==>")
         env.Replace(ASASCOMSTR =      "<== Assembling \nSource: ${TARGET.posix}.i  \nTarget: $TARGET.posix \nCommand:\n$ASASCOM \n==>")
         env.Replace(ASCOMSTR =        "<== Assembling \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$ASASCOM \n==>")         
         env.Replace(ASCPPCOMSTR =     "<== Preprocess \nSource: $SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n$ASCPPCOM \n==>")
         env.Replace(CXXCOMSTR =       "<== Compiling  \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CXXCOMCPY \n==>")
         env.Replace(CXXPPCOMSTR =     "<== Preprocess \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CXXPPCOM \n==>")
         env.Replace(CXXPPCOM_ASTR =   "<== Preprocess \nSource: $SOURCE.posix \nTarget: ${TARGET.posix}.pp \nCommand:\n$CXXPPCOM \n==>")
         env.Replace(CXXMIXCOMSTR =    "<== Generate   \nSource: $SOURCE.posix \nTarget: ${TARGET.posix}.mix \nCommand:\n$CXXMIXCOM \n==>")         
         env.Replace(CXXPPCLCOMSTR =   "<== Cleaning   \nSource: $SOURCE.posix \nTarget: $TARGET.posix \nCommand:\n$CXXPPCLCOM \n==>")
         env.Replace(CXXPPCLCOM_ASTR = "<== Cleaning   \nSource: ${TARGET.posix}.pp \nTarget: ${TARGET.posix}.i \nCommand:\n$CXXPPCL_ACOM \n==>")
         env.Replace(ARCOMSTR =        "<== Archiving  \nTarget: $TARGET.posix \nCommand:\n$ARCOM \n==>")
         env.Replace(RANLIBCOMSTR =    "<== Indexing   \nTarget: $TARGET.posix \nCommand:\n$RANLIBCOM \n==>")
         env.Replace(LINKCOMSTR =      "<== Linking    \nTarget: $TARGET.posix \nCommand:\n$LINKCOM \n==>")
      
   elif verbose == 1:
      if progress:
         Progress('Evaluating $TARGET\n')
         
      env.Replace(CCCOMSTR =        "=== Compiling  $SOURCE.name ==> $TARGET")
      env.Replace(CCPPCOMSTR =      "=== Preprocess $SOURCE.name ==> $TARGET")
      env.Replace(CCPPCOM_ASTR =    "=== Preprocess $SOURCE.name ==> ${TARGET}.pp")
      env.Replace(CCMIXCOMSTR =     "=== Generate   $SOURCE.name ==> $TARGET")
      env.Replace(CCMIXCOM_ASTR =   "=== Generate   $SOURCE.name ==> ${TARGET}.mix")
      env.Replace(CCPPCLCOMSTR =    "=== Cleaning   $SOURCE.name ==> $TARGET")
      env.Replace(CCPPCLCOM_ASTR =  "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
      env.Replace(HDEPTREECOMSTR =  "=== Generate   ${TARGET.name}.pp ==> ${TARGET}.dep")
      env.Replace(ASASCOMSTR =      "=== Assembling ${TARGET.name}.i ==> $TARGET")
      env.Replace(ASCOMSTR =        "=== Assembling $SOURCE.name ==> $TARGET")
      env.Replace(ASCPPCOMSTR =     "=== Preprocess $SOURCE.name ==> ${TARGET}.pp")
      env.Replace(CXXCOMSTR =       "=== Compiling  $SOURCE.name ==> $TARGET")
      env.Replace(CXXPPCOMSTR =     "=== Preprocess $SOURCE.name ==> $TARGET")
      env.Replace(CXXPPCOM_ASTR =   "=== Preprocess $SOURCE.name ==> ${TARGET}.pp")
      env.Replace(CXXMIXCOMSTR =    "=== Generate   $SOURCE.name ==> ${TARGET}.mix")
      env.Replace(CXXPPCLCOMSTR =   "=== Cleaning   $SOURCE.name ==> $TARGET")
      env.Replace(CXXPPCLCOM_ASTR = "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET}.i")
      env.Replace(ARCOMSTR =        "=== Archiving  $TARGET")
      env.Replace(RANLIBCOMSTR =    "=== Indexing   $TARGET")
      env.Replace(LINKCOMSTR =      "=== Linking    $TARGET")

      if 0 and env.has_key('PLATFORM') and env['PLATFORM'] == 'cygwin':
         if progress:
            Progress('Evaluating $TARGET.posix\n')
      
         env.Replace(CCCOMSTR =        "=== Compiling  $SOURCE.name ==> $TARGET.posix")
         env.Replace(CCPPCOMSTR =      "=== Preprocess $SOURCE.name ==> $TARGET.posix")
         env.Replace(CCPPCOM_ASTR =    "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp")
         env.Replace(CCMIXCOMSTR =     "=== Generate   $SOURCE.name ==> $TARGET.posix")
         env.Replace(CCMIXCOM_ASTR =   "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix")
         env.Replace(CCPPCLCOMSTR =    "=== Cleaning   $SOURCE.name ==> $TARGET.posix")
         env.Replace(CCPPCLCOM_ASTR =  "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
         env.Replace(HDEPTREECOMSTR =  "=== Generate   ${TARGET.name}.pp ==> ${TARGET.posix}.dep")
         env.Replace(ASASCOMSTR =      "=== Assembling ${TARGET.name}.i ==> $TARGET.posix")
         env.Replace(ASCOMSTR =        "=== Assembling $SOURCE.name ==> $TARGET.posix")
         env.Replace(ASCPPCOMSTR =     "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp")
         env.Replace(CXXCOMSTR =       "=== Compiling  $SOURCE.name ==> $TARGET.posix")
         env.Replace(CXXPPCOMSTR =     "=== Preprocess $SOURCE.name ==> $TARGET.posix")
         env.Replace(CXXPPCOM_ASTR =   "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp")
         env.Replace(CXXMIXCOMSTR =    "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix")
         env.Replace(CXXPPCLCOMSTR =   "=== Cleaning   $SOURCE.name ==> $TARGET.posix")
         env.Replace(CXXPPCLCOM_ASTR = "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i")
         env.Replace(ARCOMSTR =        "=== Archiving  $TARGET.posix")
         env.Replace(RANLIBCOMSTR =    "=== Indexing   $TARGET.posix")
         env.Replace(LINKCOMSTR =      "=== Linking    $TARGET.posix")

   else:
      if progress:
         #Progress('.', interval=10, file=sys.stderr)
         Progress('Evaluating $TARGET\n')

      env.Replace(CCCOMSTR =        "=== Compiling  ${BUILDPATH}/$SOURCE.name")
      env.Replace(CCPPCOMSTR =      "=== Preprocess ${BUILDPATH}/$SOURCE.name")
      env.Replace(CCPPCOM_ASTR =    env['CCPPCOMSTR'])
      env.Replace(CCMIXCOMSTR =     "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")      
      env.Replace(CCMIXCOM_ASTR =   env['CCMIXCOMSTR'])
      env.Replace(CCPPCLCOMSTR =    "=== Cleaning   ${BUILDPATH}/$SOURCE.name")
      env.Replace(CCPPCLCOM_ASTR =  "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")
      env.Replace(HDEPTREECOMSTR =  "=== Generate   ${BUILDPATH}/${TARGET.name}.dep")
      env.Replace(ASASCOMSTR =      "=== Assembling ${BUILDPATH}/${TARGET.name}.i")
      env.Replace(ASCOMSTR =        "=== Assembling ${BUILDPATH}/$SOURCE.name")
      env.Replace(ASCPPCOMSTR =     env['CCPPCOMSTR'])
      env.Replace(CXXCOMSTR =       "=== Compiling  ${BUILDPATH}/$SOURCE.name")
      env.Replace(CXXPPCOMSTR =     "=== Preprocess ${BUILDPATH}/$SOURCE.name")
      env.Replace(CXXPPCOM_ASTR =   env['CXXPPCOMSTR'])
      env.Replace(CXXMIXCOMSTR =    "=== Generate   ${BUILDPATH}/${TARGET.name}.mix")      
      env.Replace(CXXPPCLCOMSTR =   "=== Cleaning   ${BUILDPATH}/$SOURCE.name")
      env.Replace(CXXPPCLCOM_ASTR = "=== Cleaning   ${BUILDPATH}/${TARGET.name}.pp")
      env.Replace(ARCOMSTR =        "=== Archiving  ${BUILDPATH}/$TARGET.name")
      env.Replace(RANLIBCOMSTR =    "=== Indexing   ${BUILDPATH}/$TARGET.name")
      env.Replace(LINKCOMSTR =      "=== Linking    ${BUILDPATH}/$TARGET.name")

   if buildspec != 'none' or BUILDSPEC is not None:
      #----------------------------------------------------------------------
      # Use the default verbose level for stdout choose by user force full
      # command output to buildspec file for use by static analysis tools.
      #  o add full verbose output for logging to buildspec only
      #  o combine both messages together so we can seperate them in the custom logging function
      #  o the first part of the message is for stdout 
      #  o the second part (full command) is for for XML buildspec
      #----------------------------------------------------------------------
      env.Replace(CCCOMSTR =        "=== Compiling  ${BUILDPATH}/$SOURCE.name &&& $CCCOMCPY")
      env.Replace(CCPPCOMSTR =      "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $CCPPCOM")
      env.Replace(CCPPCOM_ASTR =    "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $CCPPCOM")
      env.Replace(CCMIXCOMSTR =     "=== Generate   $SOURCE.name ==> $TARGET.posix &&& $CCMIXCOM")      
      env.Replace(CCMIXCOM_ASTR =   "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CCMIXCOM")      
      env.Replace(CCPPCLCOMSTR =    "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CCPPCLCOM")
      env.Replace(CCPPCLCOM_ASTR =  "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i &&& $CCPPCL_ACOM")
      env.Replace(HDEPTREECOMSTR =  "=== Generate   ${TARGET.name}.pp ==> ${TARGET.posix}.dep &&& $HDEPTREECOM")
      env.Replace(ASASCOMSTR =      "=== Assembling ${TARGET.name}.i ==> $TARGET.posix &&& $ASASCOM")
      env.Replace(ASCOMSTR =        "=== Assembling $SOURCE.name ==> $TARGET.posix &&& $ASASCOM")
      env.Replace(ASCPPCOMSTR =     "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $ASCPPCOM")
      env.Replace(CXXCOMSTR =       "=== Compiling  $SOURCE.name ==> $TARGET.posix &&& $CXXCOMCPY")
      env.Replace(CXXPPCOMSTR =     "=== Preprocess $SOURCE.name ==> $TARGET.posix &&& $CXXPPCOM")
      env.Replace(CXXPPCOM_ASTR =   "=== Preprocess $SOURCE.name ==> ${TARGET.posix}.pp &&& $CXXPPCOM")
      env.Replace(CXXMIXCOMSTR =    "=== Generate   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CXXMIXCOM")      
      env.Replace(CXXPPCLCOMSTR =   "=== Cleaning   $SOURCE.name ==> ${TARGET.posix}.mix &&& $CXXPPCLCOM")      
      env.Replace(CXXPPCLCOM_ASTR = "=== Cleaning   ${TARGET.name}.pp ==> ${TARGET.posix}.i &&& $CXXPPCL_ACOM")
      env.Replace(ARCOMSTR =        "=== Archiving  $TARGET.posix &&& $ARCOM")
      env.Replace(RANLIBCOMSTR =    "=== Indexing   $TARGET.posix &&& $RANLIBCOM")
      env.Replace(LINKCOMSTR =      "=== Linking    $TARGET.posix &&& $LINKCOM")
  
   #print env.Dump()


