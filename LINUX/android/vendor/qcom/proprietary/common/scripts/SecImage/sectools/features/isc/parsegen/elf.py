# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Nov 25, 2013

@author: hraghav
'''

import math
import struct

from sectools.common.parsegen import elf, PAD_BYTE_1, PAD_BYTE_0
from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.format import ParseGenElfDelegate
from sectools.common.parsegen.elf.header.ident import ELFCLASS64
from sectools.common.parsegen.elf.segment.format import repr_phdrs
from sectools.common.parsegen.elf.segment.type import PT_NULL, PT_NOTE, \
    PT_LOAD, PT_PHDR
from sectools.common.parsegen.mbn.format import ALIGNED_IMAGE_SIZE, ParseGenMbn
from sectools.common.parsegen.mbn.header import MbnHdr40B, MBN_HDR_VERSION_3, MBN_HDR_VERSION_5
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_base import ValPolicy
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import properties_repr, hex_addr, TablePrinter
from sectools.features.isc.defines import SECBOOT_VERSION_2_0, AUTHORITY_QC
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.mbn import SecParseGenMbn, SECBOOT_MBN_HDR, MAX_SIG_SIZE

# Debug files
FILE_PROG_SEG_IN            = 'sec_elf_in_prog_segment'
FILE_HASH_SEG_IN            = 'sec_elf_in_hash_segment'
FILE_PROG_HASH_REMOVED_IN   = 'sec_elf_in_prog_hash_removed'
FILE_DECRYPTED_IN           = 'sec_elf_in_decrypted'

# Elf file type
ELF_FILE_TYPE               = 'elf'

# Constants
ELF_BLOCK_ALIGN             = 0x1000        # Block alignment size
MAX_PHDR_COUNT              = 100           # Maximum allowable program headers

# Full flags for specific segments
PF_OS_SEGMENT_HASH_FULL     = 0x02200000
PF_OS_SEGMENT_BOOT_FULL     = 0x03200000
PF_OS_SEGMENT_PHDR_FULL     = 0x07000000

# Selected policy for overlapping segments
POLICY_OVERLAPPING_SEGMENTS_PHYSICAL = ValPolicy(ValPolicy.WARN)
POLICY_OVERLAPPING_SEGMENTS_VIRTUAL = ValPolicy(ValPolicy.WARN)
POLICY_NON_LOAD_OUTSIDE_LOAD = ValPolicy(ValPolicy.ERROR)

# Dictionary for use in addr validation
PHY_ADDR_VALIDATION_DICT = {'isPhy': True, 'policy': (lambda: POLICY_OVERLAPPING_SEGMENTS_PHYSICAL), 'tString': 'Phys',
                            'eString': 'Following segments physical address overlap: ' + '\n'}

VIR_ADDR_VALIDATION_DICT = {'isPhy': False, 'policy': (lambda: POLICY_OVERLAPPING_SEGMENTS_VIRTUAL), 'tString': 'Vir',
                            'eString': 'Following segments virtual address overlap: ' + '\n'}

# Segments to ignore for hashing
NON_HASHABLE_SEGMENTS = [PT_PHDR]

def set_non_hashable_segments(segments):
    global NON_HASHABLE_SEGMENTS
    NON_HASHABLE_SEGMENTS = segments

# Segments allowed to exist outside the LOAD segments
NON_ENCAP_SEGMENTS = [PT_NOTE]

def set_non_encap_segments(segments):
    global NON_ENCAP_SEGMENTS
    NON_ENCAP_SEGMENTS = segments

# Segments not encrypted
NON_ENCRYPT_SEGMENTS = [PT_NOTE]

def set_non_encrypt_segments(segments):
    global NON_ENCRYPT_SEGMENTS
    NON_ENCRYPT_SEGMENTS = segments

# Returns the last segment in a list of phdrs
get_last_seg = lambda x: sorted([s for s in x], key=lambda s: s.p_offset)[-1]

# Test API to modify error policy
def set_overlapping_segments_policies(phyPolicy, virPolicy):
    global POLICY_OVERLAPPING_SEGMENTS_PHYSICAL
    global POLICY_OVERLAPPING_SEGMENTS_VIRTUAL
    POLICY_OVERLAPPING_SEGMENTS_PHYSICAL = phyPolicy
    POLICY_OVERLAPPING_SEGMENTS_VIRTUAL = virPolicy


# Test API to save error policy
def get_overlapping_segments_policies():
    return POLICY_OVERLAPPING_SEGMENTS_PHYSICAL, POLICY_OVERLAPPING_SEGMENTS_VIRTUAL

class SecParseGenElfDelegate(ParseGenElfDelegate):

    def __init__(self, parsegen, val_ph, val_v):
        ParseGenElfDelegate.__init__(self, parsegen)

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
        if ParseGenElfDelegate.segment_to_put(self, phdr):
            return True
        elif phdr.p_type in NON_ENCAP_SEGMENTS:
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

        if not POLICY_OVERLAPPING_SEGMENTS_PHYSICAL.is_ignore():
            # Validate physical addresses don't overlap
            if self.validate_ph_addrs:
                self.validate_addresses(PHY_ADDR_VALIDATION_DICT)

        if not POLICY_OVERLAPPING_SEGMENTS_VIRTUAL.is_ignore():
            # Validate virtual addresses don't overlap
            if self.validate_vir_addrs:
                self.validate_addresses(VIR_ADDR_VALIDATION_DICT)

        if not POLICY_NON_LOAD_OUTSIDE_LOAD.is_ignore():
            load_ranges = []
            non_load_ranges = []

            # Collect address ranges
            for idx, phdr in enumerate(self.parsegen.phdrs):
                if phdr.p_type == PT_LOAD:
                    load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz))
                elif not (phdr.p_type == PT_NULL or phdr.p_filesz == 0 or
                          self.segment_to_put(phdr)):
                    non_load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz, phdr, idx + 3))

            # Sort ranges by start address
            load_ranges.sort(key=lambda x: x[0])
            non_load_ranges.sort(key=lambda x: x[0])

            # Check for non-encapsulated segments
            non_encap = []
            non_encap_sno = []
            for d in range(len(non_load_ranges)):
                for s in range(len(load_ranges)):
                    if (load_ranges[s][0] <= non_load_ranges[d][0] and
                        non_load_ranges[d][1] <= load_ranges[s][1]):
                        break
                else:
                    # if non load segment not encapsulated in any segment, add to non_encapsulated list
                    non_encap.append(non_load_ranges[d][2])
                    non_encap_sno.append(non_load_ranges[d][3])

            if non_encap:
                # Create the error string
                POLICY_NON_LOAD_OUTSIDE_LOAD.run("Following non-loadable segments are not completely encapsulated by a load segment: " + '\n' +
                                                 repr_phdrs(non_encap, non_encap_sno))


class SecParseGenElf(SecParseGenMbn):

    def __init__(self,
                 data,
                 imageinfo=None,
                 elf_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):

        SecParseGenBase.__init__(self,
                                 data,
                                 imageinfo,
                                 general_properties,
                                 encdec,
                                 debug_dir,
                                 debug_prefix,
                                 debug_suffix)

        # Check the arguments
        if imageinfo is not None:
            elf_properties = imageinfo.image_type.elf_properties
            general_properties = imageinfo.general_properties
        if elf_properties is None:
            raise RuntimeError('ELF properties must not be None.')

        # Initialize internal properties
        self._image_type = 0
        self._serial_num = None
        self._max_elf_segments = MAX_PHDR_COUNT
        self._validate_ph_addrs = True
        self._validate_vir_addrs = False


        # Set properties from the config file
        self.image_type = elf_properties.image_type
        self.hash_seg_placement = elf_properties.hash_seg_placement
        self.serial_num = general_properties.testsig_serialnum
        if elf_properties.max_elf_segments is not None:
            self.max_elf_segments = elf_properties.max_elf_segments
        if elf_properties.validate_ph_addrs is not None:
            self.validate_ph_addrs = elf_properties.validate_ph_addrs
        if elf_properties.validate_vir_addrs is not None:
            self.validate_vir_addrs = elf_properties.validate_vir_addrs

        # Initialize the elf parsegen
        self._elf_parsegen = elf.ParseGenElf(data,
                                             self.debug_dir,
                                             self.debug_prefix,
                                             self.debug_suffix)
        self._elf_parsegen.stabilize_phdrs()

        # Remove the prog header and hash segment
        phdr_segment, hash_segment = self.extract_phdr_hash(self._elf_parsegen)
        self.store_debug_data(FILE_PROG_SEG_IN, phdr_segment)
        self.store_debug_data(FILE_HASH_SEG_IN, hash_segment)
        self.store_debug_data(FILE_PROG_HASH_REMOVED_IN, self._elf_parsegen.get_data())

        self.hash_segment = hash_segment

        # If hash_segment is empty, create dummy hash_segment
        if not hash_segment:
            hash_segment = self._generate_default_hash_segment(self.secboot_version)

        # Initialize the base now
        SecParseGenMbn.__init__(self,
                                data=hash_segment,
                                imageinfo=None,
                                mbn_properties=self._get_sec_parsegen_mbn_properties(),
                                general_properties=general_properties,
                                encdec=self.encdec,
                                debug_dir=debug_dir,
                                debug_prefix=debug_prefix,
                                debug_suffix=debug_suffix)

        if self._elf_parsegen.ehdr.e_ident_class == ELFCLASS64 or self.secboot_version == SECBOOT_VERSION_2_0:
            self._mbn_parsegen.invalidate_pointers = True

        # Set the elf parsegen delegate
        delegate = SecParseGenElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
        self._elf_parsegen.delegate = delegate

        # Check if the file is encrypted
        if self.is_encrypted():
            self._decrypt_data()
            self.store_debug_data(FILE_DECRYPTED_IN, self._elf_parsegen.get_data())

            # Get the original data
            self._elf_parsegen = elf.ParseGenElf(self._elf_parsegen.get_data(),
                                                 self.debug_dir,
                                                 self.debug_prefix,
                                                 self.debug_suffix)

            # Ensure that hashes match. If they don't we can assume that decryption failed
            if self._mbn_parsegen.code[32*2:] != self.get_hash_table():
                raise RuntimeError("Decryption of image failed. This can be caused by the use of an invalid L3 key")

            # Set the elf parsegen delegate
            delegate = SecParseGenElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
            self._elf_parsegen.delegate = delegate

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n'
                'SecElf Properties: ' + '\n'
                '' + properties_repr([('image_type', self.image_type),
                                      ('max_elf_segments', self.max_elf_segments),
                                      ('testsig_serialnum', hex_addr(self.serial_num) if self.serial_num is not None else self.serial_num), ]))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return ELF_FILE_TYPE

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
        integrity_check = True if (integrity_check or sign or encrypt) else False

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt)

        if not (integrity_check or sign or encrypt):
            data = self._elf_parsegen.get_data()
        else:
            # Add the header and hash segment
            prog_phdr_entry, hash_phdr_entry, hash_segment_size = self._add_phdr_and_hash_segs(integrity_check, sign, encrypt)

            try:
                # Generate the hash segment now
                hash_segment = self.get_hash_segment(integrity_check, sign, encrypt)

                # Check here for sizes mismatching just in case
                if len(hash_segment) != hash_segment_size:
                    raise RuntimeError('Estimating the size of the hash table was wrong. Estimate - ' +
                                       str(hash_segment_size) + ', Actual - ' + str(len(hash_segment)))

                # Re-add the hash segment, this time with the real data
                self._elf_parsegen.remove_segment(hash_phdr_entry)
                self._elf_parsegen.remove_segment(prog_phdr_entry)
                self._elf_parsegen.add_segment(prog_phdr_entry, '')
                self._elf_parsegen.add_segment(hash_phdr_entry, hash_segment, index=1)

                # If encrypting, change the process segment data
                if encrypt:
                    parsegen_updater = ParseGenEncDec(self.store_debug_data, self.encrypt_segment)
                    parsegen_updater.update_parsegen(self.encdec.get_segment_num_scheme(), self._elf_parsegen)
                try:
                    # Get the elf data
                    data = self._elf_parsegen.get_data()
                finally:
                    # Restore the process segment data
                    if encrypt:
                        try: parsegen_updater.revert_parsegen(self._elf_parsegen)
                        except Exception as e: logger.warning(str(e))
            finally:
                # Remove the prog
                try: self._elf_parsegen.remove_segment(prog_phdr_entry)
                except Exception as e: logger.warning(str(e))

                # Remove the hash
                try: self._elf_parsegen.remove_segment(hash_phdr_entry)
                except Exception as e: logger.warning(str(e))

        return data

    @property
    def data_to_sign(self):
        # Add the header and hash segment
        prog_phdr_entry, hash_phdr_entry, _ = self._add_phdr_and_hash_segs(self.integrity_check, self.sign, self.encrypt)

        # Get the data to sign from the hash segment
        data = self.get_hash_segment_data_to_sign()

        # Remove the prog and hash
        self._elf_parsegen.remove_segment(prog_phdr_entry)
        self._elf_parsegen.remove_segment(hash_phdr_entry)

        return data

    def contains_integrity_check(self):
        return self.hash_segment != ''

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def extract_phdr_hash(self, elf_parsegen):
        phdr_segment, hash_segment = '', ''
        phdr_entry, hash_entry = None, None

        # Find the phdr entries
        for phdr in elf_parsegen.phdrs:
            if phdr.f_os_segment_type == segment.flags.PF_OS_SEGMENT_PHDR:
                phdr_entry = phdr
            elif phdr.f_os_segment_type == segment.flags.PF_OS_SEGMENT_HASH:
                hash_entry = phdr
            if phdr_entry and hash_entry:
                break

        # Remove the phdr entries
        if phdr_entry:
            phdr_segment = elf_parsegen.get_segment_data(phdr_entry)
            elf_parsegen.remove_segment(phdr_entry)
        if hash_entry:
            hash_segment = elf_parsegen.get_segment_data(hash_entry)
            elf_parsegen.remove_segment(hash_entry)

        return phdr_segment, hash_segment

    def get_hash_segment(self, integrity_check, sign, encrypt):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        return SecParseGenMbn._get_data_int(self, sign, encrypt)

    def get_hash_segment_data_to_sign(self):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        return SecParseGenMbn.data_to_sign.fget(self)

    def get_hash_table(self):
        # Check the number of headers that can be hashed
        phdr_count = len(self._elf_parsegen.phdrs)
        if phdr_count > self.max_elf_segments:
            raise RuntimeError('Number of prog header entries in ELF "' + str(phdr_count) + '" '
                               'is more than the maximum supported "' + str(self.max_elf_segments) + '"')

        hash_table = ''

        def get_hash(data):
            hash_data = self._get_hash(data)
            if ((phdr.f_os_segment_type in [segment.flags.PF_OS_SEGMENT_HASH]) or
                (phdr.f_os_access_type in [segment.flags.PF_OS_ACCESS_NOTUSED,
                                           segment.flags.PF_OS_ACCESS_SHARED]) or
                len(data) == 0):
                return PAD_BYTE_0 * len(hash_data)
            else:
                return hash_data

        for phdr in self._elf_parsegen.phdrs:
            # Constants
            page_mode = phdr.f_os_page_mode
            non_paged = segment.flags.PF_OS_NON_PAGED_SEGMENT
            paged = segment.flags.PF_OS_PAGED_SEGMENT

            # Hash as segment
            if ((page_mode == non_paged) or
                (page_mode == paged and self.hash_pageseg_as_segment)):
                if phdr.p_type not in NON_HASHABLE_SEGMENTS:
                    data_to_hash = self._elf_parsegen.get_segment_data(phdr)
                    hash_table += get_hash(data_to_hash)

            # Hash as pages
            elif (page_mode == paged):
                # Check if the vaddr is page aligned
                offset = 0
                size = phdr.p_filesz
                nonalign = phdr.p_vaddr & (ELF_BLOCK_ALIGN - 1)
                if nonalign:
                    offset += (ELF_BLOCK_ALIGN - nonalign)
                    size -= (ELF_BLOCK_ALIGN - nonalign)

                # Ensure size is page aligned
                if (size & (ELF_BLOCK_ALIGN - 1)) > 0:
                    raise RuntimeError('Paged segment is not page aligned. Size: ' + hex(size))

                # Go through the pages
                data = self._elf_parsegen.get_segment_data(phdr)[offset:]
                while data:
                    data_to_hash = data[:ELF_BLOCK_ALIGN]
                    hash_table += get_hash(data_to_hash)
                    data = data[ELF_BLOCK_ALIGN:]

            # Invalid page mode flag
            else:
                raise RuntimeError('Invalid page mode encountered.')

        # Insert testsig at this point if its needed
        if self.serial_num is not None:
            hash_table = self._insert_serial_in_ht(hash_table, self._serial_num)
        return hash_table

    def _add_phdr_and_hash_segs(self, integrity_check, sign, encrypt):
        """
        Add the prog hdr and hash_seg in the right places according to
        elf_properties.hash_seg_placement
        """
        #### Add the prog phdr entry first
        # The data in the prog and hash at this time maybe dummy data
        phdr_class = self._elf_parsegen.get_new_phdr_entry()
        prog_phdr_entry = self._get_prog_phdr_entry(phdr_class(),
                                                    self._elf_parsegen.ehdr)
        proghdr_end = prog_phdr_entry.p_offset + prog_phdr_entry.p_filesz

        # Get the size of the hash segment
        hash_segment_size = self._compute_hash_segment_size(integrity_check, sign, encrypt)
        hash_segment_addr = self._compute_hash_address(self._elf_parsegen)

        align_func = lambda x: int(math.ceil(float(x + 1) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        if not self.hash_seg_placement or self.hash_seg_placement == 'POST_PHDR_ALIGNED':
            hash_segment_offset = align_func(proghdr_end)
            shift_data = (prog_phdr_entry.p_offset,
                          hash_segment_offset + hash_segment_size - prog_phdr_entry.p_offset,
                          ELF_BLOCK_ALIGN)
            logger.debug('Adding hash seg after phdrs, aligned to 0x1000')
        elif self.hash_seg_placement == 'POST_PHDR':
            hash_segment_offset = proghdr_end
            shift_data = (prog_phdr_entry.p_offset,
                          hash_segment_offset + hash_segment_size - prog_phdr_entry.p_offset,
                          ELF_BLOCK_ALIGN)
            logger.debug('Adding hash seg directly after phdrs')
        elif self.hash_seg_placement == 'POST_PHDR_LOAD_ALIGNED':
            hash_segment_offset = proghdr_end
            shift_data = (prog_phdr_entry.p_offset,
                          align_func(hash_segment_offset + align_func(hash_segment_size) - prog_phdr_entry.p_offset),
                          ELF_BLOCK_ALIGN)
            logger.debug('Adding hash seg directly after phdrs. Placing next segment at 4k boundary.')
        elif self.hash_seg_placement == 'END_64B_ALIGNED':
            # Get size of the hash seg minus sign, certs, etc
            size_of_hash_seg = self._compute_hash_segment_size(False, False, False)

            # Shift the last load segment so the end of hash table is 64B aligned
            last_phdr_entry = get_last_seg(self._elf_parsegen.phdrs)
            last_seg_remainder = (last_phdr_entry.p_offset + last_phdr_entry.p_filesz + size_of_hash_seg) % 64
            if last_seg_remainder > 0:
                self._elf_parsegen.shift(last_phdr_entry.p_offset, 64 - last_seg_remainder, after=True)

            # Get the offset for hash segment
            hash_segment_offset = last_phdr_entry.p_offset + last_phdr_entry.p_filesz
            shift_data = (prog_phdr_entry.p_offset,
                          prog_phdr_entry.p_filesz,
                          None)
            logger.debug('Adding hash seg after last segment with end of hash table 64B aligned')
        else:
            raise RuntimeError('Unknown hash_seg_placement value: %s' % self.hash_seg_placement)

        hash_phdr_entry = self._get_hash_phdr_entry(phdr_class(),
                                                    hash_segment_size,
                                                    hash_segment_addr,
                                                    hash_segment_offset
                                                    )
        # shift to make room for the phdr table
        self._elf_parsegen.shift(shift_data[0],
                                 shift_data[1],
                                 shift_data[2])

        # Add segment
        self._elf_parsegen.add_segment(prog_phdr_entry, '')

        # hash seg contains dummy data for now
        self._elf_parsegen.add_segment(hash_phdr_entry, PAD_BYTE_1 * hash_segment_size, index=1)

        # Update the dest pointer for the mb
        if hasattr(self._mbn_parsegen.header, 'image_dest_ptr'):
            self._mbn_parsegen.header.image_dest_ptr = hash_segment_addr + self._mbn_parsegen.header.get_size()

        return prog_phdr_entry, hash_phdr_entry, hash_segment_size

    def _compute_hash_segment_size(self, integrity_check, sign, encrypt):
        hash_table_len = ((len(self._get_hash('')) * 2) +
                          len(self.get_hash_table()))

        # Check configuration
        signatures = 1
        if self.oem_signing_enabled and self.qc_signing_enabled:
            signatures += 1

        encrypt_params = len(self.encdec.get_encryption_parameters_blob()) if encrypt or (sign and signatures == 2) else 0
        cert_chain_size = self.cert_chain_size * signatures if sign else 0
        sig_size = MAX_SIG_SIZE * (signatures - 1) if sign else 0
        if sign:
            if ((self.authority != AUTHORITY_QC and self._data_signature_qc) or
                    (self._data_signature_qc and self._data_signature)):
                sig_size += len(self._data_signature_qc)
            else:
                sig_size += self.sig_size

        # Sum
        return (self._mbn_parsegen.get_header().get_size() + hash_table_len + sig_size + cert_chain_size +
                encrypt_params)

    def _get_hash(self, data):
        from sectools.features.isc.signer.utils.hmac import HMAC
        h = HMAC.HASH_ALGO_MAP[self.segment_hash_algorithm]()
        h.update(data)
        return h.digest()

    def _generate_default_hash_segment(self, secboot_version):
        # Get the hash table
        hash_table = self.get_hash_table()

        # Create the header
        mbn_version = SECBOOT_MBN_HDR[secboot_version]
        mbn = ParseGenMbn(header_size=MbnHdr40B.get_size(),
                          version=mbn_version)
        mbn.code = hash_table
        header = mbn.header
        header.image_id = self.image_type
        image_dest_ptr = self._compute_hash_address(self._elf_parsegen) + MbnHdr40B.get_size()
        header.image_size = ALIGNED_IMAGE_SIZE(len(hash_table))
        header.code_size = len(hash_table)
        header.sig_ptr = image_dest_ptr + header.code_size
        header.sig_size = 0
        header.cert_chain_ptr = header.sig_ptr + header.sig_size
        header.cert_chain_size = 0
        mbn.invalidate_pointers = True
        if mbn_version == MBN_HDR_VERSION_3:
            header.flash_parti_ver = MBN_HDR_VERSION_3
            header.image_src = 0
            header.image_dest_ptr = image_dest_ptr
        elif mbn_version == MBN_HDR_VERSION_5:
            header.version = MBN_HDR_VERSION_5
            header.sig_size_qc = 0
            header.cert_chain_size_qc = 0
        else:
            raise RuntimeError('Unsupported MBN header version: ' + str(mbn_version))

        return mbn.get_data()

    def _get_hash_phdr_entry(self, hash_phdr, size, addr, offset):
        hash_phdr.p_type = segment.type.PT_NULL
        hash_phdr.p_flags = PF_OS_SEGMENT_HASH_FULL
        hash_phdr.p_align = ELF_BLOCK_ALIGN
        hash_phdr.p_offset = offset
        hash_phdr.p_memsz = int(math.ceil(float(size) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        hash_phdr.p_filesz = size
        hash_phdr.p_vaddr = addr
        hash_phdr.p_paddr = addr
        return hash_phdr

    def _get_prog_phdr_entry(self, prog_phdr, ehdr):
        prog_phdr.p_type = segment.type.PT_NULL
        prog_phdr.p_flags = PF_OS_SEGMENT_PHDR_FULL
        prog_phdr.p_filesz = ehdr.e_phoff + ((ehdr.e_phnum + 2) * ehdr.e_phentsize)
        return prog_phdr

    def _get_sec_parsegen_mbn_properties(self):
        from sectools.features.isc.parsegen.config.auto_gen_obj_config import Cfg_Mbn_Properties
        properties = Cfg_Mbn_Properties()
        properties.header_size = MbnHdr40B.get_size()
        return properties

    def _decrypt_data(self):
        if self.encdec is None:
            logger.error("Input image is encrypted but decryption of image failed. There are two possible causes:\n"
                + "\t1) Selected_encryptor is not set in config file or command line args.\n"
                + "\t2) You are attempting to sign but not encrypt a previously encryted image. This is not allowed.")
            raise RuntimeError('Image is encrypted. Cannot proceed without a decryptor.')

        decryptor = self.encdec.get_decryptor(encryption_parameters_blob=self.encryption_params,
                                              key=self.encdec.get_dummy_key())

        parsegen_updater = ParseGenEncDec(self.store_debug_data, self.encrypt_segment)
        parsegen_updater.decrypt(self.encdec.get_segment_num_scheme(), self._elf_parsegen, decryptor)

    def _insert_serial_in_ht(self, hash_table, serial):
        hash_len = len(self._get_hash(''))
        serial_packed = struct.pack('L', serial)
        return (hash_table[:hash_len] +
                serial_packed +
                hash_table[hash_len + len(serial_packed):])

    def _compute_hash_address(self, elf_parsegen):
        max_phdr = None
        for phdr in elf_parsegen.phdrs:
            if max_phdr is None or phdr.p_paddr > max_phdr.p_paddr:
                if (phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_SWAPPED):
                    max_phdr = phdr

        return (((max_phdr.p_paddr + max_phdr.p_memsz - 1) &
                 (~(ELF_BLOCK_ALIGN - 1))) +
                ELF_BLOCK_ALIGN)

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'image_type'
    #--------------------------------------------------------------------------
    @property
    def image_type(self):
        return self._image_type

    @image_type.setter
    def image_type(self, value):
        self.validate_image_type(value)
        self._image_type = value

    def validate_image_type(self, value):
        pass

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'max_elf_segments'
    #--------------------------------------------------------------------------
    @property
    def max_elf_segments(self):
        return self._max_elf_segments

    @max_elf_segments.setter
    def max_elf_segments(self, value):
        self._max_elf_segments = value

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'serial_num'
    #--------------------------------------------------------------------------
    @property
    def serial_num(self):
        return self._serial_num

    @serial_num.setter
    def serial_num(self, value):
        if value is not None:
            self.validate_serial_num(value)
            value = Attribute.init(num_bits=32, string=value).value
        self._serial_num = value

    def validate_serial_num(self, value):
        if Attribute.validate(num_bits=32, string=value) is False:
            raise RuntimeError('testsig serial number ' + str(value) + ' is not a valid 32 bit number')

    #--------------------------------------------------------------------------
    # Get & Set functions for 'validate_ph_addrs'
    #--------------------------------------------------------------------------
    @property
    def validate_ph_addrs(self):
        return self._validate_ph_addrs

    @validate_ph_addrs.setter
    def validate_ph_addrs(self, value):
        self._validate_ph_addrs = value

    #--------------------------------------------------------------------------
    # Get & Set functions for 'validate_vir_addrs'
    #--------------------------------------------------------------------------
    @property
    def validate_vir_addrs(self):
        return self._validate_vir_addrs

    @validate_vir_addrs.setter
    def validate_vir_addrs(self, value):
        self._validate_vir_addrs = value


class ParseGenEncDec(object):

    class ParseGenEncDecBase(ParseGenElfDelegate):

        def __init__(self, parsegen, store_debug_data, encrypt_segment):
            ParseGenElfDelegate.__init__(self, parsegen)
            self.orig_delegate = parsegen.delegate
            self.store_debug_data = store_debug_data
            self.encrypt_segment = encrypt_segment
            self.segment_number = -1

        def segment_to_put(self, phdr):
            if not self.increase_if_processing:
                self.segment_number += 1
            return self.orig_delegate.segment_to_put(phdr)

        def process_segment_data(self, phdr, data):
            if (phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH and
                phdr.p_type not in NON_ENCRYPT_SEGMENTS):
                if self.increase_if_processing:
                    self.segment_number += 1
                data = self.encrypt_segment(data, self.segment_number)
                self.store_debug_data('encr_' + str(self.segment_number), data)
            return data

        def decrypt(self, decryptor):
            seg = self.decrypt_seg_number
            increase_if_processing = self.increase_if_processing
            for phdr in self.parsegen.phdrs:
                if (self.parsegen.delegate.segment_to_put(phdr) and
                    phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH and
                    phdr.p_type not in NON_ENCRYPT_SEGMENTS):
                    data = decryptor.decrypt_segment(self.parsegen.segments[phdr], seg)
                    self.store_debug_data('decr_' + str(seg), data)
                    self.parsegen.segments[phdr] = data
                    if increase_if_processing:
                        seg += 1
                if not increase_if_processing:
                    seg += 1

        @property
        def increase_if_processing(self):
            return False

        @property
        def decrypt_seg_number(self):
            return 0


    class ParseGenEncDecZeroBased(ParseGenEncDecBase):

        @property
        def increase_if_processing(self):
            return True

        @property
        def decrypt_seg_number(self):
            return 0


    class ParseGenEncDecTwoBased(ParseGenEncDecBase):

        @property
        def increase_if_processing(self):
            return False

        @property
        def decrypt_seg_number(self):
            return 2


    ParseGenEncDecMap = \
    {
        BaseEncdec.SEGMENT_NUMBER_SCHEME.ZERO_BASED : ParseGenEncDecZeroBased,
        BaseEncdec.SEGMENT_NUMBER_SCHEME.TWO_BASED  : ParseGenEncDecTwoBased,
    }

    def __init__(self, store_debug_data, encrypt_segment):
        self.store_debug_data = store_debug_data
        self.encrypt_segment = encrypt_segment
        self.orig_delegate = None

    # Used for encryption
    def update_parsegen(self, segment_scheme, parsegen):
        self.orig_delegate = parsegen.delegate
        parsegen.delegate = self.ParseGenEncDecMap[segment_scheme](parsegen, self.store_debug_data, self.encrypt_segment)

    # Used for encryption
    def revert_parsegen(self, parsegen):
        parsegen.delegate = self.orig_delegate
        self.orig_delegate = None

    # Used for decryption
    def decrypt(self, segment_scheme, parsegen, decryptor):
        delegate = self.ParseGenEncDecMap[segment_scheme](parsegen, self.store_debug_data, self.encrypt_segment)
        delegate.decrypt(decryptor)
