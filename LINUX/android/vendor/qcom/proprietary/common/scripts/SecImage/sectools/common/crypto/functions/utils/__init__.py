# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Oct 25, 2014

@author: hraghav
'''

import abc
import subprocess

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

    FORMAT_DER = 'der'
    FORMAT_PEM = 'pem'

    FORMATS_SUPPORTED = [FORMAT_DER, FORMAT_PEM]

    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'

    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def hash(self, hashing_algorithm, file_to_hash):
        pass


