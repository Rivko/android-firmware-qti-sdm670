# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import binascii

from sectools.common import crypto
from sectools.features.isc.encryption_service.base.encryptor import BaseEncryptor
from sectools.features.isc.encryption_service.utility_functions import compute_segment_iv_uie


class Encryptor(BaseEncryptor):
    '''
    The Encryptor Object. Performs encryption based on unified image encryption methodology.
    '''

    def __init__(self, encryption_parameters, debug_dir=None):
        BaseEncryptor.__init__(self, encryption_parameters, debug_dir=debug_dir)

    def encrypt_segment(self, binary_segment, segment_num):
        '''
        Encrypt elf segments using cbc encryption
        input:
        binary_segment: A string representing the binary segment that needs to be encrypted.
        segment_num: The segment number, used to calculate the segment IV

        output:
        encrypted_binary_segment: CBC encrypted segment
        '''
        enc_key = self.encryption_parameters.get_image_encryption_key()

        key_size = len(enc_key)
        seg_size = len(binary_segment)
        if seg_size < key_size and seg_size != 0:
            raise RuntimeError("In order to encrypt a segment, the segment's size must be greater than or equal to the L3 encryption key's size.\n\t"
                               "Segment size: {0}B, L3 key size: {1}B".format(seg_size, key_size))

        computed_segment_iv = compute_segment_iv_uie(segment_num, self.encryption_parameters.get_image_encryption_iv())
        encryption_key, segment_iv = binascii.hexlify(enc_key), binascii.hexlify(computed_segment_iv)

        if seg_size % 16 == 0:
            encrypted_binary_segment = crypto.aes_cbc.encrypt(binary_segment, encryption_key, segment_iv)
        else:
            encrypted_binary_segment = crypto.aes_cbc_cts.encrypt(binary_segment, encryption_key, segment_iv)

        return encrypted_binary_segment
