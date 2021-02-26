# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import math

from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.segment.multi_image import MultiImageSegment
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.utils.c_misc import properties_repr
from sectools.features.isc.advanced_defines import multi_image_string
from sectools.features.isc.parsegen.elf_mbn_v3_base.elf_v3_base import get_last_seg
from sectools.features.isc.parsegen.elf_support.elf_vars import ELF_BLOCK_ALIGN
from ..elf_mbn_v6.elf_v6 import ElfV6


class ElfV6MultiImage(ElfV6):

    def __init__(self,
                 data,
                 imageinfo=None,
                 elf_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        # Check the arguments
        if imageinfo is not None:
            elf_properties = imageinfo.image_type.elf_properties
            general_properties = imageinfo.general_properties
        if elf_properties is None:
            raise RuntimeError('ELF properties must not be None.')

        super(ElfV6MultiImage, self).__init__(data,
                                              imageinfo=imageinfo,
                                              elf_properties=elf_properties,
                                              general_properties=general_properties,
                                              encdec=encdec,
                                              debug_dir=debug_dir,
                                              debug_prefix=debug_prefix,
                                              debug_suffix=debug_suffix,
                                              validating=validating,
                                              signing=signing,
                                              parsegens=parsegens,
                                              sign_attr=sign_attr)

        self.mem_size = None
        self.validating = validating

        # Perform multi image segment operations
        self.multi_image_segment_addr = general_properties.multi_image_segment_addr
        self._multi_image_segment = None
        self.add_or_update_multi_image_segment()

    def __repr__(self):
        def segment_properties(seg, segment_name):
            if seg:
                return segment_name + ' Segment Properties: ' + '\n' + repr(seg) + '\n'
            else:
                return ''
        return ('Base Properties: ' + '\n' + properties_repr(self._repr_properties()) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n' +
                segment_properties(self._multi_image_segment, multi_image_string()))

    # --------------------------------------------------------------------------
    # Mult Image Helper methods
    # --------------------------------------------------------------------------
    def add_or_update_multi_image_segment(self):
        multi_image_segment_data = self._extract_existing_multi_image_segment()
        self._multi_image_segment = MultiImageSegment(
            self.debug_dir if self.signing else None, self.segment_hash_algorithm,
            data=multi_image_segment_data, images=self.dependent_parsegens)
        self._add_multi_image_segment()

    def _extract_existing_multi_image_segment(self):
        multi_image_segment, multi_image_entry = None, None
        # Find the phdr entry for the multi image segment
        for phdr in self._elf_parsegen.phdrs:
            if phdr.p_type == PT_LOAD:
                if multi_image_entry is None:
                    multi_image_entry = phdr
                    self.mem_size = phdr.p_memsz
                else:
                    raise RuntimeError(multi_image_string() + " image contains "
                                       "multiple LOAD segments.\n       "
                                       "Only 1 LOAD segment is allowed to exist.")

        # Extract segment data and remove segment
        if multi_image_entry:
            multi_image_segment = self._elf_parsegen.get_segment_data(multi_image_entry)
            self._elf_parsegen.remove_segment(multi_image_entry)
        return multi_image_segment

    def _add_multi_image_segment(self):
        # Get the size and physical address of the multi image segment
        multi_image_segment_size = self._multi_image_segment.get_size()
        multi_image_segment_addr = int(self.multi_image_segment_addr, 16)
        multi_image_phdr_entry = self._get_multi_image_phdr_entry(multi_image_segment_size,
                                                                  multi_image_segment_addr)
        # Add segment
        self._elf_parsegen.add_segment(multi_image_phdr_entry, self._multi_image_segment.pack())
        # Set ELF Entry Point to match physical and virtual address
        self._elf_parsegen.ehdr.e_entry = multi_image_segment_addr

    def _get_multi_image_offset(self):
        if self._elf_parsegen.phdrs:
            phdr = get_last_seg(self._elf_parsegen.phdrs)
        else:
            phdr = self._get_prog_phdr_entry(self._elf_parsegen.get_new_phdr_entry()(),
                                             self._elf_parsegen.ehdr)
        return int(math.ceil(float(phdr.p_offset + phdr.p_filesz) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)

    def _get_multi_image_phdr_entry(self, size, addr):
        multi_image_phdr = self._elf_parsegen.get_new_phdr_entry()()
        multi_image_phdr.p_type = segment.type.PT_LOAD
        multi_image_phdr.p_align = ELF_BLOCK_ALIGN
        multi_image_phdr.p_offset = self._get_multi_image_offset()
        #Persist the mem size for validation, set it similar to file size for any other operations
        if self.validating:
            multi_image_phdr.p_memsz = self.mem_size
        else:
            multi_image_phdr.p_memsz = size
        multi_image_phdr.p_filesz = size
        multi_image_phdr.p_vaddr = addr
        multi_image_phdr.p_paddr = addr
        return multi_image_phdr