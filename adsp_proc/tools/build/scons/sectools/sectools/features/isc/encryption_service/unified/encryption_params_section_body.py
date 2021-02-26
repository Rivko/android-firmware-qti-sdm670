#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os
import struct
import binascii
import math
from sectools.common.utils.c_misc import hexdump
from sectools.common.crypto import crypto_functions
from sectools.features.isc.encryption_service import key_service as key_service_module
from sectools.common.utils.c_logging import logger

class EncryptionParamsSectionBody(object):

    RSVD_BYTE_LEN_BYTES = 1
    ENCRYPTED_KEY_PAYLOAD_LEN_BYTES = 16
    MAC_LEN_BYTES = 16
    BASE_IV_LEN_BYTES = 16

    class B0block(object):

        B0_BLOCK_SIZE_BYTES = 16

        L1_B0_FLG_FLD_LEN_BYTES = 1
        L1_IV_FLD_LEN_BYTES = 13
        L1_B0_Q_FLD_LEN_BYTES = 2

        L1_B0_FLAGS_VAL = 0x79
        L1_B0_Q_VAL = 0x10

        @classmethod
        def get_spec_size(self):
            return EncryptionParamsSectionBody.B0block.B0_BLOCK_SIZE_BYTES

        @classmethod
        def get_iv_val(cls):
            return os.urandom(EncryptionParamsSectionBody.B0block.L1_IV_FLD_LEN_BYTES)
            # return "N1N1N1N1N1N1N" #os.urandom(EncryptionParamsSectionBody.B0block.L1_IV_FLD_LEN_BYTES)

        def __init__(self, binary_blob=None):
            if binary_blob == None:
                self.flag_fld = EncryptionParamsSectionBody.B0block.L1_B0_FLAGS_VAL
                self.nonce_fld = EncryptionParamsSectionBody.B0block.get_iv_val()
                self.q_fld = EncryptionParamsSectionBody.B0block.L1_B0_Q_VAL
            else:
                self._decode_binary_header_blob(binary_blob)


        def _generate_binary_blob(self):
            binary_blob = struct.pack("=B", self.flag_fld)
            binary_blob += self.nonce_fld
            binary_blob += struct.pack(">H", self.q_fld)
            return binary_blob

        def get_binary_blob(self):
            return self._generate_binary_blob()



        def _decode_binary_header_blob(self, binary_blob):
            if len(binary_blob) != EncryptionParamsSectionBody.B0block.B0_BLOCK_SIZE_BYTES:
                raise RuntimeError("B0 block blob is of the wrong size")

            string_offset = 0
            string_end = EncryptionParamsSectionBody.B0block.L1_B0_FLG_FLD_LEN_BYTES
            self.flag_fld, = struct.unpack("B", binary_blob[string_offset:string_end])

            string_offset = string_end
            string_end += EncryptionParamsSectionBody.B0block.L1_IV_FLD_LEN_BYTES
            self.nonce_fld = binary_blob[string_offset:string_end]

            string_offset = string_end
            string_end += EncryptionParamsSectionBody.B0block.L1_B0_Q_FLD_LEN_BYTES
            self.q_fld, = struct.unpack(">H", binary_blob[string_offset:string_end])

    class L2AssociatedData(object):

        L2_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES = 2
        MAJOR_VERSION_FLD_LEN_BYTES = 1
        MINOR_VERSION_FLD_LEN_BYTES = 1
        KEY_LADDER_LEN_FLD_LEN_BYTES = 1
        IMAGE_ID_BITMAP_FLD_LEN_BYTES = 4

        SPEC_SIZE_BYTES = 32
        L2_ASSOCIATED_DATA_SIZE_VAL = 32
        RSVD_FLD_VAL = 0
        MAJOR_VERSION_FLD_VAL = 1
        MINOR_VERSION_FLD_VAL = 0
        KEY_LADDER_LEN_FLD_VAL = 3
        IMAGE_ID_BITMAP_FLD_VAL = 0x1


        @classmethod
        def get_spec_size(self):
            return EncryptionParamsSectionBody.L2AssociatedData.SPEC_SIZE_BYTES

        def __init__(self, binary_blob=None, image_id_bitmap=None):
            if binary_blob == None:
                self.l2_associated_data_size = EncryptionParamsSectionBody.L2AssociatedData.L2_ASSOCIATED_DATA_SIZE_VAL
                self.major_version = EncryptionParamsSectionBody.L2AssociatedData.MAJOR_VERSION_FLD_VAL
                self.minor_version = EncryptionParamsSectionBody.L2AssociatedData.MINOR_VERSION_FLD_VAL
                self.key_ladder_length = EncryptionParamsSectionBody.L2AssociatedData.KEY_LADDER_LEN_FLD_VAL
                self.image_id_bitmap = image_id_bitmap  # EncryptionParamsSectionBody.L2AssociatedData.IMAGE_ID_BITMAP_FLD_VAL
            else:
                self._decode_binary_header_blob(binary_blob)

        def _generate_binary_blob(self):
            binary_blob = struct.pack(">H", self.l2_associated_data_size)
            if self.image_id_bitmap > 31:
                raise RuntimeError("SW_ID is too large, max value allowed is 0x1F, it is set to 0x%x" % (self.image_id_bitmap))
            binary_blob += struct.pack("=BBBBBBIIIIII",
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      self.major_version,
                                      self.minor_version,
                                      self.key_ladder_length,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      2 ** self.image_id_bitmap,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L2AssociatedData.RSVD_FLD_VAL
                                      )
            return binary_blob

        def _decode_binary_header_blob(self, binary_blob):
            if len(binary_blob) != EncryptionParamsSectionBody.L2AssociatedData.SPEC_SIZE_BYTES:
                raise RuntimeError("L2AssociatedData block blob is of the wrong size")

            string_offset = 0
            string_end = EncryptionParamsSectionBody.L2AssociatedData.L2_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES
            self.l2_associated_data_size, = struct.unpack(">H", binary_blob[string_offset:string_end])

            string_offset = string_end + EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES * 2
            string_end = string_offset + EncryptionParamsSectionBody.L2AssociatedData.MAJOR_VERSION_FLD_LEN_BYTES + \
                                         EncryptionParamsSectionBody.L2AssociatedData.MINOR_VERSION_FLD_LEN_BYTES + \
                                         EncryptionParamsSectionBody.L2AssociatedData.KEY_LADDER_LEN_FLD_LEN_BYTES + \
                                         EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES + \
                                         EncryptionParamsSectionBody.L2AssociatedData.IMAGE_ID_BITMAP_FLD_LEN_BYTES
            tmp = 0
            self.major_version, self.minor_version, self.key_ladder_length, tmp, image_id_bitmap_fld_val, = struct.unpack("=BBBBI", binary_blob[string_offset:string_end])
            self.image_id_bitmap = int(math.log(image_id_bitmap_fld_val, 2))
        def get_binary_blob(self):
            return self._generate_binary_blob()


    class L3AssociatedData(object):

        L3_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES = 2
        IMAGE_ENC_ALGO_FLD_LEN_BYTES = 1
        IMAGE_ENC_MODE_FLD_LEN_BYTES = 1
        ENCRYPT_ALL_SEGMENTS_FLD_LEN_BYTES = 1
        NUMBER_OF_SEGMENTS_FLD_LEN_BYTES = 1
        SEGMENT_BITMAP_31_0_FLD_LEN_BYTES = 4
        SEGMENT_BITMAP_63_32_FLD_LEN_BYTES = 4
        SEGMENT_BITMAP_95_64_FLD_LEN_BYTES = 4
        SEGMENT_BITMAP_127_96_FLD_LEN_BYTES = 4




        L3_ASSOCIATED_DATA_SIZE_VAL = 32
        RSVD_FLD_VAL = 0
        IMAGE_ENC_ALGO_FLD_VAL = 1
        IMAGE_ENC_MODE_FLD_VAL = 1
        ENCRYPT_ALL_SEGMENTS_FLD_VAL = 1
        NUMBER_OF_SEGMENTS_FLD_VAL = 0
        SEGMENT_BITMAP_31_0_FLD_VAL = 0
        SEGMENT_BITMAP_63_32_FLD_VAL = 0
        SEGMENT_BITMAP_95_64_FLD_VAL = 0
        SEGMENT_BITMAP_127_96_FLD_VAL = 0

        SPEC_SIZE_BYTES = 32

        @classmethod
        def get_spec_size(self):
            return EncryptionParamsSectionBody.L3AssociatedData.SPEC_SIZE_BYTES

        def __init__(self, binary_blob=None):
            if binary_blob == None:
                self.l3_associated_data_size = EncryptionParamsSectionBody.L3AssociatedData.L3_ASSOCIATED_DATA_SIZE_VAL
                self.image_enc_algo = EncryptionParamsSectionBody.L3AssociatedData.IMAGE_ENC_ALGO_FLD_VAL
                self.image_enc_mode = EncryptionParamsSectionBody.L3AssociatedData.IMAGE_ENC_MODE_FLD_VAL
                self.encrypt_all_segments = EncryptionParamsSectionBody.L3AssociatedData.ENCRYPT_ALL_SEGMENTS_FLD_VAL
                self.number_of_segments = EncryptionParamsSectionBody.L3AssociatedData.NUMBER_OF_SEGMENTS_FLD_VAL
                self.segment_bitmap_31_0 = EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_31_0_FLD_VAL
                self.segment_bitmap_63_32 = EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_63_32_FLD_VAL
                self.segment_bitmap_95_64 = EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_95_64_FLD_VAL
                self.segment_bitmap_127_96 = EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_127_96_FLD_VAL
            else:
                self._decode_binary_header_blob(binary_blob)




        def _generate_binary_blob(self):
            binary_blob = struct.pack(">H", self.l3_associated_data_size)
            binary_blob += struct.pack("=BBBBBBIBBBBIIII",
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      self.image_enc_algo,
                                      self.image_enc_mode,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      self.encrypt_all_segments,
                                      self.number_of_segments,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      EncryptionParamsSectionBody.L3AssociatedData.RSVD_FLD_VAL,
                                      self.segment_bitmap_31_0,
                                      self.segment_bitmap_63_32,
                                      self.segment_bitmap_95_64,
                                      self.segment_bitmap_127_96,
                                      )
            return binary_blob

        def get_binary_blob(self):
            return self._generate_binary_blob()


        def _decode_binary_header_blob(self, binary_blob):
            if len(binary_blob) != EncryptionParamsSectionBody.L3AssociatedData.SPEC_SIZE_BYTES:
                raise RuntimeError("L3AssociatedData block blob is of the wrong size")

            string_offset = 0
            string_end = EncryptionParamsSectionBody.L3AssociatedData.L3_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES
            self.l3_associated_data_size, = struct.unpack('>H', binary_blob[string_offset:string_end])

            string_offset = string_end
            string_end = string_offset + \
                         EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES * 2 + \
                         EncryptionParamsSectionBody.L3AssociatedData.IMAGE_ENC_ALGO_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.L3AssociatedData.IMAGE_ENC_MODE_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES * 2 + \
                         EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES * 4 + \
                         EncryptionParamsSectionBody.L3AssociatedData.ENCRYPT_ALL_SEGMENTS_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.L3AssociatedData.NUMBER_OF_SEGMENTS_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.RSVD_BYTE_LEN_BYTES * 2 + \
                         EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_31_0_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_63_32_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_95_64_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody.L3AssociatedData.SEGMENT_BITMAP_127_96_FLD_LEN_BYTES
            tmp = 0
            tmp, tmp, self.image_enc_algo, self.image_enc_mode, tmp, tmp, tmp, self.encrypt_all_segments, self.number_of_segments, tmp, tmp, self.segment_bitmap_31_0, self.segment_bitmap_63_32, self.segment_bitmap_95_64, self.segment_bitmap_127_96, = struct.unpack("=BBBBBBIBBBBIIII", binary_blob[string_offset:string_end])





    def __init__(self, enc_param_section_body_blob=None, image_id_bitmap=None, l1_key_blob=None):

        self.key_service = key_service_module.KeyService()
        if enc_param_section_body_blob is None:
            self._create_new_header(image_id_bitmap=image_id_bitmap, l1_key_blob=l1_key_blob)
        else:
            self._decode_binary_header_blob(enc_param_section_body_blob, l1_key_blob=l1_key_blob)

    def _create_new_header(self, image_id_bitmap, l1_key_blob):
        self.L2B0block = EncryptionParamsSectionBody.B0block()
        self.L2_IV = self.L2B0block.nonce_fld
        self.L2_associated_data = EncryptionParamsSectionBody.L2AssociatedData(image_id_bitmap=image_id_bitmap)
        self.l1_key = l1_key_blob
        self.l2_key = self.key_service.get_l2_key()
        self.L1_key_payload, self.L2_MAC = self._get_l1_encrypted_l2_key(self.l1_key)
        self.L3B0block = EncryptionParamsSectionBody.B0block()
        self.L3_IV = self.L3B0block.nonce_fld
        self.L3_associated_data = EncryptionParamsSectionBody.L3AssociatedData()
        self.l3_key = self.key_service.get_l3_key()
        self.L2_key_payload, self.L3_MAC, = self._get_l2_encrypted_l3_key(self.l3_key)
        self.base_iv = self._get_base_image_iv()


    def get_binary_blob(self):
        return self._generate_binary_blob()

    def _generate_binary_blob(self):
        binary_blob = ""
        binary_blob += self.L2B0block.get_binary_blob()
        binary_blob += self.L2_associated_data.get_binary_blob()
        binary_blob += self.L1_key_payload
        binary_blob += self.L2_MAC
        binary_blob += self.L3B0block.get_binary_blob()
        binary_blob += self.L3_associated_data.get_binary_blob()
        binary_blob += self.L2_key_payload
        binary_blob += self.L3_MAC
        binary_blob += self.base_iv
        return binary_blob

    def _decode_binary_header_blob(self, binary_blob, l1_key_blob):

        string_offset = 0
        string_end = EncryptionParamsSectionBody.B0block.get_spec_size()
        self.L2B0block = EncryptionParamsSectionBody.B0block(binary_blob[string_offset:string_end])
        self.L2_IV = self.L2B0block.nonce_fld
        logger.debug("L2 IV: \n" + hexdump(self.L2_IV))

        string_offset = string_end
        string_end += EncryptionParamsSectionBody.L2AssociatedData.get_spec_size()
        self.L2_associated_data = EncryptionParamsSectionBody.L2AssociatedData(binary_blob[string_offset:string_end])

        string_offset = string_end
        string_end += EncryptionParamsSectionBody.ENCRYPTED_KEY_PAYLOAD_LEN_BYTES
        self.L1_key_payload = binary_blob[string_offset:string_end]


        string_offset = string_end
        string_end += EncryptionParamsSectionBody.MAC_LEN_BYTES
        self.L2_MAC = binary_blob[string_offset:string_end]

        self.l2_key = crypto_functions.ccm_decrypt_binary(self.L1_key_payload + self.L2_MAC,
                                                          binascii.hexlify(l1_key_blob),
                                                          binascii.hexlify(self.L2_IV),
                                                          binascii.hexlify(self.L2_associated_data.get_binary_blob()))
        logger.debug("decrypted L2 key: \n" + hexdump(self.l2_key))
        string_offset = string_end
        string_end += EncryptionParamsSectionBody.B0block.get_spec_size()
        self.L3B0block = EncryptionParamsSectionBody.B0block(binary_blob[string_offset:string_end])
        self.L3_IV = self.L3B0block.nonce_fld
        logger.debug("L3 IV: \n" + hexdump(self.L3_IV))

        string_offset = string_end
        string_end += EncryptionParamsSectionBody.L3AssociatedData.get_spec_size()
        self.L3_associated_data = EncryptionParamsSectionBody.L3AssociatedData(binary_blob[string_offset:string_end])

        string_offset = string_end
        string_end += EncryptionParamsSectionBody.ENCRYPTED_KEY_PAYLOAD_LEN_BYTES
        self.L2_key_payload = binary_blob[string_offset:string_end]

        string_offset = string_end
        string_end += EncryptionParamsSectionBody.MAC_LEN_BYTES
        self.L3_MAC = binary_blob[string_offset:string_end]

        self.l3_key = crypto_functions.ccm_decrypt_binary(self.L2_key_payload + self.L3_MAC,
                                                          binascii.hexlify(self.l2_key),
                                                          binascii.hexlify(self.L3_IV),
                                                          binascii.hexlify(self.L3_associated_data.get_binary_blob()))

        logger.debug("decrypted L3 key: \n" + hexdump(self.l3_key))
        string_offset = string_end
        string_end += EncryptionParamsSectionBody.BASE_IV_LEN_BYTES
        self.base_iv = binary_blob[string_offset:string_end]


    def _get_l1_encrypted_l2_key(self, l1_key_blob):
        pt = self.key_service.get_l2_key()
        key = binascii.hexlify(l1_key_blob)
        iv = binascii.hexlify(self.L2_IV)
        aad = binascii.hexlify(self.L2_associated_data.get_binary_blob())
        logger.debug("L1 encrypted L2 key:")
        logger.debug("PT = \n" + hexdump(pt))
        logger.debug("KEY = \n" + hexdump(key))
        logger.debug("IV = \n" + hexdump(iv))
        logger.debug("AAD = \n" + hexdump(aad))

        (enc_l2_key, encryption_key, image_iv, hex_preexisting_aad) = crypto_functions.ccm_encrypt_binary(pt,
                                                                                                          key,
                                                                                                          iv,
                                                                                                          aad)
        return enc_l2_key[0:16], enc_l2_key[16:32]


    def _get_l2_encrypted_l3_key(self, l3_key_pt):
        pt = l3_key_pt
        key = binascii.hexlify(self.key_service.get_l2_key())
        iv = binascii.hexlify(self.L3_IV)
        aad = binascii.hexlify(self.L3_associated_data.get_binary_blob())
        logger.debug("L2 encrypted L3 key:")
        logger.debug("PT = \n" + hexdump(pt))
        logger.debug("KEY = \n" + hexdump(key))
        logger.debug("IV = \n" + hexdump(iv))
        logger.debug("AAD = \n" + hexdump(aad))

        (enc_l3_key, encryption_key, image_iv, hex_preexisting_aad) = crypto_functions.ccm_encrypt_binary(pt,
                                                                                                          key,
                                                                                                          iv,
                                                                                                          aad)
        return enc_l3_key[0:16], enc_l3_key[16:32]

    def _get_base_image_iv(self):
        return os.urandom(EncryptionParamsSectionBody.BASE_IV_LEN_BYTES)

    def get_l3_key(self):
        return self.l3_key

    def get_image_iv(self):
        return self.base_iv
