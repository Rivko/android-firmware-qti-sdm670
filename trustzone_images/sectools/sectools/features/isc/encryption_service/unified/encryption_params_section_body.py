# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import binascii
import math
import os
import struct

from sectools.common import crypto
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import create_mismatch_table
from sectools.common.utils.c_misc import hexdump
from sectools.common.utils.c_misc import store_debug_data_to_file
from sectools.features.isc import advanced_defines


class EncryptionParamsSectionBody_1_0_L2_1_0(object):

    RSVD_BYTE_LEN_BYTES = 1
    ENCRYPTED_KEY_PAYLOAD_LEN_BYTES = 16
    MAC_LEN_BYTES = 16
    BASE_IV_LEN_BYTES = 16

    class B0block(object):

        B0_BLOCK_SIZE_BYTES = 16
        L1_B0_FLG_FLD_LEN_BYTES = 1
        L1_IV_FLD_LEN_BYTES = 13
        L1_B0_Q_FLD_LEN_BYTES = 2
        L1_B0_FLAGS = 0x79
        L1_B0_Q = 0x10

        def __init__(self, binary_blob=None):
            if binary_blob is None:
                self.flag_fld = self.L1_B0_FLAGS
                self.nonce_fld = self.get_iv_val()
                self.q_fld = self.L1_B0_Q
            else:
                self._decode_binary_header_blob(binary_blob)

        @classmethod
        def get_spec_size(cls):
            return cls.B0_BLOCK_SIZE_BYTES

        @classmethod
        def get_iv_val(cls):
            return os.urandom(cls.L1_IV_FLD_LEN_BYTES)

        def get_binary_blob(self):
            return self._generate_binary_blob()

        def _generate_binary_blob(self):
            binary_blob = struct.pack("=B", self.flag_fld)
            binary_blob += self.nonce_fld
            binary_blob += struct.pack(">H", self.q_fld)
            return binary_blob

        def _decode_binary_header_blob(self, binary_blob):
            if len(binary_blob) != self.B0_BLOCK_SIZE_BYTES:
                raise RuntimeError("B0 block blob is of the wrong size")

            offset = 0
            end = self.L1_B0_FLG_FLD_LEN_BYTES
            self.flag_fld, = struct.unpack("B", binary_blob[offset:end])

            offset = end
            end += self.L1_IV_FLD_LEN_BYTES
            self.nonce_fld = binary_blob[offset:end]

            offset = end
            end += self.L1_B0_Q_FLD_LEN_BYTES
            self.q_fld, = struct.unpack(">H", binary_blob[offset:end])

    class L2AssociatedData_1_0(object):

        L2_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES = 2
        MAJOR_VERSION_FLD_LEN_BYTES = 1
        MINOR_VERSION_FLD_LEN_BYTES = 1
        KEY_LADDER_LEN_FLD_LEN_BYTES = 1
        IMAGE_ID_BITMAP_FLD_LEN_BYTES = 4
        SPEC_SIZE_BYTES = 32
        L2_ASSOCIATED_DATA_SIZE = 32
        RSVD_FLD = 0
        MAJOR_VERSION = 1
        MINOR_VERSION = 0
        KEY_LADDER_LEN_FLD = 3
        IMAGE_ID_BITMAP_FLD = 0x1

        def __init__(self, image_id, binary_blob=None, validating=False):
            self.image_id = image_id
            logger.debug("Created v{0}.{1} Encryption Parameter L2 Associated Data".format(self.MAJOR_VERSION,
                                                                                           self.MINOR_VERSION))
            self.major_version = self.MAJOR_VERSION
            self.minor_version = self.MINOR_VERSION
            if binary_blob is None:
                self.l2_associated_data_size = self.L2_ASSOCIATED_DATA_SIZE
                self.key_ladder_length = self.KEY_LADDER_LEN_FLD
            else:
                self._decode_binary_blob(binary_blob, validating)

        def get_binary_blob(self):
            return self._generate_binary_blob()

        @classmethod
        def get_spec_size(cls):
            return cls.SPEC_SIZE_BYTES

        def _generate_binary_blob(self):
            binary_blob = struct.pack(">H", self.l2_associated_data_size)
            binary_blob += struct.pack("=BBBBBB",
                                       self.RSVD_FLD,
                                       self.RSVD_FLD,
                                       self.major_version,
                                       self.minor_version,
                                       self.key_ladder_length,
                                       self.RSVD_FLD)
            binary_blob += self._generate_binary_blob_image_id()
            binary_blob += self._generate_binary_blob_reserved()
            return binary_blob

        def _generate_binary_blob_image_id(self):
                if self.image_id > 31:
                    raise RuntimeError("SW_ID is too large. Maximum value allowed when encrypting/decrypting "
                                       "image is 0x1F. It is set to 0x%X" % self.image_id)
                return struct.pack("=I", 2 ** self.image_id)

        def _generate_binary_blob_reserved(self):
            return struct.pack("=QQL", self.RSVD_FLD, self.RSVD_FLD, self.RSVD_FLD)

        def _decode_binary_blob(self, binary_blob, validating):
            if len(binary_blob) != self.SPEC_SIZE_BYTES:
                raise RuntimeError("L2 Associated Data blob is of the wrong size")

            offset = 0
            end = self.L2_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES
            self.l2_associated_data_size, = struct.unpack(">H", binary_blob[offset:end])

            offset = end + EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES * 2
            end = offset + self.MAJOR_VERSION_FLD_LEN_BYTES + \
                  self.MINOR_VERSION_FLD_LEN_BYTES + \
                  self.KEY_LADDER_LEN_FLD_LEN_BYTES + \
                  EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES

            major_version, minor_version, self.key_ladder_length, tmp = struct.unpack("=BBBB",
                                                                                      binary_blob[offset:end])
            offset = end

            if (major_version, minor_version) != (self.MAJOR_VERSION, self.MINOR_VERSION):
                raise RuntimeError(("Encryption Parameters L2 Associated Data version \"{0}.{1}\" does not match "
                                    "expected version \"{2}.{3}\"\n       Ensure that the correct selected_encryptor value is "
                                    "set.".format(major_version, minor_version,
                                                  self.MAJOR_VERSION, self.MINOR_VERSION)))

            image_id = int(round(math.log(self._decode_image_id(binary_blob[offset:]), 2)))
            if image_id != self.image_id:
                if validating:
                    errstr = list()
                    mismatches = list()
                    mismatches.append(("sw_id", "0x%X" % image_id, "0x%X" % self.image_id))
                    create_mismatch_table(mismatches, errstr, operation="encryption",
                                          data_type_to_compare="Attribute", image_region="Encryption Parameters")
                    raise RuntimeError('Following validations failed for the image:\n       ' +
                                       '\n       '.join([(str(i + 1) + '. ' + e) for i, e in enumerate(errstr)]))
                else:
                    logger.warning(("Extracted Encryption Parameters sw_id"
                                    " value \"{0}\" does not match config value "
                                    "\"{1}\"\n\t Encryption Parameters sw_id value "
                                    "will be updated with value \"{1}\"".format(hex(image_id), hex(self.image_id))))

        def _decode_image_id(self, data):
            image_id_bitmap, = struct.unpack("=I", data[:self.IMAGE_ID_BITMAP_FLD_LEN_BYTES])
            return image_id_bitmap

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
        L3_ASSOCIATED_DATA_SIZE = 32
        RSVD_FLD = 0
        IMAGE_ENC_ALGO_FLD = 1
        IMAGE_ENC_MODE_FLD = 1
        ENCRYPT_ALL_SEGMENTS_FLD = 1
        NUMBER_OF_SEGMENTS_FLD = 0
        SEGMENT_BITMAP_31_0_FLD = 0
        SEGMENT_BITMAP_63_32_FLD = 0
        SEGMENT_BITMAP_95_64_FLD = 0
        SEGMENT_BITMAP_127_96_FLD = 0
        SPEC_SIZE_BYTES = 32

        def __init__(self, binary_blob=None, encrypted_segments_indices=None):
            if binary_blob is None:
                self.l3_associated_data_size = self.L3_ASSOCIATED_DATA_SIZE
                self.image_enc_algo = self.IMAGE_ENC_ALGO_FLD
                self.image_enc_mode = self.IMAGE_ENC_MODE_FLD
                self.encrypted_segments_indices = encrypted_segments_indices
                if self.encrypted_segments_indices is not None:
                        self._update_encrypted_segment_bitmap()
                else:
                    self.encrypt_all_segments = self.ENCRYPT_ALL_SEGMENTS_FLD
                    self.number_of_segments = self.NUMBER_OF_SEGMENTS_FLD
                    self.segment_bitmap_31_0 = self.SEGMENT_BITMAP_31_0_FLD
                    self.segment_bitmap_63_32 = self.SEGMENT_BITMAP_63_32_FLD
                    self.segment_bitmap_95_64 = self.SEGMENT_BITMAP_95_64_FLD
                    self.segment_bitmap_127_96 = self.SEGMENT_BITMAP_127_96_FLD
            else:
                self._decode_binary_header_blob(binary_blob)

        def get_binary_blob(self):
            return self._generate_binary_blob()

        @classmethod
        def get_spec_size(cls):
            return cls.SPEC_SIZE_BYTES

        def _generate_binary_blob(self):
            binary_blob = struct.pack(">H", self.l3_associated_data_size)
            binary_blob += struct.pack("=BBBBBBIBBBBIIII",
                                       self.RSVD_FLD,
                                       self.RSVD_FLD,
                                       self.image_enc_algo,
                                       self.image_enc_mode,
                                       self.RSVD_FLD,
                                       self.RSVD_FLD,
                                       self.RSVD_FLD,
                                       self.encrypt_all_segments,
                                       self.number_of_segments,
                                       self.RSVD_FLD,
                                       self.RSVD_FLD,
                                       self.segment_bitmap_31_0,
                                       self.segment_bitmap_63_32,
                                       self.segment_bitmap_95_64,
                                       self.segment_bitmap_127_96,
                                       )
            return binary_blob

        def _decode_binary_header_blob(self, binary_blob):
            if len(binary_blob) != self.SPEC_SIZE_BYTES:
                raise RuntimeError("L3AssociatedData block blob is of the wrong size")

            string_offset = 0
            string_end = self.L3_ASSOCIATED_DATA_SIZE_FLD_LEN_BYTES
            self.l3_associated_data_size, = struct.unpack('>H', binary_blob[string_offset:string_end])

            string_offset = string_end
            string_end = string_offset + \
                         EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES * 2 + \
                         self.IMAGE_ENC_ALGO_FLD_LEN_BYTES + \
                         self.IMAGE_ENC_MODE_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES * 2 + \
                         EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES * 4 + \
                         self.ENCRYPT_ALL_SEGMENTS_FLD_LEN_BYTES + \
                         self.NUMBER_OF_SEGMENTS_FLD_LEN_BYTES + \
                         EncryptionParamsSectionBody_1_0_L2_1_0.RSVD_BYTE_LEN_BYTES * 2 + \
                         self.SEGMENT_BITMAP_31_0_FLD_LEN_BYTES + \
                         self.SEGMENT_BITMAP_63_32_FLD_LEN_BYTES + \
                         self.SEGMENT_BITMAP_95_64_FLD_LEN_BYTES + \
                         self.SEGMENT_BITMAP_127_96_FLD_LEN_BYTES
            tmp, tmp, self.image_enc_algo, self.image_enc_mode, tmp, tmp, tmp, self.encrypt_all_segments, self.number_of_segments, tmp, tmp, self.segment_bitmap_31_0, self.segment_bitmap_63_32, self.segment_bitmap_95_64, self.segment_bitmap_127_96, = struct.unpack("=BBBBBBIBBBBIIII", binary_blob[string_offset:string_end])

        def _update_encrypted_segment_bitmap(self):
            self.encrypt_all_segments = 0
            self.number_of_segments = len(self.encrypted_segments_indices)
            self.segment_bitmap_31_0 = 0
            self.segment_bitmap_63_32 = 0
            self.segment_bitmap_95_64 = 0
            self.segment_bitmap_127_96 = 0
            for seg_idx in self.encrypted_segments_indices:
                if seg_idx <= 31:
                    self.segment_bitmap_31_0 = self.segment_bitmap_31_0 | (1 << (seg_idx % 32))
                elif 32 <= seg_idx <= 63:
                    self.segment_bitmap_63_32 = self.segment_bitmap_63_32 | (1 << (seg_idx % 32))
                elif 64 <= seg_idx <= 95:
                    self.segment_bitmap_95_64 = self.segment_bitmap_63_32 | (1 << (seg_idx % 32))
                elif 96 <= seg_idx <= 127:
                    self.segment_bitmap_127_96 = self.segment_bitmap_127_96 | (1 << (seg_idx % 32))
                else:
                    raise RuntimeError("Cannot encrypt ELF images containing more than 128 segments")

    def __init__(self,
                 image_id,
                 l1_key=None,
                 l2_key=None,
                 l3_key=None,
                 enc_l2_key=None,
                 enc_l3_key=None,
                 is_enc_key_provider=False,
                 enc_param_section_body_blob=None,
                 validating=False,
                 debug_dir=None,
                 encrypted_segments_indices=None,
                 **kwargs):
        self.image_id = image_id
        self.is_enc_key_provider = is_enc_key_provider
        if enc_param_section_body_blob is None:
            self._create_new(l1_key, l2_key, l3_key, enc_l2_key, enc_l3_key, encrypted_segments_indices, **kwargs)
        else:
            self._decode_binary_blob(enc_param_section_body_blob, validating, l1_key, debug_dir, **kwargs)

    def get_binary_blob(self):
        return self._generate_binary_blob()

    def _create_new(self, l1_key, l2_key, l3_key, enc_l2_key, enc_l3_key, encrypted_segments_indices, **kwargs):
        self.L2B0block = EncryptionParamsSectionBody_1_0_L2_1_0.B0block()
        self.L2_IV = self.L2B0block.nonce_fld
        self.L2_associated_data = self._get_l2_associated_data_class()(self.image_id).get_binary_blob()
        self.L3B0block = EncryptionParamsSectionBody_1_0_L2_1_0.B0block()
        self.L3_IV = self.L3B0block.nonce_fld
        self.L3_associated_data = EncryptionParamsSectionBody_1_0_L2_1_0.L3AssociatedData(encrypted_segments_indices=encrypted_segments_indices).get_binary_blob()
        self.l3_key = l3_key
        self.base_iv = os.urandom(EncryptionParamsSectionBody_1_0_L2_1_0.BASE_IV_LEN_BYTES)

        # determine if encrypted l2 and l3 keys are provided or need to be generated
        if None not in (l1_key, l2_key, l3_key):
            self.l1_key = l1_key
            self.l2_key = l2_key
            # generate encrypted keys
            self.L1_key_payload, self.L2_MAC = self._get_l1_encrypted_l2_key()
            self.L2_key_payload, self.L3_MAC, = self._get_l2_encrypted_l3_key()
        else:
            # get lengths of encrypted keys
            len_of_enc_l2_key = len(enc_l2_key)
            len_of_enc_l3_key = len(enc_l3_key)

            if self.is_enc_key_provider:
                if len_of_enc_l2_key == 80 and len_of_enc_l3_key == 80:
                    self.L2_wrapped = enc_l2_key
                    self.L3_wrapped = enc_l3_key
                else:
                    raise RuntimeError("Wrapped L2 key and wrapped L3 key received from "
                                       "encrypted key provider are of \nunsupported lengths "
                                       "{0} bytes and {1} bytes. They must each be 80 bytes in length."
                                       .format(len_of_enc_l2_key, len_of_enc_l3_key))
            else:
                if len_of_enc_l2_key == 32 and len_of_enc_l3_key == 32:
                    # dissect encrypted keys
                    self.L1_key_payload, self.L2_MAC = enc_l2_key[0:16], enc_l2_key[16:32]
                    self.L2_key_payload, self.L3_MAC = enc_l3_key[0:16], enc_l3_key[16:32]
                else:
                    raise RuntimeError("Encrypted L2 key and encrypted L3 key are of unsupported lengths "
                                       "{0} bytes and {1} bytes.\nThey must each be 32 bytes in length."
                                       .format(len_of_enc_l2_key, len_of_enc_l3_key))

    def _generate_binary_blob(self):
        binary_blob = b""
        if self.is_enc_key_provider:
            binary_blob += self.L2_wrapped
            binary_blob += self.L3_wrapped
            binary_blob += self.base_iv
        else:
            binary_blob += self.L2B0block.get_binary_blob()
            binary_blob += self.L2_associated_data
            binary_blob += self.L1_key_payload
            binary_blob += self.L2_MAC
            binary_blob += self.L3B0block.get_binary_blob()
            binary_blob += self.L3_associated_data
            binary_blob += self.L2_key_payload
            binary_blob += self.L3_MAC
            binary_blob += self.base_iv
        return binary_blob

    def _decode_binary_blob(self, binary_blob, validating, l1_key, debug_dir, **kwargs):
        string_offset = 0
        string_end = EncryptionParamsSectionBody_1_0_L2_1_0.B0block.get_spec_size()

        self.L2B0block = EncryptionParamsSectionBody_1_0_L2_1_0.B0block(binary_blob[string_offset:string_end])
        self.L2_IV = self.L2B0block.nonce_fld
        logger.debug("L2 IV: \n" + hexdump(self.L2_IV))

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.L2AssociatedData_1_0.get_spec_size()
        self.L2_associated_data = binary_blob[string_offset:string_end]
        self._get_l2_associated_data_class()(self.image_id, self.L2_associated_data, validating)

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.ENCRYPTED_KEY_PAYLOAD_LEN_BYTES
        self.L1_key_payload = binary_blob[string_offset:string_end]

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.MAC_LEN_BYTES
        self.L2_MAC = binary_blob[string_offset:string_end]

        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_ENCRYPTED_L2_KEY, self.L1_key_payload + self.L2_MAC, debug_dir)
        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_L2_IMAGE_IV, self.L2_IV, debug_dir)
        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_L2_ADD, self.L2_associated_data, debug_dir)

        try:
            self.l2_key = crypto.aes_ccm.decrypt(self.L1_key_payload + self.L2_MAC,
                                                 binascii.hexlify(l1_key),
                                                 binascii.hexlify(self.L2_IV),
                                                 binascii.hexlify(self.L2_associated_data))
            logger.debug("L2 Key extracted from image: \n" + hexdump(self.l2_key))
            logger.debug("Any previously generated L2 keys will be ignored.")
        except:
            raise RuntimeError("Extraction of L2 key from image failed. This can be caused by the use of an invalid L1 key.")

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.B0block.get_spec_size()
        self.L3B0block = EncryptionParamsSectionBody_1_0_L2_1_0.B0block(binary_blob[string_offset:string_end])
        self.L3_IV = self.L3B0block.nonce_fld
        logger.debug("L3 IV: \n" + hexdump(self.L3_IV))

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.L3AssociatedData.get_spec_size()
        self.L3_associated_data = binary_blob[string_offset:string_end]

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.ENCRYPTED_KEY_PAYLOAD_LEN_BYTES
        self.L2_key_payload = binary_blob[string_offset:string_end]

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.MAC_LEN_BYTES
        self.L3_MAC = binary_blob[string_offset:string_end]

        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_ENCRYPTED_L3_KEY, self.L2_key_payload + self.L3_MAC, debug_dir)
        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_L3_IMAGE_IV, self.L3_IV, debug_dir)
        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_L3_ADD, self.L3_associated_data, debug_dir)

        try:
            self.l3_key = crypto.aes_ccm.decrypt(self.L2_key_payload + self.L3_MAC,
                                                 binascii.hexlify(self.l2_key),
                                                 binascii.hexlify(self.L3_IV),
                                                 binascii.hexlify(self.L3_associated_data))
            logger.debug("L3 Key extracted from image: \n" + hexdump(self.l3_key))
            logger.debug("Any previously generated L3 keys will be ignored.")
        except:
            raise RuntimeError("Extraction of L3 key from image failed. This can be caused by the use of an invalid L1 or L2 key.")

        string_offset = string_end
        string_end += EncryptionParamsSectionBody_1_0_L2_1_0.BASE_IV_LEN_BYTES
        self.base_iv = binary_blob[string_offset:string_end]

    def _get_l1_encrypted_l2_key(self):
        pt = self.l2_key
        key = binascii.hexlify(self.l1_key)
        iv = binascii.hexlify(self.L2_IV)
        aad = binascii.hexlify(self.L2_associated_data)
        logger.debug("L1 encrypted L2 key:")
        logger.debug("PT = \n" + hexdump(pt))
        logger.debug("KEY = \n" + hexdump(key))
        logger.debug("IV = \n" + hexdump(iv))
        logger.debug("AAD = \n" + hexdump(aad))

        enc_l2_key = crypto.aes_ccm.encrypt(pt, key, iv, aad)
        return enc_l2_key[0:16], enc_l2_key[16:32]

    def _get_l2_encrypted_l3_key(self):
        pt = self.l3_key
        key = binascii.hexlify(self.l2_key)
        iv = binascii.hexlify(self.L3_IV)
        aad = binascii.hexlify(self.L3_associated_data)
        logger.debug("L2 encrypted L3 key:")
        logger.debug("PT = \n" + hexdump(pt))
        logger.debug("KEY = \n" + hexdump(key))
        logger.debug("IV = \n" + hexdump(iv))
        logger.debug("AAD = \n" + hexdump(aad))

        enc_l3_key = crypto.aes_ccm.encrypt(pt, key, iv, aad)
        return enc_l3_key[0:16], enc_l3_key[16:32]

    @classmethod
    def _get_l2_associated_data_class(cls):
        return cls.L2AssociatedData_1_0

    def get_l3_key(self):
        return self.l3_key

    def get_image_iv(self):
        return self.base_iv


