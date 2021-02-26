#!/bin/sh

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

# call this from project root to update data make and bp files

QTI_DATA_HAL_COMPONENTS=(
    "vendor.qti.hardware.data.latency@1.0"
)

QTI_DATA_HAL_SRC_FOLDER=vendor/qcom/proprietary/interfaces/data

for i in "${QTI_DATA_HAL_COMPONENTS[@]}"
do
    echo "Update $i Android.bp"
    hidl-gen -Landroidbp -r vendor.qti.hardware.data:$QTI_DATA_HAL_SRC_FOLDER \
        -randroid.hidl:system/libhidl/transport $i

    echo "Update $i Android.mk"
    hidl-gen -Lmakefile -r vendor.qti.hardware.data:$QTI_DATA_HAL_SRC_FOLDER \
        -randroid.hidl:system/libhidl/transport $i
done
