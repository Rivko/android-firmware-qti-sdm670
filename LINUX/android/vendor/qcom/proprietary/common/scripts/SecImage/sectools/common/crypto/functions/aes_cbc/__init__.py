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

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_AES_CBC
from sectools.common.crypto.functions.common import gen_random_data


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

    def __init__(self, module):
        self.module = module

    def gen_key(self, n=16):
        return gen_random_data(n)

    def gen_iv(self, n=16):
        return gen_random_data(n)

    @abc.abstractmethod
    def encrypt(self, message, key, iv):
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, iv):
        pass