class EncryptionParamsSectionBody_1_0_L2_1_1(EncryptionParamsSectionBody_1_0_L2_1_0):

    class L2AssociatedData_1_1(EncryptionParamsSectionBody_1_0_L2_1_0.L2AssociatedData_1_0):

        MINOR_VERSION = 1
        IMAGE_ID_BITMAP_FLD_LEN_BYTES = 16

        def _generate_binary_blob_image_id(self):
            if self.image_id > 127:
                raise RuntimeError("SW_ID is too large. Maximum value allowed when encrypting/decrypting "
                                   "image is 0x7F. It is set to 0x%X" % self.image_id)
            image_id_bitmap = 2 ** self.image_id
            upper_image_id_bitmap = image_id_bitmap % (2 ** 64)
            lower_image_id_bitmap = int(image_id_bitmap / (2 ** 64))
            return struct.pack("=QQ", upper_image_id_bitmap, lower_image_id_bitmap)

        def _generate_binary_blob_reserved(self):
            return struct.pack("=Q", self.RSVD_FLD)

        def _decode_image_id(self, data):
            image_id_bitmap_upper, image_id_bitmap_lower = struct.unpack("=QQ",
                                                                         data[:self.IMAGE_ID_BITMAP_FLD_LEN_BYTES])
            return image_id_bitmap_lower * (2 ** 64) + image_id_bitmap_upper

    @classmethod
    def _get_l2_associated_data_class(cls):
        return cls.L2AssociatedData_1_1


