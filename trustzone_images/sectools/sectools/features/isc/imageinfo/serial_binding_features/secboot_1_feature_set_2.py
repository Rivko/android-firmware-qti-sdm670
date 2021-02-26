# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.datautils.enumerated_types import ENABLE
from sectools.features.isc.cfgparser.defines import ENUMERATED_DEBUG_MAPPING
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from .feature_set_base import FeatureSetBase
from .serial_bound_attributes import ActivationEnablementNotSupported
from .serial_bound_attributes import CrashDumpSingleNoMultiSerialBound
from .serial_bound_attributes import DebugUnconditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RootRevokeActivateEnableNotSupported
from .serial_bound_attributes import RotEnNotSupported
from .serial_bound_attributes import SerialBoundNotSupported
from .serial_bound_attributes import UIEKeySwitchEnableNotSupported
from ..serial_binding_features import MAX_NUM_SERIALS_MAP

ALLOWED_CHIPSETS = (
    "2432", "8009lat", "8016", "8039", "8064", "806x", "8076", "807x",
    "8084", "8905", "8909", "8909W", "8916", "9206", "9206tx", "9607", "ipq5018")


class Secboot1FeatureSet2(DebugUnconditionalSingleSerialBound,
                          SerialBoundNotSupported,
                          RotEnNotSupported,
                          UIEKeySwitchEnableNotSupported,
                          RevocationEnablementNotSupported,
                          ActivationEnablementNotSupported,
                          RootRevokeActivateEnableNotSupported,
                          CrashDumpSingleNoMultiSerialBound,
                          FeatureSetBase):

    feature_id = "secboot_1_feature_set_2"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_1_0]
    debug_enablement_value = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def allowed_chipsets(cls):
        return ALLOWED_CHIPSETS
