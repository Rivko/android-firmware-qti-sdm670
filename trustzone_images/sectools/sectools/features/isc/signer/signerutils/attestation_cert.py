# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import re

from .attestation_cert_base import AttestationCertificateBase


class AttestationCertificate(AttestationCertificateBase):

    def __init__(self, cert_data):
        self.multi_serial_numbers = []
        super(AttestationCertificate, self).__init__(cert_data)

    @classmethod
    def process_ou_field(cls, ou_str):
        ou_number, ou_value, ou_id = super(
            AttestationCertificate, cls).process_ou_field(ou_str)

        # field is soc_vers
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

    def get_sign_attrs(self):
        sign_attr_dict = {}
        for k, v in self.subject_list:
            # Update OU fields
            if k == 'ou':
                # Get the OU decomposition
                ou_number, ou_value, ou_id = self.process_ou_field(v)

                # Update the multi_serial_numbers value
                if ou_id == 'sn':
                    self.multi_serial_numbers = self.multi_serial_numbers + [
                        '0x' + i for i in ou_value.split() if i != '00000000']
                    ou_value = self.multi_serial_numbers
                    ou_id = 'multi_serial_numbers'

                # Update the soc_vers value
                if ou_id == 'soc_vers':
                    ou_value = ['0x' + i for i in ou_value.split() if i != '0000']

                # Update the key and value
                k, v = ou_id, ou_value
                if k.startswith('sha'):
                    # Update hash_algorithm
                    k, v = self.process_hash_algorithm(k, v)
                elif k in ['oem_id', 'model_id', 'hw_id', 'sw_id', 'app_id',
                           'debug', 'crash_dump', 'rot_en', 'revocation_enablement',
                           'activation_enablement', 'root_revoke_activate_enable',
                           'uie_key_switch_enable']:
                    # Update the hex values
                    v = '0x' + v
                elif k in ['in_use_soc_hw_version',
                           'use_serial_number_in_signing',
                           'oem_id_independent',
                           ]:
                    # Update the boolean values
                    v = int(v)
                sign_attr_dict[k] = v
        return sign_attr_dict
