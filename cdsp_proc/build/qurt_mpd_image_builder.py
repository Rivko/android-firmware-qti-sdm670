#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create MBN file
#
# Copyright (c) 2011,2015,2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/qurt_mpd_image_builder.py#1 $
#  $DateTime: 2018/08/07 05:36:15 $
#  $Author: pwbldsvc $
#  $Change: 16823152 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 03/23/17   sm        Update to use generated version of qurt-image-build.py.
# 08/31/15   clew      Add Support for RAMFS and Restartable options
# 10/03/11   dxiang    Clean up
#============================================================================
import os
import subprocess
import string
import re
import zipfile
import glob
import shutil
import fnmatch
import operator
from SCons.Script import *


#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
qurt_builder = "${BUILD_ROOT}/core/kernel/qurt/build/${ROOT_IMAGE}/${PROC}/${SHORT_BUILDPATH}/install/${QURT_BUILD_CONFIG}/scripts/qurt-image-build.py"

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('qurtimagesconsbuilder')

def generate(env):
   qurtimage_generate(env)
   # import pdb;pdb.set_trace()
   # env['MyBuild'] = []

def qurtimage_generate(env):
   qurtimage_act = env.GetBuilderAction(qurtimage_builder)
   qurtimage_bld = env.Builder(action = qurtimage_act,
                                      emitter = qurtimage_emitter,
                                      suffix = '.elf')

   env.Append(BUILDERS = {'QurtImageBuilder' : qurtimage_bld})

def qurtimage_builder(target, source, env):
   source_path, source_file = os.path.split(str(source[0]))
   target_path, target_file = os.path.split(str(target[0]))
   source_base = os.path.splitext(str(source[0]))[0]
   target_base = os.path.splitext(str(target[0]))[0]
   
   target_pbn = str(target[0])
   source_elf = str(source[0])
   #source_tool = str(source[1])
   #source_for_signature = env.get('PHYS_ADDR')

   #env.qurtimage(source_tool, source_elf, source_for_signature, target_pbn)


#def qurtimage(source, target, env, for_signature):
   ''' Find the "best" python interpreter to use '''
   ''' Execute the following strings one at a time,
       catching errors on a per line basis, until we get an interpreter '''
   interp = None
   for s in ['interp = env["PYTHON_INTERP"]',
             'import SCons',
             'interp = SCons.Util.python_interpreter_command()',
             'import sys',
             'interp = sys.executable',
             'interp = SCons.Util.WhereIs("python")',
             'interp = "python"']:
      if not interp:
         try:
            exec(s)
         except Exception:
            pass

   #qurt-image-build.py should be in one of the listed directories. Moves around due to strip/pack. Will be fixed with kernel change to hardcode location of script

   if   os.path.isfile( env.RealPath(qurt_builder)   ):
      tool=env.RealPath(qurt_builder)
   else:
      env.PrintError('Could not find qurt-image-build.py.  Exiting.')
      sys.exit(1)

   #Alias
   aliasdict = env.get('ALIAS',{})
   ramfslist = env.get('RAMFS', [])
   restartlist = env.get('RESTART', [])

   if ramfslist:
      ramfslist = [str(i) for i in ramfslist]
      ramfslist = reduce(operator.add, ramfslist)

   if restartlist:
      restartlist = [str(i) for i in restartlist]
      restartlist = reduce(operator.add, restartlist)

   arglist=[]
   nameindex=[]
   #import pdb;pdb.set_trace()
   for w in source:
      name = str(w)

      if name in restartlist:
         arglist += ['-R ']
	 if name.endswith('.elf'):
            aliaskey = aliasdict.keys()
            if len(aliaskey) > 0:
               for elfpath in aliaskey:
                  if name in elfpath:
                     nameindex=aliaskey.index(elfpath)
                     arglist += ['%s=%s' % (aliasdict[aliaskey[nameindex]] , aliaskey[nameindex])]
            else:
               arglist += ['%s' % name]
      elif name in ramfslist:
         arglist += ['--file %s' % name]
      else:
         if name.endswith('.elf'):
            aliaskey = aliasdict.keys()
            if len(aliaskey) > 0:
               for elfpath in aliaskey:
                  if name in elfpath:
                     nameindex=aliaskey.index(elfpath)
                     arglist += ['%s=%s' % (aliasdict[aliaskey[nameindex]] , aliaskey[nameindex])]
            else:
               arglist += ['%s' % name]

   cmd = ' '.join(Flatten([interp, tool,
                         arglist,
                         '-o',
                         [str(w) for w in target if str(w).endswith(('.elf','.pbn'))],
                         #target_pbn,
                         '-p',
                         env.get('PHYS_ADDR','0x1e000000'),
                         '--assert_free_microimage=%s' % env.get('MICROIMAGE_RESERVED','0K')
                         ]))

   print cmd
   std_out, std_err, return_val = env.ExecCmds(cmd)
   if(return_val):
      env.PrintError(std_err)
      sys.exit(return_val)



def qurtimage_emitter(target, source, env):
#   import pdb;pdb.set_trace()
   if env.get('ROOT_IMAGE') is None:
      env.PrintError("The qurt_mpd_image_builder.py has detected that  the ROOT_IMAGE variable is not")
      env.PrintError("defined in this environment.  This variable must be defined before this ")
      env.PrintError("builder's action is invoked.  ROOT_IMAGE shpuld be set to the SCons image name")
      env.PrintError("that has performed the linking for the Root PD, and is the SCons image that QuRT ")
      env.PrintError("has been linked into.")
      sys.exit(1)
   if env.get('QURT_BUILD_CONFIG') is None:
      env.PrintError("The qurt_mpd_image_builder.py has detected that QURT_BUILD_CONFIG variable is ")
      env.PrintError("not defined in the environment.  This variable must be defined before this ")
      env.PrintError("builder's action is invoked.")
      sys.exit(1)
   source += [qurt_builder]

   return (target, source)
