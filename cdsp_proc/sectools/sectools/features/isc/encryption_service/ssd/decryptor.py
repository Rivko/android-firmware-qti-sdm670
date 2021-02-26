# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import binascii

from sectools.common import crypto
from sectools.features.isc.encryption_service.base.decryptor import BaseDecryptor
from sectools.features.isc.encryption_service.utility_functions import compute_segment_iv_ssd

class Decryptor(BaseDecryptor):
    '''
    Provides Unified Image Architecture specific decryption
    '''


    def __init__(self, encryption_parameters, debug_dir=None):
        BaseDecryptor.__init__(self, encryption_parameters, debug_dir=debug_dir)

    def decrypt_segment(self, encrypted_binary_segment, segment_num):
        computed_segment_iv = compute_segment_iv_ssd(segment_num, self.encryption_parameters.get_image_encryption_iv())
        ct_buf_len=len(encrypted_binary_segment)
        # how much data are we going to encrypt
        data_to_dec_len = ct_buf_len - (ct_buf_len % 16)
        data_to_dec=encrypted_binary_segment[:data_to_dec_len]
        decrypted_segment = crypto.aes_cbc.decrypt(data_to_dec, binascii.hexlify(self.encryption_parameters.get_image_encryption_key()), binascii.hexlify(computed_segment_iv))
        decrypted_segment+=encrypted_binary_segment[data_to_dec_len:]
        return decrypted_segment

