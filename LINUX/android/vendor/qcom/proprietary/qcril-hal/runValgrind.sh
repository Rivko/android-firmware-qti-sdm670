#!/bin/bash
#******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************


valgrind  --leak-check=full --track-origins=yes --show-reachable=yes --error-limit=no --log-file=./valgrind.log  --gen-suppressions=all  --suppressions=./valgrind.suppress  $ANDROID_HOST_OUT/bin/riltest --gtest_output=xml:valgrind_test_report.xml
