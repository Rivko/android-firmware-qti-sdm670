# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
Created on Oct 25, 2014

@author: hraghav
"""

import abc

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_AES_CCM
from sectools.common.utils.c_data import gen_random_data


class AesCcmImplDiscovery(FuncImplDiscovery):
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_AES_CCM

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_CRYPTO_CCM
            from sectools.common.crypto.functions.aes_ccm.crypto_ccm import AesCcmCryptoCcmImpl
            modules[MOD_CRYPTO_CCM] = AesCcmCryptoCcmImpl
        except Exception:
            pass
        return modules


class AesCcmBase(object):
    def __init__(self, module):
        self.module = module

    @classmethod
    def gen_key(cls, n=16):
        """Returns a random key of size specified by n.

        :param int n: Number of bytes

        :return: Key
        :rtype: str
        """
        return gen_random_data(n)

    @classmethod
    def gen_iv(cls, n=13):
        """Returns a random IV of size specified by n.

        :param int n: Number of bytes

        :return: IV
        :rtype: str
        """
        return gen_random_data(n)

    @abc.abstractmethod
    def encrypt(self, message, key, iv, aad):
        """Encrypt the message (binary blob) using the given key, iv and aad.

        :param str message: Binary blob to encrypt
        :param str key: Key for encryption
        :param str iv: IV for encryption
        :param str aad: AAD for encryption

        :return: Encrypted message
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, iv, aad):
        """Decrypt the encrypted message (binary blob) using the given key, iv and aad.

        :param str message: Encrypted binary blob to decrypt
        :param str key: Key for decryption
        :param str iv: IV for decryption
        :param str aad: AAD for decryption

        :return: Decrypted message
        :rtype: str
        """
        pass
