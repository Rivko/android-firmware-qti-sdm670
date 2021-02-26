# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

# This file is used to generate the autogenerated code and blueprint/makefiles for the hals in
# com.qualcomm.qti.imscmservice@1.0 service

HIDL_ROOT_DIR="$QCPATH/ims-ship/rcs/interfaces/"
LIBHIDL_COMPONENT='-r android.hidl:system/libhidl/transport'

IMS_CM_INTERFACE_LIB='com.qualcomm.qti.imscmservice@1.0'

LOC="$QCPATH/ims-ship/rcs/interfaces/imscmservice/1.0/default"

hidl-gen -Landroidbp -r com.qualcomm.qti:$HIDL_ROOT_DIR $LIBHIDL_COMPONENT $IMS_CM_INTERFACE_LIB
hidl-gen -Lmakefile -r com.qualcomm.qti:$HIDL_ROOT_DIR $LIBHIDL_COMPONENT $IMS_CM_INTERFACE_LIB

#hidl-gen -o $LOC -Lc++-impl -r com.qualcomm.qti:$HIDL_ROOT_DIR $LIBHIDL_COMPONENT $IMS_CM_INTERFACE_LIB

