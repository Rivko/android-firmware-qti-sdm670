# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc

from sectools.common.crypto.functions import FuncImplDiscovery

function_name = "eciesad"


class EciesImplDiscovery(FuncImplDiscovery):
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return function_name

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_ECIESAD
            from sectools.common.crypto.functions.eciesad.ecies import EciesImpl
            modules[MOD_ECIESAD] = EciesImpl
        except Exception:
            pass
        return modules


class EciesBase(object):
    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def encrypt(self, key, aad, message):
        """Encrypt the message (binary blob) using the given key and aad.

        :param str key: Public key for encryption
        :param str aad: AAD for encryption
        :param str message: Binary blob to encrypt

        :return: Cryptogram
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def decrypt(self, key, cryptogram):
        """Decrypt the encrypted message (binary blob) using the given key and aad.

        :param str key: Private key for decryption
        :param str cryptogram: Cryptogram containing encrypted binary blob to decrypt

        :return: Decrypted message
        :rtype: str
        """
        pass
