# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Sep 28, 2015

@author: hraghav
'''

import binascii
import hashlib
import re

from sectools.common.crypto import cert as cert_functions, utils
from sectools.common.utils.c_logging import logger


class HMAC(object):

    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'
    HASH_ALGO_MAP = {
                        None             : hashlib.sha256,
                        HASH_ALGO_SHA256 : hashlib.sha256,
                        HASH_ALGO_SHA1   : hashlib.sha1,
                        HASH_ALGO_SHA384 : hashlib.sha384,
                    }

    IPAD = 0x3636363636363636
    OPAD = 0x5C5C5C5C5C5C5C5C

    HMAC_TYPE_QC = 'QC'
    HMAC_TYPE_SHA = 'SHA'

    def __init__(self, msm_id=None, sw_id=None, hash_algo=None):
        self._hash_algo = None

        self.msm_id = msm_id
        self.sw_id = sw_id
        self.hash_algo = hash_algo
        self.hmac_type = self.HMAC_TYPE_QC

    @property
    def hash_algo(self):
        return self._hash_algo

    @hash_algo.setter
    def hash_algo(self, val):
        if val is None:
            val = self.HASH_ALGO_SHA256
        self._hash_algo = val

    def set_hmac_type_from_config(self, signing_attributes):
        self.hmac_type = (self.HMAC_TYPE_QC if (not signing_attributes or
                                                signing_attributes.hmac is None or
                                                signing_attributes.hmac)
                          else self.HMAC_TYPE_SHA)

    def __str__(self):
        return ('msm_id : ' + hex(self.msm_id) + '\n'
                'sw_id  : ' + hex(self.sw_id) + '\n'
                'hash_algo  : ' + self.hash_algo + '\n'
                'hmac_type : ' + self.hmac_type + '\n'
                'ipad  : ' + hex(self.IPAD) + '\n'
                'opad  : ' + hex(self.OPAD))

    def is_equal(self, obj):
        if self.hmac_type == obj.hmac_type:
            return (self.hash_algo == obj.hash_algo and self.sw_id == obj.sw_id and
                    self.msm_id == obj.msm_id) or (self.hmac_type == self.HMAC_TYPE_SHA)
        return False

    def init_from_config(self, signing_attributes):
        # Short
        sa = signing_attributes

        # HW_ID MSB can be JTAG_ID or SOC_HW_VERSION
        # Use MSM_PART
        if (sa.in_use_soc_hw_version == 0 or
            (sa.msm_part is not None and sa.soc_hw_version is None)):
            msm_identifier = int(sa.msm_part, 16)

        # Use SOC_HW_VERSION
        else:
            # Solution 5: Mask is supplied
            if sa.mask_soc_hw_version is not None:
                msm_identifier = (int(sa.soc_hw_version, 16) & (int(sa.mask_soc_hw_version, 16) | 0xFFFF0000))
            # Solution 6: Mask is not supplied
            else:
                msm_identifier = (int(sa.soc_hw_version, 16) & 0xFFFFFFFF)

        # HW_ID LSB can be oem_id + model_id or serial_number
        # Use serial number
        if sa.use_serial_number_in_signing == 1:
            serial_number = (int(sa.serial_number, 16) & 0xFFFFFFFF)
            self.msm_id = msm_identifier << 32 | serial_number
        # Use oem_id and model_id
        else:
            oem_id = (int(sa.oem_id, 16) & 0xFFFF)
            model_id = (int(sa.model_id, 16) & 0xFFFF)
            self.msm_id = msm_identifier << 32 | oem_id << 16 | model_id

        # Set the internal vars
        self.sw_id = int(sa.sw_id, 16)
        self.hash_algo = sa.hash_algorithm
        self.set_hmac_type_from_config(signing_attributes)

    def init_from_cert(self, cert):
        # Extract the params from the cert
        cert_text = cert_functions.get_cert_text(cert, utils.FORMAT_PEM,
                                                 cert_functions.CERT_TEXT_FORM_X509)
        params = cert_functions.get_params_from_subject(cert_text)

        # Patterns to look for
        sw_id_re = re.compile(r'01 ([0-9A-F]{16}) SW_ID')
        hw_id_re = re.compile(r'02 ([0-9A-F]{16}) HW_ID')
        hash_algo_re = re.compile(r'07 ([0-9]{4}) SHA([0-9]+)')

        # Extract the fields
        if 'OU' in params.keys() and type(params['OU']) == list:
            params['OU'].sort()
        sw_id_element = sw_id_re.match(params['OU'][0])
        hw_id_element = hw_id_re.match(params['OU'][1])
        if len(params['OU']) > 6:
            hash_algo_element = hash_algo_re.match(params['OU'][6])
        else:
            hash_algo_element = None

        # Check if fields are missing
        if sw_id_element == None:
            logger.critical("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
            raise RuntimeError("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
        elif hw_id_element == None:
            logger.critical("Error in certificate subject. HW_ID field not found. Not a valid certificate. Exiting")
            raise RuntimeError("Error in certificate subject. HW_ID field not found. Not a valid certificate. Exiting")

        # Update the fields
        logger.debug("Valid certificate: Found SW_ID and HW_ID")
        sw_id_text = sw_id_element.group(1)
        hw_id_text = hw_id_element.group(1)
        hash_algo_text = hash_algo_element.group(2) if hash_algo_element is not None else '256'
        logger.debug('SW_ID = ' + sw_id_text)
        logger.debug('HW_ID = ' + hw_id_text)
        self.msm_id = int(hw_id_text, 16)
        self.sw_id = int(sw_id_text, 16)
        self.hash_algo = 'sha' + hash_algo_text

    def hmac_qc(self, data):
        # Set the basic parameters
        msm_id = self.msm_id
        sw_id = self.sw_id
        hash_lib_func = self.HASH_ALGO_MAP.get(self.hash_algo, None)
        if hash_lib_func is None:
            raise RuntimeError('Unknown HASH algorithm: ' + str(self.hash_algo))

        # Unhexlify the input
        def unhexlify(val):
            val = format(val, 'x')
            return binascii.unhexlify(('0' * (len(val) % 2)) + val)
        Si = unhexlify(sw_id ^ self.IPAD)
        So = unhexlify(msm_id ^ self.OPAD)

        # Step 1
        msg_step1 = hash_lib_func(data).hexdigest()
        msg_step1_bin = binascii.a2b_hex(msg_step1)
        logger.debug2("H(code image) : " + msg_step1)

        # Step 2
        msg_step2 = hash_lib_func(Si + msg_step1_bin).hexdigest()
        msg_step2_bin = binascii.a2b_hex(msg_step2)
        logger.debug2("H[(SWID^ipad) || H(code image)] : " + msg_step2)

        # Step 3
        msg_step3 = hash_lib_func(So + msg_step2_bin).hexdigest()
        msg_step3_bin = binascii.a2b_hex(msg_step3)
        logger.debug2("H[(MSMID^opad) || H[(SWID^ipad) || H(code image)]] : " + msg_step3)

        return msg_step3_bin

    def hmac_sha(self, data, sha_algo=None):
        hash_lib_func = self.HASH_ALGO_MAP.get(self.hash_algo, None)
        if hash_lib_func is None:
            raise RuntimeError('Unknown HASH algorithm: ' + str(self.hash_algo))

        # Create hash
        msg = hash_lib_func(data).hexdigest()
        logger.debug2("H(code image) : " + msg)

        return binascii.a2b_hex(msg)

    def hmac(self, data):
        hmac_map = {
            self.HMAC_TYPE_QC: self.hmac_qc,
            self.HMAC_TYPE_SHA: self.hmac_sha
        }
        # Print the input
        logger.debug(str(self))
        return hmac_map[self.hmac_type](data)
