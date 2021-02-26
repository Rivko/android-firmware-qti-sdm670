# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on March 11, 2016

@author: coryf
'''

from sectools.common.crypto import utils
from sectools.common.crypto.functions.common import run_command
from sectools.common.crypto.functions.ecdsa import EcdsaBase
from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger

OPENSSL_PKEYUTL_MODE = 'pkeyutl'


class EcdsaOpenSSLImpl(EcdsaBase):

    formats_map = \
        {
            utils.FORMAT_DER: 'DER',
            utils.FORMAT_PEM: 'PEM'
        }

    curve_size_map = \
        {
            EcdsaBase.CURVE_256: 256,
            EcdsaBase.CURVE_384: 384
        }

    """
    Values are in bytes. Formula used to calculate signature size is: (num_bits_in_curve/8)*2+2+6

    num_bits_in_curve: either 256 or 384 depending on the curve used
    /8: used to convert bits to bytes
    *2: used to account for the 2 key values (s and r)
    +2: used to account for most significant big endian bytes required to make s and r values positive integers
    +6: used to account for 0x30, b1, 0x02, b2, 0x02, and b3 bytes required by ASN.1 encoding of signature

    Format of ASN.1 DER signature is: 0x30 b1 0x02 b2 (vr) 0x02 b3 (vs)

     ~ 0x30 is a single byte which designates the start of signature,
     ~ b1 is a single byte equal to the length in bytes of the remaining bytes,
     ~ 0x02 is a single byte which designates that an integer is following,
     ~ b2 is a single byte equal to the length in bytes of (vr),
     ~ (vr) is the signed big-endian encoding of the value "r", of minimal length,
     ~ 0x02 is a single byte which designates that an integer is following,
     ~ b3 is a single byte equal to the length in bytes of (vs),
     ~ (vs) is the signed big-endian encoding of the value "s", of minimal length.
    """
    max_sig_size_map = \
        {
            EcdsaBase.CURVE_256: 72,
            EcdsaBase.CURVE_384: 104
        }

    def __init__(self, module):
        self.openssl = module

    def gen_keys(self, curve, priv_key_path=None, pub_key_path=None):
        """ Generate ECDSA Key pair

            :param curve: Name of curve to use. Either "secp256r1" or "secp384r1"
            :type curve: str
            :param priv_key_path: File path to store private key
            :type priv_key_path: str
            :param pub_key_path: File path to store public key
            :type pub_key_path: str

            :return: generated private_key and public_key
        """

        # Generate the key pair using openssl
        priv_key = self._gen_ecdsa_key_pair(curve)

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

        return priv_key, pub_key

    def sign(self, message, key):
        """ Encrypt a message with a key.

            :param message: message to sign
            :param key: key with which to sign
            :return signed_message: signed version of message
        """
        signature = self._pkeyutl(message, key, ['-sign'], [] + [])
        return self.add_sig_padding(key, signature)

    def add_sig_padding(self, key, signature):
        # Add padding at end of signature
        sig_size = len(signature)
        max_sig_size = None
        if EcdsaBase.CURVE_256 in key or EcdsaBase.CURVE_256_PRIME in key:
            # Signature was created using secp256r1 curve
            max_sig_size = self.max_sig_size_map[EcdsaBase.CURVE_256]
        elif EcdsaBase.CURVE_384 in key:
            # Signature was created using secp384r1 curve
            max_sig_size = self.max_sig_size_map[EcdsaBase.CURVE_384]
        else:
            # It could not be determined which curve was used to create the signature so determine
            # max_sig_size based on which max_sig_size_map value is closest to the signature's unpadded size
            max_sig_sizes = sorted(self.max_sig_size_map.values())
            for size in max_sig_sizes:
                if sig_size <= size:
                    max_sig_size = size
        # Ensure that a valid max_sig_size was found
        if max_sig_size is None or (max_sig_size is not None and sig_size > max_sig_size):
            raise RuntimeError("ECDSA signature is of unsupported length.")

        # Add padding of required length to signature
        return signature.ljust(max_sig_size, PAD_BYTE_0)

    @staticmethod
    def strip_sig_padding(signature):
        # Check magic number is 0x30
        if signature[0] != '\x30':
            raise RuntimeError("ECDSA signature does not match ASN.1 DER encoding spec.")

        sig_size = len(signature)
        sig_size_without_padding = int(signature[1].encode('hex'), 16) + 2

        # If no padding exists return signature unmodified
        if sig_size == sig_size_without_padding:
            return signature

        # Ensure padding only contains zeros
        padding_size = sig_size - sig_size_without_padding
        padding = signature[sig_size_without_padding:]
        if len(padding) != padding_size:
            raise RuntimeError("ECDSA padding is of invalid length.")
        for byte in padding:
            if byte != PAD_BYTE_0:
                raise RuntimeError("ECDSA signature padding contains non-zero value.")

        # Strip padding from signature
        return signature[:sig_size_without_padding]

    def verify(self, message, key, signature):
        """ Decrypt and verify an encrypted message using a public key and signature

        :param message: message to verify
        :param key: public key
        :param signature: signature
        :return verified: boolean outcome of verification
        :rtype: bool
        """
        signature = EcdsaOpenSSLImpl.strip_sig_padding(signature)

        expected_retcode = 1
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        signature_path = c_path.create_tmp_file(signature)

        cmd = ([self.openssl, OPENSSL_PKEYUTL_MODE, '-verify',
                '-pubin',
                '-inkey', key_path,
                '-sigfile', signature_path,
                '-in', message_path])
        try:
            output = run_command(cmd, expected_retcode)
            if output.strip() == 'Signature Verified Successfully':
                return True
            else:
                return False
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)
            c_path.remove_tmp_file(signature_path)

    def get_key_in_format(self, key, inform, outform, is_public=False):
        inform_opt = self._get_format_opt(inform)
        outform_opt = self._get_format_opt(outform)
        key_path = c_path.create_tmp_file(key)

        try:
            cmd = [self.openssl, 'ec',
                   '-in', key_path,
                   '-inform', inform_opt,
                   '-outform', outform_opt]
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

    def _gen_ecdsa_key_pair(self, curve):
        cmd = [self.openssl, 'ecparam', '-genkey', '-name', curve, '-outform', 'PEM', '-noout', '-text']
        return run_command(cmd)

    def _get_pub_from_priv(self, priv_path):
        cmd = [self.openssl, 'ec', '-in', priv_path, '-pubout']
        return run_command(cmd)

    def _get_format_opt(self, form):
        try:
            form_opt = self.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')
        return form_opt

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
