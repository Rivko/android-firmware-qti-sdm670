# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json
import re

from sectools.common import crypto
from sectools.common.utils.c_data import get_duplicates, remove_duplicates
from sectools.features.isc.defines import SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0
from sectools.features.isc.signer.signerutils import get_hmac_params_from_config


class Certificate(object):

    def __init__(self):
        pass

    @classmethod
    def get_text(cls, cert_data):
        return crypto.cert.get_text(cert_data)

    @classmethod
    def get_subject_list_from_text(cls, text):
        match = re.search('Subject: (.*?)\n', text, re.IGNORECASE)
        if match is None:
            raise RuntimeError('Could not get the SUBJECT field from the attestation certificate')
        subject = match.group(1)
        return [s.strip().split('=') for s in re.split(',|/', subject.lower())]

    @classmethod
    def get_exponent_from_text(cls, text):
        match = re.search('Exponent: (\d+) \(.*?\)\n', text, re.IGNORECASE)
        """exponent is only required for RSA, not for ECDSA"""
        if match is not None:
            exponent = match.group(1)
            return int(exponent, 10)
        else:
            return None

    @classmethod
    def get_hash_algorithm(cls, subject_dict, text):
        # OU field takes precedence
        if subject_dict.get('hash_algorithm', None) is not None:
            return subject_dict['hash_algorithm']
        # OU field wasn't found so get hash algo from Signature Algorithm field
        else:
            matchs = [
                re.search('Signature Algorithm:.*\n.*Hash Algorithm: (sha[0-9]+)', text, re.IGNORECASE),
                re.search('Signature Algorithm:.*(sha[0-9]+)WithRSAEncryption', text, re.IGNORECASE),
                re.search('Signature Algorithm:.*ecdsa-with-(sha[0-9]+)', text, re.IGNORECASE)
            ]
            for match in matchs:
                if match is not None:
                    return match.group(1).lower()
            else:
                raise RuntimeError('Could not get the Hash Algorithm field from the attestation certificate')


class AttestationCertificate(Certificate):

    def __init__(self, cert_data):
        self.cert_data = cert_data
        self.cert_text = self.get_text(cert_data)
        self.subject_list = self.get_subject_list_from_text(self.cert_text)
        self.multi_serial_numbers = []
        subject_dict, ou_dict = self.process_subject_list(self.subject_list)
        self.subject_dict = subject_dict
        self.subject_dict['exponent'] = self.get_exponent_from_text(self.cert_text)
        self.subject_dict['hash_algorithm'] = self.get_hash_algorithm(self.subject_dict, self.cert_text)
        self.ou_dict = ou_dict

    def process_ou_field(self, ou_str):
        match = re.match('([^ ]+) ([^ ]+) ([^ ]+)', ou_str)
        if match is None:
            raise RuntimeError('OU field OU=' + ou_str + ' is incorrectly formatted')
        ou_number = match.group(1)
        ou_value = match.group(2)
        ou_id = match.group(3)

        # field is soc vers
        if ou_number == '11':
            match = re.match('([^ ]+) ((?:[^ ]{4} ){10})([^ ]+)', ou_str)
            if match is None:
                raise RuntimeError('OU field OU=' + ou_str + ' is incorrectly formatted')
            ou_value = match.group(2)
            ou_id = match.group(3)

        # field is serials so pattern match again to extract serials
        if ou_number == '16':
            match = re.match('([^ ]+) ((?:[^ ]{8} ){6})([^ ]+)', ou_str)
            if match is None:
                raise RuntimeError('OU field OU=' + ou_str + ' is incorrectly formatted')
            ou_value = match.group(2)
            ou_id = match.group(3)

        return ou_number, ou_value, ou_id

    def process_subject_list(self, subject_list):
        subject_dict = {}
        ou_dict = {}

        for k, v in subject_list:
            # Update OU fields
            if k == 'ou':

                # Get the OU decomposition
                ou_number, ou_value, ou_id = self.process_ou_field(v)

                # Update the multi_serial_numbers value
                if ou_id == 'sn':
                    self.multi_serial_numbers = self.multi_serial_numbers + ['0x' + i for i in ou_value.split() if i != '00000000']
                    ou_value = self.multi_serial_numbers
                    ou_id = 'multi_serial_numbers'

                # Update the soc_vers value
                if ou_id == 'soc_vers':
                    ou_value = ['0x' + i for i in ou_value.split() if i != '0000']

                # Update the OU dictionary
                ou_dict_value = {
                                    'number' : ou_number,
                                    'value'  : ou_value,
                                }
                ou_dict[ou_id] = ou_dict_value

                # Update the key and value
                k = ou_id
                v = ou_value

            # Update the sha value
            if k.startswith('sha'):
                sha_mapping = {
                                'sha1'  : '0000',
                                'sha256': '0001',
                                'sha384': '0002',
                              }
                if (k, v) not in sha_mapping.items():
                    raise RuntimeError('Invalid sha value: ' + k + ':' + v)
                v = k
                k = 'hash_algorithm'

            # Update the hex values
            elif k in ['oem_id', 'model_id', 'hw_id', 'sw_id', 'app_id',
                       'debug', 'crash_dump', 'rot_en', 'revocation_enablement',
                       'activation_enablement', 'root_revoke_activate_enable',
                       'uie_key_switch_enable']:
                v = '0x' + v

            # Update the boolean values
            elif k in ['in_use_soc_hw_version',
                       'use_serial_number_in_signing',
                       'oem_id_independent',
                       ]:
                v = int(v)

            subject_dict[k] = v

        return subject_dict, ou_dict