class EncryptionParamsSectionBody_2_0_L2_1_2(EncryptionParamsSectionBody_1_0_L2_1_1):

    class L2AssociatedData_1_2(EncryptionParamsSectionBody_1_0_L2_1_1.L2AssociatedData_1_1):

        MINOR_VERSION = 2

        def __init__(self, image_id, feature_id, binary_blob=None, validating=False):
            self.feature_id = feature_id
            super(EncryptionParamsSectionBody_2_0_L2_1_2.L2AssociatedData_1_2, self).__init__(image_id,
                                                                                              binary_blob=binary_blob,
                                                                                              validating=validating)

        def _generate_binary_blob_feature_id(self):
            return struct.pack("=I", self.feature_id)

        def _generate_binary_blob_reserved(self):
            return self._generate_binary_blob_feature_id() + struct.pack("=I", self.RSVD_FLD)

    def _get_l1_encrypted_l2_key(self):
        key = binascii.hexlify(self.l1_key)
        aad = binascii.hexlify(self.L2_associated_data)
        message = binascii.hexlify(self.l2_key)
        logger.debug("L1 encrypted L2 key:")
        logger.debug("KEY = \n" + hexdump(key))
        logger.debug("AAD = \n" + hexdump(aad))
        logger.debug("MSG = \n" + hexdump(message))
        return binascii.unhexlify(crypto.eciesad.encrypt(key, aad, message))

    def _create_new(self, l1_key, l2_key, l3_key, enc_l2_key, enc_l3_key, encrypted_segments_indices, **kwargs):
        self.L2_associated_data = self._get_l2_associated_data_class()(self.image_id, kwargs["feature_id"]).get_binary_blob()

        self.L3B0block = EncryptionParamsSectionBody_1_0_L2_1_0.B0block()
        self.L3_IV = self.L3B0block.nonce_fld
        self.L3_associated_data = EncryptionParamsSectionBody_1_0_L2_1_0.L3AssociatedData(encrypted_segments_indices=encrypted_segments_indices).get_binary_blob()
        self.l3_key = l3_key
        self.base_iv = os.urandom(EncryptionParamsSectionBody_1_0_L2_1_0.BASE_IV_LEN_BYTES)

        self.l1_key = l1_key
        self.l2_key = l2_key

        # generate encrypted keys
        self.cryptogram = self._get_l1_encrypted_l2_key()
        self.L2_key_payload, self.L3_MAC, = self._get_l2_encrypted_l3_key()

    def _generate_binary_blob(self):
        binary_blob = b""
        binary_blob += self.cryptogram
        binary_blob += self.L3B0block.get_binary_blob()
        binary_blob += self.L3_associated_data
        binary_blob += self.L2_key_payload
        binary_blob += self.L3_MAC
        binary_blob += self.base_iv
        return binary_blob

    def _decode_binary_blob(self, binary_blob, validating, l1_key, debug_dir, **kwargs):
        raise RuntimeError("Encryption Parameters Section Body v2.0 class does not support decoding")

    @classmethod
    def _get_l2_associated_data_class(cls):
        return cls.L2AssociatedData_1_2
