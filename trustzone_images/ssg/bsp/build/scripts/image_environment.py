""" 
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
 
#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return True

def generate(env):
  """
  Sets up per-image environment after InitImageVars() 
  secure_app_builder adds this to the list of tools then loaded by InitBuildConfig
  """
  
  #set the architecture type of the target processor for the current image
  if env['PROC'] in ['A53_64']:
    env.Replace(APP_EXEC_MODE = 'aarch64')
  elif env['PROC'] in ['scorpion']:
    env.Replace(APP_EXEC_MODE = 'aarch32')
  elif env['PROC'] in ['x86-32', 'x86-64']:
    env.Replace(APP_EXEC_MODE = 'x86')
