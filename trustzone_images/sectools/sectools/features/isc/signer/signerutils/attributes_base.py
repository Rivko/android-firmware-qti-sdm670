# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from six import text_type

from past.builtins import basestring

import json

from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.signer.signerutils import get_hmac_params_from_config
from .attestation_cert_base import AttestationCertificateBase


class SimpleAttribute(object):
    pass


class SigningAttributesBase(object):

    SECBOOT_VERSIONS = (None, SECBOOT_VERSION_1_0)
    INJECTED_SIGNATURE_ATTRIBUTES = ("key_size",)
    SIGNATURE_ATTRIBUTES = (
        "exponent", "hash_algorithm", "dsa_type", "ecdsa_curve", "rsa_padding")
    EXCLUDE_NON_ATTEST = ('msm_part',)
    OPTIONAL_BOOLS = ('in_use_soc_hw_version',
                      'use_serial_number_in_signing')
    OPTIONAL_STRING = ()

    attestation_certificate_class = AttestationCertificateBase

    def __init__(self):
        self.exponent = SimpleAttribute()
        self.dsa_type = SimpleAttribute()
        self.key_size = SimpleAttribute()
        self.rsa_padding = SimpleAttribute()
        self.hash_algorithm = SimpleAttribute()
        self.msm_part = SimpleAttribute()
        self.oem_id = SimpleAttribute()
        self.model_id = SimpleAttribute()
        self.hw_id = SimpleAttribute()
        self.sw_id = SimpleAttribute()
        self.debug = SimpleAttribute()
        self.in_use_soc_hw_version = SimpleAttribute()
        self.use_serial_number_in_signing = SimpleAttribute()

        self.cert_text = None
        self.from_hash_segment = False

        self.signing_attribute_names = [
            k for k, v in vars(self).items() if isinstance(v, SimpleAttribute)]

    def update_from_attest_cert(self, cert_data, excluded=()):
        # Create the attestation cert object
        cert = self.attestation_certificate_class(cert_data)
        self.cert_text = cert.cert_text

        # Update from the certificate
        for attr in self.signing_attribute_names:
            if attr in excluded:
                continue
            val = cert.sign_attr_dict.get(attr, None)
            setattr(self, attr, val)

    def update_image_info_attrs(self, attrs):
        # Update the imageinfo
        for attr in self.signing_attribute_names:
            val = getattr(self, attr)
            setattr(attrs, attr, val)

    def update_from_json(self, json_data):
        json_attrs = json.loads(json_data)
        for attr in self.signing_attribute_names:
            json_attr = json_attrs.get(attr, None)
            if isinstance(json_attr, text_type):
                json_attr = str(json_attr)
            setattr(self, attr, json_attr)

    def get_dict(self, exclude=()):
        retval = {}
        for attr in self.signing_attribute_names:
            if attr in exclude:
                continue
            retval[attr] = getattr(self, attr)
        return retval

    def get_json(self, exclude=()):
        return json.dumps(self.get_dict(exclude))

    def update_from_image_info_attrs(self, attrs):

        def sanitize(attr_, val_):
            if attr_ == 'hash_algorithm' and val_ is None:
                val_ = 'sha256'
            elif attr_ == 'hw_id':
                if attrs.secboot_version in self.SECBOOT_VERSIONS:
                    val_ = '0x' + get_hmac_params_from_config(attrs).msm_id_str
                else:
                    val_ = getattr(self, "msm_part")
            return val_

        # Update from the imageinfo
        for attr in self.signing_attribute_names:
            val = getattr(attrs, attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

    def update_from_hash_segment_and_attest_cert(self, hash_segment_data, cert_data):

        def sanitize(attr_, val_):
            if attr_ in ("sw_id", "hw_id", "oem_id", "model_id",
                         "debug") and not isinstance(val_, basestring):
                val_ = hex(val_).strip("L")
            return val_

        # Update from the hash segment
        for attr in self.signing_attribute_names:
            val = hash_segment_data.get(attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

        if cert_data:
            # Get exponent from cert
            cert = self.attestation_certificate_class(cert_data)
            self.cert_text = cert.cert_text
            for attr in self.SIGNATURE_ATTRIBUTES:
                # Get signature attributes from cert
                setattr(self, attr, cert.sign_attr_dict.get(attr, None))

        self.from_hash_segment = True

    def compare(self, other, exclude=()):
        mismatch = []
        for attr in self.signing_attribute_names:
            if attr in exclude:
                continue

            value_self = getattr(self, attr)
            value_other = getattr(other, attr)

            def clean_value(attr_, value):
                if isinstance(value, str):
                    value = value.lower()
                    if value.startswith('0x'):
                        value = str(hex(int(value, 16))).rstrip("L")
                if attr_ in self.OPTIONAL_BOOLS and value is None:
                    value = 0
                if attr_ in self.OPTIONAL_STRING and value is None:
                    value = '0x0'
                return value

            value_self = clean_value(attr, value_self)
            value_other = clean_value(attr, value_other)
            if value_self != value_other:
                mismatch.append((attr, value_self, value_other))

        return mismatch
