# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.datautils.enumerated_types import ENABLE
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.cfgparser.defines import ENUMERATED_DEBUG_MAPPING
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from .secboot_3_feature_set_1 import Secboot3FeatureSetBase
from .serial_bound_attributes import ActivationEnablementNotSupported
from .serial_bound_attributes import CrashDumpNotSupported
from .serial_bound_attributes import DebugConditionalMultiSerialBound
from .serial_bound_attributes import DebugConditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RootRevokeActivateEnableNotSupported
from .serial_bound_attributes import RotEnNotSupported
from .serial_bound_attributes import SingleMultiSerialBound
from .serial_bound_attributes import UIEKeySwitchEnableNotSupported
from ..serial_binding_features import MAX_NUM_SERIALS_MAP


class Secboot3_1FeatureSet1(DebugConditionalSingleSerialBound,
                            DebugConditionalMultiSerialBound,
                            SingleMultiSerialBound,
                            RotEnNotSupported,
                            UIEKeySwitchEnableNotSupported,
                            RevocationEnablementNotSupported,
                            ActivationEnablementNotSupported,
                            RootRevokeActivateEnableNotSupported,
                            CrashDumpNotSupported,
                            Secboot3FeatureSetBase):

    feature_id = "secboot_3_1_feature_set_1"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_2_0]
    debug_enablement_value = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]

    def is_matched(self, config):
        if super(Secboot3_1FeatureSet1, self).is_matched(config):
            return config.secboot_version == SECBOOT_VERSION_1_0
        return False

    @classmethod
    def is_plugin(cls):
        return True
