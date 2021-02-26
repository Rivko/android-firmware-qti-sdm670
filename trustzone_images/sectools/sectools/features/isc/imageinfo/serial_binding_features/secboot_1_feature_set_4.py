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
from .serial_bound_attributes import CrashDumpNotSupported
from .serial_bound_attributes import DebugUnconditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RootRevokeActivateEnableNotSupported
from .serial_bound_attributes import RotEnSingleNoMultiSerialBound
from .serial_bound_attributes import SerialBoundNotSupported
from .serial_bound_attributes import UIEKeySwitchEnableNotSupported
from ..serial_binding_features import MAX_NUM_SERIALS_MAP

# ALLOWED_CHIPSETS must be a tuple.
ALLOWED_CHIPSETS = ("8996", "8996au")


class Secboot1FeatureSet4(DebugUnconditionalSingleSerialBound,
                          SerialBoundNotSupported,
                          RotEnSingleNoMultiSerialBound,
                          UIEKeySwitchEnableNotSupported,
                          RevocationEnablementNotSupported,
                          ActivationEnablementNotSupported,
                          RootRevokeActivateEnableNotSupported,
                          CrashDumpNotSupported,
                          FeatureSetBase):

    feature_id = "secboot_1_feature_set_4"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_1_0]
    debug_enablement_value = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def allowed_chipsets(cls):
        return ALLOWED_CHIPSETS
