# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Mar 31, 2014

@author: hraghav
'''

from sectools.common.parsegen.secdat.segment.type_fuse import defines as df
from sectools.common.parsegen.secdat.segment.type_fuse.fuse import repr_fuse_entries, \
    extract_fuse_entries, pack_fuse_entries, FuseEntry
from sectools.common.parsegen.secdat.segment.type_fuse.header import extract_hdr, \
    create_empty_hdr, create_hdr
from sectools.common.utils.c_misc import store_debug_data_to_file


class ParseGenSegmentFuse(object):

    def __init__(self, data=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 ):

        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix

        # Read the data
        self.set_data(data)

    def __repr__(self):
        return ('Fuse Header: ' + '\n' + repr(self.hdr) + '\n'
                'Fuse Entries: ' + '\n' + repr_fuse_entries(self.fuse_entries))

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def set_data(self, data=None):
        """
        Extract the various sections of the segment data:
        1. Fuse Header
        2. Fuses
        """
        # If data is not given, initialize empty secdat
        if data is None:
            data = self._create_empty_segment()

        # Extract the fuse header
        data, self.hdr = extract_hdr(data)
        self.version = self.hdr.version
        self.store_debug_data(df.FILE_HDR_IN, self.hdr.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.hdr), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the fuses
        self.fuse_entries = extract_fuse_entries(data, self.hdr)
        self.store_debug_data(df.FILE_FUSE_ENTRIES_IN, pack_fuse_entries(self.fuse_entries))
        self.store_debug_data(df.FILE_FUSE_ENTRIES_IN_REPR, repr_fuse_entries(self.fuse_entries), suffix=df.FILE_FUSE_ENTRIES_IN_REPR_SUFFIX)

    def get_data(self, version=None):
        if version is None:
            version = self.version

        # Generate segment data binary blob
        fuse_entries_data = pack_fuse_entries(self.fuse_entries)

        # Generate header
        self.hdr = create_hdr(self.hdr, version, len(fuse_entries_data), len(self.fuse_entries.keys()))

        # Return the complete secdat
        return self.hdr.pack() + fuse_entries_data

    def get_fuse(self, address):
        if not self.has_fuse(address):
            raise RuntimeError('Cannot get fuse for address: ' + hex(address) + '. Fuse does not exist.')
        return self.fuse_entries[address]

    def has_fuse(self, address):
        return address in self.fuse_entries

    def remove_fuse(self, address):
        if not self.has_fuse(address):
            raise RuntimeError('Cannot remove fuse for address: ' + hex(address) + '. Fuse does not exist.')
        del self.fuse_entries[address]

    def add_fuse(self, region, address, operation, lsb, msb):
        fuse = FuseEntry()
        fuse.region = region
        fuse.address = address
        fuse.lsb = lsb
        fuse.msb = msb
        fuse.operation = operation
        self.fuse_entries[address] = fuse

    def clean(self):
        self.fuse_entries = {}

    def _create_empty_segment(self):
        header = create_empty_hdr()
        return header.pack()
