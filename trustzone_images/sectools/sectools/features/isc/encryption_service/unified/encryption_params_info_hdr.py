# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from six import int2byte
from sectools.common.utils.c_logging import logger


class EncryptionParamsInfoHdr_1_0(object):

    class Defines(object):
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

        def __init__(self, defines, section_num=None, eps_hdr_blob=None):
            if eps_hdr_blob is None:
                self.major_version = defines.MAJOR_VERSION
                self.minor_version = defines.MINOR_VERSION
                self.offset = EncryptionParamsInfoHdr_1_0.Defines.HEADER_SIZE_BYTES + \
                              section_num * EncryptionParamsInfoHdr_1_0.Defines.SECTION_SIZE_BYTES
                logger.debug("Created v{0}.{1} Encryption Parameter Section Header".format(self.major_version,
                                                                                           self.minor_version))
            else:
                self._decode_binary_blob(defines, eps_hdr_blob)

        def binary_blob(self):
            blob = struct.pack('=Hcc',
                               self.offset,
                               int2byte(self.major_version),
                               int2byte(self.minor_version))
            return blob

        def _decode_binary_blob(self, defines, binary_blob):
            self.offset, self.major_version, self.minor_version = struct.unpack('=HBB', binary_blob)
            logger.debug("Decoded Encryption Parameter Section Header as v{0}.{1}".format(defines.MAJOR_VERSION,
                                                                                          defines.MINOR_VERSION))
            if (self.major_version, self.minor_version) != (defines.MAJOR_VERSION, defines.MINOR_VERSION):
                raise RuntimeError("Encryption Parameter Section Header version \"{0}.{1}\" does not match expected version \"{2}.{3}\"".format(self.major_version, self.minor_version, defines.MAJOR_VERSION, defines.MINOR_VERSION))

    def __init__(self, enc_param_header_blob=None):
        if enc_param_header_blob is None:
            self._create_new_header()
        else:
            self._decode_binary_header_blob(enc_param_header_blob)

    def _decode_binary_enc_param_header(self, binary_blob):
        pass

    def _create_new_header(self):
            self.magic_number = EncryptionParamsInfoHdr_1_0.Defines.MAGIC_NUM
            self.number_of_eps = 0
            self.eps = []

    def _generate_binary_blob(self):
        encryption_params_hdr_blob = struct.pack("=Icccc",
                                                 self.magic_number,
                                                 int2byte(self.number_of_eps),
                                                 int2byte(EncryptionParamsInfoHdr_1_0.Defines.RSVD_BYTE),
                                                 int2byte(EncryptionParamsInfoHdr_1_0.Defines.RSVD_BYTE),
                                                 int2byte(EncryptionParamsInfoHdr_1_0.Defines.RSVD_BYTE))
        for eps in self.eps:
            encryption_params_hdr_blob += eps.binary_blob()

        if len(encryption_params_hdr_blob) < EncryptionParamsInfoHdr_1_0.Defines.HEADER_SIZE_BYTES:
            return self._pad(encryption_params_hdr_blob,
                             EncryptionParamsInfoHdr_1_0.Defines.HEADER_SIZE_BYTES - len(encryption_params_hdr_blob))

        return encryption_params_hdr_blob

    def _pad(self, destination_string, num_bytes):
        pad_str = int2byte(0) * num_bytes
        return destination_string + pad_str

    def get_header_blob(self):
        if self.validate_header() is True:
            return self._generate_binary_blob()
        else:
            raise RuntimeError("Encryption params header is Not valid")

    def validate_header(self):
        return 0 < len(self.eps) <= EncryptionParamsInfoHdr_1_0.Defines.MAX_EPS_SUPPORTED

    def add_encryption_param_section(self):
        if self.number_of_eps < EncryptionParamsInfoHdr_1_0.Defines.MAX_EPS_SUPPORTED:
            self.number_of_eps += 1
            self.eps.append(EncryptionParamsInfoHdr_1_0.EpsFields(self.get_defines(), section_num=self.number_of_eps - 1))

    def _decode_binary_header_blob(self, enc_param_header_blob):
        if len(enc_param_header_blob) != EncryptionParamsInfoHdr_1_0.Defines.HEADER_SIZE_BYTES:
            raise RuntimeError("Encryption parameters blob is of the wrong size")

        string_offset = 0
        string_end = EncryptionParamsInfoHdr_1_0.Defines.NUM_EPS_FLD_SIZE_BYTES + EncryptionParamsInfoHdr_1_0.Defines.MAGIC_NUM_FLD_SIZE_BYTES
        self.magic_number, self.number_of_eps = struct.unpack('=IB', enc_param_header_blob[string_offset:string_end])
        if self.magic_number != EncryptionParamsInfoHdr_1_0.Defines.MAGIC_NUM:
            raise RuntimeError("Image's encryption parameters has invalid magic number: \"{0}\".".format(hex(self.magic_num)))
        if self.number_of_eps <= 0 or self.number_of_eps > EncryptionParamsInfoHdr_1_0.Defines.MAX_EPS_SUPPORTED:
            raise RuntimeError("The number of EPS in blob is out of range")

        string_offset = string_end + EncryptionParamsInfoHdr_1_0.Defines.ENC_HDR_RSVD_FLD_BYTE_SIZE

        self.eps = []
        for eps in range(self.number_of_eps):
            self.eps.append(EncryptionParamsInfoHdr_1_0.EpsFields(self.get_defines(), eps_hdr_blob=enc_param_header_blob[string_offset + EncryptionParamsInfoHdr_1_0.Defines.EPS_FLD_LINE_SIZE_BYTES * eps: string_offset + EncryptionParamsInfoHdr_1_0.Defines.EPS_FLD_LINE_SIZE_BYTES * eps + EncryptionParamsInfoHdr_1_0.Defines.EPS_FLD_LINE_SIZE_BYTES]))

    @staticmethod
    def get_defines():
        return EncryptionParamsInfoHdr_1_0.Defines

    @classmethod
    def byte_sequence(cls):
        defines = cls.get_defines()
        return struct.pack(
            "=IccccHccI",
            defines.MAGIC_NUM,
            int2byte(defines.MAX_EPS_SUPPORTED - 1),
            int2byte(defines.RSVD_BYTE),
            int2byte(defines.RSVD_BYTE),
            int2byte(defines.RSVD_BYTE),
            defines.HEADER_SIZE_BYTES,
            int2byte(defines.MAJOR_VERSION),
            int2byte(defines.MINOR_VERSION),
            defines.RSVD_BYTE)



class EncryptionParamsInfoHdr_2_0(EncryptionParamsInfoHdr_1_0):

    class Defines(EncryptionParamsInfoHdr_1_0.Defines):
        MAJOR_VERSION = 2

    @staticmethod
    def get_defines():
        return EncryptionParamsInfoHdr_2_0.Defines
