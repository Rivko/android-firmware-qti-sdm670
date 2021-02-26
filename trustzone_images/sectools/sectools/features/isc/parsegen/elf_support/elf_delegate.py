# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.elf.format import ParseGenElfDelegate
from sectools.common.parsegen.elf.segment.format import repr_phdrs
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.parsegen.elf.segment.type import PT_NULL
from sectools.common.parsegen.elf.segment.type import PT_PHDR
from sectools.common.utils.c_misc import TablePrinter
from sectools.features.isc.parsegen.elf_support.elf_vars import NON_ENCAP_SEGMENTS
from sectools.features.isc.parsegen.elf_support.elf_vars import PHY_ADDR_VALIDATION_DICT
from sectools.features.isc.parsegen.elf_support.elf_vars import POLICY_NON_LOAD_OUTSIDE_LOAD
from sectools.features.isc.parsegen.elf_support.elf_vars import POLICY_OVERLAPPING_SEGMENTS_PHYSICAL
from sectools.features.isc.parsegen.elf_support.elf_vars import POLICY_OVERLAPPING_SEGMENTS_VIRTUAL
from sectools.features.isc.parsegen.elf_support.elf_vars import VIR_ADDR_VALIDATION_DICT
from sectools.features.isc.parsegen.elf_support.elf_vars import elf_vars


class ElfDelegate(ParseGenElfDelegate):

    def __init__(self, parsegen, val_ph, val_v):
        super(ElfDelegate, self).__init__(parsegen)

        self.validate_ph_addrs = val_ph
        self.validate_vir_addrs = val_v
        self.validate()

    def get_seg_ranges(self, phdrs, phys=False):
        # Collect all the phys addr ranges
        if phys:
            seg_ranges = [(p.p_paddr, p.p_paddr + p.p_memsz, p.p_memsz) for p in phdrs]
        else:
            seg_ranges = [(p.p_offset, p.p_offset + p.p_filesz, p.p_filesz) for p in phdrs]

        # Sort ranges by start address
        seg_ranges.sort(key=lambda x: x[0])
        return seg_ranges

    def segment_is_overlapped(self, idx, seg_ranges):
        check_start, check_end = True, True
        if idx == 0:
            check_start = False
        if idx == len(seg_ranges) - 1:
            check_end = False

        # Check overlap on start
        if check_start:
            if seg_ranges[idx][0] < seg_ranges[idx - 1][1]:
                return True
        # Check overlap on end
        if check_end:
            if seg_ranges[idx][1] > seg_ranges[idx + 1][0]:
                return True
        return False

    def segment_to_put(self, phdr, seg_ranges=None):
        if super(ElfDelegate, self).segment_to_put(phdr):
            return True
        elif phdr.p_type in elf_vars[NON_ENCAP_SEGMENTS]:
            for idx, p in enumerate(self.parsegen.phdrs):
                if p is phdr:
                    break
            else:
                raise RuntimeError('Invalid phdr.')
            if seg_ranges is None:
                seg_ranges = self.get_seg_ranges(self.parsegen.phdrs)
            if not self.segment_is_overlapped(idx, seg_ranges):
                return True
        return False

    def validate_addresses(self, addr_dict):

        seg_ranges = []

        # Collect all the addr ranges
        for phdr in self.parsegen.phdrs:
            if self.segment_to_put(phdr):
                if addr_dict['isPhy']:
                    seg_ranges.append((phdr.p_paddr, phdr.p_paddr + phdr.p_memsz, phdr.p_memsz))
                else:
                    seg_ranges.append((phdr.p_vaddr, phdr.p_vaddr + phdr.p_memsz, phdr.p_memsz))

        # Sort ranges by start address
        seg_ranges.sort(key=lambda x: x[0])

        # Check for overlaps
        overlapping = []
        for idx in range(len(seg_ranges) - 1):
            if seg_ranges[idx + 1][0] < seg_ranges[idx][1]:
                overlapping.append((seg_ranges[idx], seg_ranges[idx + 1]))

        if overlapping:
            # Create table header
            table = TablePrinter([1])
            table.insert_data(0, 0, 'S.No.')
            table.insert_data(0, 1, 'Segment A', column_end=2)
            table.insert_data(0, 3, 'Segment B', column_end=4)

            # Create sub header
            table.insert_data(1, 1, addr_dict['tString'])
            table.insert_data(1, 2, 'Size')
            table.insert_data(1, 3, addr_dict['tString'])
            table.insert_data(1, 4, 'Size')

            # Add all the overlapping segments
            for idx, overlap in enumerate(overlapping):
                table.insert_data(idx + 2, 1, hex(overlap[0][0]))
                table.insert_data(idx + 2, 2, hex(overlap[0][2]))
                table.insert_data(idx + 2, 3, hex(overlap[1][0]))
                table.insert_data(idx + 2, 4, hex(overlap[1][2]))

            # Create the error string
            addr_dict['policy']().run(addr_dict['eString'] + '\n'.join(['  ' + l for l in table.get_data().split('\n')]))

    def validate(self):

        if not elf_vars[POLICY_OVERLAPPING_SEGMENTS_PHYSICAL].is_ignore():
            # Validate physical addresses don't overlap
            if self.validate_ph_addrs:
                self.validate_addresses(PHY_ADDR_VALIDATION_DICT)

        if not elf_vars[POLICY_OVERLAPPING_SEGMENTS_VIRTUAL].is_ignore():
            # Validate virtual addresses don't overlap
            if self.validate_vir_addrs:
                self.validate_addresses(VIR_ADDR_VALIDATION_DICT)

        if not elf_vars[POLICY_NON_LOAD_OUTSIDE_LOAD].is_ignore():
            load_ranges = []
            non_load_ranges = []

            # Collect address ranges
            for idx, phdr in enumerate(self.parsegen.phdrs):
                if phdr.p_type == PT_LOAD:
                    load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz))
                elif not (phdr.p_type == PT_NULL or phdr.p_type == PT_PHDR or phdr.p_filesz == 0 or self.segment_to_put(phdr)):
                    non_load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz, phdr, idx + 3))

            # Sort ranges by start address
            load_ranges.sort(key=lambda x: x[0])
            non_load_ranges.sort(key=lambda x: x[0])

            # Check for non-encapsulated segments
            non_encap = []
            non_encap_sno = []
            for d in range(len(non_load_ranges)):
                for s in range(len(load_ranges)):
                    # if true, means non-loadable segment is encapsulated within a loadable segment
                    if load_ranges[s][0] <= non_load_ranges[d][0] and non_load_ranges[d][1] <= load_ranges[s][1]:
                        break  # means else-case IS NOT executed and non-loadable segment is encapsulated within a loadable segment
                # else case of inner for-loop if executed if break statement never executed meaning non-loadable segment is not encapsulated in any segment
                else:
                    non_encap.append(non_load_ranges[d][2])
                    non_encap_sno.append(non_load_ranges[d][3])

            if non_encap:
                # Create the error string
                elf_vars[POLICY_NON_LOAD_OUTSIDE_LOAD].run(
                    "Following non-loadable segments are not completely encapsulated by a load segment: " + '\n' +
                    repr_phdrs(non_encap, non_encap_sno))
