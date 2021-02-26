# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Apr 4, 2014

@author: hraghav
'''


from sectools.common.parsegen.secdat.header.format import SECDATVER_DESCRIPTION
from sectools.common.parsegen.secdat.segment import defines as df
from sectools.common.utils.c_misc import TablePrinter
from sectools.common.utils.struct_base import StructBase


class SecdatSegment(object):
    """Secdat Version Common Segment Class"""

    def __init__(self):
        self.offset = None
        self.type = None
        self.attribute = None
        self.data = None

    def copy(self, other):
        self.offset = other.offset
        self.type = other.type
        self.attribute = other.attribute
        self.data = other.data

    @classmethod
    def supported_versions(cls):
        return df.SEG_TYPE_DESCRIPTION.keys()

    @classmethod
    def supports(cls, version):
        return version in cls.supported_versions()


class SecdatSegmentV1(StructBase, SecdatSegment):
    """Secdat Version 1 Segment Class"""

    SEG_STARTADDR_V1 = 48

    def __init__(self, data=None, offset=0):
        SecdatSegment.__init__(self)
        StructBase.__init__(self, data=data, offset=offset)

    def _unpack_data_list(self, unpacked):
        self.offset = self.SEG_STARTADDR_V1
        self.type = 0
        self.attribute = 0

    def _pack_data_list(self):
        return []

    @classmethod
    def get_format(cls):
        return ''

    @classmethod
    def supported_versions(cls):
        return [df.SEG_TYPE_EFUSE]


class SecdatSegmentV2(StructBase, SecdatSegment):
    """Secdat Version 2 Segment Class"""

    def __init__(self, data=None, offset=0):
        SecdatSegment.__init__(self)
        StructBase.__init__(self, data=data, offset=offset)

    def _unpack_data_list(self, unpacked):
        self.offset = unpacked[0]
        self.type = unpacked[1]
        self.attribute = unpacked[2]

    def _pack_data_list(self):
        return [self.offset,
                self.type,
                self.attribute]

    @classmethod
    def get_format(cls):
        return ('I' * 1) + ('H' * 2)

    @classmethod
    def supported_versions(cls):
        return df.SEG_TYPE_DESCRIPTION.keys()


SECDAT_SEGS_VER = {
                    df.SEG_VER1 : SecdatSegmentV1,
                    df.SEG_VER2 : SecdatSegmentV2,
                  }


def extract_segments(data, hdr):
    retval = []
    header_class = _get_seghdr_class(hdr.version)
    seg_hdr_size = header_class.get_size()

    # Extract the segment headers
    seghdrs = []
    for idx in range(hdr.segnum):
        seg_hdr_data = data[:seg_hdr_size]
        data = data[seg_hdr_size:]

        # Ensure data is of segment header length
        if len(seg_hdr_data) < header_class.get_size():
            raise RuntimeError('Data length "' + str(len(seg_hdr_data)) + '" is less than ' +
                               str(SECDATVER_DESCRIPTION[hdr.segnum]) + ' Secdat Segment Header length "' + str(header_class.get_size()) + '"')

        # Create the segment header class
        seghdrs.append(header_class(seg_hdr_data))

    # Create the list and dict
    seghdrs.sort(key=lambda x: x.offset)
    for idx, seghdr in enumerate(seghdrs):
        if idx == len(seghdrs) - 1:
            end_offset = len(data)
        else:
            end_offset = seghdrs[idx + 1].offset - seghdrs[idx].offset
        seghdr.data = df.SEG_TYPE_CLASS[seghdr.type](data[:end_offset])
        data = data[end_offset:]
        retval.append(seghdr)

    return retval

def create_segments(segments, version, hdr_size):
    retval = []
    segment_cls = _get_seghdr_class(version)
    base_offset = hdr_size + (len(segments) * segment_cls.get_size())
    seg_data_len = 0

    for segment in segments:
        segment_obj = segment_cls()
        if not segment_obj.supports(segment.type):
            raise RuntimeError(df.SEG_TYPE_DESCRIPTION.get(segment.type) + ' is not supported in version: ' + str(version))
        segment_obj.copy(segment)
        segment_obj.offset = base_offset + seg_data_len
        retval.append(segment_obj)
        seg_data_len += len(segment_obj.data.get_data())

    return retval

def repr_seghdrs(segments):
    retval = TablePrinter()

    COLUMN_SNO = 0
    COLUMN_OFF = 1
    COLUMN_TYPE = 2
    COLUMN_ATTR = 3

    retval.insert_data(0, COLUMN_SNO, 'S.No')
    retval.insert_data(0, COLUMN_OFF, 'Offset')
    retval.insert_data(0, COLUMN_TYPE, 'Type')
    retval.insert_data(0, COLUMN_ATTR, 'Attribute')

    for idx, segment in enumerate(segments):
        retval.insert_data(idx + 1, COLUMN_SNO, str(idx + 1))
        retval.insert_data(idx + 1, COLUMN_OFF, hex(segment.offset))
        retval.insert_data(idx + 1, COLUMN_TYPE, df.SEG_TYPE_DESCRIPTION.get(segment.type))
        retval.insert_data(idx + 1, COLUMN_ATTR, hex(segment.attribute))

    return retval.get_data()

def pack_seghdrs(segments):
    return ''.join([segment.pack() for segment in segments])

def pack_segments(segments):
    return ''.join([segment.data.get_data() for segment in segments])

def _get_seghdr_class(version):
    # Get the appropriate segment header class
    header_class = SECDAT_SEGS_VER.get(version, None)
    if header_class is None:
        supported_classes = [str(key) + '-' + str(SECDATVER_DESCRIPTION[key]) for key in SECDAT_SEGS_VER.keys()]
        raise RuntimeError('File contains unsupported segment header class: ' + str(version) + '\n'
                           '    ' + 'Supported classes are: ' + str(supported_classes))
    return header_class

