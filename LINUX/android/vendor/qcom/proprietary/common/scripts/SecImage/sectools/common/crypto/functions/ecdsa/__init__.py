# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import abc

from sectools.common.crypto import utils
from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_ECDSA


class EcdsaImplDiscovery(FuncImplDiscovery):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_ECDSA

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_OPENSSL
            from sectools.common.crypto.functions.ecdsa.openssl import EcdsaOpenSSLImpl
            modules[MOD_OPENSSL] = EcdsaOpenSSLImpl
        except Exception:
            pass
        return modules


class EcdsaBase(object):

    # Key text constants
    PUB_KEY_BEGIN = '-----BEGIN PUBLIC KEY-----\n'
    PUB_KEY_END = '\n-----END PUBLIC KEY-----\n'

    CURVE_256 = 'secp256r1'
    CURVE_256_PRIME = 'prime256v1'
    CURVE_384 = 'secp384r1'

    curves = [CURVE_256, CURVE_384]

    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def gen_keys(self, curve, priv_key_path=None, pub_key_path=None):
        pass

    @abc.abstractmethod
    def sign(self, message, key):
        pass

    @abc.abstractmethod
    def verify(self, message, key, signature):
        pass

    def get_key_format(self, key):
        if 'BEGIN PRIVATE KEY' in key or 'BEGIN EC PRIVATE KEY' in key:
            return utils.FORMAT_PEM
        else:
            return utils.FORMAT_DER

    @abc.abstractmethod
    def get_key_in_format(self, key, inform, outform, is_public=False):
        pass

    @abc.abstractmethod
    def get_public_key_from_private(self, priv_key):
        pass
