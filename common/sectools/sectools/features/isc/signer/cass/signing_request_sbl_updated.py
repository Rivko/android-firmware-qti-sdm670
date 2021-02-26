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


class SBLUpdatedSigningRequest(BaseCASSSigningRequest):

    def __init__(self, to_sign, description, signing_attributes):
        logger.debug("Creating CASS Updated SBL Signing Request...")
        self.signing_attributes = signing_attributes
        BaseCASSSigningRequest.__init__(self, to_sign, description, signing_attributes)

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "sbl_updated_cass_signing_request"

    @classmethod
    def get_supported_capabilities(cls):
        return ["SBL_updated_capability_1", "SBL_updated_capability_2"]

    def _create_attributes_to_add(self):
        additions = {}

        # Add required attributes

        # DEBUG
        if self.signing_attributes.debug is not None:
            additions.update({Certificate.SIGNATTR_DEBUG: self.signing_attributes.debug})
        else:
            raise RuntimeError("CASS Updated SBL Signing Request requires a debug value.")

        # TODO: make sure exp_3 and exponent combination is correct
        # EXP_3
        if self.signing_attributes.exponent == 3:
            additions.update({BaseCASSSigningRequest.EXP_3: 'TRUE'})
        else:
            additions.update({BaseCASSSigningRequest.EXP_3: 'FALSE'})

        # Add optional attributes

        # IN_USE_SOC_HW_VERSION
        if self.signing_attributes.in_use_soc_hw_version is not None:
            if self.signing_attributes.in_use_soc_hw_version == 1:
                additions.update({Certificate.SIGNATTR_IN_USE_SOC_HW_VERSION: "TRUE"})
            elif self.signing_attributes.in_use_soc_hw_version == 0:
                additions.update({Certificate.SIGNATTR_IN_USE_SOC_HW_VERSION: "FALSE"})
            else:
                raise RuntimeError("in_use_soc_hw_version value of {0} is invalid for CASS Updated SBL Signing Request.".format(self.signing_attributes.in_use_soc_hw_version))

        # EXPONENT
        if self.signing_attributes.exponent is not None:
            additions.update({BaseCASSSigningRequest.EXPONENT: hex(self.signing_attributes.exponent)})

        # OEM_ID_INDEPENDENT
        if self.signing_attributes.oem_id_independent is not None:
            if self.signing_attributes.oem_id_independent == 1:
                additions.update({Certificate.SIGNATTR_OEM_ID_INDEPENDENT: "TRUE"})
            elif self.signing_attributes.oem_id_independent == 0:
                additions.update({Certificate.SIGNATTR_OEM_ID_INDEPENDENT: "FALSE"})
            else:
                raise RuntimeError("oem_id_independent value of {0} is invalid for CASS Updated SBL Signing Request.".format(self.signing_attributes.oem_id_independent))

        return additions

    def _create_attributes_to_override(self):
        return None

    def _create_attributes_to_remove(self):
        return None