class SigningAttributes(object):

    ATTR_STR_DICT = \
    {
        'exponent': None,
        'hash_algorithm': None,
        'msm_part': None,
        'oem_id': None,
        'model_id': None,
        'hw_id': None,
        'sw_id': None,
        'app_id': None,
        'debug': None,
        'multi_serial_numbers': [],
        'crash_dump': None,
        'rot_en': None,
        'in_use_soc_hw_version': None,
        'soc_vers': [],
        'use_serial_number_in_signing': None,
        'oem_id_independent': None,
        'revocation_enablement': None,
        'activation_enablement': None,
        'root_revoke_activate_enable': None,
        'uie_key_switch_enable': None,
        'anti_rollback_version': None
    }

    EXCLUDE_NON_ATTEST = ['msm_part']
    EXCLUDE_HASH_SEGMENT_METADATA = EXCLUDE_NON_ATTEST + ["exponent", "hash_algorithm"]
    EXCLUDE_LIST = ['multi_serial_numbers']
    OPTIONAL_BOOLS = ['in_use_soc_hw_version',
                      'use_serial_number_in_signing',
                      'oem_id_independent',
                      ]
    OPTIONAL_STRING = ['revocation_enablement',
                       'activation_enablement',
                       'root_revoke_activate_enable',
                       'uie_key_switch_enable',
                       'rot_en',
                       'app_id',
                       'anti_rollback_version'
                      ]

    def __init__(self):
        self.exponent = None
        self.hash_algorithm = None
        self.msm_part = None
        self.oem_id = None
        self.model_id = None
        self.hw_id = None
        self.sw_id = None
        self.app_id = None
        self.debug = None
        self.multi_serial_numbers = []
        self.crash_dump = None
        self.rot_en = None
        self.in_use_soc_hw_version = None
        self.soc_vers = []
        self.use_serial_number_in_signing = None
        self.oem_id_independent = None
        self.revocation_enablement = None
        self.activation_enablement = None
        self.root_revoke_activate_enable = None
        self.uie_key_switch_enable = None
        self.anti_rollback_version = None
        self.cert_text = None
        self.from_hash_segment = False

    def sanitize(self, tag, val):
        return val

    def update_from_attest_cert(self, cert_data):
        # Create the attestation cert object
        cert = AttestationCertificate(cert_data)
        self.cert_text = cert.cert_text

        # Update from the certificate
        for attr in self.ATTR_STR_DICT:
            val = cert.subject_dict.get(attr, None)
            val = self.sanitize(attr, val)
            setattr(self, attr, val)

    def update_from_hash_segment_and_attest_cert(self, hash_segment_data, cert_data):
        def sanitize(attr, val):
            if attr in ("soc_vers", "multi_serial_numbers"):
                sanitized_list = list()
                for item in val:
                    if item != 0:
                        if attr == "soc_vers":
                            sanitized_list.append("{0:#0{1}x}".format(item, 6))
                        else:
                            sanitized_list.append("{0:#0{1}x}".format(item, 10))
                val = sanitized_list
            elif attr in ("sw_id",
                          "hw_id",
                          "oem_id",
                          "model_id",
                          "app_id",
                          "anti_rollback_version",
                          "rot_en",
                          "root_revoke_activate_enable",
                          "uie_key_switch_enable",
                          "debug") and not isinstance(val, basestring):
                val = hex(val).strip("L")
            return val

        # Update from the hash segment
        for attr in self.ATTR_STR_DICT:
            val = hash_segment_data.get(attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

        if cert_data:
            # Get exponent from cert
            cert = AttestationCertificate(cert_data)
            self.cert_text = cert.cert_text
            attr = "exponent"
            exponent = cert.subject_dict.get(attr, None)
            exponent = self.sanitize(attr, exponent)
            setattr(self, attr, exponent)

            # Get signature algorithm from cert
            attr = "hash_algorithm"
            hash_algorithm = cert.subject_dict.get(attr, None)
            hash_algorithm = self.sanitize(attr, hash_algorithm)
            setattr(self, attr, hash_algorithm)

        self.from_hash_segment = True


    def update_from_image_info_attrs(self, attrs):
        def sanitize(attr, val):
            if attr == 'hash_algorithm' and val is None:
                val = 'sha256'
            elif attr == 'hw_id':
                if attrs.secboot_version in (None, SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0):
                    val = '0x' + get_hmac_params_from_config(attrs).msm_id_str
                else:
                    val = getattr(self, "msm_part")
            val = self.sanitize(attr, val)
            return val

        # Update from the imageinfo
        for attr in self.ATTR_STR_DICT:
            val = getattr(attrs, attr, None)
            val = sanitize(attr, val)
            setattr(self, attr, val)

    def update_image_info_attrs(self, attrs):
        # Update the imageinfo
        for attr in self.ATTR_STR_DICT:
            val = getattr(self, attr)
            setattr(attrs, attr, val)

    def update_from_json(self, json_data):
        json_attrs = json.loads(json_data)
        for attr in self.ATTR_STR_DICT:
            json_attr = json_attrs.get(attr, None)
            if isinstance(json_attr, unicode):
                json_attr = str(json_attr)
            setattr(self, attr, json_attr)

    def get_dict(self, exclude=[]):
        retval = {}
        for attr in self.ATTR_STR_DICT:
            if attr in exclude:
                continue
            retval[attr] = getattr(self, attr)
        return retval

    def get_json(self, exclude=[]):
        return json.dumps(self.get_dict(exclude))

    def compare_lists(self, config_attr, attr, dup_in_cert_is_error, missing_is_error):
        is_multi_serial_numbers = attr == 'multi_serial_numbers'
        if is_multi_serial_numbers:
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

    def compare(self, other, exclude=[]):
        mismatch = []
        for attr in self.ATTR_STR_DICT:
            if attr in exclude:
                continue

            # Handle comparison of debug serials
            if attr == 'multi_serial_numbers' or attr == 'soc_vers':
                match, cert_errors, config_errors = self.compare_lists(other, attr, True, True)
                if not match:
                    mismatch.append((attr, cert_errors, config_errors))
                continue

            value_self = getattr(self, attr)
            value_other = getattr(other, attr)

            def clean_value(attr, value):
                if isinstance(value, str):
                    value = value.lower()
                    if value.startswith('0x'):
                        value = str(hex(int(value, 16))).rstrip("L")
                if attr in self.OPTIONAL_BOOLS and value is None:
                    value = 0
                if attr in self.OPTIONAL_STRING and value is None:
                    value = '0x0'
                return value

            value_self = clean_value(attr, value_self)
            value_other = clean_value(attr, value_other)
            if value_self != value_other:
                mismatch.append((attr, value_self, value_other))

        return mismatch
