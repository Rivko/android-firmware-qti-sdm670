# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
Created on Sep 28, 2015

@author: hraghav
"""

import binascii
import hashlib
import re

from sectools.common.crypto import cert as cert_functions
from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.signerutils import get_msm_id, get_sw_id


class HMAC(object):
    HASH_ALGO_SHA1 = 'sha1'
    HASH_ALGO_SHA256 = 'sha256'
    HASH_ALGO_SHA384 = 'sha384'
    HASH_ALGO_MAP = {
        None: hashlib.sha256,
        HASH_ALGO_SHA256: hashlib.sha256,
        HASH_ALGO_SHA1: hashlib.sha1,
        HASH_ALGO_SHA384: hashlib.sha384,
    }

    IPAD = 0x3636363636363636
    OPAD = 0x5C5C5C5C5C5C5C5C

    HMAC_TYPE_QTI = 'QTI'
    HMAC_TYPE_SHA = 'SHA'

    def __init__(self, msm_id=None, sw_id=None, hash_algo=None):
        self._hash_algo = None

        self.msm_id = msm_id
        self.sw_id = sw_id
        self.hash_algo = hash_algo
        self.hmac_type = self.HMAC_TYPE_QTI

    @property
    def hash_algo(self):
        return self._hash_algo

    @hash_algo.setter
    def hash_algo(self, val):
        if val is None:
            val = self.HASH_ALGO_SHA256
        self._hash_algo = val

    def set_hmac_type_from_config(self, signing_attributes):
        self.hmac_type = (self.HMAC_TYPE_QTI if (not signing_attributes or
                                                signing_attributes.hmac is None or
                                                signing_attributes.hmac)
                          else self.HMAC_TYPE_SHA)

    def __str__(self):
        return ('msm_id : ' + hex(self.msm_id) + '\n' +
                'sw_id  : ' + hex(self.sw_id) + '\n' +
                'hash_algo  : ' + self.hash_algo + '\n' +
                'hmac_type : ' + self.hmac_type + '\n' +
                'ipad  : ' + hex(self.IPAD) + '\n' +
                'opad  : ' + hex(self.OPAD))

    def is_equal(self, obj):
        if self.hmac_type == obj.hmac_type:
            return (self.hash_algo == obj.hash_algo and self.sw_id == obj.sw_id and
                    self.msm_id == obj.msm_id) or (self.hmac_type == self.HMAC_TYPE_SHA)
        return False

    def init_from_config(self, signing_attributes):
        sa = signing_attributes
        self.msm_id = get_msm_id(sa)
        self.sw_id = get_sw_id(sa)
        self.hash_algo = sa.hash_algorithm
        self.set_hmac_type_from_config(sa)

    def init_from_cert(self, cert):
        # Extract the params from the cert
        cert_text = cert_functions.get_text(cert)
        params = cert_functions.get_subject_params(cert_functions.get_subject(cert_text))

        # Patterns to look for
        sw_id_re = re.compile(r'01 ([0-9A-F]{16}) SW_ID')
        hw_id_re = re.compile(r'02 ([0-9A-F]{16}) HW_ID')
        hash_algo_re = re.compile(r'07 ([0-9]{4}) SHA([0-9]+)')

        # Extract the fields
        if 'OU' in params.keys() and type(params['OU']) == list:
            params['OU'].sort()
        sw_id_element, hw_id_element, hash_algo_element = None, None, None
        for p in params['OU']:
            if sw_id_element is None:
                sw_id_element = sw_id_re.match(p)
            if hw_id_element is None:
                hw_id_element = hw_id_re.match(p)
            if hash_algo_element is None:
                hash_algo_element = hash_algo_re.match(p)

        # Check if fields are missing
        if sw_id_element is None:
            logger.critical("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
            raise RuntimeError("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
        elif hw_id_element is None:
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

    def init_from_image_attributes(self, extracted_attributes, signing_attributes=None):
        if extracted_attributes.from_hash_segment:
            self.msm_id = get_msm_id(signing_attributes)
            self.sw_id = get_sw_id(signing_attributes)
        else:
            self.msm_id = int(extracted_attributes.hw_id, 16)
            self.sw_id = int(extracted_attributes.sw_id, 16)
        self.hash_algo = extracted_attributes.hash_algorithm

    def hmac_qti(self, data):
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
            self.HMAC_TYPE_QTI: self.hmac_qti,
            self.HMAC_TYPE_SHA: self.hmac_sha
        }
        # Print the input
        logger.debug(str(self))
        return hmac_map[self.hmac_type](data)
