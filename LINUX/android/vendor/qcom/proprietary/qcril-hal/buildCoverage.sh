#!/bin/bash
#******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************
err_exit() {
    if [ "$1" ]; then
        echo -e 1>&2 "\nERROR: $1"
    fi
    exit 1
}
lcov -d $ANDROID_BUILD_TOP/vendor/qcom/proprietary/qcril-hal/bin -b $ANDROID_BUILD_TOP -c -o bin/cov.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov"
fi

genhtml bin/cov.info -o bin/output
if [ $? -ne 0 ]; then
    err_exit "    FAILED: genhtml"
fi

#firefox bin/output/index.html  &
