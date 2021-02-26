# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from re import findall

from six import ensure_str

from sectools.common.utils.compat import ensure_byte, ensure_int
from sectools.common.crypto import utils
from sectools.common.crypto.discovery.openssl import OPENSSL_PKEYUTL_FIXED_VERSION_MIN
from sectools.common.crypto.discovery.openssl import OpenSSLDiscoveryImpl
from sectools.common.crypto.functions.ecdsa import EcdsaBase
from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_process import run_command

OPENSSL_ECDSA_MODE = 'ec'
OPENSSL_PKEYUTL_MODE = 'pkeyutl'


class EcdsaOpenSSLImpl(EcdsaBase):
    formats_map = {
        utils.FORMAT_DER: 'DER',
        utils.FORMAT_PEM: 'PEM'
    }

    curve_size_map = {
        EcdsaBase.CURVE_256: 256,
        EcdsaBase.CURVE_384: 384
    }

    R_S_SIZES = [48, 49]

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
        super(EcdsaOpenSSLImpl, self).__init__(module)
        self.openssl = module

    def gen_keys(self, curve, priv_key_path=None, pub_key_path=None):
        # Generate the key pair using openssl
        priv_key = self._gen_ecdsa_key_pair(curve)

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

    def sign(self, message, key, curve):
        signature = self._pkeyutl(message, key, ['-sign'], [] + [])
        return self.add_sig_padding(signature, curve)

    @staticmethod
    def add_sig_padding(signature, curve):
        # Add padding at end of signature
        sig_size = len(signature)
        max_sig_size = EcdsaOpenSSLImpl.max_sig_size_map.get(curve)
        # Ensure that a valid max_sig_size was found
        if max_sig_size is None or (max_sig_size is not None and sig_size > max_sig_size):
            raise RuntimeError("ECDSA signature is of unsupported length.")

        # Add padding of required length to signature
        return signature.ljust(max_sig_size, PAD_BYTE_0)

    @staticmethod
    def strip_sig_padding(signature):
        # Check magic number is 0x30
        if ensure_byte(signature[0]) != b'\x30':
            raise RuntimeError("ECDSA signature does not match ASN.1 DER encoding spec.")

        sig_size = len(signature)
        sig_size_without_padding = ensure_int(signature[1]) + 2

        # If no padding exists return signature unmodified
        if sig_size == sig_size_without_padding:
            return signature

        # Ensure padding only contains zeros
        padding_size = sig_size - sig_size_without_padding
        padding = signature[sig_size_without_padding:]
        if len(padding) != padding_size:
            raise RuntimeError("ECDSA padding is of invalid length.")
        for byte in padding:
            if ensure_byte(byte) != PAD_BYTE_0:
                raise RuntimeError("ECDSA signature padding contains non-zero value.")

        # Strip padding from signature
        return signature[:sig_size_without_padding]

    def verify(self, message, key, signature):
        # Remove padding and save signature to file
        signature = self.strip_sig_padding(signature)
        signature_path = c_path.create_tmp_file(signature)

        # Return if message matches signature
        try:
            expected_retcode = True ^ OpenSSLDiscoveryImpl.is_supported_version(
                self.openssl, min_version=OPENSSL_PKEYUTL_FIXED_VERSION_MIN)
            output = self._pkeyutl(message, key, ['-verify', '-pubin'],
                                   ['-sigfile', signature_path], expected_retcode=expected_retcode, ignore_retcode=True)
            return output.strip() == b'Signature Verified Successfully'
        finally:
            c_path.remove_tmp_file(signature_path)

    def get_key_in_format(self, key, outform, is_public=False):
        # Avoid error by reconverting
        inform = self.get_key_format(key)
        if inform == outform:
            return key

        key_path = c_path.create_tmp_file(key)
        try:
            cmd = [self.openssl, OPENSSL_ECDSA_MODE,
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
            cmd = [self.openssl, OPENSSL_ECDSA_MODE, '-in', key_path, '-pubout']
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(key_path)

    def _gen_ecdsa_key_pair(self, curve):
        cmd = [self.openssl, 'ecparam', '-genkey',
               '-name', curve,
               '-outform', 'PEM',
               '-noout', '-text']
        return run_command(cmd)

    @classmethod
    def _get_format_opt(cls, form):
        try:
            return cls.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')

    def _pkeyutl(self, message, key, preopts, options, expected_retcode=0, ignore_retcode=False):
        message_path = c_path.create_tmp_file(message)
        key_path = c_path.create_tmp_file(key)
        cmd = ([self.openssl, OPENSSL_PKEYUTL_MODE] + preopts +
               ['-inkey', key_path, '-in', message_path] + options)
        try:
            return run_command(cmd, expected_retcode, ignore_retcode=ignore_retcode)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(key_path)

    def get_asn1_text(self, signature):
        signature_path = c_path.create_tmp_file(signature)
        try:
            cmd = [self.openssl, 'asn1parse', '-in', signature_path, '-inform', utils.FORMAT_DER]
            return run_command(cmd, large_output=True)
        finally:
            c_path.remove_tmp_file(signature_path)

    def is_valid_r_s_size(self, signature, ecdsa_curve):
        is_valid = False
        if ecdsa_curve == EcdsaBase.CURVE_384:
            try:
                match = findall("([0-9]+) prim: INTEGER", ensure_str(self.get_asn1_text(self.strip_sig_padding(signature))))
                is_valid = int(match[0]) in self.R_S_SIZES and int(match[1]) in self.R_S_SIZES
            except:
                pass
        else:
            is_valid = True
        return is_valid
