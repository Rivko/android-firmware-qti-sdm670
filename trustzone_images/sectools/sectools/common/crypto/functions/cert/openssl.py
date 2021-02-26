# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import calendar
import time
from binascii import unhexlify
from re import search, sub

from six import ensure_str
from sectools.common.crypto import utils
from sectools.common.crypto.functions.cert import CertBase
from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_process import run_command
from sectools.common.utils.compat import ensure_byte

OPENSSL_CERT_MODE = 'x509'


class CertOpenSSLImpl(CertBase):
    formats_map = {
        utils.FORMAT_DER: 'DER',
        utils.FORMAT_PEM: 'PEM',
    }

    def __init__(self, module):
        super(CertOpenSSLImpl, self).__init__(module)
        self.openssl = module

    def create_cert(self, priv_key, subject_params=None, config=None, self_sign=False,
                    hash_algo=CertBase.HASH_ALGO_SHA256, days=None, serial=None, sign_algo=CertBase.SIGN_ALGO_RSA_PKCS,
                    pad_hash_algo=CertBase.HASH_ALGO_SHA256, pad_salt_len=CertBase.PAD_PSS_SALT_1):
        priv_path = c_path.create_tmp_file(priv_key)
        try:
            cmd = [self.openssl, 'req', '-new', '-key', priv_path]
            if subject_params is not None:
                subject = self.get_subject_from_params(subject_params)
                cmd += ['-subj', subject]
            if config is not None:
                cmd += ['-config', config]
            if self_sign:
                cmd += ['-x509']
                if days is not None:
                    cmd += ['-days', str(days)]
                if serial is not None:
                    cmd += ['-set_serial', str(serial)]
                if hash_algo is not None:
                    cmd += ['-' + hash_algo]
                if sign_algo == self.SIGN_ALGO_RSA_PSS:
                    cmd += ['-sigopt', 'rsa_padding_mode:pss',
                            '-sigopt', 'rsa_pss_saltlen:' + pad_salt_len,
                            '-sigopt', 'digest:' + pad_hash_algo]
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(priv_path)

    def sign_cert(self, cert, ca_cert, ca_priv_key, extfile=None, hash_algo=CertBase.HASH_ALGO_SHA256,
                  days=None, serial=None, sign_algo=CertBase.SIGN_ALGO_RSA_PKCS,
                  pad_hash_algo=CertBase.HASH_ALGO_SHA256, pad_salt_len=CertBase.PAD_PSS_SALT_1):
        cert_path, ca_cert_path, ca_priv_path = None, None, None
        try:
            cert_path = c_path.create_tmp_file(cert)
            ca_cert_path = c_path.create_tmp_file(ca_cert)
            ca_priv_path = c_path.create_tmp_file(ca_priv_key)

            cmd = [self.openssl, OPENSSL_CERT_MODE, '-req',
                   '-in', cert_path,
                   '-CAkey', ca_priv_path,
                   '-CA', ca_cert_path,
                   ]
            if days is not None:
                cmd += ['-days', str(days)]
            if serial is not None:
                cmd += ['-set_serial', str(serial)]
            if extfile is not None:
                cmd += ['-extfile', extfile]
            if hash_algo is not None:
                cmd += ['-' + hash_algo]
            if sign_algo == self.SIGN_ALGO_RSA_PSS:
                cmd += ['-sigopt', 'rsa_padding_mode:pss',
                        '-sigopt', 'rsa_pss_saltlen:' + pad_salt_len,
                        '-sigopt', 'digest:' + pad_hash_algo]
            else:
                cmd += ['-CAcreateserial']
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(cert_path)
            c_path.remove_tmp_file(ca_cert_path)
            c_path.remove_tmp_file(ca_priv_path)

    def get_cert_in_format(self, cert, outform):
        # Avoid error by reconverting
        inform = self.get_cert_format(cert)
        if inform == outform:
            return cert

        inform_opt = self._get_format_opt(inform)
        outform_opt = self._get_format_opt(outform)
        cert_path = c_path.create_tmp_file(cert)
        try:
            cmd = [self.openssl, OPENSSL_CERT_MODE,
                   '-in', cert_path,
                   '-inform', inform_opt,
                   '-outform', outform_opt]
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(cert_path)

    def get_pubkey(self, cert):
        cert_path = c_path.create_tmp_file(cert)
        try:
            cmd = [self.openssl, OPENSSL_CERT_MODE,
                   '-in', cert_path,
                   '-pubkey', '-noout',
                   '-inform', self._get_format_opt(self.get_cert_format(cert))]
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(cert_path)

    def get_text(self, cert):
        cert_path = c_path.create_tmp_file(cert)
        try:
            cmd = [self.openssl, OPENSSL_CERT_MODE,
                   '-in', cert_path,
                   '-inform', self._get_format_opt(self.get_cert_format(cert)),
                   '-noout', '-text',
                   '-certopt', 'ext_dump']
            return run_command(cmd, large_output=True, ret_binary=False)
        finally:
            c_path.remove_tmp_file(cert_path)

    def get_asn1_text(self, cert):
        cert_path = c_path.create_tmp_file(cert)
        try:
            cmd = [self.openssl, 'asn1parse',
                   '-in', cert_path,
                   '-inform', self._get_format_opt(self.get_cert_format(cert))]
            return run_command(cmd, large_output=True)
        finally:
            c_path.remove_tmp_file(cert_path)

    def _get_format_opt(self, form):
        try:
            return self.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')

    def validate_cert_chain(self, cert, ca_cert_chain):
        ca_cert_chain_path, cert_path = None, None
        try:
            ca_cert_chain_path = c_path.create_tmp_file(ca_cert_chain)
            cert_path = c_path.create_tmp_file(cert)

            # Check validity time
            valid_from, _ = self.get_validity(self.get_text(cert))
            lag = calendar.timegm(valid_from) - int(time.time())
            if lag > 0:
                logger.warning('Local machine\'s time is slow by at least ' + str(lag) + ' seconds.')

            cmd = [self.openssl, 'verify',
                   '-attime', str(calendar.timegm(valid_from)),
                   '-CAfile', ca_cert_chain_path, cert_path]
            return (cert_path + ': OK') in run_command(cmd, ret_binary=False)
        except RuntimeError:
            return False
        finally:
            c_path.remove_tmp_file(ca_cert_chain_path)
            c_path.remove_tmp_file(cert_path)

    def split_cert_chain_bin(self, cert_chain_bin):
        offset = 0
        cert_chain = []
        while True:
            if (not cert_chain_bin[offset:] or
                    all([ensure_byte(x) == PAD_BYTE_1 for x in cert_chain_bin[offset:]])):
                # No more data or remaining data is all padding.
                break
            try:
                # Leveraging openssl's capabilities to extract one certificate
                # from the chain at a time.
                cert_pem = self.get_cert_in_format(cert_chain_bin[offset:], utils.FORMAT_PEM)
                cert_der = self.get_cert_in_format(cert_pem, utils.FORMAT_DER)
            except RuntimeError:
                raise RuntimeError("Failed to extract certificate chain")
            cert_chain.append(cert_der)
            offset += len(cert_der)
        return cert_chain

    def is_root_cert(self, cert_der):
        """ Determine whether a certificate is root CA by verifying itself.

        :param cert_der: certificate data in DER format
        :return: True if the certificate is a root CA, and False if not.
        """
        cert_pem = self.get_cert_in_format(cert_der, utils.FORMAT_PEM)
        return self.validate_cert_chain(cert_pem, cert_pem)

    def extract_sig_from_cert(self, certificate):
        signature = None
        try:
            signature = unhexlify(sub(r"[:\r\n\t\s]*", "", search('Signature.*\r*\n* *(30(.*\r*\n*)*)',
                                                                  self.get_text(certificate)).group(1)))
        except Exception as e:
            logger.debug("Failed to extract sig: {}".format(str(e)))
            pass

        return signature
