# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import re

from six import ensure_str

from sectools.common import crypto
from sectools.common.crypto.functions.cert import split_subject_string


class AttestationCertificateBase(object):

    sig_algo_patterns = [
        ("rsa", "pss", re.compile('Signature Algorithm:.*\n.*Hash Algorithm: (sha[0-9]+)', re.IGNORECASE)),
        ("rsa", "pkcs", re.compile('Signature Algorithm:.*(sha[0-9]+)WithRSAEncryption', re.IGNORECASE)),
        ("ecdsa", None, re.compile('Signature Algorithm:.*ecdsa-with-(sha[0-9]+)', re.IGNORECASE))
    ]

    def __init__(self, cert_data):
        self.cert_data = cert_data
        self.cert_text = self.get_text()
        self.subject_list = self.get_subject_list()
        self.sign_attr_dict = self.get_sign_attrs()
        self.sign_attr_dict['exponent'] = self.get_exponent_from_text()
        self.sign_attr_dict["ecdsa_curve"] = self.get_ecdsa_curve_from_text()
        self.sign_attr_dict['hash_algorithm'] = self.get_hash_algorithm()
        dsa_type, rsa_padding = self.get_dsa_type_rsa_padding_from_text()
        self.sign_attr_dict["dsa_type"] = dsa_type
        self.sign_attr_dict["rsa_padding"] = rsa_padding

    @staticmethod
    def process_hash_algorithm(k, v):
        sha_mapping = {
            'sha1': '0000',
            'sha256': '0001',
            'sha384': '0002',
        }
        if (k, v) not in sha_mapping.items():
            raise RuntimeError('Invalid sha value: ' + k + ':' + v)
        return 'hash_algorithm', k

    @classmethod
    def process_ou_field(cls, ou_str):
        ou_number, ou_value, ou_id = "", "", ""
        match = re.match('([^ ]+) ([^ ]+) ([^ ]+)', ou_str)
        if match:
            ou_number = match.group(1)
            ou_value = match.group(2)
            ou_id = match.group(3)
        return ou_number, ou_value, ou_id

    def get_text(self):
        return crypto.cert.get_text(self.cert_data)

    def get_subject_list(self):
        match = re.search('Subject: (.*?)\r*\n', ensure_str(self.cert_text), re.IGNORECASE)
        if match is None:
            raise RuntimeError('Could not get the SUBJECT field from the attestation certificate')
        return split_subject_string(match.group(1).lower())

    def get_sign_attrs(self):
        sign_attr_dict = {}
        for k, v in self.subject_list:
            # Update OU fields
            if k == 'ou':
                # Get the OU decomposition
                ou_number, ou_value, ou_id = self.process_ou_field(v)
                # Update the key and value
                k, v = ou_id, ou_value
                if k.startswith('sha'):
                    # Update hash_algorithm
                    k, v = self.process_hash_algorithm(k, v)
                elif k in ['oem_id', 'model_id', 'hw_id', 'sw_id', 'debug']:
                    # Update the hex values
                    v = '0x' + v
                elif k in ['in_use_soc_hw_version', 'use_serial_number_in_signing']:
                    # Update the boolean values
                    v = int(v)
                sign_attr_dict[k] = v
        return sign_attr_dict

    def get_exponent_from_text(self):
        match = re.search('Exponent: (\d+) \(.*?\)', ensure_str(self.cert_text), re.IGNORECASE)
        """exponent is only required for RSA, not for ECDSA"""
        if match is not None:
            exponent = match.group(1)
            return int(exponent, 10)
        else:
            return None

    def get_ecdsa_curve_from_text(self):
        return get_ecdsa_curve_from_text(self.cert_text)

    def get_dsa_type_rsa_padding_from_text(self):
        for dsa_type, rsa_padding, p in self.sig_algo_patterns:
            match = p.search(ensure_str(self.cert_text))
            if match is not None:
                return dsa_type, rsa_padding
        raise RuntimeError(
            'Could not get the DSA Type and RSA Padding fields from the attestation certificate')

    def get_hash_algorithm(self):
        # OU field takes precedence
        if self.sign_attr_dict.get('hash_algorithm'):
            return self.sign_attr_dict['hash_algorithm']
        # OU field wasn't found so get hash algo from Signature Algorithm field
        for dsa_type, rsa_padding, p in self.sig_algo_patterns:
            match = p.search(ensure_str(self.cert_text))
            if match is not None:
                return match.group(1).lower()
        raise RuntimeError(
            'Could not get the Hash Algorithm field from the attestation certificate')


def get_ecdsa_curve_from_text(text):
    match = re.search(r"ASN1 OID:\s+(\w+)[\r\n]", ensure_str(text))
    if match:
        ecdsa_curve = match.group(1)
        # According to https://www.ietf.org/rfc/rfc5480.txt, section 2.1.1.1, Named Curve,
        # "the secp256r1 curve was referred to as prime256v1."
        return "secp256r1" if ecdsa_curve == "prime256v1" else ecdsa_curve
    else:
        return None
