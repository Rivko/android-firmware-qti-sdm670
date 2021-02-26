#===========================================================================
#
#  @file HWIOBuilder.py
#  @brief HWIO "Builder" module for SCons
#
#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/hwio/build/scripts/HWIOBuilder.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

#------------------------------------------------------------------------------
# Imports
#------------------------------------------------------------------------------

import SCons.Action
import os
import string
import threading


#------------------------------------------------------------------------------
# HWIOBuilder class
#------------------------------------------------------------------------------

class HWIOScons:

  def __init__(self, *args, **kwargs):
    # Create the critical section lock
    self.lock = threading.RLock()

    # Create an empty dictionary for storing HWIO definitions indexed
    # by address file name.
    self.bases = {}


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------

def exists(env):
  return env.Detect(['hwio_builder','hwio'])


def raise_error (env, msg):
  env.PrintError(msg)
  raise RuntimeError, msg


def generate(env):
  rootenv = env.get('IMAGE_ENV')

  # Create an empty list of units to build.
  rootenv['HWIO_UNITS'] = []

  # Create HWIO data class.
  rootenv['HWIO_DATA'] = HWIOScons()
   
  # Add method to enviroment
  rootenv.AddMethod(add_hwio_file, "AddHWIOFile")

  # Define the HWIO registers builder 
  hwioregs_act = SCons.Action.Action(hwioregs_builder, "=== Generating ${TARGET}")
  hwioregs_bld = env.Builder(action = hwioregs_act)
  rootenv.Append(BUILDERS = {'HWIORegistersBuilder' : hwioregs_bld})


#-------------------------------------------------------------------------------
# add_hwio_file
#
# Function for env.AddHWIOFile.  Defines a new set of HWIO files to generate.
#-------------------------------------------------------------------------------

def add_hwio_file (env, targets, registers, **args):

  # Bail out if we are not needed
  if not env.IsKeyEnable(targets) or not env.has_key('HWIO_IMAGE'):
    return None

  if type(registers) is not list:
    registers = [registers]

  image_env = env.get('IMAGE_ENV')

  # Set the source dependency as the address file
  source = env.get('ADDRESS_FILE')

  target_hwioregs = []
  for regfile in registers:
    if 'filename' in regfile:
      env.PrintDebugInfo('hwio', 'Adding hwio registers: ' + regfile['filename'])
      target = env.HWIORegistersBuilder(regfile['filename'], source)[0]
      target.hwio_registers = regfile
      target_hwioregs.append(target)

  hwio_units = image_env['HWIO_UNITS']
  hwio_units.append(target_hwioregs)

  return target_hwioregs


#-------------------------------------------------------------------------------
# hwioregs_builder
#  
# Builder function for HWIO register files.
#-------------------------------------------------------------------------------

def hwioregs_builder (target, source, env):
 
  # Get the HWIO data store.
  image_env = env.get('IMAGE_ENV')
  hwio = image_env.get('HWIO_DATA')

  # Acquire the lock to ensure we load a single address file at a time.
  hwio.lock.acquire()

  # Load the HWIO bases from the address file if not done yet. 
  if not source[0].path in hwio.bases:
    hwio.bases[source[0].path] = env.HWIOLoadAddressFile(source[0].path)
  bases = hwio.bases[source[0].path]

  # Extract the list of register dictionaries
  registers = []
  for tgt in target:
    registers.append(tgt.hwio_registers)

  # Create the register files
  env.HWIOGenerateRegisters(bases, registers)

  # Release the lock
  hwio.lock.release()

  return None


