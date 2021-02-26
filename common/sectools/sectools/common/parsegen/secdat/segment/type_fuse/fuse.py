# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Aug 15, 2014

@author: hraghav
'''

from sectools.common.parsegen.secdat.segment.type_fuse.defines import FUSEPROV_REGION_ETYPE_DICT,\
    FUSEPROV_OPERATION_ETYPE_DICT
from sectools.common.utils.c_misc import TablePrinter
from sectools.common.utils.struct_base import StructBase


class FuseEntry(StructBase):
    """Fuse Common Header Class"""

    def _unpack_data_list(self, unpacked):
        self.region = unpacked[0]
        self.address = unpacked[1]
        self.lsb = unpacked[2]
        self.msb = unpacked[3]
        self.operation = unpacked[4]

    def _pack_data_list(self):
        return[self.region,
               self.address,
               self.lsb,
               self.msb,
               self.operation,]

    @classmethod
    def get_format(self):
        return ('I' * 5)

    def validate(self):
        # Check the region type is supported
        if self.region not in FUSEPROV_REGION_ETYPE_DICT.keys():
            raise RuntimeError('Invalid region type in the fuse: ' + str(self.region))

        # Check the operation is supported
        if self.operation not in FUSEPROV_OPERATION_ETYPE_DICT.keys():
            raise RuntimeError('Invalid operation in the fuse: ' + str(self.operation))


def extract_fuse_entries(data, hdr):
    retval = {}

    fuse_entry_class = FuseEntry
    fuse_entry_size = fuse_entry_class.get_size()
    for idx in range(hdr.fuse_count):
        fuse_entry_data = data[:fuse_entry_size]
        data = data[fuse_entry_size:]

        # Ensure data is of fuse entry length
        if len(fuse_entry_data) < fuse_entry_class.get_size():
            raise RuntimeError('Data length "' + str(len(fuse_entry_data)) + '" is less than ' +
                               'fuse entry length "' + str(fuse_entry_class.get_size()) + '"')
        fuse_entry = fuse_entry_class(fuse_entry_data)

        # Ensure the entry is not repeated
        if fuse_entry.address in retval:
            raise RuntimeError('Fuse: ' + hex(fuse_entry.address) + ' occurs multiple times in the segment.')
        retval[fuse_entry.address] = fuse_entry

    return retval

def pack_fuse_entries(fuse_entries):
    data = ''
    for fuse in sorted(fuse_entries.values(), key=lambda x: x.address):
        data += fuse.pack()
    return data

def repr_fuse_entries(fuse_entries):
    retval = TablePrinter()

    COLUMN_SNO = 0
    COLUMN_ADDR = 1
    COLUMN_OP = 2
    COLUMN_MSB = 3
    COLUMN_LSB = 4
    COLUMN_REGION = 5

    retval.insert_data(0, COLUMN_SNO, 'S.No')
    retval.insert_data(0, COLUMN_ADDR, 'Address')
    retval.insert_data(0, COLUMN_OP, 'Operation')
    retval.insert_data(0, COLUMN_MSB, 'MSB')
    retval.insert_data(0, COLUMN_LSB, 'LSB')
    retval.insert_data(0, COLUMN_REGION, 'Region Type')

    for idx, fuse in enumerate(sorted(fuse_entries.values(), key=lambda x: x.address)):
        retval.insert_data(idx + 1, COLUMN_SNO, str(idx + 1))
        retval.insert_data(idx + 1, COLUMN_ADDR, hex(fuse.address)[2:].rstrip('L').rjust(8,'0'))
        retval.insert_data(idx + 1, COLUMN_OP, FUSEPROV_OPERATION_ETYPE_DICT.get(fuse.operation))
        retval.insert_data(idx + 1, COLUMN_MSB, hex(fuse.msb)[2:].rstrip('L').rjust(8,'0'))
        retval.insert_data(idx + 1, COLUMN_LSB, hex(fuse.lsb)[2:].rstrip('L').rjust(8,'0'))
        retval.insert_data(idx + 1, COLUMN_REGION, FUSEPROV_REGION_ETYPE_DICT[fuse.region])

    return retval.get_data()
