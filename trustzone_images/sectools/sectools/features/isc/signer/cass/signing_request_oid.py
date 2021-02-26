# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.c_logging import logger
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.signer import signerutils
from sectools.features.isc.signer.cass.signing_request_base import BaseCASSSigningRequest
from sectools.features.isc.signer.signerutils.certificate import Certificate
from sectools.features.isc.signer.signerutils.certificate import SignAttr


class OidSigningRequest(BaseCASSSigningRequest):

    OID_DEBUG = "0x0000000000000002"
    OID_SW_ID = "0x0000000000000000"
    OID_HW_ID = "0x0000000000000000"
    OID_OEM_ID = "0x0000"
    OID_MODEL_ID = "0x0000"
    OID_SHA256 = "sha256"
    OID_SHA256_LIST = [None, OID_SHA256]

    def __init__(self, hash_to_sign, data_to_sign, image_path, signing_attributes):
        logger.debug("Creating CASS OID Signing Request...")
        self.signing_attributes = signing_attributes

        if self.signing_attributes.object_id is None:
            raise RuntimeError("CASS OID Signing Request requires an object id value.")

        BaseCASSSigningRequest.__init__(self, hash_to_sign, data_to_sign, image_path, signing_attributes)

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "oid_cass_signing_request"

    @classmethod
    def get_supported_capabilities(cls):
        return ["hexagon_adsp_auth", "hexagon_adsp_oemid", "ADSP_auth", "ADSP_test", "opendsp_adsp_auth_signing"]

    @classmethod
    def get_secboot_version(cls):
        return SECBOOT_VERSION_1_0

    @staticmethod
    def _validate_oid_config_attribute(attribute_name, value, expected_value):
        if value != expected_value:
            raise RuntimeError("{0} value of {1} is invalid for CASS OID Signing Request. {0} must have a value of {2}.".format(attribute_name, value, expected_value))

    def _create_attributes_to_add(self):
        additions = {}

        # DEBUG
        OidSigningRequest._validate_oid_config_attribute("debug", self.signing_attributes.debug, OidSigningRequest.OID_DEBUG)
        additions.update({SignAttr.DEBUG: OidSigningRequest.OID_DEBUG})

        # TCG_MIN
        attr_min, attr_max = Certificate.GetOIDAttrName(self.signing_attributes.object_id.name)
        if self.signing_attributes.object_id.min:
            additions.update({attr_min: self.signing_attributes.object_id.min})

        # TCG_MAX
        if self.signing_attributes.object_id.max:
            additions.update({attr_max: self.signing_attributes.object_id.max})

        return additions

    def _create_attributes_to_override(self):
        overrides = {}

        # Override fixed value attributes

        # SW_ID
        OidSigningRequest._validate_oid_config_attribute("sw_id", self.signing_attributes.sw_id, OidSigningRequest.OID_SW_ID)
        overrides.update({SignAttr.SW_ID: {SignAttr.SW_ID: OidSigningRequest.OID_SW_ID}})

        # HW_ID
        OidSigningRequest._validate_oid_config_attribute("hw_id (constructed from msm_part, soc_hw_version, and model_id)", "0x" + signerutils.get_hmac_params_from_config(self.signing_attributes).msm_id_str, OidSigningRequest.OID_HW_ID)
        overrides.update({SignAttr.HW_ID: {SignAttr.HW_ID: OidSigningRequest.OID_HW_ID}})

        # OEM_ID
        OidSigningRequest._validate_oid_config_attribute("oem_id", self.signing_attributes.oem_id, OidSigningRequest.OID_OEM_ID)
        overrides.update({SignAttr.OEM_ID: {SignAttr.OEM_ID: OidSigningRequest.OID_OEM_ID}})

        # MODEL_ID
        OidSigningRequest._validate_oid_config_attribute("model_id", self.signing_attributes.model_id, OidSigningRequest.OID_MODEL_ID)
        overrides.update({SignAttr.MODEL_ID: {SignAttr.MODEL_ID: OidSigningRequest.OID_MODEL_ID}})

        # SHA256
        OidSigningRequest._validate_oid_config_attribute("hash_algorithm", OidSigningRequest.OID_SHA256 if self.signing_attributes.hash_algorithm in OidSigningRequest.OID_SHA256_LIST else self.signing_attributes.hash_algorithm, OidSigningRequest.OID_SHA256)
        overrides.update({SignAttr.SHA256: {SignAttr.SHA256: "TRUE"}})

        return overrides

    def _create_attributes_to_remove(self):
        return [SignAttr.CRASH_DUMP, SignAttr.APP_ID]
