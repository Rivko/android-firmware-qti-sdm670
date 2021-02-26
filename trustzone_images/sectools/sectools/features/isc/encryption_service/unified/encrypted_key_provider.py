# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created Nov 20th, 2015 by coryf
"""

import binascii
import os

from sectools.common.rpc.http import HTTPClient
from sectools.common.utils.c_logging import logger
from sectools.features.isc.encryption_service.unified.base_encrypted_key_provider import BaseEncryptedKeyProvider

CUR_DIR = os.path.dirname(os.path.abspath(__file__))


class ServerEncryptedKeyProvider(BaseEncryptedKeyProvider):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "UIE_QTI"

    def __init__(self, uie_capability, server_url, server_cert_path):
        self.l2_key = None
        self.l3_key = None
        self.clear_l3_key = None
        self._process_server_response(self._get_server_response(uie_capability, server_url, server_cert_path))

    @staticmethod
    def _get_server_response(uie_capability, server_url, server_cert_path):

        if not uie_capability:
            raise RuntimeError("UIE_capability was not set.")
        if not server_url:
            raise RuntimeError("UIE_server_url was not set.")

        url = server_url + uie_capability
        logger.info("Attempting to connect to UIE server...")
        response = HTTPClient.query_server(url, cert_file=server_cert_path)
        if "error" in response["uie"]:
            raise RuntimeError("Encrypted Key Provider returned error code: " + response["uie"]["error"]["code"] + ".\n" + response["uie"]["error"]["message"])
        else:
            return response

    def _process_server_response(self, response):
        self.l2_key = binascii.unhexlify(response["uie"]["result"]["l2Wrapped"])
        self.l3_key = binascii.unhexlify(response["uie"]["result"]["l3Wrapped"])
        self.clear_l3_key = binascii.unhexlify(response["uie"]["result"]["l3"])

    def get_encrypted_l2_key(self):
        return self.l2_key

    def get_encrypted_l3_key(self):
        return self.l3_key

    def get_clear_l3_key(self):
        return self.clear_l3_key

    @classmethod
    def is_qti(cls):
        return True
