# ==========================================================================
#
#  AOP build system launcher
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================

#!/bin/bash

cd './aop_proc/build/'
./build_670.sh $1
./build_605.sh $1