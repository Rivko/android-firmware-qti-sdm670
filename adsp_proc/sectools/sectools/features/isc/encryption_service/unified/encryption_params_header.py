# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on May 20, 2014

@author: vikramn
'''
import struct


class EncryptionParamsHeader(object):
    '''
    Encryption Parameters Header
    '''

    class Defines:
        RSVD_BYTE = 0
        ENC_HDR_RSVD_FLD_BYTE_SIZE = 3
        MAGIC_NUM = 0x514D5349
        MAJOR_VERSION = 1
        MINOR_VERSION = 0
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
            if eps_hdr_blob is None:
                self.major_version = EncryptionParamsHeader.Defines.MAJOR_VERSION
                self.minor_version = EncryptionParamsHeader.Defines.MINOR_VERSION
                self.offset = EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES + section_num * EncryptionParamsHeader.Defines.SECTION_SIZE_BYTES
            else:
                self._decode_binary_blob(eps_hdr_blob)

        def binary_blob(self):
            blob = struct.pack('=Hcc',
                               self.offset,
                               chr(self.major_version),
                               chr(self.minor_version))
            return blob

        def _decode_binary_blob(self, binary_blob):
            self.offset, self.major_version, self.minor_version = struct.unpack('=HBB', binary_blob)
            if (self.major_version, self.minor_version) != (EncryptionParamsHeader.Defines.MAJOR_VERSION, EncryptionParamsHeader.Defines.MINOR_VERSION):
                raise RuntimeError("Encryption Parameter Section Header version \"{0}.{1}\" does not match expected version \"{2}.{3}\"".format(self.major_version, self.minor_version, EncryptionParamsHeader.Defines.MAJOR_VERSION, EncryptionParamsHeader.Defines.MINOR_VERSION))

    def __init__(self, enc_param_header_blob=None):
        if enc_param_header_blob is None:
            self._create_new_header()
        else:
            self._decode_binary_header_blob(enc_param_header_blob)

    def _decode_binary_enc_param_header(self, binary_blob):
        pass

    def _create_new_header(self):
            self.magic_number = EncryptionParamsHeader.Defines.MAGIC_NUM
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

        if len(encryption_params_hdr_blob) < EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES:
            return self._pad(encryption_params_hdr_blob, EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES - len(encryption_params_hdr_blob))

        return encryption_params_hdr_blob

    def _pad(self, destination_string, num_bytes):
        pad_str = chr(0) * num_bytes
        return destination_string + pad_str

    def get_header_blob(self):
        if self.validate_header() is True:
            return self._generate_binary_blob()
        else:
            raise RuntimeError("Encryption params header is Not valid")

    def validate_header(self):
        valid = True
        valid &= True if len(self.eps) > 0 and len(self.eps) <= EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED else False
        return valid

    def add_encryption_param_section(self):
        if self.number_of_eps < EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED:
            self.number_of_eps += 1
            self.eps.append(EncryptionParamsHeader.EpsFields(section_num=self.number_of_eps - 1))

    def _decode_binary_header_blob(self, enc_param_header_blob):
        if len(enc_param_header_blob) != EncryptionParamsHeader.Defines.HEADER_SIZE_BYTES:
            raise RuntimeError("Encryption parameters blob is of the wrong size")

        string_offset = 0
        string_end = EncryptionParamsHeader.Defines.NUM_EPS_FLD_SIZE_BYTES + EncryptionParamsHeader.Defines.MAGIC_NUM_FLD_SIZE_BYTES
        self.magic_number, self.number_of_eps = struct.unpack('=IB', enc_param_header_blob[string_offset:string_end])
        if self.magic_number != EncryptionParamsHeader.Defines.MAGIC_NUM:
            raise RuntimeError("Image's encryption parameters has invalid magic number: \"{0}\".".format(hex(self.magic_num)))
        if self.number_of_eps <= 0 or self.number_of_eps > EncryptionParamsHeader.Defines.MAX_EPS_SUPPORTED:
            raise RuntimeError("The number of EPS in blob is out of range")

        string_offset = string_end + EncryptionParamsHeader.Defines.ENC_HDR_RSVD_FLD_BYTE_SIZE

        self.eps = []
        for eps in range(self.number_of_eps):
            self.eps.append(EncryptionParamsHeader.EpsFields(eps_hdr_blob=enc_param_header_blob[string_offset + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES * eps : string_offset + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES * eps + EncryptionParamsHeader.Defines.EPS_FLD_LINE_SIZE_BYTES]))
