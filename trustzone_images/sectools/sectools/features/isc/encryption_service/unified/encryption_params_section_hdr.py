# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct


class EncryptionParamsSectionHdr_1_0(object):

    class Defines(object):
        MAGIC_NUM = 0x51554945
        EPS_SIZE_BYTES = 192
        EPS_SECTION_HDR_FLD_MAGIC_NUM_SIZE_BYTES = 4
        EPS_SECTION_HDR_FLD_EPS_SIZE_SIZE_BYTES = 2
        EPS_SECTION_HDR_FLD_KEY_LADDER_ALGO_SIZE_BYTES = 1
        EPS_SECTION_HDR_FLD_KEY_LADDER_MODE_SIZE_BYTES = 1
        EPS_SECTION_HDR_FLD_ROOT_KEY_ID_SIZE_BYTES = 1
        EPS_SECTION_HDR_FLD_RSVD_SIZE_BYTES = 1
        EPS_SECTION_HDR_FLD_ROOT_KEY_INFO_ID_SIZE_BYTES = 2
        EPS_SECTION_HDR_FLD_RSVD_ROW_SIZE_BYTES = 4
        EPS_SECTION_HDR_FLD_RSVD = 0

    class RootKeyType(object):
        RTL_QTI = 0
        OTP_OEM = 1
        OTP_QTI = 2

    class KeyLadderAlgorithm(object):
        AES128 = 0
        AES256 = 1

    class AESKeyLadderMode(object):
        CCM = 1

    def __init__(self, enc_param_section_header_blob=None, root_key_type=None):
        if enc_param_section_header_blob is None:
            self._create_new_header(root_key_type)
        else:
            self._decode_binary_header_blob(enc_param_section_header_blob)

    def _create_new_header(self, root_key_type):
        self.magic_number = EncryptionParamsSectionHdr_1_0.Defines.MAGIC_NUM
        self.encryption_params_section_size = EncryptionParamsSectionHdr_1_0.Defines.EPS_SIZE_BYTES
        self.key_ladder_algo = EncryptionParamsSectionHdr_1_0.KeyLadderAlgorithm.AES256
        self.key_ladder_mode = EncryptionParamsSectionHdr_1_0.AESKeyLadderMode.CCM
        self.root_key_type = EncryptionParamsSectionHdr_1_0.RootKeyType.OTP_OEM if root_key_type is None else root_key_type
        self.root_key_info = EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD

    def _generate_binary_blob(self):
        encryption_params_section_hdr_blob = struct.pack("=IHBBBBHI",
                                                         self.magic_number,
                                                         self.encryption_params_section_size,
                                                         self.key_ladder_algo,
                                                         self.key_ladder_mode,
                                                         EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD,
                                                         self.root_key_type,
                                                         self.root_key_info,
                                                         EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD)
        return encryption_params_section_hdr_blob

    def _decode_binary_header_blob(self, encryption_params_section_hdr_blob):
        string_end = EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_MAGIC_NUM_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_EPS_SIZE_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_KEY_LADDER_ALGO_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_KEY_LADDER_MODE_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_ROOT_KEY_ID_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_ROOT_KEY_INFO_ID_SIZE_BYTES + \
                     EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD_ROW_SIZE_BYTES

        self.magic_number, self.encryption_params_section_size, self.key_ladder_algo,\
            self.key_ladder_mode, tmp, self.root_key_type, self.root_key_info, tmp, =\
            struct.unpack('=IHBBBBHI', encryption_params_section_hdr_blob[:string_end])

    def get_header_blob(self):
        if self.validate_header():
            return self._generate_binary_blob()
        else:
            raise RuntimeError("Encryption Parameters Section Header validation failed")

    def validate_header(self):
        return True


class EncryptionParamsSectionHdr_2_0(EncryptionParamsSectionHdr_1_0):

    class Defines(EncryptionParamsSectionHdr_1_0.Defines):
        EPS_SIZE_BYTES = 280

    class KeyLadderAlgorithm(EncryptionParamsSectionHdr_1_0.KeyLadderAlgorithm):
        ECIES = 2

    class ECIESKeyLadderMode(object):
        P224_HMACSHA224 = 0
        P256_HMACSHA256 = 1
        P384_HMACSHA384 = 2
        P521_HMACSHA512 = 3

    def _create_new_header(self, root_key_type):
        self.magic_number = EncryptionParamsSectionHdr_1_0.Defines.MAGIC_NUM
        self.encryption_params_section_size = EncryptionParamsSectionHdr_2_0.Defines.EPS_SIZE_BYTES
        self.l2_key_algo = EncryptionParamsSectionHdr_2_0.KeyLadderAlgorithm.ECIES
        self.l2_key_mode = EncryptionParamsSectionHdr_2_0.ECIESKeyLadderMode.P256_HMACSHA256
        self.l3_key_algo = EncryptionParamsSectionHdr_1_0.KeyLadderAlgorithm.AES256
        self.l3_key_mode = EncryptionParamsSectionHdr_1_0.AESKeyLadderMode.CCM
        self.root_key_type = EncryptionParamsSectionHdr_1_0.RootKeyType.OTP_OEM if root_key_type is None else root_key_type
        self.root_key_info = EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD

    def _generate_binary_blob(self):
        encryption_params_section_hdr_blob = struct.pack("=IHBBBBHI",
                                                         self.magic_number,
                                                         self.encryption_params_section_size,
                                                         self.l2_key_algo * 16 + self.l2_key_mode,
                                                         self.l3_key_algo * 16 + self.l3_key_mode,
                                                         EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD,
                                                         self.root_key_type,
                                                         self.root_key_info,
                                                         EncryptionParamsSectionHdr_1_0.Defines.EPS_SECTION_HDR_FLD_RSVD)
        return encryption_params_section_hdr_blob
