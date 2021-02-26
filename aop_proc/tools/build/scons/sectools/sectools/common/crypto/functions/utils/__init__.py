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

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_UTILS


class UtilsImplDiscovery(FuncImplDiscovery):
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_UTILS

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_OPENSSL
            from sectools.common.crypto.functions.utils.openssl import UtilsOpenSSLImpl
            modules[MOD_OPENSSL] = UtilsOpenSSLImpl
        except Exception:
            pass
        return modules


class UtilsBase(object):
    # Format types of crypto assets
    FORMAT_DER = 'der'
    FORMAT_PEM = 'pem'
    FORMATS_SUPPORTED = [FORMAT_DER, FORMAT_PEM]

    # Hash algorithms supported
    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'

    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def hash(self, message, hash_algo):
        """Returns the hash value of the given message.

        :param message: Message to be hashed.
        :param hash_algo: Hash algorithm to use for hashing. Use HASH_ALGO_* to see supported hash algorithms.

        :return: Hash of the message.
        :rtype: str
        """
        pass
