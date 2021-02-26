# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Oct 26, 2014

@author: hraghav
'''

from sectools.common.crypto import utils
from sectools.common.crypto.functions.rsa import RsaBase
from sectools.common.crypto.functions.common import run_command
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger


OPENSSL_RSAUTL_MODE = 'rsautl'
OPENSSL_PKEYUTL_MODE = 'pkeyutl'
OPENSSL_RSA_MODE = 'rsa'


class RsaOpenSSLImpl(RsaBase):

    exponents_map = \
    {
        RsaBase.EXPONENT_3      : '3',
        RsaBase.EXPONENT_257    : '257',
        RsaBase.EXPONENT_65537  : '65537',
    }

    keysizes_map = \
    {
        RsaBase.KEYSIZE_1024    : '1024',
        RsaBase.KEYSIZE_2048    : '2048',
        RsaBase.KEYSIZE_4096    : '4096',
    }

    paddings_map = \
    {
        RsaBase.RSA_PAD_OAEP    : 'oaep',
        RsaBase.RSA_PAD_PKCS    : 'pkcs1',
        RsaBase.RSA_PAD_PSS     : 'pss',
    }

    rsautil_paddings_map = \
    {
        RsaBase.RSA_PAD_OAEP    : 'oaep',
        RsaBase.RSA_PAD_PKCS    : 'pkcs',
        RsaBase.RSA_PAD_SSL     : 'ssl',
        RsaBase.RSA_PAD_RAW     : 'raw'
    }

    hash_map = \
    {
        RsaBase.RSA_HASH_SHA256 : 'sha256',
        RsaBase.RSA_HASH_SHA384 : 'sha384',
    }

    formats_map = \
    {
        utils.FORMAT_DER       : 'DER',
        utils.FORMAT_PEM       : 'PEM',
    }

    def __init__(self, module):
        self.openssl = module

    def gen_keys(self, exponent, keysize, priv_key_path=None, pub_key_path=None):
        """ Generate RSA Key pair

            input:

            key_size_in_bits: Size of the key in bits.
            key_exponent: [3, 65537]
                          Exponent used in key calculation.
            priv_key_path: File name for storing private key
            pub_key_path: File name for storing public key

            output:

            returned value: {"public_key": [Generated public key],
                             "private_key": [Generated private key] }
                             Dictionary holding the values of public and private keys
        """
        # Generate the key pair using openssl
        priv_key = self._gen_rsa_key_pair(exponent, keysize)

        # Generate the public key file
        try:
            # Save the private key to file
            priv_path = priv_key_path if priv_key_path is not None else c_path.create_tmp_file()
            logger.debug('Writing generated private key to PEM file: ' + priv_path)
            with open(priv_path, 'wb') as fd:
                fd.write(priv_key)

            # Extract the public key from the private key
            pub_key = self._get_pub_from_priv(priv_path)
        finally:
            # Cleanup temp file
            if priv_key_path is None:
                c_path.remove_tmp_file(priv_path)

        # Save the public key to file
        if pub_key_path is not None:
            logger.debug('Writing generated public key to PEM file: ' + pub_key_path)
            with open(pub_key_path, 'wb') as fd:
                fd.write(pub_key)

        return (priv_key, pub_key)

    def sign(self, message, key, padding=RsaBase.RSA_PAD_PSS,
             hash_algo=RsaBase.RSA_HASH_SHA256,
             salt_len=RsaBase.RSA_PAD_PSS_SALT_1):
        """ Encrypt a message with a private key
        input:
            message: String representing message
            private_key: String representing the private key

        output:
            signature: String representing encrypted message
        """
        padding_opt = self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)
        hash_opt = self._get_hash_opt(hash_algo)
        pad_opts = []
        if padding == RsaBase.RSA_PAD_PSS:
            pad_opts += ['-pkeyopt', 'rsa_pss_saltlen:' + salt_len,
                         '-pkeyopt', 'digest:' + hash_opt]
        return self._pkeyutl(message, key, ['-sign'],
                             ['-pkeyopt', 'rsa_padding_mode:'+padding_opt] + pad_opts)

    def verify(self, message, key, signature, padding=RsaBase.RSA_PAD_PSS,
               hash_algo=RsaBase.RSA_HASH_SHA256,
               salt_len=RsaBase.RSA_PAD_PSS_SALT_1):
        """ Decrypt an encrypted message with a public key
        input:
            encrypted_message = String representation of encrypted message
            public_key = String representation of public key

        output:
            message = String representing decrypted message
        """
        padding_opt = self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)
        hash_opt = self._get_hash_opt(hash_algo)
        pad_opts = []
        signature_path = None
        if padding == RsaBase.RSA_PAD_PSS:
            signature_path = c_path.create_tmp_file(signature)
            pad_opts += ['-sigfile', signature_path,
                         '-pkeyopt', 'rsa_pss_saltlen:' + salt_len,
                         '-pkeyopt', 'digest:' + hash_opt]
        try:
            output = self._pkeyutl(message, key, ['-verify', '-pubin'],
                                   ['-pkeyopt', 'rsa_padding_mode:'+padding_opt],
                                   expected_retcode=1)
            if output.strip() == 'Signature Verified Successfully':
                return True
            else:
                return False
        finally:
            if signature_path is not None:
                c_path.remove_tmp_file(signature_path)

    def encrypt(self, message, key, padding=RsaBase.RSA_PAD_OAEP):
        """ Decrypt an encrypted message with a private key
        input:
            encrypted_message = String representation of encrypted message
            private_key = String representation of private key

        output:
            message = String representing decrypted message
        """
        padding_opt = self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)
        return self._rsautl(message, key, ['-encrypt', '-pubin'],
                            ['-'+padding_opt])

    def decrypt(self, message, key, padding=RsaBase.RSA_PAD_OAEP):
        """ Decrypt an encrypted message with a private key
        input:
            encrypted_message = String representation of encrypted message
            private_key = String representation of private key

        output:
            message = String representing decrypted message
        """
        padding_opt = self._get_pad_opt(OPENSSL_RSAUTL_MODE, padding)
        return self._rsautl(message, key, ['-decrypt'],
                            ['-'+padding_opt])

    def get_key_in_format(self, key, inform, outform, is_public=False):
        inform_opt = self._get_format_opt(inform)
        outform_opt = self._get_format_opt(outform)
        key_path = c_path.create_tmp_file(key)

        try:
            cmd = [self.openssl, OPENSSL_RSA_MODE,
                   '-in', key_path,
                   '-inform', inform_opt,
                   '-outform', outform_opt,]
            if is_public:
                cmd.append('-pubin')
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(key_path)

    def get_public_key_from_private(self, priv_key):
        key_path = c_path.create_tmp_file(priv_key)
        try:
            return self._get_pub_from_priv(key_path)
        finally:
            c_path.remove_tmp_file(key_path)

    def _get_opts(self, exponent, keysize):
        try:
            exponent_opt = self.exponents_map[exponent]
        except KeyError:
            raise RuntimeError('Exponent: ' + str(exponent) + ' is not supported.')
        try:
            keysize_opt = self.keysizes_map[keysize]
        except KeyError:
            raise RuntimeError('Keysize: ' + str(keysize) + ' is not supported.')
        return exponent_opt, keysize_opt

    def _get_pad_opt(self, mode, padding):
        try:
            if mode == OPENSSL_RSAUTL_MODE:
                padding_opt = self.rsautil_paddings_map[padding]
            else:
                padding_opt = self.paddings_map[padding]
        except KeyError:
            raise RuntimeError('Padding: ' + str(padding) + ' is not supported.')
        return padding_opt

    def _get_hash_opt(self, algo):
        try:
            hash_opt = self.hash_map[algo]
        except KeyError:
            raise RuntimeError('Hash algorithm: ' + str(algo) + ' is not supported.')
        return hash_opt

    def _get_format_opt(self, form):
        try:
            form_opt = self.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')
        return form_opt

    def _gen_rsa_key_pair(self, exponent, keysize):
        exponent_opt, keysize_opt = self._get_opts(exponent, keysize)
        cmd = [self.openssl, 'genpkey', '-algorithm', 'RSA', '-outform', 'PEM',
               '-pkeyopt', 'rsa_keygen_bits:' + keysize_opt,
               '-pkeyopt', 'rsa_keygen_pubexp:' + exponent_opt]
        return run_command(cmd)

    def _get_pub_from_priv(self, priv_path):
        cmd = [self.openssl, 'rsa', '-in', priv_path, '-pubout']
        return run_command(cmd)

    def _pkeyutl(self, message, key, preopts, options,
                    expected_retcode=0):
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        cmd = ([self.openssl, OPENSSL_PKEYUTL_MODE] + preopts +
               ['-inkey', key_path, '-in', message_path] + options)
        try:
            return run_command(cmd, expected_retcode)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)

    def _rsautl(self, message, key, preopts, options,
                expected_retcode=0):
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        cmd = ([self.openssl, OPENSSL_RSAUTL_MODE] + preopts +
               ['-inkey', key_path, '-in', message_path] + options)
        try:
            return run_command(cmd, expected_retcode)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)
