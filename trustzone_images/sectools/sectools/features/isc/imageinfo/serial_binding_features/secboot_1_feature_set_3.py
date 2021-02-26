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
from .serial_bound_attributes import ActivationEnablementSingleNoMultiSerialBound
from .serial_bound_attributes import CrashDumpSingleNoMultiSerialBound
from .serial_bound_attributes import DebugConditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementSingleNoMultiSerialBound
from .serial_bound_attributes import RootRevokeActivateEnableNotSupported
from .serial_bound_attributes import RotEnNotSupported
from .serial_bound_attributes import SingleSerialBound
from .serial_bound_attributes import UIEKeySwitchEnableNotSupported
from ..serial_binding_features import MAX_NUM_SERIALS_MAP

# ALLOWED_CHIPSETS must be a tuple.
ALLOWED_CHIPSETS = ("401x", "4020")


class Secboot1FeatureSet3(DebugConditionalSingleSerialBound,
                          SingleSerialBound,
                          RotEnNotSupported,
                          UIEKeySwitchEnableNotSupported,
                          RevocationEnablementSingleNoMultiSerialBound,
                          ActivationEnablementSingleNoMultiSerialBound,
                          RootRevokeActivateEnableNotSupported,
                          CrashDumpSingleNoMultiSerialBound,
                          FeatureSetBase):

    feature_id = "secboot_1_feature_set_3"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_1_0]
    debug_enablement_value = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def allowed_chipsets(cls):
        return ALLOWED_CHIPSETS
