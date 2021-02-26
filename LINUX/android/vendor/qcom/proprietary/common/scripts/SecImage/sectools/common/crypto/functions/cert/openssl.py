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
import subprocess

from sectools.common.crypto import utils
from sectools.common.crypto.functions.cert import CertBase
from sectools.common.crypto.functions.common import run_command
from sectools.common.utils import c_path

OPENSSL_CERT_MODE = 'x509'


class CertOpenSSLImpl(CertBase):

    formats_map = \
    {
        utils.FORMAT_DER       : 'DER',
        utils.FORMAT_PEM       : 'PEM',
    }

    def __init__(self, module):
        self.openssl = module

    def get_cert_in_format(self, cert, inform, outform):

        if "BEGIN CERTIFICATE" not in cert and outform == utils.FORMAT_DER:
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

    def create_cert(self, priv_key, subject_params=None, config=None,
                    hash_algo=CertBase.HASH_ALGO_SHA256, self_sign=False, days=None, serial=None,
                    padding=CertBase.PAD_PKCS, pad_hash_algo=CertBase.HASH_ALGO_SHA256,
                    pad_salt_len=CertBase.PAD_PSS_SALT_1):
        """ Create a self signed certificate
        input:
            subject_params = {
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm",
                             }

            Dictionary of parameters to put in the certificate. The parameters above are an example

            days = validity period of certificate in days
            configfile = configfile used by openssl
            serial_num = Serial number of certificate
            padding = Type of padding

        output:
            certificate: String representation of PEM certificate.
        """
        priv_path = c_path.create_tmp_file(priv_key)
        try:
            cmd = [self.openssl, 'req', '-new', '-key', priv_path]
            if subject_params is not None:
                subject = self.get_subject_from_params(subject_params)
                cmd += ['-subj', subject]
            if config is not None:
                cmd += ['-config', config]
            if hash_algo is not None:
                cmd += ['-' + hash_algo]
            if self_sign:
                cmd += ['-x509']
                if padding == self.PAD_PSS:
                    cmd += ['-sigopt', 'rsa_padding_mode:pss',
                            '-sigopt', 'rsa_pss_saltlen:'+pad_salt_len,
                            '-sigopt', 'digest:'+pad_hash_algo]
            if serial is not None:
                cmd += ['-set_serial', str(serial)]
            if days is not None:
                cmd += ['-days', str(days)]
            return run_command(cmd)
        finally:
            c_path.remove_tmp_file(priv_path)

    def sign_cert(self, cert, ca_cert, ca_priv_key, days=None, serial=None, hash_algo=CertBase.HASH_ALGO_SHA256,
                  extfile=None, padding=CertBase.PAD_PKCS, pad_hash_algo=CertBase.HASH_ALGO_SHA256,
                  pad_salt_len=CertBase.PAD_PSS_SALT_1):
        cert_path, ca_cert_path, ca_priv_path = None, None, None
        try:
            cert_path = c_path.create_tmp_file(cert)
            ca_cert_path = c_path.create_tmp_file(ca_cert)
            ca_priv_path = c_path.create_tmp_file(ca_priv_key)

            cmd = [self.openssl, 'x509', '-req',
                   '-in', cert_path,
                   '-CA', ca_cert_path,
                   '-CAkey', ca_priv_path,
                   ]
            if serial is not None:
                cmd += ['-set_serial', str(serial)]
            else:
                cmd += ['-CAcreateserial']
            if hash_algo is not None:
                cmd += ['-' + hash_algo]
            if days is not None:
                cmd += ['-days', str(days)]
            if extfile is not None:
                cmd += ['-extfile', extfile]
            if padding == self.PAD_PSS:
                cmd += ['-sigopt', 'rsa_padding_mode:pss',
                        '-sigopt', 'rsa_pss_saltlen:'+pad_salt_len,
                        '-sigopt', 'digest:'+pad_hash_algo]
            return run_command(cmd)
        finally:
            if cert_path is not None: c_path.remove_tmp_file(cert_path)
            if ca_cert_path is not None: c_path.remove_tmp_file(ca_cert_path)
            if ca_priv_path is not None: c_path.remove_tmp_file(ca_priv_path)

    def get_pubkey_from_cert(self, cert, inform, text_output=False):
        inform_opt = self._get_format_opt(inform)
        cert_path, pub_path = None, None
        try:
            cert_path = c_path.create_tmp_file(cert)
            cmd = [self.openssl, OPENSSL_CERT_MODE, '-pubkey', '-noout',
                   '-in', cert_path,
                   '-inform', inform_opt]
            pub_key = run_command(cmd)
            if text_output:
                pub_path = c_path.create_tmp_file(pub_key)
                cmd = [self.openssl, 'rsa', '-pubin', '-text', '-noout'
                       '-in', cert_path,
                       '-inform', inform_opt]
                pub_key = run_command(cmd)
        finally:
            if cert_path is not None: c_path.remove_tmp_file(cert_path)
            if pub_path is not None: c_path.remove_tmp_file(pub_path)
        return pub_key

    def get_cert_text(self, cert, inform, outform):
        """ Return text contents of certificate
        input:
            der_certificate_path = path to DER certficate

        output:
            certificate_text = String representation certificate contents
        """
        # Any preprocessing
        if outform == CertBase.CERT_TEXT_FORM_X509 and inform == utils.FORMAT_PEM:
            cert = self.get_cert_in_format(cert, utils.FORMAT_PEM, utils.FORMAT_DER)
            inform = utils.FORMAT_DER
        elif outform == CertBase.CERT_TEXT_FORM_ASN1 and inform == utils.FORMAT_DER:
            cert = self.get_cert_in_format(cert, utils.FORMAT_DER, utils.FORMAT_PEM)
            inform = utils.FORMAT_PEM

        cert_path = c_path.create_tmp_file(cert)
        try:
            cmd = [self.openssl, outform,
                   '-in', cert_path,
                   '-inform', inform,
                   ]

            # Create command based on formats
            if outform == CertBase.CERT_TEXT_FORM_X509:
                if inform != utils.FORMAT_DER:
                    raise RuntimeError('Invalid input format specified for output format ' + outform)
                cmd += ['-noout', '-text', '-certopt', 'ext_dump']
            elif outform == CertBase.CERT_TEXT_FORM_ASN1:
                if inform != utils.FORMAT_PEM:
                    raise RuntimeError('Invalid input format specified for output format ' + outform)
            else:
                raise RuntimeError('Invalid output format ' + str(outform))

            return subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
        finally:
            c_path.remove_tmp_file(cert_path)

    def _get_format_opt(self, form):
        try:
            form_opt = self.formats_map[form]
        except KeyError:
            raise RuntimeError('Format: ' + str(form) + ' is not supported.')
        return form_opt

    def validate_cert_chain(self, root_cert, ca_cert_chain):
        """ Verify the certificate chain to be valid

        input:
        certificate_chain: [cert1,cert2,cert3]
        List of certificates (*in PEM FORMAT*) in the certificate chain. It assumes that the last certificate is the Root CA certificate.

        output:
        [True|False]
        Boolean value
        """
        ca_cert_chain_path, root_cert_path = None, None
        try:
            ca_cert_chain_path = c_path.create_tmp_file(ca_cert_chain)
            root_cert_path = c_path.create_tmp_file(root_cert)
            cmd = [self.openssl, 'verify', '-CAfile', ca_cert_chain_path, root_cert_path]
            validity = run_command(cmd).rstrip()
            return validity == (root_cert_path + ': OK')
        finally:
            if ca_cert_chain_path is not None:
                c_path.remove_tmp_file(ca_cert_chain_path)
            if root_cert_path is not None:
                c_path.remove_tmp_file(root_cert_path)
