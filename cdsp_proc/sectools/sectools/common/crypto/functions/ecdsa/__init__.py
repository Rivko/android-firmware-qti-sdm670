# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
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

    # Curve options
    CURVE_256 = 'secp256r1'
    CURVE_256_PRIME = 'prime256v1'
    CURVE_384 = 'secp384r1'

    curves = [CURVE_256, CURVE_384]

    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def gen_keys(self, curve, priv_key_path=None, pub_key_path=None):
        """Generates a ECDSA key pair.

        :param str curve: Curve to use for key generation. Use CURVE_* to see supported exponents.
        :param str|None priv_key_path: Optional path to store the private key.
        :param str|None pub_key_path: Optional path to store the public key.

        :return: Tuple containing the key pair (Private Key, Public Key)
        :rtype: (str, str)
        """
        pass

    @abc.abstractmethod
    def sign(self, message, key):
        """Signs the message using the provided key.

        :param str message: Message to be signed.
        :param str key: Key to be used for signing.

        :return: Signature
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def verify(self, message, key, signature):
        """Authenticates the message against the signature using the provided key.

        :param str message: Message to be authenticated.
        :param str key: Key to be used for authentication.
        :param str signature: Signature against which the message should be authenticated.

        :return: If message matches the signature.
        :rtype: bool
        """
        pass

    @classmethod
    def get_key_format(cls, key):
        """Returns the format of the key.

        :param key: Key to get format of.

        :return: Format of the key.
        :rtype: str
        """
        if ('BEGIN PRIVATE KEY' in key or 'BEGIN EC PRIVATE KEY' in key or
                'BEGIN PUBLIC KEY' in key):
            return utils.FORMAT_PEM
        else:
            return utils.FORMAT_DER

    @abc.abstractmethod
    def get_key_in_format(self, key, outform, is_public=False):
        """Returns the key in the format specified.

        :param str key: Key to convert.
        :param str outform: Format to convert the key in.
        :param bool is_public: If key is public.

        :return: Key in the format specified
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def get_public_key_from_private(self, priv_key):
        """Returns the public key from the private key.

        :param priv_key: Private key.

        :return: Public key
        :rtype: str
        """
        pass
