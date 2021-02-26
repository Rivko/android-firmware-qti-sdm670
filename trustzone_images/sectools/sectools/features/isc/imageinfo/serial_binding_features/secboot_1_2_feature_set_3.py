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
from .feature_set_base import FeatureSetBase
from .serial_bound_attributes import ActivationEnablementNotSupported
from .serial_bound_attributes import CrashDumpNotSupported
from .serial_bound_attributes import DebugConditionalMultiSerialBound
from .serial_bound_attributes import DebugConditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RootRevokeActivateEnableSingleNoMultiSerialBound
from .serial_bound_attributes import RotEnNotSupported
from .serial_bound_attributes import SingleMultiSerialBound
from .serial_bound_attributes import UIEKeySwitchEnableSingleNoMultiSerialBound
from ..serial_binding_features import MAX_NUM_SERIALS_MAP

# ALLOWED_CHIPSETS must be a tuple.
ALLOWED_CHIPSETS = ("sdm670", "sdm845")


class Secboot1_2FeatureSet3(DebugConditionalSingleSerialBound,
                            DebugConditionalMultiSerialBound,
                            SingleMultiSerialBound,
                            RotEnNotSupported,
                            UIEKeySwitchEnableSingleNoMultiSerialBound,
                            RevocationEnablementNotSupported,
                            ActivationEnablementNotSupported,
                            RootRevokeActivateEnableSingleNoMultiSerialBound,
                            CrashDumpNotSupported,
                            FeatureSetBase):

    feature_id = "secboot_1_2_feature_set_3"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_2_0]
    debug_enablement_value = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_2_0]

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def allowed_chipsets(cls):
        return ALLOWED_CHIPSETS
