# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on May 20, 2014

@author: vikramn
'''
import struct
from sectools.common.utils.c_misc import hexdump
from sectools.features.isc.encryption_service.ssd.encryption_parameters import EncryptionParameters

class EncryptionParamsHeader(object):
    '''
    Encryption Parameters Header
    '''

    class Defines:
        RSVD_BYTE = 0
        ENC_HDR_RSVD_FLD_BYTE_SIZE = 3
        MAGIC_NUM = 0x514D5349
        EPS0_MAJ_VER = 1
        EPS1_MAJ_VER = 1
        EPS0_MIN_VER = 0
        EPS1_MIN_VER = 0
        EPS_HDR_STATIC_SIZE_BYTES = 8
        EPS_HDR_EPS_ROW_SIZE_BYTES = 4
        MAX_EPS_SUPPORTED = 2
        HEADER_SIZE_BYTES = EPS_HDR_STATIC_SIZE_BYTES + MAX_EPS_SUPPORTED * EPS_HDR_EPS_ROW_SIZE_BYTES
        SECTION_SIZE_BYTES = 192
        MAGIC_NUM_FLD_SIZE_BYTES = 4
        NUM_EPS_FLD_SIZE_BYTES = 1
        EPS_FLD_LINE_SIZE_BYTES = 4

    class EpsFields(object):
        def __init__(self, section_num=None, eps_hdr_blob=None):
            if eps_hdr_blob == None:
                self.offset = EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES + section_num * EncryptionParamsHeader.Defines.SECTION_SIZE_BYTES
                self.maj_version = 1
                self.min_version = 0
            else:
                self._decode_binary_blob(eps_hdr_blob)


        def binary_blob(self):
            blob = struct.pack('=Hcc',
                             self.offset,
                             chr(self.maj_version),
                             chr(self.min_version),

                             )
            return blob

        def _decode_binary_blob(self, binary_blob):
            self.offset, self.maj_version, self.min_version = struct.unpack('=HBB', binary_blob)


    def __init__(self, enc_param_header_blob=None, magic_number=None):
        '''
        '''
        if enc_param_header_blob is None:
            self._create_new_header(magic_number)
        else:
            self._decode_binary_header_blob(enc_param_header_blob)

    def _decode_binary_enc_param_header(self, binary_blob):
        pass

    def _create_new_header(self, magic_number):
            if magic_number is None:
                magic_number = EncryptionParamsHeader.Defines.MAGIC_NUM
            self.magic_number = magic_number
            self.number_of_eps = 0
            self.eps = []

    def _generate_binary_blob(self):
        encryption_params_hdr_blob = struct.pack("=Icccc",
                                                 self.magic_number,
                                                 chr(self.number_of_eps),
                                                 chr(EncryptionParamsHeader.Defines.RSVD_BYTE),
                                                 chr(EncryptionParamsHeader.Defines.RSVD_BYTE),
                                                 chr(EncryptionParamsHeader.Defines.RSVD_BYTE),
                                                 )
        for eps in self.eps:
            encryption_params_hdr_blob += eps.binary_blob()

        if len (encryption_params_hdr_blob) < EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES:
            return self._pad(encryption_params_hdr_blob, EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES - len (encryption_params_hdr_blob))


        return encryption_params_hdr_blob

    def _pad(self, destination_string, num_bytes):
        pad_str = chr(0) * num_bytes
        return destination_string + pad_str



    def get_header_blob(self):
        if self.validate_header() == True:
            return self._generate_binary_blob()
        else:
            raise RuntimeError("Encryption params header is Not valid")


    def validate_header(self):
        valid = True
        valid &= True if len(self.eps) > 0 and len(self.eps) <= EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED else False
        return valid

    def add_encryption_param_section(self):
        if(self.number_of_eps < EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED):
            self.number_of_eps += 1
            self.eps.append(EncryptionParamsHeader.EpsFields(self.number_of_eps - 1))

    def _decode_binary_header_blob(self, enc_param_header_blob):
        if len(enc_param_header_blob) != EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES:
            raise RuntimeError("Encryption parameters blob is of the wrong size")

        string_offset = 0
        string_end = EncryptionParamsHeader.Defines.NUM_EPS_FLD_SIZE_BYTES + EncryptionParamsHeader.Defines.MAGIC_NUM_FLD_SIZE_BYTES
        self.magic_number, self.number_of_eps = struct.unpack('=IB', enc_param_header_blob[string_offset:string_end])
        if(self.magic_number != EncryptionParamsHeader.Defines.MAGIC_NUM):
            raise RuntimeError("Magic number mismatch! Expected: " + hex(EncryptionParamsHeader.Defines.MAGIC_NUM) + " Found: " + hex(self.magic_number))
        if(self.number_of_eps <= 0 or self.number_of_eps > EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED):
            raise RuntimeError("The number of EPS in blob is out of range")

        string_offset = string_end + EncryptionParamsHeader.Defines.ENC_HDR_RSVD_FLD_BYTE_SIZE

        self.eps = []
        for eps in range(self.number_of_eps):
            self.eps.append(EncryptionParamsHeader.EpsFields(self, enc_param_header_blob[string_offset + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES * eps : string_offset + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES * eps + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES]))
