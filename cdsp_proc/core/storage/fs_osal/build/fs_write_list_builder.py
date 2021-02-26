#============================================================================
#
# EFS Write List to file.
#
# GENERAL DESCRIPTION
#   Builder that writes a list into a file. The destination file is the
#   'target' argument, the list to print is passed using the variable
#   FS_WRITE_LIST.
#
# Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1.c4/storage/fs_osal/build/fs_write_list_builder.py#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $ $Change: 16759227 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when        who  what, where, why
# ----------  ---  ----------------------------------------------------------
# 2012-03-08  wek  Create.
#
#============================================================================

#----------------------------------------------------------------------------
# Hooks for Scons
# The generate function gets called by the function LoadToolScript()
# It registers/publishes the methods to the environment.
#----------------------------------------------------------------------------
def generate(env):
    fs_write_list_act = env.GetBuilderAction (fs_write_list_builder)
    fs_write_list_bld = env.Builder(action = fs_write_list_act,
                                   suffix = '.txt')
    env.Append(BUILDERS = {'FsWriteList' : fs_write_list_bld})

def exists(env):
   return env.Detect('fs_write_list_builder')

#----------------------------------------------------------------------------
# Writes out the list into a file.
#
# Parameters:
# env           Environment. Passed automatically.
# target        Output file path. This will be the name of the text file.
# source        Ignored.
# FS_WRITE_LIST List of items to write into the output file
#----------------------------------------------------------------------------
def fs_write_list_builder (env, target, source):
  if env.has_key ('FS_WRITE_LIST'):
    write_list = env.get ('FS_WRITE_LIST')
  else:
    raise RuntimeError, 'EFS List creation: FS_WRITE_LIST is not defined'

  out_file = str(target[0])
  fout = open (out_file, 'w')
  for item in write_list:
    fout.write (str(item) + '\n')
  fout.close ()
  return

