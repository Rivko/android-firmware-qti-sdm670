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

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_AES_CBC
from sectools.common.utils.c_data import gen_random_data


class AesCbcImplDiscovery(FuncImplDiscovery):
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_AES_CBC

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_OPENSSL
            from sectools.common.crypto.functions.aes_cbc.openssl import AesCbcOpenSSLImpl
            modules[MOD_OPENSSL] = AesCbcOpenSSLImpl
        except Exception:
            pass
        return modules


class AesCbcBase(object):

    CIPHERNAME_128 = '-aes-128-cbc'
    CIPHERNAME_192 = '-aes-192-cbc'
    CIPHERNAME_256 = '-aes-256-cbc'
    CIPHERNAMES = [CIPHERNAME_128, CIPHERNAME_192, CIPHERNAME_256]

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
    def gen_iv(cls, n=16):
        """Returns a random IV of size specified by n.

        :param int n: Number of bytes

        :return: IV
        :rtype: str
        """
        return gen_random_data(n)

    @abc.abstractmethod
    def encrypt(self, message, key, iv, ciphername=CIPHERNAME_128):
        """Encrypt the message (binary blob) using the given key and iv.

        :param str message: Binary blob to encrypt
        :param str key: Key for encryption
        :param str iv: IV for encryption
        :param str ciphername: Cipher to use for encryption, either '-aes-128-cbc', '-aes-192-cbc', or '-aes-256-cbc'

        :return: Encrypted message
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, iv, ciphername=CIPHERNAME_128):
        """Decrypt the encrypted message (binary blob) using the given key and iv.

        :param str message: Encrypted binary blob to decrypt
        :param str key: Key for decryption
        :param str iv: IV for decryption
        :param str ciphername: Cipher to use for decryption, either '-aes-128-cbc', '-aes-192-cbc', or '-aes-256-cbc'

        :return: Decrypted message
        :rtype: str
        """
        pass
