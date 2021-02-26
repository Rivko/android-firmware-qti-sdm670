# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
Created on Oct 26, 2014

@author: hraghav
"""

from sectools.common.crypto.functions.aes_ccm import AesCcmBase
from sectools.common.utils import c_path
from sectools.common.utils.c_process import run_command


class AesCcmCryptoCcmImpl(AesCcmBase):
    def __init__(self, module):
        super(AesCcmCryptoCcmImpl, self).__init__(module)
        self.crypto_ccm = module

    def encrypt(self, message, key, iv, aad):
        message_path, encrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            encrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_ccm,
                   '--input-file', message_path,
                   '--key', key,
                   '--iv', iv,
                   '--output', encrypted_message_path,
                   '--operation=encrypt',
                   '--aad', aad]
            run_command(cmd, log=False)
            return c_path.load_data_from_file(encrypted_message_path)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(encrypted_message_path)

    def decrypt(self, message, key, iv, aad):
        message_path, decrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            decrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_ccm,
                   '--input-file', message_path,
                   '--key', key,
                   '--iv', iv,
                   '--output', decrypted_message_path,
                   '--operation=decrypt',
                   '--aad', aad]
            output = run_command(cmd, log=False)

            # Check there is no issue with the output
            if "Caught HashVerificationFailed..." in output:
                raise RuntimeError(output)

            return c_path.load_data_from_file(decrypted_message_path)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(decrypted_message_path)
