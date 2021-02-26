# ===============================================================================
#
#  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import math
import weakref

from . import defines as df
from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.parsegen.elf.header.format import extract_ehdr, \
    create_empty_ehdr
from sectools.common.parsegen.elf.header.ident import ELFCLASS32
from sectools.common.parsegen.elf.section.format import extract_shdrs, \
    repr_shdrs, pack_shdrs, extract_sections, zero_out_sections
from sectools.common.parsegen.elf.segment.flags import PF_OS_SEGMENT_HASH, \
    PF_OS_SEGMENT_PHDR
from sectools.common.parsegen.elf.segment.format import extract_phdrs, repr_phdrs, \
    pack_phdrs, extract_segments, get_phdr_class
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.parsegen.elf.segment.type import PT_NULL
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_debug_data_to_file


class ParseGenElfDelegate(object):

    def __init__(self, parsegen):
        self.parsegen_ref = weakref.ref(parsegen)

    @property
    def parsegen(self):
        return self.parsegen_ref()

    def segment_to_put(self, phdr):
        return (phdr.p_type == PT_LOAD or
                phdr.f_os_segment_type == PF_OS_SEGMENT_HASH or
                (phdr.p_type == PT_NULL and phdr.p_offset > 0))

    def process_segment_data(self, phdr, data):
        return data


