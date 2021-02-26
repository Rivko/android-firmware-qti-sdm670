# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from sectools.common.utils.struct_base import StructBase


MBN_HDR_VERSION_3 = 3
MBN_HDR_VERSION_5 = 5


class MbnHdr40B(StructBase):

    def _unpack_data_list(self, unpacked):
        self.image_id = unpacked[0]
        self.flash_parti_ver = unpacked[1]
        self.image_src = unpacked[2]
        self.image_dest_ptr = unpacked[3]
        self.image_size = unpacked[4]
        self.code_size = unpacked[5]
        self.sig_ptr = unpacked[6]
        self.sig_size = unpacked[7]
        self.cert_chain_ptr = unpacked[8]
        self.cert_chain_size = unpacked[9]

    def _pack_data_list(self):
        return [self.image_id,
                self.flash_parti_ver,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size]

    @classmethod
    def get_format(cls):
        return 'I'*10

    @staticmethod
    def supports_qc_signing():
        return False

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_3

    def get_version_in_bin(self):
        return self.flash_parti_ver

    def set_version_in_bin(self, val):
        self.flash_parti_ver = val

    def validate(self):
        pass

class MbnHdr80B(MbnHdr40B):

    def _unpack_data_list(self, unpacked):
        self.codeword = unpacked[0]
        self.magic = unpacked[1]
        self.image_id = unpacked[2]
        self.reserved_1 = unpacked[3]
        self.reserved_2 = unpacked[4]
        self.image_src = unpacked[5]
        self.image_dest_ptr = unpacked[6]
        self.image_size = unpacked[7]
        self.code_size = unpacked[8]
        self.sig_ptr = unpacked[9]
        self.sig_size = unpacked[10]
        self.cert_chain_ptr = unpacked[11]
        self.cert_chain_size = unpacked[12]
        self.reserved_3 = unpacked[13]
        self.reserved_4 = unpacked[14]
        self.reserved_5 = unpacked[15]
        self.reserved_6 = unpacked[16]
        self.reserved_7 = unpacked[17]
        self.reserved_8 = unpacked[18]
        self.reserved_9 = unpacked[19]

    def _pack_data_list(self):
        return [self.codeword,
                self.magic,
                self.image_id,
                self.reserved_1,
                self.reserved_2,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size,
                self.reserved_3,
                self.reserved_4,
                self.reserved_5,
                self.reserved_6,
                self.reserved_7,
                self.reserved_8,
                self.reserved_9]

    @classmethod
    def get_format(cls):
        return 'I'*20

    @staticmethod
    def supports_qc_signing():
        return False

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_3

    def get_version_in_bin(self):
        return None

    def set_version_in_bin(self, val):
        pass

    def validate(self):
        pass


class MbnHdr40BV5(MbnHdr40B):

    def _unpack_data_list(self, unpacked):
        self.image_id = unpacked[0]
        self.version = unpacked[1]
        self.sig_size_qc = unpacked[2]
        self.cert_chain_size_qc = unpacked[3]
        self.image_size = unpacked[4]
        self.code_size = unpacked[5]
        self.sig_ptr = unpacked[6]
        self.sig_size = unpacked[7]
        self.cert_chain_ptr = unpacked[8]
        self.cert_chain_size = unpacked[9]

    def _pack_data_list(self):
        pack_order = [self.image_id,
                      self.version,
                      self.sig_size_qc,
                      self.cert_chain_size_qc,
                      self.image_size,
                      self.code_size,
                      self.sig_ptr,
                      self.sig_size,
                      self.cert_chain_ptr,
                      self.cert_chain_size
                      ]
        return pack_order

    @staticmethod
    def supports_qc_signing():
        return True

    @staticmethod
    def get_version():
        return MBN_HDR_VERSION_5

    def get_version_in_bin(self):
        return self.version

    def set_version_in_bin(self, val):
        self.version = val

    def validate(self):
        if self.sig_size and self.get_version_in_bin() != self.get_version():
            raise RuntimeError('MBN header version (%s) does not match expected version (%s).' %
                               (self.get_version_in_bin(), self.get_version()))


"""
Map of: MbnHeaderSize -> Mbn Header Class

Dictionary containing a mapping of the class to use for parsing the mbn header
based on the header type specified
"""
MBN_HDR_OBJS = [MbnHdr80B, MbnHdr40B, MbnHdr40BV5]
MBN_HDRS = dict([((m.get_size(), m.get_version()), m) for m in MBN_HDR_OBJS])


def extract_header(data, header_size, version=MBN_HDR_VERSION_3):
    if (header_size, version) not in MBN_HDRS.keys():
        raise RuntimeError('Unexpected header size, version provided:\n'
                           '    Header size - ' + str(header_size) + ', version - ' + str(version) + '\n'
                           '    ' + 'Supported header types: ' + str(MBN_HDRS.keys()))
    header = MBN_HDRS[(header_size, version)](data)
    data = data[header_size:]
    return data, header


def create_empty_header(header_size, version=MBN_HDR_VERSION_3):
    if (header_size, version) not in MBN_HDRS.keys():
        raise RuntimeError('Unexpected header size, version provided: Header size - ' + str(header_size) +
                           ', version - ' + str(version) + '\n'
                           '    ' + 'Supported header types: ' + str(MBN_HDRS.keys()))
    header = MBN_HDRS[(header_size, version)]()
    header.set_version_in_bin(header.get_version())
    return header.pack()
