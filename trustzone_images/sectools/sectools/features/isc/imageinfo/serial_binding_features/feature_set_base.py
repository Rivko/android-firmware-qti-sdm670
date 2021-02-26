# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta

from six import with_metaclass
from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.utils.c_data import remove_duplicates_ordered
from sectools.features.isc.cfgparser.chipset_profiles import CHIPSET_PROFILES
from sectools.features.isc.advanced_defines import PLATFORM_BINDING_INDEPENDENT
from . import serial_bound_feature_manager


class FeatureSetBase(with_metaclass(ABCMeta, SecPluginIntf_Abs)):

    feature_id = None
    max_serial_number_count = None
    debug_enablement_value = None

    def __init__(self):
        self.msm_parts = []
        self.chipset_soc_vers = []
        for chipset in self.allowed_chipsets():
            self.msm_parts += [x[3:-3] for x in CHIPSET_PROFILES[chipset]["msm_part"]]
            # Make sure soc_vers only use lowercase letters. The is_matched method
            # uses the same to find a matching feature set.
            self.chipset_soc_vers += [
                x.lower() for x in CHIPSET_PROFILES[chipset].get("soc_vers", [])]

    @classmethod
    def get_plugin_manager(cls):
        return serial_bound_feature_manager

    @classmethod
    def is_plugin(cls):
        return False

    @classmethod
    def get_plugin_id(cls):
        return cls.feature_id

    @classmethod
    def check_max_serial_number_count(cls, valid_serial_numbers):
        if cls.max_serial_number_count is None:
            raise NotImplementedError("max_serial_number_count must be implemented")
        if len(valid_serial_numbers) > cls.max_serial_number_count:
            verb_be = "is" if cls.max_serial_number_count == 1 else "are"
            raise RuntimeError(
                "{0} serials were provided. A maximum of {1} {2} allowed.".format(
                    len(valid_serial_numbers), cls.max_serial_number_count, verb_be))

    def is_matched(self, config):
        if config.platform_independent:
            # Cannot use soc_hw_version/soc_vers/msm_part to determine feature set since image is not bound to any
            # hardware values
            return False
        else:
            if config.in_use_soc_hw_version:
                soc_vers = []
                if config.soc_vers:
                    soc_vers.extend(config.soc_vers)
                soc_hw_version = config.soc_hw_version
                if soc_hw_version:
                    soc_vers.append(soc_hw_version[:-4])
                soc_vers = remove_duplicates_ordered(*soc_vers)
                if soc_vers:
                    return (set([x.lower() for x in soc_vers]) -
                            set(self.chipset_soc_vers)) == set()
            else:
                msm_part = config.msm_part
                if any([x.upper() in msm_part.upper()[3:-3] for x in self.msm_parts]):
                    return True
            return False

    @classmethod
    def allowed_chipsets(cls):
        raise NotImplementedError("allowed_chipsets must be implemented")

    @classmethod
    def bind(cls, config):
        """ Terminate the super-chain of bind method of all feature implementations. """
        pass

    @classmethod
    def update_image_serial_bound_notes(cls, image_signing_attributes, serial_binding_notes):
        """ Terminate the super-chain of the method of all feature implementations. """
        for attr_name, attr_value in serial_binding_notes.items():
            serial_binding_notes[attr_name] = remove_duplicates_ordered(*attr_value)
