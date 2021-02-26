# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from . import __secfile__
__secfile__.init()

from sectools.common.core.plugin import SecPluginMgr
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import extract_image_id_from_sw_id
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0, SP_MCP_SW_ID, SP_APPS_SW_ID


def get_signing_request_class(signing_request_class_id, secboot_version, sw_id):
    return CASS_signing_request_mgr.get_signing_request_class(signing_request_class_id, secboot_version, sw_id)


class CASSSigningRequestMgr(SecPluginMgr):

    def get_signing_request_class(self, cass_capability, secboot_version, sw_id):
        logger.debug("CASS capability set to \"{0}\"".format(cass_capability))
        signing_requests = self.get_map()
        supported_capabilities = list()
        secboot_version_signing_request_id_map = {}
        # search for a signing request that supports the specified cass capability
        for signing_request_id, signing_request_class in signing_requests.items():
            supported_capabilities.extend(signing_request_class.get_supported_capabilities())
            secboot_version_signing_request_id_map[signing_request_class.get_secboot_version()] = signing_request_id
            if cass_capability in signing_request_class.get_supported_capabilities():
                return signing_request_class

        # no signing request was found that supports the specified cass capability so print supported capabilities
        logger.debug("\"{0}\" is not a pre-configured CASS capability\n".format(cass_capability) +
                "The pre-configured CASS capabilities are: " + ", ".join(supported_capabilities))
        # For MBNv3 SPSS images use SBL2 Engine
        img_id = extract_image_id_from_sw_id(sw_id)
        if (img_id == SP_MCP_SW_ID or img_id == SP_APPS_SW_ID) and secboot_version == SECBOOT_VERSION_1_0:
            secboot_version = SECBOOT_VERSION_2_0
        logger.info("Using CASS Secure Boot {0} signing request for CASS capability \"{1}\"".format(secboot_version,
                                                                                                    cass_capability))
        try:
            return signing_requests[secboot_version_signing_request_id_map[secboot_version]]
        except KeyError:
            raise RuntimeError("CASS Signer does not support Secboot version {0}".format(secboot_version))

CASS_signing_request_mgr = CASSSigningRequestMgr()

import sectools.features.isc.signer.cass.signing_request_oid
import sectools.features.isc.signer.cass.signing_request_sbl
import sectools.features.isc.signer.cass.signing_request_secure_boot_2
import sectools.features.isc.signer.cass.signing_request_secure_boot_3
