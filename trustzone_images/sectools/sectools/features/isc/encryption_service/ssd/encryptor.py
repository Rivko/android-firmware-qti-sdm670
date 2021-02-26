# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.utils import c_path
from sectools.features.isc.encryption_service.base.encryptor import BaseEncryptor

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
        if len(binary_segment) < 16 and len(binary_segment)!=0:
            raise RuntimeError("The input plaintext is less than the minimum.")
        else:
            pt_fn=c_path.create_tmp_file(data=binary_segment)
            op_fn=c_path.create_tmp_file()
            self.encryption_parameters.ssd_p.enc_segment(segment_num, pt_fn, op_fn)
            encrypted_binary_segment=c_path.load_data_from_file(op_fn)
            os.unlink(pt_fn)
            os.unlink(op_fn)
            return encrypted_binary_segment
