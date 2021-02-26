# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Apr 2, 2014

@author: hraghav
'''

from sectools.common.parsegen.secdat.header.defines import SECDAT_MAGIC_1, \
    SECDAT_MAGIC_2, SECDATVER_DESCRIPTION, SECDATVER1, SECDATVER2
from sectools.common.utils.struct_base import StructBase


class SecdatHdrCommon(StructBase):
    """Secdat Common Header Class"""

    def _unpack_data_list(self, unpacked):
        self.magic1 = unpacked[0]
        self.magic2 = unpacked[1]
        self.version = unpacked[2]
        self.datasize = unpacked[3]
        self.info = unpacked[4]
        self.segnum = None

    def _pack_data_list(self):
        return[self.magic1,
               self.magic2,
               self.version,
               self.datasize,
               self.info]

    @classmethod
    def get_format(self):
        return ('I' * 4) + ('16s')

    def validate(self):
        # Check that the magic is correct
        if self.magic1 != SECDAT_MAGIC_1:
            raise RuntimeError('Invalid magic1 in the Secdat file: ' + str(self.magic1) + '\n'
                               'This is not a Secdat file.')
        if self.magic2 != SECDAT_MAGIC_2:
            raise RuntimeError('Invalid magic2 in the Secdat file: ' + str(self.magic2) + '\n'
                               'This is not a Secdat file.')
        # Check the version is correct
        if self.version not in SECDAT_HDRS.keys():
            raise RuntimeError('Invalid version in the Secdat file: ' + str(self.version))

    def copy(self, other):
        self.magic1 = other.magic1
        self.magic2 = other.magic2
        self.version = other.version
        self.datasize = other.datasize
        self.info = other.info
        self.segnum = other.segnum


class SecdatHdrV1(SecdatHdrCommon):
    """Secdat Version 1 Header Class"""

    FIXED_NUM_SEG = 1

    def __init__(self, data=None, offset=0):
        SecdatHdrCommon.__init__(self, data=data, offset=offset)
        if data is None or len(data) <= self.size:
            self.segnum = 0

    def _unpack_data_list(self, unpacked_data):
        SecdatHdrCommon._unpack_data_list(self, unpacked_data)
        self.segnum = self.FIXED_NUM_SEG
        self.reserved = unpacked_data[5:]

    def _pack_data_list(self):
        return SecdatHdrCommon._pack_data_list(self) + list(self.reserved)

    @classmethod
    def get_format(cls):
        return SecdatHdrCommon.get_format() + 'IIII'


class SecdatHdrV2(SecdatHdrCommon):
    """Secdat Version 2 Header Class"""

    def _unpack_data_list(self, unpacked_data):
        SecdatHdrCommon._unpack_data_list(self, unpacked_data)
        self.segnum = unpacked_data[5]
        self.reserved = unpacked_data[6:]

    def _pack_data_list(self):
        return (SecdatHdrCommon._pack_data_list(self) +
                  [self.segnum] + list(self.reserved))

    @classmethod
    def get_format(cls):
        return SecdatHdrCommon.get_format() + 'IIII'


def extract_hdr(data):
    # Ensure data is of common secdat header length
    if len(data) < SecdatHdrCommon.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than common Secdat Header length "' + str(SecdatHdrCommon.get_size()) + '"')

    # Get the appropriate secdat header class
    header = SecdatHdrCommon(data)
    header_class = _get_hdr_class(header.version)

    # Ensure data is of secdat header length
    if len(data) < header_class.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than ' +
                           str(SECDATVER_DESCRIPTION[header.version]) + ' Secdat Header length "' + str(header_class.get_size()) + '"')

    # Extract the header
    header = header_class(data)
    data = data[header.get_size():]
    return data, header

def get_hdr_size(version):
    header_class = _get_hdr_class(version)
    return header_class.get_size()

def create_header(hdr, version, segnum, datasize, info=None):
    header_obj = _get_hdr_class(version)()
    header_obj.copy(hdr)
    header_obj.version = version
    header_obj.segnum = segnum
    header_obj.datasize = datasize
    if info is not None:
        header_obj.info = info
    return header_obj

def create_empty_header():
    header = SecdatHdrV1()
    header.magic1 = SECDAT_MAGIC_1
    header.magic2 = SECDAT_MAGIC_2
    header.version = SECDATVER1
    return header


SECDAT_HDRS = {
                SECDATVER1 : SecdatHdrV1,
                SECDATVER2 : SecdatHdrV2,
              }


def _get_hdr_class(version):
    # Get the appropriate header class
    header_class = SECDAT_HDRS.get(version, None)
    if header_class is None:
        supported_classes = [str(key) + '-' + str(SECDATVER_DESCRIPTION[key]) for key in SECDAT_HDRS.keys()]
        raise RuntimeError('File contains unsupported header class: ' + str(version) + '\n'
                           '    ' + 'Supported classes are: ' + str(supported_classes))
    return header_class
