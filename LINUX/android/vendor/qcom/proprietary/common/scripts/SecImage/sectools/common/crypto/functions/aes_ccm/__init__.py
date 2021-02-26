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

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_AES_CCM
from sectools.common.crypto.functions.common import gen_random_data


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

    def gen_key(self, n=16):
        return gen_random_data(n)

    def gen_iv(self, n=13):
        return gen_random_data(n)

    @abc.abstractmethod
    def encrypt(self, message, key, iv, aad):
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, iv, aad):
        pass

