# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import binascii
import hashlib

from sectools.common.utils.c_logging import logger


class Hasher(object):
    '''
    Hasher service. Provides various hashing implementations.
    '''


    def __init__(self):
        '''
        Constructor
        '''
        pass

    def get_hash(self, data, hmac_params=None, sha_algo=None):
        if hmac_params is not None:
            return self.qcom_hmac(data, hmac_params, sha_algo)
        else:
            return self.sha_hash(data, sha_algo)

    def qcom_hmac(self, data, hmac_params, sha_algo=None):
        if not data:
            raise RuntimeError('No data to hash')
        if not hmac_params:
            raise RuntimeError('Input parameters to the HMAC function are incorrect')
        else:
            msm_id = hmac_params.MSM_ID
            sw_id = hmac_params.SW_ID
            ipad = 0x3636363636363636
            opad = 0x5C5C5C5C5C5C5C5C
            if sha_algo == 'sha1':
                hashlib_calc = hashlib.sha1
            elif sha_algo == 'sha384':
                hashlib_calc = hashlib.sha384
            elif sha_algo == 'sha256' or sha_algo is None:
                hashlib_calc = hashlib.sha256
            else:
                raise RuntimeError('unknown SHA algorithm: ' + str(sha_algo))
            logger.debug("MSM_ID key : " + hex(msm_id))
            logger.debug("SW_ID key : " + hex(sw_id))
            logger.debug("ipad : " + hex(ipad))
            logger.debug("opad : " + hex(opad))

            def unhexlify(val):
                val = format(val, 'x')
                return binascii.unhexlify(('0' * (len(val) % 2)) + val)
            Si = unhexlify(sw_id ^ ipad)
            So = unhexlify(msm_id ^ opad)

            msg_step1 = hashlib_calc(data).hexdigest()
            msg_step1_bin = binascii.a2b_hex(msg_step1)
            logger.debug2("H(code image) : " + msg_step1)
            msg_step2 = hashlib_calc(Si + msg_step1_bin).hexdigest()
            msg_step2_bin = binascii.a2b_hex(msg_step2)
            logger.debug2("H[(SWID^ipad) || H(code image)] : " + msg_step2)
            msg_step3 = hashlib_calc(So + msg_step2_bin).hexdigest()
            msg_step3_bin = binascii.a2b_hex(msg_step3)
            logger.debug2("H[(MSMID^opad) || H[(SWID^ipad) || H(code image)]] : " + msg_step3)

            hmac = msg_step3_bin
            return hmac

    def sha_hash(self, data, sha_algo=None):
        if not data:
            raise RuntimeError('No data to hash')

        if sha_algo == 'sha1':
            hashlib_calc = hashlib.sha1
        elif sha_algo == 'sha256':
            hashlib_calc = hashlib.sha256
        elif sha_algo == 'sha384':
            hashlib_calc = hashlib.sha384
        else:
            logger.warning('No SHA algo specified. Defaulting to SHA-256')
            hashlib_calc = hashlib.sha256

        msg = hashlib_calc(data).hexdigest()
        msg_bin = binascii.a2b_hex(msg)
        logger.debug2("H(code image) : " + msg)

        return msg_bin
