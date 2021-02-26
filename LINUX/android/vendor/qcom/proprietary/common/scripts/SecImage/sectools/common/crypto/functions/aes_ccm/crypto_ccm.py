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

from sectools.common.crypto.functions.aes_ccm import AesCcmBase
from sectools.common.crypto.functions.common import run_command
from sectools.common.utils import c_path


class AesCcmCryptoCcmImpl(AesCcmBase):

    def __init__(self, module):
        self.crypto_ccm = module

    def encrypt(self, message, key, iv, aad):
        """ Function to encrypt binary with a CCM 128 bit cipher.
        input:
        binary_blob: Binary blob to encrypt
        hex_preexisting_128_bit_key: hex representarion of 128bit key | None,
        if None, the key is generated

        hex_preexisting_iv: hex representarion of image IV | None,
        if None, the IV is generated

        hex_preexisting_aad: hex representation of Additional Authentication data needed by the algorithm

        output:
        (encrypted_binary, encryption_key, image_iv, hex_preexisting_aad): Tuple with the encrypted binary, the key, the IV, and the AAD
        """
        message_path, encrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            encrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_cbc, '--operation=encrypt',
                   '--input-file', message_path,
                   '--output', encrypted_message_path,
                   '--key', key,
                   '--iv', iv,
                   '--aad', aad]
            run_command(cmd)
            return c_path.load_data_from_file(encrypted_message_path)
        finally:
            if message_path is not None:
                c_path.remove_tmp_file(message_path)
            if encrypted_message_path is not None:
                c_path.remove_tmp_file(encrypted_message_path)

    def decrypt(self, message, key, iv, aad):
        """ Function to decrypt a CCM encrypted binary.
        input:
        encrypted_blob: Encrypted Binary blob to decrypt
        hex_preexisting_128_bit_key: hex representarion of 128bit key

        hex_preexisting_iv: hex representarion of image IV

        hex_preexisting_aad: hex representation of Additional Authentication data needed by the algorithm

        output:
        plaintext_binary: Decrypted plaintext binary
        """
        message_path, decrypted_message_path = None, None
        try:
            message_path = c_path.create_tmp_file(message)
            decrypted_message_path = c_path.create_tmp_file()

            cmd = [self.crypto_ccm, '--operation=decrypt',
                   '--input-file', message_path,
                   '--output', decrypted_message_path,
                   '--key', key,
                   '--iv', iv,
                   '--aad', aad]
            run_command(cmd)
            return c_path.load_data_from_file(decrypted_message_path)
        finally:
            if message_path is not None:
                c_path.remove_tmp_file(message_path)
            if decrypted_message_path is not None:
                c_path.remove_tmp_file(decrypted_message_path)
