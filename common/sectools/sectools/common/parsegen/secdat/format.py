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

import defines as df
from sectools.common.parsegen.secdat.footer.format import extract_ftr, get_ftr_size, \
    create_footer, create_empty_footer
from sectools.common.parsegen.secdat.header.format import extract_hdr, get_hdr_size, \
    create_header, create_empty_header
from sectools.common.parsegen.secdat.segment.defines import SEG_TYPE_DESCRIPTION,\
    SEG_TYPE_CLASS
from sectools.common.parsegen.secdat.segment.format import repr_seghdrs, \
    pack_seghdrs, create_segments, SecdatSegment, extract_segments,\
    pack_segments
from sectools.common.utils.c_misc import store_debug_data_to_file, \
    store_data_to_file, load_data_from_file


class ParseGenSecdat(object):

    def __init__(self, data=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 ):

        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Read the data
        self.set_data(data)

    def __repr__(self):
        return ('Secdat Header: ' + '\n' + repr(self.hdr) + '\n'
                'Secdat Segment Headers: ' + '\n' + repr_seghdrs(self.segments) + '\n' +
                '\n'.join([(SEG_TYPE_DESCRIPTION.get(segment.type).upper() + ':\n' + repr(segment.data)) for segment in self.segments])
                )

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
        Extract the various segments/sections of the data:
        1. Secdat header
        2. Footer
        3. Segment header
        4. Segments
        """
        # If data is not given, initialize empty secdat
        if data is None:
            data = self._create_empty_secdat()

        # Extract the footer
        data, self.ftr = extract_ftr(data)
        self.store_debug_data(df.FILE_FTR_IN, self.ftr.pack())

        # Extract the header
        data, self.hdr = extract_hdr(data)
        self.store_debug_data(df.FILE_HDR_IN, self.hdr.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.hdr), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Dump the individual segments
        self.segments = extract_segments(data, self.hdr)
        self.store_debug_data(df.FILE_SHDR_IN, pack_seghdrs(self.segments))
        self.store_debug_data(df.FILE_SHDR_IN_REPR, repr_seghdrs(self.segments), suffix=df.FILE_SHDR_IN_REPR_SUFFIX)
        idx = 0
        for seg_type, segment in enumerate(self.segments):
            self.store_debug_data(df.FILE_SEGMENT_IN.format(idx, seg_type), segment.data.get_data())
            idx += 1

    def get_data(self, version, hdr_info=None):
        # Get footer size
        ftr_size = get_ftr_size(version)

        # Get header size
        hdr_size = get_hdr_size(version)

        # Generate segment data
        self.segments = create_segments(self.segments, version, hdr_size)
        segment_hdr_data = pack_seghdrs(self.segments)
        segment_data = pack_segments(self.segments)

        # Generate header
        self.hdr = create_header(self.hdr, version, len(self.segments), len(segment_data) + ftr_size, hdr_info)

        # Create data for footer generation
        data = self.hdr.pack() + segment_hdr_data + segment_data

        # Generate footer
        self.ftr = create_footer(self.ftr, version, data)

        return data + self.ftr.pack()

    def read_from_file(self, file_path):
        self.set_data(load_data_from_file(file_path))

    def write_to_file(self, file_path, version, hdr_info=None):
        store_data_to_file(file_path, self.get_data(version, hdr_info))

    def _get_segment_idx(self, segment_type):
        for idx, segment in enumerate(self.segments):
            if segment.type == segment_type:
                return idx
        raise KeyError(str(segment_type) + ' segment does not exist.')

    def get_segment_data(self, segment_type):
        if not self.has_segment(segment_type):
            raise RuntimeError('Cannot get data for segment type: ' + str(segment_type) + '. Segment does not exist.')
        return self.segments[self._get_segment_idx(segment_type)].data

    def get_segment_attrs(self, segment_type):
        if not self.has_segment(segment_type):
            raise RuntimeError('Cannot get attrs for segment type: ' + str(segment_type) + '. Segment does not exist.')
        return self.segments[self._get_segment_idx(segment_type)].attrs

    def has_segment(self, segment_type):
        try:
            self._get_segment_idx(segment_type)
            return True
        except Exception:
            return False

    def remove_segment(self, segment_type):
        if not self.has_segment(segment_type):
            raise RuntimeError('Cannot remove segment type: ' + str(segment_type) + '. Segment does not exist.')
        del self.segments[self._get_segment_idx(segment_type)]

    def add_segment(self, segment_type, segment_data, segment_attrs=0):
        if not SecdatSegment.supports(segment_type):
            raise RuntimeError(str(segment_type) + ' is not supported')

        segment = SecdatSegment()
        segment.offset = 0
        segment.type = segment_type
        segment.attribute = segment_attrs
        segment.data = SEG_TYPE_CLASS[segment_type](segment_data)
        if self.has_segment(segment_type):
            self.remove_segment(segment_type)
        self.segments.append(segment)

    def _create_empty_secdat(self):
        header = create_empty_header()
        footer = create_empty_footer()
        return header.pack() + footer.pack()
