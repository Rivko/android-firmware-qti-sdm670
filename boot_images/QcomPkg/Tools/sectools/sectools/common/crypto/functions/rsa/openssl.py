# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
Created on Oct 26, 2014

@author: hraghav
"""

from sectools.common.crypto import utils
from sectools.common.crypto.functions.rsa import RsaBase
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_process import run_command

OPENSSL_RSAUTL_MODE = 'rsautl'
OPENSSL_PKEYUTL_MODE = 'pkeyutl'
OPENSSL_RSA_MODE = 'rsa'


class RsaOpenSSLImpl(RsaBase):

    exponents_map = {
        RsaBase.EXPONENT_3: '3',
        RsaBase.EXPONENT_257: '257',
        RsaBase.EXPONENT_65537: '65537',
    }

    keysizes_map = {
        RsaBase.KEYSIZE_1024: '1024',
        RsaBase.KEYSIZE_2048: '2048',
        RsaBase.KEYSIZE_4096: '4096',
    }

    paddings_map = {
        RsaBase.RSA_PAD_OAEP: 'oaep',
        RsaBase.RSA_PAD_PKCS: 'pkcs1',
        RsaBase.RSA_PAD_PSS: 'pss',
    }

    rsautil_paddings_map = {
        RsaBase.RSA_PAD_OAEP: 'oaep',
        RsaBase.RSA_PAD_PKCS: 'pkcs',
        RsaBase.RSA_PAD_SSL: 'ssl',
        RsaBase.RSA_PAD_RAW: 'raw'
    }

    hash_map = {
        RsaBase.RSA_HASH_SHA256: 'sha256',
        RsaBase.RSA_HASH_SHA384: 'sha384',
    }

    formats_map = {
        utils.FORMAT_DER: 'DER',
        utils.FORMAT_PEM: 'PEM',
    }

    def __init__(self, module):
        super(RsaOpenSSLImpl, self).__init__(module)
        self.openssl = module

    def gen_keys(self, exponent, keysize, priv_key_path=None, pub_key_path=None):
        # Generate the key pair using openssl
        priv_key = self._gen_rsa_key_pair(exponent, keysize)

        # Save the private key to file
        priv_path = priv_key_path if priv_key_path is not None else c_path.create_tmp_file()

        # Generate the public key file
        try:
            logger.debug('Writing generated private key to PEM file: ' + priv_path)
            c_path.store_data_to_file(priv_path, priv_key)

            # Extract the public key from the private key
            pub_key = self.get_public_key_from_private(priv_key)
        finally:
            # Cleanup temp file
            c_path.remove_tmp_file(priv_path)

        # Save the public key to file
        if pub_key_path is not None:
            logger.debug('Writing generated public key to PEM file: ' + pub_key_path)
            c_path.store_data_to_file(pub_key_path, pub_key)

        return priv_key, pub_key

    def sign(self, message, key, padding=RsaBase.RSA_PAD_PSS, hash_algo=RsaBase.RSA_HASH_SHA256,
             salt_len=RsaBase.RSA_PAD_PSS_SALT_1):
        # Create options for the rsa operation
        pad_opts = ['-pkeyopt', 'rsa_padding_mode:' + self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)]
        if padding == self.RSA_PAD_PSS:
            pad_opts += ['-pkeyopt', 'rsa_pss_saltlen:' + salt_len,
                         '-pkeyopt', 'digest:' + self._get_hash_opt(hash_algo)]

        # Return signature
        return self._pkeyutl(message, key, ['-sign'], pad_opts)

    def verify(self, message, key, signature, padding=RsaBase.RSA_PAD_PSS, hash_algo=RsaBase.RSA_HASH_SHA256,
               salt_len=RsaBase.RSA_PAD_PSS_SALT_1):
        # Create options for the rsa operation
        pad_opts = ['-pkeyopt', 'rsa_padding_mode:' + self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)]
        if padding == self.RSA_PAD_PSS:
            hash_opt = self._get_hash_opt(hash_algo)
            pad_opts += ['-pkeyopt', 'rsa_pss_saltlen:' + salt_len,
                         '-pkeyopt', 'digest:' + hash_opt]

        signature_path = c_path.create_tmp_file(signature)
        pad_opts = ['-sigfile', signature_path] + pad_opts

        # Return if message matches signature
        try:
            output = self._pkeyutl(message, key, ['-verify', '-pubin'], pad_opts, expected_retcode=1)
            return 'Signature Verified Successfully' in output
        finally:
            c_path.remove_tmp_file(signature_path)

    def encrypt(self, message, key, padding=RsaBase.RSA_PAD_OAEP):
        return self._rsautl(message, key, ['-encrypt', '-pubin'],
                            ['-' + self._get_pad_opt(OPENSSL_PKEYUTL_MODE, padding)])

    def decrypt(self, message, key, padding=RsaBase.RSA_PAD_OAEP):
        return self._rsautl(message, key, ['-decrypt'], ['-' + self._get_pad_opt(OPENSSL_RSAUTL_MODE, padding)])

    def get_key_in_format(self, key, outform, is_public=False):
        # Avoid error by reconverting
        inform = self.get_key_format(key)
        if inform == outform:
            return key

        key_path = c_path.create_tmp_file(key)
        try:
            cmd = [self.openssl, OPENSSL_RSA_MODE,
                   '-in', key_path,
                   '-inform', self._get_format_opt(inform),
                   '-outform', self._get_format_opt(outform)]
            if is_public:
                cmd.append('-pubin')
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(key_path)

    def get_public_key_from_private(self, priv_key):
        key_path = c_path.create_tmp_file(priv_key)
        try:
            cmd = [self.openssl, OPENSSL_RSA_MODE, '-in', key_path, '-pubout']
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(key_path)

    def get_text_from_key(self, key, inform, is_public=False):
        key_path = c_path.create_tmp_file(key)
        try:
            cmd = [self.openssl, 'rsa', '-text', '-noout',
                   '-in', key_path,
                   '-inform', self._get_format_opt(inform)]
            if is_public:
                cmd.append('-pubin')
            return run_command(cmd)
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
            return self.rsautil_paddings_map[padding] if mode == OPENSSL_RSAUTL_MODE else self.paddings_map[padding]
        except KeyError:
            raise RuntimeError('Padding: ' + str(padding) + ' is not supported.')

    def _get_hash_opt(self, algo):
        try:
            hash_opt = self.hash_map[algo]
        except KeyError:
            raise RuntimeError('Hash algorithm: ' + str(algo) + ' is not supported.')
        return hash_opt

    def _get_format_opt(self, form):
        try:
            return self.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')

    def _gen_rsa_key_pair(self, exponent, keysize):
        exponent_opt, keysize_opt = self._get_opts(exponent, keysize)
        cmd = [self.openssl, 'genpkey',
               '-algorithm', 'RSA',
               '-outform', 'PEM',
               '-pkeyopt', 'rsa_keygen_bits:' + keysize_opt,
               '-pkeyopt', 'rsa_keygen_pubexp:' + exponent_opt]
        return run_command(cmd)

    def _pkeyutl(self, message, key, preopts, options, expected_retcode=0):
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        cmd = ([self.openssl, OPENSSL_PKEYUTL_MODE] + preopts +
               ['-inkey', key_path, '-in', message_path] + options)
        try:
            return run_command(cmd, expected_retcode)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)

    def _rsautl(self, message, key, preopts, options, expected_retcode=0):
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        cmd = ([self.openssl, OPENSSL_RSAUTL_MODE] + preopts +
               ['-inkey', key_path, '-in', message_path] + options)
        try:
            return run_command(cmd, expected_retcode)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)
