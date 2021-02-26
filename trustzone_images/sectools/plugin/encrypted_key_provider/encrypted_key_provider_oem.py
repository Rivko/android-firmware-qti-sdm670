# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


"""
Created Dec 10th, 2015 by coryf
"""
import binascii

from sectools.features.isc.encryption_service.unified.base_encrypted_key_provider import BaseEncryptedKeyProvider


class OEMEncryptedKeyProvider(BaseEncryptedKeyProvider):

    """
    This class must implement get_encrypted_l2_key(), get_encrypted_l3_key(), and get_clear_l3_key()
    to return the byte values of the corresponding keys (encrypted L2, encrypted L3, or unencrypted L3).
    The byte values returned by the getters can either be set in the constructor (or a method called by
    the constructor as is the case in this example) or the values can be directly determined and returned
    in the getters. The key values must be available whenever the getters are called.
    """

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "encrypted_key_provider_oem"

    def __init__(self):
        self.l2_key = None
        self.l3_key = None
        self.clear_l3_key = None
        self._set_key_values()

    def _set_key_values(self):
        self.l2_key = binascii.unhexlify("7901234567890abcef012345678900100020000001000300400100000000000000000000000000000000000000000000072ec6994d0c8f45ca0104bd8c8184ce49c0ac09c04404d2fe7483dbdf12d93d")
        self.l3_key = binascii.unhexlify("79000102030405060708090a0b0c00100020000001010000000000000100000000000000000000000000000000000000c7890399b59fa499b4e402095bb004abb515c7ed06d8af3ce28a0a6df01fddfe")
        self.clear_l3_key = binascii.unhexlify("000102030405060708090a0b0c0d0e0f")

    def get_encrypted_l2_key(self):
        """
        :return: byte representation of the encrypted L2 key
        """
        return self.l2_key

    def get_encrypted_l3_key(self):
        """
        :return: byte representation of the encrypted L3 key
        """
        return self.l3_key

    def get_clear_l3_key(self):
        """
        :return: byte representation of the unencrypted L3 key
        """
        return self.clear_l3_key
