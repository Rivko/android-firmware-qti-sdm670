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
        self.l2_key = binascii.unhexlify("dee0dbf81487f41444dc2b469c78b899a69787771ded67bd5de15a6b9de9466e")
        self.l3_key = binascii.unhexlify("bdda31cdc6388d5b331702d042abb7988288d9219436529bdfa101c457ce7f51")
        self.clear_l3_key = binascii.unhexlify("482db58ab50d87f1836354d79da012b5")

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
