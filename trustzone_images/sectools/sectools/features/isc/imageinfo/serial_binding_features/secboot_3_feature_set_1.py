# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.cfgparser.auto_gen_xml_config import complex_multi_serial_numbers
from . import MAX_NUM_SERIALS_MAP
from .feature_set_base import FeatureSetBase
from .serial_bound_attributes import ActivationEnablementNotSupported
from .serial_bound_attributes import CrashDumpNotSupported
from .serial_bound_attributes import MetaDataDebug
from .serial_bound_attributes import MetaDataRootRevokeActivateEnable
from .serial_bound_attributes import MetaDataSerialBound
from .serial_bound_attributes import MetaDataUIEKeySwtichEnable
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RotEnNotSupported

# ALLOWED_CHIPSETS must be a tuple.
ALLOWED_CHIPSETS = ("sdx24", "sdx55", "sm8150", "sm8250", "sm6150", "sm7150", "sc8180x", "qcs405", "nicobar", "9205",
                    "saipan", "rennell", "ipq6018", "lahaina", "kamorta", "bitra", "agatti", "moselle", "chitwan",
                    "makena", "olympic", "cedros", "mannar", "sc7180", "fraser", "qtang2", "kodiak")


class Secboot3FeatureSetBase(FeatureSetBase):

    @classmethod
    def allowed_chipsets(cls):
        return ALLOWED_CHIPSETS


class Secboot3FeatureSet1(MetaDataDebug,
                          MetaDataSerialBound,
                          RotEnNotSupported,
                          MetaDataUIEKeySwtichEnable,
                          RevocationEnablementNotSupported,
                          ActivationEnablementNotSupported,
                          MetaDataRootRevokeActivateEnable,
                          CrashDumpNotSupported,
                          Secboot3FeatureSetBase):

    feature_id = "secboot_3_feature_set_1"
    max_serial_number_count = MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_3_0]

    @classmethod
    def bind(cls, config):
        """ Update signing attributes pertaining to serial-binding.

        The affected attributes are:
          * use_serial_number_in_signing - always set to 1
          * multi_serial_numbers - all serial numbers go here
          * serial_number - set to 0 to avoid any potential ambiguity
                  downstream when signer puts together the meta data

        In addition to all the signing attributes above, the following
        serial-binding attributes can benefit, without any code change:
          * debug
          * uie_key_switch_enable
          * root_revoke_activate_enable
        """
        config.general_properties.use_serial_number_in_signing = 1
        config.general_properties.multi_serial_numbers = \
            complex_multi_serial_numbers(config.valid_serial_numbers)
        config.general_properties.serial_number = "0x%08x" % 0
        super(Secboot3FeatureSet1, cls).bind(config)

    def is_matched(self, config):
        if config.platform_independent or (not config.platform_independent and super(Secboot3FeatureSet1,
                                                                                     self).is_matched(config)):
            return config.secboot_version == SECBOOT_VERSION_3_0
        return False

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        cls.serial_numbers = []
        if image_signing_attributes.multi_serial_numbers:
            cls.serial_numbers = image_signing_attributes.multi_serial_numbers
        super(Secboot3FeatureSet1, cls).update_image_serial_bound_notes(
            image_signing_attributes, serial_binding_notes)
