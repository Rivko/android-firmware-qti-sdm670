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

from sectools.common.parsegen.secdat.segment.type_fuse.defines import FUSEVER_DESCRIPTION,\
    FUSEVER1
from sectools.common.utils.struct_base import StructBase


class FuseHdrCommon(StructBase):
    """Fuse Common Header Class"""

    def _unpack_data_list(self, unpacked):
        self.version = unpacked[0]
        self.seg_size = unpacked[1]
        self.fuse_count = unpacked[2]
        self.reserved = unpacked[3:]

    def _pack_data_list(self):
        return[self.version,
               self.seg_size,
               self.fuse_count] + list(self.reserved)

    @classmethod
    def get_format(self):
        return ('I' * 7)

    def validate(self):
        # Check the version is correct
        if self.version not in FUSEVER_DESCRIPTION.keys():
            raise RuntimeError('Invalid version in the fuse segment: ' + str(self.version))

    def copy(self, other):
        self.version = other.version
        self.seg_size = other.seg_size
        self.fuse_count = other.fuse_count


class FuseHdrV1(FuseHdrCommon):
    pass


FUSE_HDRS = {
                FUSEVER1 : FuseHdrV1,
            }


def extract_hdr(data):
    # Ensure data is of common fuse header length
    if len(data) < FuseHdrCommon.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than common Fuse Header length "' + str(FuseHdrCommon.get_size()) + '"')

    # Get the appropriate secdat header class
    header = FuseHdrCommon(data)
    header_class = _get_hdr_class(header.version)

    # Ensure data is of secdat header length
    if len(data) < header_class.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than ' +
                           str(FUSEVER_DESCRIPTION[header.version]) + ' Fuse Header length "' + str(header_class.get_size()) + '"')

    # Extract the header
    header = header_class(data)
    data = data[header.get_size():]
    return data, header

def get_hdr_size(version):
    header_class = _get_hdr_class(version)
    return header_class.get_size()

def create_hdr(hdr, version, size, fuse_count):
    header_obj = _get_hdr_class(version)()
    header_obj.copy(hdr)
    header_obj.version = version
    header_obj.seg_size = size
    header_obj.fuse_count = fuse_count
    return header_obj

def create_empty_hdr():
    header = FuseHdrV1()
    header.version = FUSEVER1
    return header

def _get_hdr_class(version):
    # Get the appropriate header class
    header_class = FUSE_HDRS.get(version, None)
    if header_class is None:
        supported_classes = [str(key) + '-' + str(FUSEVER_DESCRIPTION[key]) for key in FUSE_HDRS.keys()]
        raise RuntimeError('Invalid fuse header version: ' + str(version) + '\n'
                           '    ' + 'Supported versions are: ' + str(supported_classes))
    return header_class


