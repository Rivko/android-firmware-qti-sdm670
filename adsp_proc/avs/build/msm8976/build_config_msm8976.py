#===============================================================================
#
# AVS build configuration
#
# GENERAL DESCRIPTION
#    AVS build configuration for different chipsets & AVSTest must be specified here.
#
#    set in the AVS image scons script file.
#       1) A normal library with symbols - no need specify it
#       2) A stub library  USES_<LIB NAME IN CAPITAL CASE>_STUB
#       3) A stripped library USES_<LIB NAME IN CAPITAL CASE>_STRIP
#
# Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/avs.build/1.0/msm8976/build_config_msm8976.py#1 $
#  $DateTime: 2017/08/07 22:00:22 $
#  $Author: pwbldsvc $
#  $Change: 14036754 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/21/12   kk      Build configuration
# 04/18/13   nr      Stub out ultrasound stream manager for 8x10 and 9x25
#
#===============================================================================


def chipset_specific_build_config(env):
    env.Replace(USES_DTS =False)
    env.Replace(USES_MIDI =False)

    if env.get('AVS_TEST') == True:
        env.Append(CFLAGS='-DADSP_TEST_ENTRY -DSIM -DSIM_DEFINED')
        env.Replace(USES_DTS =True)
        env.Replace(USES_MIDI =True)
        import re
        temp=env.get('CC_DFLAGS')
        yy = re.sub('-DASSERT=ASSERT_FATAL','',temp)
        env.Replace(CC_DFLAGS=yy)

        #stripping/stubbing is now controlled through json.
        #for AVS_TEST all libs are regular/stub (strip not possible).
        #a library cannot be stub on target and strip on AVS_TEST. If it's stub on target, it's stub on SIM too.
        #details about this: msm8996/readme.txt  or <chipset>/readme.txt
    else:

        env.Replace(USES_DTS =True)
        env.Replace(USES_MIDI =True)
        # NOTE: Please ensure that every newly added library should be properly stripped
        # for those symbols which are proprietary. Do not add here. Add in json.
        
        #stripping/stubbing is now controlled through json.
        
    return



