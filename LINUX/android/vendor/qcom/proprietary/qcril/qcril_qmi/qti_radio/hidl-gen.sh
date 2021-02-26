# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

QCRADIOHIDL_COMPONENT='com.qualcomm.qti:vendor/qcom/proprietary/telephony-fwk/opt/telephony/interfaces'
LIBHIDL_COMPONENT='android.hidl:system/libhidl/transport'
QCRADIO_INTERFACE_LIB='com.qualcomm.qti.qtiradio@1.0'

hidl-gen -Landroidbp -r $QCRADIOHIDL_COMPONENT -r $LIBHIDL_COMPONENT $QCRADIO_INTERFACE_LIB
hidl-gen -Lmakefile -r $QCRADIOHIDL_COMPONENT -r $LIBHIDL_COMPONENT $QCRADIO_INTERFACE_LIB

