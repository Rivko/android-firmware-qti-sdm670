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

from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_AES_CBC_CTS
from sectools.common.crypto.functions.common import gen_random_data


class AesCbcCtsImplDiscovery(FuncImplDiscovery):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_AES_CBC_CTS

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_CRYPTO_CBC
            from sectools.common.crypto.functions.aes_cbc_cts.crypto_cbc import AesCbcCtsCryptoCbcImpl
            modules[MOD_CRYPTO_CBC] = AesCbcCtsCryptoCbcImpl
        except Exception:
            pass
        return modules


class AesCbcCtsBase(object):

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
