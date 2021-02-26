# Copyright (c)  2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

hidl-gen -Landroidbp -r vendor.qti.hardware.automotive:vendor/qcom/proprietary/interfaces/automotive -r android.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport vendor.qti.hardware.automotive.vehicle@1.0
hidl-gen -Lmakefile -r vendor.qti.hardware.automotive:vendor/qcom/proprietary/interfaces/automotive -r android.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport vendor.qti.hardware.automotive.vehicle@1.0
