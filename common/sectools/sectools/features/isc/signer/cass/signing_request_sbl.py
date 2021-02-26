# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Jan 28, 2016

@author: coryf
'''

from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.cass.signing_request_base import BaseCASSSigningRequest
from sectools.features.isc.signer.signerutils.certificate import Certificate


class SBLSigningRequest(BaseCASSSigningRequest):

    def __init__(self, to_sign, description, signing_attributes):
        logger.debug("Creating CASS SBL Signing Request...")
        self.signing_attributes = signing_attributes
        BaseCASSSigningRequest.__init__(self, to_sign, description, signing_attributes)

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "sbl_cass_signing_request"

    @classmethod
    def get_supported_capabilities(cls):
        return ["secboot_sha1_root", "secboot_sha2_root"]

    def _create_attributes_to_add(self):
        additions = {}

        # Add required attributes

        # DEBUG
        if self.signing_attributes.debug is not None:
            additions.update({Certificate.SIGNATTR_DEBUG: self.signing_attributes.debug})
        else:
            raise RuntimeError("CASS SBL Signing Request requires a debug value.")

        # EXP_3
        if self.signing_attributes.exponent == 3:
            additions.update({BaseCASSSigningRequest.EXP_3: 'TRUE'})
        elif self.signing_attributes.exponent == 65537:
            additions.update({BaseCASSSigningRequest.EXP_3: 'FALSE'})
        else:
            raise RuntimeError("Exponent value of {0} is invalid for CASS SBL Signing Request.".format(self.signing_attributes.exponent))

        # Add optional attributes

        # CERT_SHA256
        if self.signing_attributes.cass_attest_cert is not None:
            if self.signing_attributes.cass_attest_cert.lower() == "sha1":
                additions.update({BaseCASSSigningRequest.CERT_SHA256: "FALSE"})
            elif self.signing_attributes.cass_attest_cert.lower() == "sha256":
                additions.update({BaseCASSSigningRequest.CERT_SHA256: "TRUE"})
            else:
                raise RuntimeError("cass_attest_cert value of {0} is invalid for CASS SBL Signing Request.".format(self.signing_attributes.cass_attest_cert))

        return additions

    def _create_attributes_to_override(self):
        return None

    def _create_attributes_to_remove(self):
        return None
