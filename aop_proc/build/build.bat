@echo off
rem ==========================================================================
rem
rem  AOP build system launcher
rem
rem Copyright (c) 2016 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/aop.ho/1.1.c1/aop_proc/build/build.bat#1 $

SET BUILD_ASIC=SDM845
SET MSM_ID=845
SET HAL_PLATFORM=845
SET TARGET_FAMILY=845
SET CHIPSET=sdm845

python build_common.py %*

rem python mapread_bfam_splitmem.py ../.. > memory_%CHIPSET%.csv