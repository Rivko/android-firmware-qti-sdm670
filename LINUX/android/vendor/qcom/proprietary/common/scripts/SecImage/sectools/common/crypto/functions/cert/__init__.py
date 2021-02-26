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
import re
import subprocess

from sectools.common.crypto import utils
from sectools.common.crypto.functions import FuncImplDiscovery, FUNC_CERT


class CertImplDiscovery(FuncImplDiscovery):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return FUNC_CERT

    @classmethod
    def modules(cls):
        modules = {}
        try:
            from sectools.common.crypto.discovery.defines import MOD_OPENSSL
            from sectools.common.crypto.functions.cert.openssl import CertOpenSSLImpl
            modules[MOD_OPENSSL] = CertOpenSSLImpl
        except Exception:
            pass
        return modules


class CertBase(object):

    CERT_TEXT_FORM_X509 = 'x509'
    CERT_TEXT_FORM_ASN1 = 'asn1parse'

    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'

    # Padding schemes
    PAD_PKCS = 'pkcs'
    PAD_PSS  = 'pss'

    # PSS padding params
    PAD_PSS_SALT_1 = '-1'
    PAD_PSS_SALT_20 = '20'

    def __init__(self, module):
        self.module = module

    # Cert functions
    @abc.abstractmethod
    def create_cert(self, priv_key, subject_params=None, config=None,
                    hash_algo=None, self_sign=False, days=None, serial=None,
                    padding=PAD_PKCS, pad_hash_algo=HASH_ALGO_SHA256,
                    pad_salt_len=PAD_PSS_SALT_1):
        pass

    @abc.abstractmethod
    def sign_cert(self, cert, ca_cert, ca_priv_key, days, serial, extfile,
                  padding=PAD_PKCS, pad_hash_algo=HASH_ALGO_SHA256,
                  pad_salt_len=PAD_PSS_SALT_1):
        pass

    def get_cert_format(self, cert):
        if 'BEGIN CERTIFICATE' in cert:
            return utils.FORMAT_PEM
        else:
            return utils.FORMAT_DER

    @abc.abstractmethod
    def get_cert_in_format(self, cert, inform, outform):
        pass

    @abc.abstractmethod
    def get_pubkey_from_cert(self, cert, inform, text_output=False):
        pass

    @abc.abstractmethod
    def get_cert_text(self, cert, inform, outform):
        pass

    def get_subject_from_params(self, params):
        subject = []
        for tag, values in params.items():
            if not isinstance(values, list):
                values = [values]
            for value in values:
                subject.append(tag + '=' + value)
        return r'/' + r'/'.join(subject)

    def get_params_from_subject(self, cert):
        """ Extracts Subject parameter from provided certificate text
        input:
        certificate_text: String representation of Certificate

        output:
        certificate_subject_dictionary: Dictionary of certificate parameters
        """
        params = {}
        subject = re.search(r'Subject:.*\n?', cert).group(0).split(':')[1]
        for param in subject.split(','):
            param_split = param.strip().split('=')
            param_value = params.setdefault(param_split[0].strip(), [])
            param_value.append(param_split[1].strip())
        for param_tag, param_value in params.items():
            if len(param_value) == 1:
                params[param_tag] = param_value[0]
        return params

    # Cert Chain functions
    def create_cert_chain_bin(self, cert_chain):
        return ''.join(cert_chain)

    def split_cert_chain_bin(self, cert_chain_bin):
        cert_start_regex = r'(0\x82.{2}0\x82)'
        certs_split = re.split(cert_start_regex, cert_chain_bin, re.DOTALL)
        cert_chain = []
        for idx in range(1, len(certs_split), 2):
            cert_chain.append(''.join(certs_split[idx:idx+2]))
        return cert_chain

    @abc.abstractmethod
    def validate_cert_chain(self, cert_chain):
        pass

