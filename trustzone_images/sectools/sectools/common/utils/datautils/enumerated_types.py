# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

ENUMERATED_ROOT_KEY_TAG = "root_key"
ENUMERATED_ENABLE_DISABLE_TAG = "enable_disable"
ENUMERATED_NOP_ENABLE_DISABLE_TAG = "nop_enable_disable"
ENUMERATED_ENABLE_DISABLE_HEX64_TAG = "enable_disable_hex64"
ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG = "disable_enable_bound_hex64"


class TypeEnumeratedNopEnableDisable(object):
    pass


class TypeEnumeratedRootKeyType(object):
    pass


class TypeEnumeratedEnableDisable(object):
    pass


class TypeEnumeratedEnableDisableHex64(object):
    pass


class TypeEnumeratedDisableEnableBoundHex64(object):
    pass


ENUMERATED_TYPES = {
    ENUMERATED_ROOT_KEY_TAG: TypeEnumeratedRootKeyType,
    ENUMERATED_ENABLE_DISABLE_TAG: TypeEnumeratedEnableDisable,
    ENUMERATED_NOP_ENABLE_DISABLE_TAG: TypeEnumeratedNopEnableDisable,
    ENUMERATED_ENABLE_DISABLE_HEX64_TAG: TypeEnumeratedEnableDisableHex64,
    ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG: TypeEnumeratedDisableEnableBoundHex64,
}

enumerated_values = {}


def set_enumerated_values(tag, **kwargs):
    enumerated_values.setdefault(tag, {}).update(**kwargs)
    return ENUMERATED_TYPES[tag]()


NOP = "nop"
TRUE = "true"
FALSE = "false"
ENABLE = "enable"
DISABLE = "disable"
ENABLE_BOUND = "enable_bound"
RTL_QTI = "rtl_qti"
OTP_OEM = "otp_oem"
OTP_QTI = "otp_qti"