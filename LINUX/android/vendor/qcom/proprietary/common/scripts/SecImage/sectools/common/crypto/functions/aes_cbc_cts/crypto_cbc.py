# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Oct 26, 2014

@author: hraghav
'''

from sectools.common.crypto.functions.aes_cbc_cts import AesCbcCtsBase
from sectools.common.crypto.functions.common import run_command
from sectools.common.utils import c_path


class AesCbcCtsCryptoCbcImpl(AesCbcCtsBase):

    def __init__(self, module):
        self.crypto_cbc = module

    def encrypt(self, message, key, iv):
        """ Function to encrypt binary with a CBC 128 bit cipher.
        input:
        binary_blob: Binary blob to encrypt
        hex_preexisting_128_bit_key: hex representarion of 128bit key | None,
        if None, the key is generated

        hex_preexisting_iv: hex representarion of image IV | None,
        if None, the IV is generated

        output:
        (encrypted_binary, encryption_key, image_iv): Tuple with the encrypted binary, the key, and the IV
        """
        message_path, encrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            encrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_cbc, '--operation=encrypt',
                   '--input-file', message_path,
                   '--output', encrypted_message_path,
                   '--key', key,
                   '--iv', iv,]
            run_command(cmd)
            return c_path.load_data_from_file(encrypted_message_path)
        finally:
            if message_path is not None:
                c_path.remove_tmp_file(message_path)
            if encrypted_message_path is not None:
                c_path.remove_tmp_file(encrypted_message_path)

    def decrypt(self, message, key, iv):
        """ Function to decrypt a CBC encrypted binary.
        input:
        encrypted_blob: Encrypted Binary blob to decrypt
        hex_preexisting_128_bit_key: hex representarion of 128bit key

        hex_preexisting_iv: hex representarion of image IV

        output:
        plaintext_binary: Decrypted plaintext binary
        """
        message_path, decrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            decrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_cbc, '--operation=decrypt',
                   '--input-file', message_path,
                   '--output', decrypted_message_path,
                   '--key', key,
                   '--iv', iv,]
            run_command(cmd)
            return c_path.load_data_from_file(decrypted_message_path)
        finally:
            if message_path is not None:
                c_path.remove_tmp_file(message_path)
            if decrypted_message_path is not None:
                c_path.remove_tmp_file(decrypted_message_path)
