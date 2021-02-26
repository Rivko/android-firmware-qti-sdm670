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

from sectools.common.crypto.functions.aes_cbc import AesCbcBase
from sectools.common.utils import c_path
from sectools.common.utils.c_process import run_command


class AesCbcOpenSSLImpl(AesCbcBase):
    def __init__(self, module):
        super(AesCbcOpenSSLImpl, self).__init__(module)
        self.openssl = module

    def encrypt(self, message, key, iv, ciphername=AesCbcBase.CIPHERNAME_128):
        message_path, encrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            encrypted_message_path = c_path.create_tmp_file()

            cmd = [self.openssl, 'enc', ciphername,
                   '-in', message_path,
                   '-out', encrypted_message_path,
                   '-K', key,
                   '-iv', iv,
                   '-nopad']
            run_command(cmd)
            return c_path.load_data_from_file(encrypted_message_path)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(encrypted_message_path)

    def decrypt(self, message, key, iv, ciphername=AesCbcBase.CIPHERNAME_128):
        message_path, decrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            decrypted_message_path = c_path.create_tmp_file()

            cmd = [self.openssl, 'enc', ciphername, '-d',
                   '-in', message_path,
                   '-K', key,
                   '-iv', iv,
                   '-out', decrypted_message_path,
                   '-nopad']
            run_command(cmd)
            return c_path.load_data_from_file(decrypted_message_path)
        finally:
            c_path.remove_tmp_file(message_path)
            c_path.remove_tmp_file(decrypted_message_path)
