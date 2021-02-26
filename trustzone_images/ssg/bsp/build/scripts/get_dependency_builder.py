#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to build a Secure App
#
# Copyright 2015-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tz/1.0.2/bsp/build/scripts/get_dependency_builder.py#1 $
#  $DateTime: 2018/02/06 03:00:17 $
#  $Author: pwbldsvc $
#  $Change: 15399933 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
#
#============================================================================
import os

def exists(env):
  True

def generate(env):
  env.AddMethod(get_ta_dependency,    "GetTADependencies")
  env.AddMethod(get_commonlib_mbn,    "GetCommonlibMBN")
  env.AddMethod(get_ta_linker_script, "GetTALinkerScript")

def get_ta_dependency(env):
  cmnlib_name = 'cmnlib.lib' if env.OfftargetTesting() else 'libcmnlib.so'
    
  applib = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/applib/build/${SHORT_BUILDPATH}/${PROC}/applib.lib')]
  cmnlib = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/commonlib/build/${SHORT_BUILDPATH}/${PROC}/' + cmnlib_name)]
  appobj = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/applib/build/${SHORT_BUILDPATH}/${PROC}/common_applib.o')]

  return applib + cmnlib, appobj

def get_commonlib_mbn(env):
  # offtarget testing won't generate a cmnlib mbn
  if env['PROC'] == 'scorpion':
    cmnlib_mbn = '${BUILD_ROOT}/build/ms/bin/${SHORT_BUILDPATH}/cmnlib.mbn'
  elif env['PROC'] == 'A53_64':
    cmnlib_mbn = '${BUILD_ROOT}/build/ms/bin/${SHORT_BUILDPATH}/cmnlib64.mbn'
  
  
def get_ta_linker_script(env):
  if env['PROC'] == 'scorpion':
    return '${BUILD_ROOT}/ssg/bsp/build/scripts/secureapp.ld'
  elif env['PROC'] == 'A53_64':
    return '${BUILD_ROOT}/ssg/bsp/build/scripts/secureapp64.ld'
