# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import base64
import os

from sectools.common.utils import c_misc
from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.base_signer import ExternalSignerError

BASE64_ENCODING = "Base64"


class SignatureResponse(object):
    def __init__(self, signature_response_root, sequenceId, encoding, signature_result_file):
        self.sequenceId = sequenceId
        self.root = signature_response_root
        self.encoding = encoding
        self.signature_result_file = signature_result_file

        if self.encoding != BASE64_ENCODING:
            raise RuntimeError("Encoding {0} is not supported by signature response.".format(encoding))

        self.status_code = self._get_tag("StatusCode").text
        logger.debug("Status returned from signature response: {0}".format(self.status_code))

    def save(self):
        if self.status_code != "1":
            raise ExternalSignerError("Error returned from signature response:\n{0}".format(self._get_tag("Error").text))
        result = self._get_result()
        path, f = os.path.split(self.signature_result_file)
        if not os.path.exists(path):
            os.makedirs(path)
        c_misc.store_data_to_file(self.signature_result_file, result)

    def _get_result(self):
        result_node = self._get_tag("Result")
        if result_node.attrib['isEncoded'].lower() == 'true':
            result = base64.b64decode(result_node.text)
        else:
            result = result_node.text
        return result

    def _get_tag(self, tag):
        node = self.root.find(tag)
        if node is None:
            raise RuntimeError("{0} is missing from signature response.".format(tag))
        return node