class ParseGenElf(object):

    def __init__(self, data=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 _class=ELFCLASS32,
                 delegate=None,
                 elf_properties=None):

        # Create empty elf file is data is None
        if data is None:
            data = create_empty_elf(_class)

        # Set the delegate
        if delegate is None:
            self.delegate = ParseGenElfDelegate(self)
        else:
            self.delegate = delegate

        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        """
        Extract the various segments/sections of the data:
        1. Elf header
        2. Prog headers
        3. Bin
        """
        # Extract the header
        self.ehdr = extract_ehdr(data)
        self.store_debug_data(df.FILE_HDR_IN, self.ehdr.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.ehdr), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the program headers
        self.phdrs = extract_phdrs(data, self.ehdr)
        self.store_debug_data(df.FILE_PHDR_IN, pack_phdrs(self.phdrs))
        self.store_debug_data(df.FILE_PHDR_IN_REPR, repr_phdrs(self.phdrs), suffix=df.FILE_PHDR_IN_REPR_SUFFIX)

        # Extract the section headers
        if elf_properties and (not getattr(elf_properties, "has_sections", False)):
            zero_out_sections(self.ehdr)
        self.shdrs = extract_shdrs(data, self.ehdr)
        # Sort sections by whether they are encapsulated by segments
        self.encapsulated_sections_map, self.non_encapsulated_sections = self.sort_sections()
        # Extract section data from section header info and dump those which are outside segments
        self.sections = extract_sections(data, self.non_encapsulated_sections)
        for idx, shdr in enumerate(self.shdrs):
            if shdr in self.non_encapsulated_sections:
                self.store_debug_data(df.FILE_SECTION_IN.format(idx), self.sections[shdr])

        # Dump the individual segments
        self.segments = extract_segments(data, self.phdrs)
        for idx, phdr in enumerate(self.phdrs):
            length = len(self.segments[phdr])
            is_load = self.delegate.segment_to_put(phdr)
            if 0 <= length <= 16:
                logger.debug(('' if is_load else 'Non-') + 'Loadable segment - ' + str(idx + 1) + ' is of size: '  + str(length))
                if is_load and 0 < length <= 16:
                    logger.warning(('' if is_load else 'Non-') + 'Loadable segment - ' + str(idx + 1) + ' is of size: '  + str(length))
            self.store_debug_data(df.FILE_SEGMENT_IN.format(idx), self.segments[phdr])

    def __repr__(self):
        retval = 'Header: ' + '\n' + repr(self.ehdr) + '\n' \
                 'Program Headers: ' + '\n' + repr_phdrs(self.phdrs)
        if self.shdrs:
            retval += '\nSection Headers: ' + '\n' + repr_shdrs(self.shdrs)
        return retval

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def get_prog_data(self):
        # Put the elf header
        data = self.ehdr.pack()

        # Put the program headers
        offset = self.ehdr.e_phoff
        data = data.ljust(offset, PAD_BYTE_0)
        data += pack_phdrs(self.phdrs)

        return data

    def get_data(self):
        # Get the ELF Header and Program Header data
        data = self.get_prog_data()

        # Get the non-encapsulated segment data
        for index, phdr in enumerate(self.phdrs):
            if self.delegate.segment_to_put(phdr):
                offset = phdr.p_offset
                segment_data = self.delegate.process_segment_data(phdr, self.segments[phdr])
                if len(segment_data):
                    data = data.ljust(offset, PAD_BYTE_0)
                    data = (data[:offset] + segment_data +
                            data[offset + len(segment_data):])

        # Get the non-encapsulated section data
        for shdr, section_data in self.sections.items():
            offset = shdr.sh_offset
            if len(section_data):
                data = data.ljust(offset, PAD_BYTE_0)
                data = (data[:offset] + section_data +
                        data[offset + len(section_data):])

        # Get the Section Header data
        if self.ehdr.e_shnum:
            data = data.ljust(self.ehdr.e_shoff, PAD_BYTE_0)
            data += pack_shdrs(self.shdrs)

        return data

    def get_segment_data(self, phdr):
        if phdr.f_os_segment_type == PF_OS_SEGMENT_PHDR and len(self.segments[phdr]) == 0:
            return self.get_prog_data()
        else:
            return self.segments[phdr]

    def remove_segment(self, phdr):
        if phdr not in self.phdrs:
            raise RuntimeError('Given phdr does not exist in the current list.')

        # Remove the phdr entry
        del self.segments[phdr]
        index = self.phdrs.index(phdr)
        self.phdrs.remove(phdr)
        self.ehdr.e_phnum -= 1

        return index

    def shift(self, offset, size, align=None, after=False):
        """
        By default, shift everything. If after == True, only
        shift segments and sections after the insertion point.
        """
        # Update size if alignment is given
        if align:
            size = int(math.ceil(float(size)/align) * align)

        # Calculate the min offset of the segments and non-encapsulated sections that will need to be moved
        min_offset = None
        hdrs = self.phdrs + self.non_encapsulated_sections
        for i_hdr in hdrs:
            if i_hdr.filesz and (min_offset is None or i_hdr.offset < min_offset):
                collision_list = [(offset, offset + size), (i_hdr.offset, i_hdr.offset + i_hdr.filesz)]
                collision_list.sort(key=lambda x: x[0])
                if collision_list[0][1] > collision_list[1][0]:
                    min_offset = i_hdr.offset

        # Shift the segments and sections
        if min_offset is not None:
            shift = offset + size - min_offset
            for i_hdr in hdrs:
                # This check does not work on shared object signing(opendsp).
                # Load segments start at p_offset = 0
                # if offset <= i_phdr.p_offset:
                if not after or i_hdr.offset + i_hdr.filesz > offset:
                    # Update offset of segment or non-encapsulated section
                    i_hdr.offset += shift
                    # Update offsets of encapsulated sections
                    if i_hdr in self.encapsulated_sections_map.keys():
                        for i_shdr in self.encapsulated_sections_map[i_hdr]:
                            i_shdr.sh_offset += shift

        # Shift the Section Header Table
        if min_offset is not None and self.ehdr.e_shnum:
            if not after or self.ehdr.e_shoff + self.ehdr.e_shentsize * self.ehdr.e_shnum > offset:
                shift = offset + size - min_offset
                self.ehdr.e_shoff += shift
                align = 4  # Section Header Table must be 4 byte aligned
                shift = align - (self.ehdr.e_shoff % align)
                if shift != align:
                    self.ehdr.e_shoff += shift

    def add_segment(self, phdr, data, index=0, toalign=True, segment_to_put=False):
        # Check the segment to add
        if segment_to_put or self.delegate.segment_to_put(phdr):
            # Shift segments if needed
            self.shift(phdr.p_offset, phdr.p_filesz, phdr.p_align if toalign else None, after=True)

        # Add the phdr entry
        self.segments[phdr] = data
        self.phdrs.insert(index, phdr)
        self.ehdr.e_phnum += 1

    def update_segment(self, phdr, data):
        idx = self.remove_segment(phdr)
        phdr.p_filesz = len(data)
        phdr.p_memsz = phdr.p_filesz
        self.add_segment(phdr, data, idx)

    def get_new_phdr_entry(self):
        return get_phdr_class(self.ehdr)

    def stabilize_phdrs(self):
        self.ehdr.e_phoff = self.ehdr.get_size()

    def get_sorted_phdrs_and_encapsulated_phdrs(self, phdrs):
        offset_phdr_map = dict()
        encapsulated_segments = []
        for phdr_1 in phdrs:
            # to handle phdrs at same offset
            offset_phdr_map.setdefault(phdr_1.p_offset, []).append(phdr_1)
            for phdr_2 in phdrs:
                if phdr_1 == phdr_2 or phdr_1.p_offset > phdr_2.p_offset:
                    continue
                if self.encapsulated_segment(phdr_1, phdr_2):
                    encapsulated_segments.append(phdr_2)
        # Sort phdrs by offsets
        sorted_phdrs = [offset_phdr_map[key] for key in sorted(offset_phdr_map)]
        sorted_phdrs_flattened = [item for sublist in sorted_phdrs for item in sublist]
        return sorted_phdrs_flattened, encapsulated_segments

    @staticmethod
    def encapsulated_segment(phdr_1, phdr_2):
        # It is guaranteed that phdr_2.p_offset >= phdr_1.p_offset
        phdr_1_end = phdr_1.p_offset + phdr_1.p_filesz
        phdr_2_end = phdr_2.p_offset + phdr_2.p_filesz
        return phdr_1.p_filesz != 0 and phdr_2.p_filesz != 0 and phdr_2_end <= phdr_1_end

    @staticmethod
    def encapsulated_section(shdr, phdr):
        section_end = shdr.sh_offset + shdr.sh_size
        segment_end = phdr.p_offset + phdr.p_filesz
        return (shdr.sh_size != 0 and phdr.p_filesz != 0 and
                shdr.sh_offset >= phdr.p_offset and section_end <= segment_end)

    def sort_sections(self):
        phdr_to_shdr_map = {}
        sections_to_add = []
        for shdr in self.shdrs:
            for phdr in self.phdrs:
                if ParseGenElf.encapsulated_section(shdr, phdr):
                    if phdr in phdr_to_shdr_map:
                        phdr_to_shdr_map[phdr].append(shdr)
                    else:
                        phdr_to_shdr_map[phdr] = [shdr]
                    break
            else:
                sections_to_add.append(shdr)

        return phdr_to_shdr_map, sections_to_add


def create_empty_elf(_class):
    header = create_empty_ehdr(_class)
    header.e_phentsize = get_phdr_class(header).get_size()
    return header.pack()
