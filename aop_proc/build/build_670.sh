# ==========================================================================
#
#  AOP build system launcher
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/aop.ho/1.1.c1/aop_proc/build/build_670.sh#1 $

export BUILD_ASIC=670
export MSM_ID=670
export HAL_PLATFORM=670
export TARGET_FAMILY=670
export CHIPSET=sdm670
export CHIPSET_PMIC=pm670
export SECPOLICY=USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN

CLANG=/pkg/qct/software/llvm/release/arm/3.9.3/bin/clang
export SD_LLVM_ROOT=`dirname $CLANG`/..

cd './aop_proc/build/'

python ./build_common.py $@
