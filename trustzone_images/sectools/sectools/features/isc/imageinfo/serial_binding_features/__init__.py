# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import sys
from glob import glob

from sectools.common.core.plugin import SecPluginMgr
from sectools.common.crypto import cert
from sectools.common.parsegen.mbn.versions.v6.headers import METADATA_MAJOR_VERSION_1, \
    METADATA_MINOR_VERSION_0
from sectools.common.utils.c_logging import logger
from sectools.features.isc.advanced_defines import AUTHORITY_QTI, \
    SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0
from sectools.features.isc.cfgparser.chipset_profiles import ChipsetProfile
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.signer.signerutils.attribute_extractor import AttributeExtractor
from sectools.features.isc.signer.signerutils.attributes import SigningAttributes

MAX_NUM_SERIALS_8 = 8
MAX_NUM_SERIALS_MAP = {
    SECBOOT_VERSION_1_0: 1,
    # Secboot version 2.0 and some version 1.0 images support
    # up to 200 serial numbers in cert SN OU field.
    SECBOOT_VERSION_2_0: 200,
    SECBOOT_VERSION_3_0: MAX_NUM_SERIALS_8,
}


class SerialBoundFeatureSetContextManager(object):

    def __init__(self, chipset_profile, valid_serial_numbers):
        self.valid_serial_numbers = valid_serial_numbers
        self.feature_set = serial_bound_feature_manager.get_feature_set(
            chipset_profile)

    def __enter__(self):
        self.feature_set.check_max_serial_number_count(self.valid_serial_numbers)
        return self.feature_set

    def __exit__(self, *args):
        pass


class SerialBoundFeatureManager(SecPluginMgr):

    # All serial-binding implementation instances are reused. They are kept in the list.
    feature_implementations = []

    def get_feature_set(self, config):
        if not self.feature_implementations:
            self.feature_implementations = [x() for x in self.get_map().values()]
        matched_implementations = [x for x in self.feature_implementations if
                                   x.is_matched(config) is True]
        if len(matched_implementations) == 1:
            return matched_implementations[0]
        logger.debug("matched_implementations: " + str(matched_implementations))
        raise RuntimeError("Serial binding is not supported for the given configuration")


serial_bound_feature_manager = SerialBoundFeatureManager()


def update_image_serial_bound_notes(authority, parsegen, imageinfo):

    if authority == AUTHORITY_QTI:
        cert_chain_der = cert.split_cert_chain_bin(parsegen._mbn_parsegen.cert_chain_qti)
    else:
        cert_chain_der = cert.split_cert_chain_bin(parsegen._mbn_parsegen.cert_chain)

    hash_segment_meta_data = parsegen.get_hash_segment_metadata(authority=authority)
    if not cert_chain_der and not hash_segment_meta_data:
        return

    image_signing_attributes = AttributeExtractor(
        cert_data=cert_chain_der[0] if cert_chain_der else None,
        hash_segment_metadata=hash_segment_meta_data,
        attributes=SigningAttributes()).attributes

    msm_part = None
    soc_hw_version = None
    soc_vers = image_signing_attributes.soc_vers
    in_use_soc_hw_version = image_signing_attributes.in_use_soc_hw_version
    if in_use_soc_hw_version:
        soc_hw_version = image_signing_attributes.hw_id[:-8]
    else:
        msm_part = image_signing_attributes.hw_id[:-8]
    platform_independent = False
    if (image_signing_attributes.metadata_major_version, image_signing_attributes.metadata_minor_version) == \
            (METADATA_MAJOR_VERSION_1, METADATA_MINOR_VERSION_0):
        if not image_signing_attributes.in_use_soc_hw_version and not image_signing_attributes.in_use_hw_id:
            platform_independent = True

    try:
        serial_binding_feature_set = serial_bound_feature_manager.get_feature_set(
            ChipsetProfile(parsegen.secboot_version, in_use_soc_hw_version, msm_part,
                           soc_vers, soc_hw_version, platform_independent))
        serial_binding_feature_set.update_image_serial_bound_notes(
            image_signing_attributes, imageinfo.create_serial_bound_notes(authority))
    except RuntimeError:
        pass


def evaluate_serial_binding_attributes(authority, imageinfo):
    if authority not in imageinfo.get_serial_bound_notes():
        return
    notes = imageinfo.get_serial_bound_notes()[authority]
    logger.debug("image signing authority: " + authority)
    if notes:
        for attr_name, attr_value in notes.items():
            logger.debug("%s is serial-bound to %s" % (attr_name, attr_value))
        intersection = sorted(list(set.intersection(*[set(x) for x in notes.values()])))
        if intersection:
            logger.warning("{authority} bound the image to serial numbers: {serial_numbers}"
                           .format(authority=authority,
                                   serial_numbers=", ".join(intersection)))
        else:
            raise RuntimeError("{authority} incorrectly bound the image to serial numbers "
                               "in such a way that it will not authenticate on any device."
                               .format(authority=authority))
    else:
        logger.debug("{authority} bound the image to all serial numbers."
                     .format(authority=authority))


# Import all serial-binding feature Python modules
for py_file_name in glob(os.path.join(os.path.dirname(__file__), "*.py")):
    module_name = os.path.splitext(os.path.basename(py_file_name))[0]
    if module_name in ["__init__"]:
        continue
    if hasattr(sys.modules[__name__], module_name):
        continue
    __import__(".".join([__name__, module_name]))
