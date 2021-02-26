#===========================================================================
#  GENERAL DESCRIPTION:
#    Contains a scons custom build for creating Sleep LPRs
#
#  Copyright (c) 2012-2015 Qualcomm Technologies, Inc. (QTI).
#  All Rights Reserved.
#  Qqualcomm Technologies Confidential and Proprietary
#===========================================================================

import os
import subprocess
import itertools
import fnmatch
import string
import time
from SCons.Script import *

#---------------------------------------------------------------------------
# Globals
#---------------------------------------------------------------------------

CORESLEEP_ENV_DESCRIPTOR = 'CORESLEEP_INFO_COLLECTOR'
SLEEP_LPR_XML_TAG = 'sleep_lpr_xml'

#---------------------------------------------------------------------------
# Plugins for the SCons framework
#---------------------------------------------------------------------------
def exists(env):
  return True;

def generate(env):
  ''' 
  Sets up a custom scons builder for sleep synth tool. Gets loaded when all
  scons files are scanned
  '''

  env.PrintDebugInfo('sleepbuilder', 'Sleep builder is getting constructed') 

  # Adding a flag to environment. This flag must be present to use this builder.
  env.AddUsesFlags('USES_CORESLEEP_TOOL', from_builds_file = True)

  rootenv = env.get('IMAGE_ENV')
  rootenv[CORESLEEP_ENV_DESCRIPTOR] = []

  # Adding a method to environment for client to interact
  rootenv.AddMethod(add_sleep_lpr_info, "AddCoreSleepLPR")

  # Creating a builder and adding it to the build environment
  sleeplpr_action = env.GetBuilderAction(core_sleep_lpr_action, action_source=None)
  sleeplpr_scanner = env.Scanner(core_sleep_lpr_scanner, name="CoreSleepLPRScanner")
  sleeplpr_bld = env.Builder( action = sleeplpr_action,
                              target_scanner = sleeplpr_scanner) 

  rootenv.Append(BUILDERS = {'CoreSleepLPRBuilder' : sleeplpr_bld })

  return

def core_sleep_lpr_scanner(node, env, path):
  '''
  This will be called once all XML files are added to the environment. Put
  dependencies on each of them
  '''
  rootenv = env.get('IMAGE_ENV')

  env.PrintDebugInfo('sleepbuilder','In scanner function')

  nodes = list(rootenv.get(CORESLEEP_ENV_DESCRIPTOR, []))
  nodes.extend([node])
  return nodes


def add_sleep_lpr_info(env, targets, input_dict):
  '''
  This function gets called when client make a call of AddCoreSleepLPR.
  '''

  if not env.IsTargetEnable(targets):
    env.PrintWarning('Found disabled target call')
    return

  rootenv = env.get('IMAGE_ENV')

  # Checking validity of input dictionary
  if SLEEP_LPR_XML_TAG not in input_dict:
    sleeplpr_error(env, SLEEP_LPR_XML_TAG + ' entry not in dictionary')
  elif None == input_dict[SLEEP_LPR_XML_TAG]:
    sleeplpr_error(env, SLEEP_LPR_XML_TAG + ' entry cannot be empty')

  # Obtaining input file list. If not a list, convert it.
  input_paths = input_dict[SLEEP_LPR_XML_TAG]
  if type(input_paths) is not list:
    input_paths = [input_paths]
   
  # Adding files at the input path to global list
  for aPath in input_paths:

    # Check if the path exists. Raise an exception if not
    if not env.PathExists(aPath):
      errString = 'Path ' + str(aPath) + ' not found'
      sleeplpr_error(env, errString)
  
    # Getting full path
    xml_fullpath = env.RealPath(aPath)

    # Checking if the path is directory or file. If directory, search for
    # xml files in that and add them to global list. If file, directy add
    # it to global list
    if os.path.isdir(xml_fullpath):
      dir_xml_files = env.FindFiles( file_name_patterns = ['*.xml', '*.XML'], root_dir = xml_fullpath )
    else:
      dir_xml_files = [xml_fullpath]

    for aFile in dir_xml_files:
      # Check if same file is specified multiple times
      if aFile in rootenv[CORESLEEP_ENV_DESCRIPTOR]:
        env.PrintDebugInfo('sleepbuilder', "Duplicate LPR XML input file specified " + str(aFile))
      else:
        env.PrintDebugInfo('sleepbuilder', 'Adding LPR XML file to list %s' %str(aFile))
        rootenv[CORESLEEP_ENV_DESCRIPTOR].append(aFile)

  return


def core_sleep_lpr_action(target, source, env):
  '''
  Will be called from sleep/build/sconscript. It will launch sleep synth tool
  '''

  rootenv = env.get('IMAGE_ENV')
  env.PrintDebugInfo('sleepbuilder', 'Action function')
  env.PrintDebugInfo('sleepbuilder', 'Curr Directory: %s (Build Location: %s)' %(os.getcwd(), rootenv['BUILD_ROOT']))
  env.PrintDebugInfo('sleepbuilder', 'No of (source, target) files (%d, %d)' %(len(target), len(source)))
  env.PrintDebugInfo('sleepbuilder', 'Input files: %d' %(len(rootenv[CORESLEEP_ENV_DESCRIPTOR]) ))
  env.PrintDebugInfo('sleepbuilder', 'BuildPath ${BUILDPATH}')

  for aFile in target:
    env.PrintDebugInfo('sleepbuilder', 'Target file %s' %(str(aFile)))

  input_xml_files = [ aFile for aFile in rootenv[CORESLEEP_ENV_DESCRIPTOR] ]

  proc_name = rootenv.get('PROC_CONFIG')

  EXE_PATH = rootenv['BUILD_ROOT']
  OP_PATH = os.path.dirname(target[0].path)
  HOP_PATH = os.path.dirname(target[3].path)

  EXE_PATH += '/core/power/sleep2.0/tools/SleepSynth/bin/SleepSynth'
  NO_MULTICORE = ' -nomc '

  if env['PLATFORM'] == 'windows':
    EXE_PATH += '.exe'
  else:
    os.chmod(EXE_PATH, 0777)      # Adding execution permission for linux
    
  SRC_XMLS = ''

  # Preparing command line for input LPR files
  for aFile in input_xml_files:
    env.PrintDebugInfo('sleepbuilder', 'Input xml: %s' %aFile)
    SRC_XMLS += ' -ip "' + aFile + '"'

  # Entire command for sleep synth tool
  sleep_synth_cmd = EXE_PATH + NO_MULTICORE + SRC_XMLS + ' -op "' + OP_PATH + '" -hop "' + HOP_PATH + '"'
  env.PrintDebugInfo('sleepbuilder', 'SynthTool command: %s' %sleep_synth_cmd);

  #import pdb; pdb.set_trace();

  # Calling SleepSynth Tool
  sop, serr, rv = env.ExecCmds(cmds=sleep_synth_cmd, target=target, shell=True)
  env.PrintDebugInfo('sleepbuilder', 'stdout: %s' %str(sop))
  env.PrintDebugInfo('sleepbuilder', 'stderr: %s' %str(serr))
  env.PrintDebugInfo('sleepbuilder', 'return: %d' %rv)

  # If tool fails, raise an exception
  if rv != 0:
    err_msg = serr + ' (SleepSynth tool failed (error code: %d))' %rv
    sleeplpr_error(env, err_msg)

  return 


def sleeplpr_error(env, error_string):
  env.PrintError("SleepLPRBuilder error found: " + error_string)
  raise Exception(error_string)

