# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import __secfile__
__secfile__.init()

from sectools.common.core.plugin import SecPluginMgr
from sectools.common.utils.c_logging import logger


def get_signing_request_class(signing_request_class_id):
    return CASS_signing_request_mgr.get_signing_request_class(signing_request_class_id)


class CASSSigningRequestMgr(SecPluginMgr):

    def get_signing_request_class(self, cass_capability):
        logger.debug("CASS capability set to \"{0}\"".format(cass_capability))
        signing_requests = self.get_map()
        supported_capabilities = list()
        # search for a signing request that supports the specified cass capability
        for signing_request_id, signing_request_class in signing_requests.iteritems():
            supported_capabilities.extend(signing_request_class.get_supported_capabilities())
            if cass_capability in signing_request_class.get_supported_capabilities():
                return signing_request_class
        # no signing request was found that supports the specified cass capability so default to secure boot 2 signing request
        logger.debug("\"{0}\" is not a pre-configured CASS capability.\n".format(cass_capability) +
                     "The pre-configured CASS capabilities are: " + ", ".join(supported_capabilities))
        logger.info("Using CASS Secure Boot 2 signing request for CASS capability \"{0}\"".format(cass_capability))
        return signing_request_secure_boot_2.SecureBoot2SigningRequest

CASS_signing_request_mgr = CASSSigningRequestMgr()

import sectools.features.isc.signer.cass.signing_request_oid
import sectools.features.isc.signer.cass.signing_request_sbl
import sectools.features.isc.signer.cass.signing_request_sbl_updated
import sectools.features.isc.signer.cass.signing_request_secure_boot_2
