# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import math

from sectools.common.parsegen.elf.segment.flags import PF_R
from sectools.common.parsegen.elf.segment.license_manager import LicenseManagerSegment
from sectools.common.parsegen.elf.segment.type import PT_NOTE
from sectools.common.utils.c_misc import properties_repr
from sectools.features.isc.parsegen.elf_mbn_v3_base.elf_v3_base import get_last_seg
from sectools.features.isc.parsegen.elf_mbn_v3_encdec.elf_v3_encdec import ElfV3EncDec
from sectools.features.isc.parsegen.elf_support.elf_vars import ELF_BLOCK_ALIGN


class ElfV3Licmngr(ElfV3EncDec):

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

        super(ElfV3Licmngr, self).__init__(data,
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
        # Check the arguments
        if imageinfo is not None:
            elf_properties = imageinfo.image_type.elf_properties
            general_properties = imageinfo.general_properties

        for property in ["lib_id", "client_id"]:
            if hasattr(general_properties, property):
                setattr(self, property, getattr(general_properties, property))
            else:
                setattr(self, property, None)
        self._license_manager_segment = None

        # Perform license manager segment operations
        self.add_or_update_license_manager_segment()

    def __repr__(self):
        def segment_properties(seg, segment_name):
            if seg:
                return segment_name + ' Segment Properties: ' + '\n' + repr(seg) + '\n'
            else:
                return ''
        return ('Base Properties: ' + '\n' + properties_repr(self._repr_properties()) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n' +
                segment_properties(self._license_manager_segment, 'License Manager'))

    # --------------------------------------------------------------------------
    # License Manager Segment Helper methods
    # --------------------------------------------------------------------------
    def add_or_update_license_manager_segment(self):
        error = list()
        if self.client_id is None:
            error.append("client_id missing from config. Add client_id value to config.")
        if self.lib_id is None:
            error.append("lib_id missing from config. Add lib_id value to config.")
        if error:
            raise RuntimeError("\n       ".join(error))
        license_manager_segment_data, paddr = self._extract_existing_license_manager_segment()
        signer = self.signer if self.signing else None
        self._license_manager_segment = LicenseManagerSegment(
            self.client_id, self.lib_id, signer=signer, data=license_manager_segment_data,
            validating=self.validating)
        self._add_license_manager_segment(paddr)

    def _extract_existing_license_manager_segment(self):
        license_manager_segment, license_manager_entry, addr = None, None, None
        # Find the phdr entry for the license manager segment
        for phdr in self._elf_parsegen.phdrs:
            if phdr.p_type == PT_NOTE:
                note_segment_data = self._elf_parsegen.get_segment_data(phdr)
                if license_manager_segment is None:
                    if LicenseManagerSegment.is_valid(self.client_id, self.lib_id, note_segment_data):
                        license_manager_segment = note_segment_data
                        license_manager_entry = phdr
                else:
                    if LicenseManagerSegment.is_valid(self.client_id, self.lib_id, note_segment_data):
                        raise RuntimeError("Image contains multiple License Manager Segments.\n       "
                                           "Only 1 License Manager Segment is allowed to exist.")
        # Remove segment
        if license_manager_entry:
            addr = license_manager_entry.p_paddr
            self._elf_parsegen.remove_segment(license_manager_entry)
        return license_manager_segment, addr

    def _add_license_manager_segment(self, addr):
        # Create phdr entry for the license manager segment
        license_manager_phdr_entry = self._get_license_manager_phdr_entry(
            self._license_manager_segment.get_size(), addr)
        # Add the license manager segment
        self._elf_parsegen.add_segment(license_manager_phdr_entry, self._license_manager_segment.pack(),
                                       index=len(self._elf_parsegen.phdrs), segment_to_put=True)

    def _get_license_manager_address(self, addr):
        if addr is not None:
            return addr
        else:
            return self._compute_hash_address(self._elf_parsegen)

    def _get_license_manager_offset(self):
        last_phdr_entry = get_last_seg(self._elf_parsegen.phdrs)
        return int(math.ceil(float(last_phdr_entry.p_offset + last_phdr_entry.p_filesz + 1) /
                             ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)

    def _get_license_manager_phdr_entry(self, size, addr):
        license_manager_phdr = self._elf_parsegen.get_new_phdr_entry()()
        license_manager_phdr.p_type = PT_NOTE
        license_manager_phdr.p_align = ELF_BLOCK_ALIGN
        license_manager_phdr.p_offset = self._get_license_manager_offset()
        license_manager_phdr.p_memsz = int(math.ceil(float(size) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        license_manager_phdr.p_filesz = size
        license_manager_phdr.p_paddr = self._get_license_manager_address(addr)
        license_manager_phdr.p_flags = int(PF_R)
        return license_manager_phdr
