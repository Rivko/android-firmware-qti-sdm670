# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from past.builtins import basestring

from sectools.common.utils.c_data import get_duplicates
from sectools.common.utils.c_data import remove_duplicates
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from .attestation_cert import AttestationCertificate
from .attributes_base import SigningAttributesBase
from .attributes_base import SimpleAttribute
from sectools.common.parsegen.mbn.versions.v6.headers import METADATA_MAJOR_VERSION_0, METADATA_MAJOR_VERSION_1, \
    METADATA_MINOR_VERSION_0
from sectools.features.isc.advanced_defines import PLATFORM_BINDING_SOC_VERS, PLATFORM_BINDING_JTAG_ID, \
    PLATFORM_BINDING_INDEPENDENT


class SigningAttributes(SigningAttributesBase):

    EXCLUDE_METADATA_VERSIONS = ('metadata_major_version', 'metadata_minor_version')
    # These values are abstracted by platform_binding and can be skipped in favor of comparing platform_binding
    EXCLUDE_PLATFORM_BINDING = ('in_use_soc_hw_version', 'in_use_hw_id', 'model_id_independent')
    # These values should be skipped when platform_binding mismatches to avoid unnecessary mismatches
    EXCLUDE_PLATFORM_BINDING_MISMATCH = ('soc_vers', 'msm_part', 'hw_id')
    # Model ID comparison should be skipped when platform_binding mismatches and one of the mismatched value is equal
    # to INDEPENDENT to avoid unnecessary mismatch
    EXCLUDE_PLATFORM_BINDING_INDEPENDENT_MISMATCH = ('model_id',)

    SECBOOT_VERSIONS = (None, SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0)
    # INJECTED_SIGNATURE_ATTRIBUTES contains those that are injected into the
    # SigningAttributes object, not derived from cert or hash segment.
    # If a MBN v6 image is unsigned, the attributes in EXCLUDE_HASH_SEGMENT_METADATA
    # are not validated.
    EXCLUDE_HASH_SEGMENT_METADATA = \
        SigningAttributesBase.EXCLUDE_NON_ATTEST + \
        SigningAttributesBase.SIGNATURE_ATTRIBUTES + \
        SigningAttributesBase.INJECTED_SIGNATURE_ATTRIBUTES + \
        EXCLUDE_METADATA_VERSIONS
    OPTIONAL_BOOLS = ('in_use_soc_hw_version',
                      'use_serial_number_in_signing',
                      'oem_id_independent')
    OPTIONAL_STRING = ('revocation_enablement',
                       'activation_enablement',
                       'root_revoke_activate_enable',
                       'uie_key_switch_enable',
                       'rot_en',
                       'crash_dump',
                       'app_id',
                       'anti_rollback_version',
                       'in_use_hw_id',
                       'model_id_independent')

    attestation_certificate_class = AttestationCertificate

    def __init__(self):
        self.metadata_major_version = SimpleAttribute()
        self.metadata_minor_version = SimpleAttribute()

        self.platform_binding = SimpleAttribute()

        self.ecdsa_curve = SimpleAttribute()
        self.app_id = SimpleAttribute()
        self.multi_serial_numbers = SimpleAttribute()
        self.crash_dump = SimpleAttribute()
        self.rot_en = SimpleAttribute()
        self.soc_vers = SimpleAttribute()
        self.oem_id_independent = SimpleAttribute()
        self.revocation_enablement = SimpleAttribute()
        self.activation_enablement = SimpleAttribute()
        self.root_revoke_activate_enable = SimpleAttribute()
        self.uie_key_switch_enable = SimpleAttribute()
        self.anti_rollback_version = SimpleAttribute()
        self.in_use_hw_id = SimpleAttribute()
        self.model_id_independent = SimpleAttribute()
        super(SigningAttributes, self).__init__()

    def update_from_attest_cert(self, cert_data, excluded=()):
        super(SigningAttributes, self).update_from_attest_cert(cert_data, excluded)
        # MBN v3 and MBN v5 only support binding to soc_hw_version/soc_vers or msm_part
        if self.in_use_soc_hw_version == 1:
            self.platform_binding = PLATFORM_BINDING_SOC_VERS
        else:
            self.platform_binding = PLATFORM_BINDING_JTAG_ID

    def update_from_hash_segment_and_attest_cert(self, hash_segment_data, cert_data, excluded=()):

        def sanitize(attr_, val_):
            if attr_ in ("soc_vers", "multi_serial_numbers"):
                sanitized_list = list()
                for item in val_:
                    if item != 0:
                        if attr_ == "soc_vers":
                            sanitized_list.append("{0:#0{1}x}".format(item, 6))
                        else:
                            sanitized_list.append("{0:#0{1}x}".format(item, 10))
                val_ = sanitized_list
            elif attr_ in ("sw_id",
                           "hw_id",
                           "oem_id",
                           "model_id",
                           "app_id",
                           "anti_rollback_version",
                           "rot_en",
                           "root_revoke_activate_enable",
                           "uie_key_switch_enable",
                           "debug") and not isinstance(val_, basestring):
                val_ = hex(val_).strip("L")
            return val_

        # Update from the hash segment
        for attr in self.signing_attribute_names:
            if attr in excluded:
                # Exclude attributes that are not derived from hash segment.
                continue
            val = hash_segment_data.get(attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

        if cert_data:
            # Get exponent from cert
            cert = AttestationCertificate(cert_data)
            self.cert_text = cert.cert_text
            for attr_name in self.SIGNATURE_ATTRIBUTES:
                attr = cert.sign_attr_dict.get(attr_name, None)
                setattr(self, attr_name, attr)

        # Determine platform_binding
        if (self.metadata_major_version, self.metadata_minor_version) in [(METADATA_MAJOR_VERSION_0,
                                                                           METADATA_MINOR_VERSION_0),
                                                                          (None, None)]:
            # MBN v3, MBN v5, and MBN v6 Metadata v0.0 only support binding to soc_hw_version/soc_vers or msm_part
            if self.in_use_soc_hw_version == 1:
                self.platform_binding = PLATFORM_BINDING_SOC_VERS
            else:
                self.platform_binding = PLATFORM_BINDING_JTAG_ID
        elif (self.metadata_major_version, self.metadata_minor_version) == (METADATA_MAJOR_VERSION_1,
                                                                            METADATA_MINOR_VERSION_0):
            # MBN v6 Metadata v1.0 supports binding to soc_hw_version/soc_vers, msm_part, or neither
            if self.in_use_soc_hw_version == 1 and self.in_use_hw_id == 0:
                self.platform_binding = PLATFORM_BINDING_SOC_VERS
            elif self.in_use_soc_hw_version == 0 and self.in_use_hw_id == 1:
                self.platform_binding = PLATFORM_BINDING_JTAG_ID
            else:
                self.platform_binding = PLATFORM_BINDING_INDEPENDENT

        self.from_hash_segment = True

    def compare_lists(self, config_attr, attr, dup_in_cert_is_error, missing_is_error):
        if attr == 'multi_serial_numbers':
            config_list = getattr(config_attr, attr).serial if getattr(config_attr, attr) is not None else []
        else:
            config_list = getattr(config_attr, attr) if getattr(config_attr, attr) is not None else []
        cert_list = getattr(self, attr) if getattr(self, attr) is not None else []

        # Lower case config items to match case of extracted subject returned by get_subject_list_from_text
        for index, item in enumerate(config_list):
            config_list[index] = item.lower()

        # Lists are empty so they match
        if config_list == cert_list == []:
            return True, None, None

        # Items in config match those in cert
        cert_without_duplicates = remove_duplicates(cert_list)
        if len(cert_list) == len(cert_without_duplicates) and set(config_list) == set(cert_without_duplicates):
            return True, None, None

        # Lists don't match. Determine which items are duplicated or missing
        cert_errors = []
        config_errors = []

        # Determine if duplicates exist in certificate
        if dup_in_cert_is_error:
            duplicates = get_duplicates(cert_list)
            if len(duplicates) > 0:
                for item in duplicates:
                    config_errors.append('No duplicate')
                    cert_errors.append('Duplicate: ' + item)

        if missing_is_error:
            # Determine items in cert that are missing from config
            for item in cert_list:
                if item not in config_list:
                    config_errors.append('Missing')
                    cert_errors.append(item)

            # Determine items in config that are missing from cert
            for item in config_list:
                if item not in cert_list:
                    config_errors.append(item)
                    cert_errors.append('Missing')

        return False, cert_errors, config_errors

    def compare(self, other, exclude=()):
        mismatch = []

        for attr in self.signing_attribute_names:
            if attr in exclude:
                continue

            # Skip validating values which are abstracted by platform_binding value
            if attr in SigningAttributes.EXCLUDE_PLATFORM_BINDING:
                continue

            # If platform_binding values mismatch, skip comparing values which are conditionally validated by Secboot
            # based on the values of flags abstracted by platform_binding
            if self.platform_binding != other.platform_binding:
                if attr in SigningAttributes.EXCLUDE_PLATFORM_BINDING_MISMATCH:
                    continue
                if PLATFORM_BINDING_INDEPENDENT in [self.platform_binding, other.platform_binding]:
                    if attr in SigningAttributes.EXCLUDE_PLATFORM_BINDING_INDEPENDENT_MISMATCH:
                        continue

            # Handle comparison of debug serials
            if attr in ['multi_serial_numbers', 'soc_vers']:
                match, cert_errors, config_errors = self.compare_lists(other, attr, True, True)
                if not match:
                    mismatch.append((attr, cert_errors, config_errors))
                continue

            value_self = getattr(self, attr)
            value_other = getattr(other, attr)

            def clean_value(attr_, value):
                if isinstance(value, str):
                    if attr_ == 'platform_binding':
                        value = value.upper()
                    else:
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
