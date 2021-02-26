#===============================================================================
# Copyright (c) 2013- by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#===============================================================================
#
# A top level script that loads all the builders
# exposed by Platform AU in ADSP builds
#
def exists(env):
	return True

def generate(env):
   env.LoadToolScript('qaic', toolpath = ['${BUILD_ROOT}/platform/build'])
   env.LoadToolScript('platform_libs', toolpath = ['${BUILD_ROOT}/platform/build'])
   env.LoadToolScript('dlinker_symbols', toolpath = ['${BUILD_ROOT}/core/bsp/build/scripts'])
   env.LoadToolScript('hash_libs', toolpath = ['${BUILD_ROOT}/platform/build'])

