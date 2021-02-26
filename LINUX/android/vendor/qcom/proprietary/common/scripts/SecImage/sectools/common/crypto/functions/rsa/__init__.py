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
import base64
import subprocess

from sectools.common.crypto import utils
from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_RSA
from sectools.common.crypto.functions.rsa import asn1
from sectools.common.utils.c_data import nice_binary_to_string, \
    nice_string_to_binary, nice_string_to_list, nice_string_to_multiline,\
    nice_hex_reversed


class RsaImplDiscovery(FuncImplDiscovery):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_RSA

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_OPENSSL
            from sectools.common.crypto.functions.rsa.openssl import RsaOpenSSLImpl
            modules[MOD_OPENSSL] = RsaOpenSSLImpl
        except Exception:
            pass
        return modules


class RsaBase(object):

    # Key text constants
    PUB_KEY_BEGIN = '-----BEGIN PUBLIC KEY-----\n'
    PUB_KEY_END = '\n-----END PUBLIC KEY-----\n'

    # Padding schemes
    RSA_PAD_OAEP = 'oaep'
    RSA_PAD_PKCS = 'pkcs'
    RSA_PAD_PSS  = 'pss'
    RSA_PAD_SSL  = 'ssl'
    RSA_PAD_RAW  = 'raw'

    # Minimum padding size
    RSA_PAD_PKCS_SIZE = 11

    # Hashing algorithms
    RSA_HASH_SHA256 = 'sha256'
    RSA_HASH_SHA384 = 'sha384'

    # Salt lengths
    RSA_PAD_PSS_SALT_1 = '-1'
    RSA_PAD_PSS_SALT_20 = '20'

    # Exponents supported
    EXPONENT_3 = 3
    EXPONENT_257 = 257
    EXPONENT_65537 = 65537

    exponents = [EXPONENT_3, EXPONENT_257, EXPONENT_65537]

    # Keysizes supported
    KEYSIZE_1024 = 1024
    KEYSIZE_2048 = 2048
    KEYSIZE_4096 = 4096

    keysizes = [KEYSIZE_1024, KEYSIZE_2048, KEYSIZE_4096]

    def __init__(self, module):
        self.module = module

    @abc.abstractmethod
    def gen_keys(self, exponent, keysize, priv_key_path=None, pub_key_path=None):
        pass

    @abc.abstractmethod
    def sign(self, message, key, padding=RSA_PAD_PSS, hash_algo=RSA_HASH_SHA256,
             salt_len=RSA_PAD_PSS_SALT_1):
        pass

    @abc.abstractmethod
    def verify(self, message, key, signature, padding=RSA_PAD_PSS,
               hash_algo=RSA_HASH_SHA256, salt_len=RSA_PAD_PSS_SALT_1):
        pass

    @abc.abstractmethod
    def encrypt(self, message, key, padding=RSA_PAD_OAEP):
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, padding=RSA_PAD_OAEP):
        pass

    def get_key_format(self, key):
        if 'BEGIN PRIVATE KEY' in key or 'BEGIN RSA PRIVATE KEY' in key:
            return utils.FORMAT_PEM
        else:
            return utils.FORMAT_DER

    @abc.abstractmethod
    def get_key_in_format(self, key, inform, outform, is_public=False):
        pass

    def block_encrypt(self, func, message, key, keylen):
        enclen = (keylen/8) - 11
        enc_message = ''
        while message:
            to_encrypt = message[:enclen]
            message = message[enclen:]
            encrypted = func(to_encrypt, key)
            enc_message += encrypted
        return enc_message

    def block_decrypt(self, func, message, key, keylen):
        declen = (keylen/8)
        dec_message = ''
        while message:
            to_decrypt = message[:declen]
            message = message[declen:]
            decrypted = func(to_decrypt, key)
            dec_message += decrypted
        return dec_message

    @abc.abstractmethod
    def get_public_key_from_private(self, priv_key):
        pass

    @classmethod
    def get_public_key_breakdown(cls, key):
        """Returns the binary key, modulus and exponent from the PEM encoded key.
        """
        # Strip the text part
        if not (key.startswith(cls.PUB_KEY_BEGIN) and
                key.endswith(cls.PUB_KEY_END)):
            raise RuntimeError('Given key is not Public key')
        key = key[len(cls.PUB_KEY_BEGIN):-len(cls.PUB_KEY_END)]

        # Convert to der format
        key_bin = base64.b64decode(key)
        key = nice_binary_to_string(key_bin, sep=' ', length=len(key_bin)).split(' ')

        # Parse the key
        key_objs = asn1.parse(key)

        # Get the modulus & exponent
        modulus = ' '.join(key_objs[0].value[2].value[0].value)
        exponent = ' '.join(key_objs[0].value[2].value[1].value)

        # return the keys in binary
        return (key_bin,
                nice_string_to_binary(modulus, sep=' '),
                nice_string_to_binary(exponent, sep=' '))

    @classmethod
    def get_public_key_from_breakdown(cls, modulus, exponent):
        """Returns the PEM key from the modulus and exponent.
        """
        def con_bin_2_list(val):
            return nice_binary_to_string(val, sep=':', length=len(val)).split(':')

        # Convert to lists
        modulus = con_bin_2_list(modulus)
        exponent = con_bin_2_list(exponent)
        for i, val in enumerate(exponent):
            if int(val, 16) != 0:
                exponent = exponent[i:]
                break

        # Create the modulus, exponent objects
        modulus_obj, exponent_obj = asn1.AsnObject(), asn1.AsnObject()
        modulus_obj.set_data(modulus_obj.TYPE_INT, modulus)
        exponent_obj.set_data(exponent_obj.TYPE_INT, exponent)

        # Create the higher level objects
        tmp2 = asn1.AsnObject()
        tmp2.set_data(modulus_obj.TYPE_SEQ, [modulus_obj, exponent_obj])
        tmp1 = asn1.AsnObject()
        tmp1.set_data(modulus_obj.TYPE_BIT, nice_string_to_list(tmp2.pack()), use_eoc=True)
        tmp01 = asn1.AsnObject()
        tmp01.set_data(modulus_obj.TYPE_NULL, [])
        tmp00 = asn1.AsnObject()
        tmp00.set_data(modulus_obj.TYPE_OID, ['2a', '86', '48', '86', 'f7', '0d', '01', '01', '01'])
        tmp0 = asn1.AsnObject()
        tmp0.set_data(modulus_obj.TYPE_SEQ, [tmp00, tmp01])
        tmp = asn1.AsnObject()
        tmp.set_data(modulus_obj.TYPE_SEQ, [tmp0, tmp1, tmp2])

        # Create the key
        key_list = nice_string_to_list(tmp.pack())
        key_bin = nice_string_to_binary(':'.join(key_list), sep=':')
        return (cls.PUB_KEY_BEGIN +
                nice_string_to_multiline(base64.b64encode(key_bin), 64) +
                cls.PUB_KEY_END)
