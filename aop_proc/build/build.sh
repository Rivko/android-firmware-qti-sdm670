# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2016, 2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Proprietary and Confidential - Qualcomm Technologies, Inc/GTDR
# ==========================================================================
# $Header: //components/rel/aop.ho/1.1.c1/aop_proc/build/build.sh#1 $

export BUILD_ASIC=SDM845
export MSM_ID=845
export HAL_PLATFORM=845
export TARGET_FAMILY=845
export CHIPSET=sdm845

cd './aop_proc/build/'

python ./build_common.py $@

