# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

class EncryptionParamsSectionHdr(object):
    '''
    classdocs
    '''

    class Defines(object):
        MAGIC_NUM=0x51554945
        EPS_SIZE_BYTES=192
        ALGORITHM_AES = 0x1
        KEY_LADDER_MODE_CCM=0x2
        EPS_SECTION_HDR_SIZE_BYTES=16
        EPS_SECTION_HDR_FLD_MAGIC_NUM_SIZE_BYTES=4
        EPS_SECTION_HDR_FLD_EPS_SIZE_SIZE_BYTES=2
        EPS_SECTION_HDR_FLD_KEY_LADDER_ALGO_SIZE_BYTES=1
        EPS_SECTION_HDR_FLD_KEY_LADDER_MODE_SIZE_BYTES=1
        EPS_SECTION_HDR_FLD_ROOT_KEY_ID_SIZE_BYTES=1
        EPS_SECTION_HDR_FLD_RSVD_SIZE_BYTES=1
        EPS_SECTION_HDR_FLD_ROOT_KEY_INFO_ID_SIZE_BYTES=2
        EPS_SECTION_HDR_FLD_RSVD_ROW_SIZE_BYTES=4
        EPS_SECTION_HDR_FLD_RSVD_VAL=0

    class ALGORITHM_TYPE:
        CEML_CIPHER_ALG_AES128            = 0x0
        CEML_CIPHER_ALG_AES256            = 0x1
        CEML_CIPHER_ALG_TRIPLE_DES        = 0x2

    class ALGORITHM_MODE_TYPE:
        CEML_CIPHER_MODE_ECB              = 0x0
        CEML_CIPHER_MODE_CBC              = 0x1
        CEML_CIPHER_MODE_CTR              = 0x2
        CEML_CIPHER_MODE_XTS              = 0x3
        CEML_CIPHER_MODE_CCM              = 0x4
        CEML_CIPHER_MODE_CMAC             = 0x5
        CEML_CIPHER_MODE_CTS              = 0x6

    def __init__(self, enc_param_section_header_blob=None, magic_number=None, root_key_type_id=None, root_key_info_id=None):
        '''
        '''
        if enc_param_section_header_blob is None:
            self._create_new_header(magic_number, root_key_type_id, root_key_info_id)
        else:
            self._decode_binary_header_blob(enc_param_section_header_blob)


    def _create_new_header(self, magic_number, root_key_type_id, root_key_info_id):
        if magic_number is None:
            magic_number = EncryptionParamsSectionHdr.Defines.MAGIC_NUM
        self.magic_number = magic_number
        self.encryption_params_section_size = EncryptionParamsSectionHdr.Defines.EPS_SIZE_BYTES
        self.key_ladder_algo = 1
        self.key_ladder_mode = 1
        self.root_key_type_id = root_key_type_id
        self.root_key_info_id = root_key_info_id

    def _generate_binary_blob(self):
        encryption_params_section_hdr_blob = struct.pack("=IHBBBBHI",
                                                         self.magic_number,
                                                         self.encryption_params_section_size,
                                                         self.key_ladder_algo,
                                                         self.key_ladder_mode,
                                                         EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_RSVD_VAL,
                                                         self.root_key_type_id,
                                                         self.root_key_info_id,
                                                         EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_RSVD_VAL,
                                                         )
        return encryption_params_section_hdr_blob

    def _decode_binary_header_blob(self, encryption_params_section_hdr_blob):
        string_offset = 0
        string_end =    EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_MAGIC_NUM_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_EPS_SIZE_SIZE_BYTES  + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_KEY_LADDER_ALGO_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_KEY_LADDER_MODE_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_ROOT_KEY_ID_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_RSVD_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_ROOT_KEY_INFO_ID_SIZE_BYTES + \
                        EncryptionParamsSectionHdr.Defines.EPS_SECTION_HDR_FLD_RSVD_ROW_SIZE_BYTES

        tmp=0
        self.magic_number, self.encryption_params_section_size, self.key_ladder_algo, self.key_ladder_mode, tmp, self.root_key_type_id, self.root_key_info_id, tmp,= struct.unpack('=IHBBBBHI',encryption_params_section_hdr_blob[string_offset:string_end])

    def get_header_blob(self):
        if self.validate_header()==True:
            return self._generate_binary_blob()
        else:
            raise RuntimeError("Encryption params header is Not valid")

    def validate_header(self):
        valid = True
        return valid


