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
import base64

from sectools.common.crypto import utils
from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_RSA
from sectools.common.crypto.functions.rsa import asn1
from sectools.common.utils.c_data import nice_binary_to_string, \
    nice_string_to_binary, nice_string_to_list, nice_string_to_multiline


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
    RSA_PAD_PSS = 'pss'
    RSA_PAD_SSL = 'ssl'
    RSA_PAD_RAW = 'raw'

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
        """Generates a RSA key pair.

        :param int exponent: Exponent to use for key generation. Use EXPONENT_* to see supported exponents.
        :param int keysize: Keysize to use for key generation. Use KEYSIZE_* to see supported keysizes.
        :param str|None priv_key_path: Optional path to store the private key.
        :param str|None pub_key_path: Optional path to store the public key.

        :return: Tuple containing the key pair (Private Key, Public Key)
        :rtype: (str, str)
        """
        pass

    @abc.abstractmethod
    def sign(self, message, key, padding=RSA_PAD_PSS, hash_algo=RSA_HASH_SHA256, salt_len=RSA_PAD_PSS_SALT_1):
        """Signs the message using the provided key.

        :param str message: Message to be signed.
        :param str key: Key to be used for signing.
        :param str padding: Padding to use for signing. Use RSA_PAD_* to see supported paddings.
        :param str hash_algo: Hash algorithms to specify for signing. Use RSA_HASH_* to see supported hash algorithms.
                              [Needed for PSS padding]
        :param str salt_len: Salt length to use. Use RSA_PAD_PSS_SALT_* to see supported hash algorithms.
                             [Needed for PSS padding]

        :return: Signature
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def verify(self, message, key, signature, padding=RSA_PAD_PSS, hash_algo=RSA_HASH_SHA256,
               salt_len=RSA_PAD_PSS_SALT_1):
        """Authenticates the message against the signature using the provided key.

        :param str message: Message to be authenticated.
        :param str key: Key to be used for authentication.
        :param str signature: Signature against which the message should be authenticated.
        :param str padding: Padding to use for signing. Use RSA_PAD_* to see supported paddings.
        :param str hash_algo: Hash algorithms to specify for signing. Use RSA_HASH_* to see supported hash algorithms.
                              [Needed for PSS padding]
        :param str salt_len: Salt length to use. Use RSA_PAD_PSS_SALT_* to see supported hash algorithms.
                             [Needed for PSS padding]

        :return: If message matches the signature.
        :rtype: bool
        """
        pass

    @abc.abstractmethod
    def encrypt(self, message, key, padding=RSA_PAD_OAEP):
        """Encrypts the message using the given key.

        :param str message: Message to be encrypted.
        :param str key: Encryption key.
        :param str padding: The padding to use for encryption.

        :return: Encrypted message
        :rtype: str
        """
        pass

    @abc.abstractmethod
    def decrypt(self, message, key, padding=RSA_PAD_OAEP):
        """Decrypts the message using the given key.

        :param str message: Encrypted message.
        :param str key: Decryption key.
        :param str padding: The padding to use for decryption.

        :return: Decrypted message
        :rtype: str
        """
        pass

    @classmethod
    def get_key_format(cls, key):
        """Returns the format of the key.

        :param key: Key to get format of.

        :return: Format of the key.
        :rtype: str
        """
        if ('BEGIN PRIVATE KEY' in key or 'BEGIN RSA PRIVATE KEY' in key or
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
    def get_text_from_key(self, key, inform, is_public=False):
        """Returns the text output of the key.

        :param str key: Key to get text from.
        :param str inform: Format of the key.
        :param bool is_public: If key is public.

        :return: Text output of key
        :rtype: str
        """
        pass

    @classmethod
    def block_encrypt(cls, func, message, key, keylen):
        """Encrypts the message using the given function and key. Keylen is used to break the message in chunks if
        needed.

        :param func: Function to use for encryption. It should take two params: message, key. Use lambda if needed.
        :param str message: Message to be encrypted.
        :param str key: Key to be used for encryption.
        :param int keylen: Length of the key.

        :return: Encrypted message
        :rtype: str
        """
        enclen = (keylen / 8) - 11
        enc_message = ''
        while message:
            to_encrypt = message[:enclen]
            message = message[enclen:]
            encrypted = func(to_encrypt, key)
            enc_message += encrypted
        return enc_message

    @classmethod
    def block_decrypt(cls, func, message, key, keylen):
        """Decrypts the message using the given function and key. Keylen is used to break the message in chunks if
        needed.

        :param func: Function to use for decryption. It should take two params: message, key. Use lambda if needed.
        :param str message: Message to be decrypted.
        :param str key: Key to be used for decryption.
        :param int keylen: Length of the key.

        :return: Decrypted message
        :rtype: str
        """
        declen = (keylen / 8)
        dec_message = ''
        while message:
            to_decrypt = message[:declen]
            message = message[declen:]
            decrypted = func(to_decrypt, key)
            dec_message += decrypted
        return dec_message

    @abc.abstractmethod
    def get_public_key_from_private(self, priv_key):
        """Returns the public key from the private key.

        :param priv_key: Private key.

        :return: Public key
        :rtype: str
        """
        pass

    @classmethod
    def get_public_key_breakdown(cls, key):
        """Returns the breakdown of the given public key.

        :param str key: PEM encoded RSA public key.

        :return: Tuple of binary key, modulus and exponent.
        :rtype: (str, str, str)
        """
        # Strip the text part
        if not (key.startswith(cls.PUB_KEY_BEGIN) and key.endswith(cls.PUB_KEY_END)):
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
        """Returns the public key created using the modulus and exponent.

        :param str modulus: Modulus string.
        :param str exponent: Exponent string.

        :return: PEM encoded RSA public key.
        :rtype: str
        """

        def con_bin_2_list(v):
            return nice_binary_to_string(v, sep=':', length=len(v)).split(':')

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
