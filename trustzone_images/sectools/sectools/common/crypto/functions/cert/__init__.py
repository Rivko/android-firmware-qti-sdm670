# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc
import re
import time
from collections import defaultdict

from six import ensure_str

from sectools.common.crypto import utils
from sectools.common.crypto.functions import FuncImplDiscovery

function_name = "cert"


class CertImplDiscovery(FuncImplDiscovery):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def func(cls):
        return function_name

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
    # Cert versions
    CERT_V3 = 3

    # Text options
    CERT_TEXT_FORM_X509 = 'x509'
    CERT_TEXT_FORM_ASN1 = 'asn1parse'

    # Cert hash algorithms
    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'

    # Padding schemes
    SIGN_ALGO_RSA_PKCS = 'pkcs'
    SIGN_ALGO_RSA_PSS = 'pss'
    SIGN_ALGO_ECDSA = 'ecdsa'

    # PSS padding params
    PAD_PSS_SALT_1 = '-1'
    PAD_PSS_SALT_20 = '20'

    def __init__(self, module):
        self.module = module

    # Cert functions
    @abc.abstractmethod
    def create_cert(self, priv_key, subject_params=None, config=None, self_sign=False, hash_algo=None, days=None,
                    serial=None, sign_algo=SIGN_ALGO_RSA_PKCS, pad_hash_algo=HASH_ALGO_SHA256,
                    pad_salt_len=PAD_PSS_SALT_1):
        """ Generate a certificate.
        input:

            certificate_params ={
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm Technologies Inc",
                                  'OU'             : [r"General Use Test Key (for testing only)", r"CDMA Technologies"]
                              }
            Dictionary of parameters to put in the certificate. The parameters above are an example
            If the same parameter has multiple values as 'OU' above, create a list of values

            ca_certificate: String representation of CA certificate used to sign the cert

            certificate_key_pair = None | key_pair = {"public_key": [Generated public key],
                                                      "private_key": [Generated private key] }

            Dictionary holding the values of public and private keys. If this is None, a key
            is generated.

            days = validity period of certificate in days

            configfile = configfile used by openssl

        output:
            certificate: String representation of PEM certificate.
            certificate_key_pair : {"public_key": certificate public key],
                                    "private_key": certificate private key] }
        """
        """ Create a self signed certificate
        input:
            subject_params = {
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm Technologies Inc",
                             }

            Dictionary of parameters to put in the certificate. The parameters above are an example

            days = validity period of certificate in days
            configfile = configfile used by openssl
            serial_num = Serial number of certificate
            sign_algo = Type of sign_algo

        output:
            certificate: String representation of PEM certificate.
        """
        """ Create a self signed certificate
        input:
            certificate_params ={
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm Technologies Inc",
                              }
            Dictionary of parameters to put in the certificate. The parameters above are an example

            key_pair = None | key_pair = {"public_key": [Generated public key],
                                          "private_key": [Generated private key] }

            Dictionary holding the values of public and private keys. If this is None, a key
            is generated.

            days = validity period of certificate in days

            configfile = configfile used by openssl

            serial_num = Serial number of certificate

        output:
            certificate: String representation of PEM certificate.
            key_pair : {"public_key": [Generated public key],
                        "private_key": [Generated private key] }
        """
        """ Creates a self signed Root cert
        input:
            root_cert_params={
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm Technologies Inc",
                                  'OU'             : [r"General Use Test Key (for testing only)", r"CDMA Technologies"]
                              }
            key_pair = {"public_key": [Generated public key],
                        "private_key": [Generated private key] }
        """
        """ Create a CSR (Certificate Signing Request)
        input:

            certificate_params ={
                                  'C'              : "US",
                                  'ST'             : "California",
                                  'L'              : "San Diego",
                                  'O'              : "ASIC",
                                  'CN'             : "Qualcomm Technologies Inc",
                              }
            Dictionary of parameters to put in the certificate. The parameters above are an example

            key_pair = None | key_pair = {"public_key": [Generated public key],
                                          "private_key": [Generated private key] }

            Dictionary holding the values of public and private keys. If this is None, a key
            is generated.

            days = validity period of certificate in days

            configfile = configfile used by openssl

        output:
            certificate_request: String representation of PEM certificate signing request (CSR).
            key_pair : {"public_key": [Generated public key],
                        "private_key": [Generated private key] }
        """
        pass

    @abc.abstractmethod
    def sign_cert(self, cert, ca_cert, ca_priv_key, extfile=None, hash_algo=HASH_ALGO_SHA256,
                  days=None, serial=None, sign_algo=SIGN_ALGO_RSA_PKCS,
                  pad_hash_algo=HASH_ALGO_SHA256, pad_salt_len=PAD_PSS_SALT_1):
        """ Sign a Certificate signing request with a higher level CA certificate
        input:
        certificate_signing_request: String form of CSR
        CA_certificate: String representation of a higher level CA certificate

        CA_key_pair : {"public_key": [Generated public key],
                    "private_key": [Generated private key] }
        The key pair of the CA_certificate

        days = validity period of certificate in days

        serial_num = Serial number of certificate

        extfile_name = Name of the extensions file to be used by openssl

        output:
        CA_signed_certificate: String representation of CA Signed certificate (PEM)
        CA_key_pair: {"public_key": CA public key,
                      "private_key": CA private key }
        """
        pass

    @classmethod
    def get_cert_format(cls, cert):
        if 'BEGIN CERTIFICATE' in ensure_str(cert, encoding='latin-1'):
            return utils.FORMAT_PEM
        else:
            return utils.FORMAT_DER

    @abc.abstractmethod
    def get_cert_in_format(self, cert, outform):
        """ Return format converted certificate
            input:
                cert = certificate to convert
                outform = format of returned certificate (either pem or der)

            output:
                certificate = certificate formatted in outform
        """
        pass

    @abc.abstractmethod
    def get_pubkey(self, cert):
        """ Return public key of certificate
            input:
                cert = certificate from which to get public key

            output:
                public_key = public key of cert
        """
        pass

    @abc.abstractmethod
    def get_text(self, cert):
        """ Return text contents of certificate
        input:
            der_certificate_path = path to DER certificate

        output:
            certificate_text = String representation certificate contents
        """
        pass

    @abc.abstractmethod
    def get_asn1_text(self, cert):
        """ Return text contents of certificate
        input:
            der_certificate_path = path to DER certificate

        output:
            certificate_text = String representation certificate contents
        """
        pass

    @classmethod
    def get_version(cls, cert_text):
        match = re.search('Version: (\d+) \(.*?\)\r*\n', ensure_str(cert_text), re.IGNORECASE)
        if match is None:
            raise RuntimeError('Could not get the Version info from certificate')
        return int(match.group(1), 10)

    @classmethod
    def get_sign_algo(cls, cert_text):
        algo_search_string_map = {
            'Signature Algorithm: ecdsa': cls.SIGN_ALGO_ECDSA,
            'Signature Algorithm: rsassaPss': cls.SIGN_ALGO_RSA_PSS,
        }
        for string, algo in algo_search_string_map.items():
            if string in ensure_str(cert_text):
                return algo
        return cls.SIGN_ALGO_RSA_PKCS

    @classmethod
    def get_validity(cls, cert_text):
        not_before = ':'.join(re.search(r'Not Before:.*\n?', cert_text).group(0).split(':')[1:]).strip()
        not_after = ':'.join(re.search(r'Not After.*\n?', cert_text).group(0).split(':')[1:]).strip()
        return (time.strptime(not_before, "%b %d %H:%M:%S %Y %Z"),
                time.strptime(not_after, "%b %d %H:%M:%S %Y %Z"))

    @classmethod
    def get_subject(cls, cert_text):
        return re.search(r'Subject:.*\r*\n?', ensure_str(cert_text)).group(0).split(':')[1]

    @classmethod
    def get_subject_params(cls, subject_string):
        """ Extracts Subject parameter from provided certificate text
        input:
        certificate_text: String representation of Certificate

        output:
        certificate_subject_dictionary: Dictionary of certificate parameters
        """
        params = defaultdict(list)
        for key, value in split_subject_string(subject_string):
            params[key].append(value)
        for param_tag, param_value in params.items():
            if len(param_value) == 1:
                params[param_tag] = param_value[0]
        if 'OU' in params.keys() and type(params['OU']) == list:
            params['OU'].sort()
        return params

    @classmethod
    def get_subject_from_params(cls, params):
        subject = []
        for tag, values in params.items():
            if not isinstance(values, list):
                values = [values]
            for value in values:
                subject.append(tag + '=' + value)
        return r'/' + r'/'.join(subject)

    # Cert Chain functions
    @classmethod
    def create_cert_chain_bin(cls, cert_chain):
        return b''.join(cert_chain)

    @abc.abstractmethod
    def split_cert_chain_bin(self, cert_chain_bin):
        pass

    @abc.abstractmethod
    def validate_cert_chain(self, cert, ca_cert_chain):
        """ Verify the certificate chain to be valid

        input:
        certificate_chain: [cert1,cert2,cert3]
        List of certificates (*in PEM FORMAT*) in the certificate chain. It assumes that the last certificate is the
        Root CA certificate.

        output:
        [True|False]
        Boolean value
        """
        pass


def split_subject_string(subject_string):
    match = re.search(r'(\w+)\s*=\s*(.+)', subject_string)
    if match is None:
        return []
    tokens = [match.group(1)] + re.split(r'\s*,\s*(\w+)\s*=\s*', match.group(2))
    split_subject = list(zip(tokens[0::2], tokens[1::2]))
    assert int((len(tokens) + 1) / 2) == len(split_subject)
    return split_subject
