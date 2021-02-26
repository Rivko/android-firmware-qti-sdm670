#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
'''
Created on Nov 25, 2013

@author: hraghav
'''

import hashlib
import math
import struct

from sectools.common.parsegen import elf, PAD_BYTE_1, PAD_BYTE_0
from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.header.ident import ELFCLASS64
from sectools.common.parsegen.elf.segment.format import repr_phdrs
from sectools.common.parsegen.elf.segment.type import PT_NULL
from sectools.common.parsegen.mbn.format import ALIGNED_IMAGE_SIZE, ParseGenMbn
from sectools.common.parsegen.mbn.header import MbnHdr40B
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_base import ValPolicy
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import properties_repr, hex_addr, TablePrinter
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.mbn import SecParseGenMbn


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
FLASH_PARTI_VERSION         = 3             # Flash partition version

# Full flags for specific segments
PF_OS_SEGMENT_HASH_FULL     = 0x02200000
PF_OS_SEGMENT_BOOT_FULL     = 0x03200000
PF_OS_SEGMENT_PHDR_FULL     = 0x07000000

# Selected policy for overlapping segments
POLICY_OVERLAPPING_SEGMENTS = ValPolicy(ValPolicy.WARN)
POLICY_NON_LOAD_OUTSIDE_LOAD = ValPolicy(ValPolicy.ERROR)

# Segments to ignore for hashing
NON_HASHABLE_SEGMENTS = [segment.type.PT_PHDR]

def set_hashable_segments(segments):
    global NON_HASHABLE_SEGMENTS
    NON_HASHABLE_SEGMENTS = segments


class SecParseGenElf(SecParseGenMbn):

    def __init__(self, data, imageinfo=None, elf_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

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

        # Set properties from the config file
        self.image_type = elf_properties.image_type
        self.serial_num = general_properties.testsig_serialnum
        if elf_properties.max_elf_segments is not None:
            self.max_elf_segments = elf_properties.max_elf_segments

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
            hash_segment = self._generate_default_hash_segment()

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

        if self._elf_parsegen.ehdr.e_ident_class == ELFCLASS64:
            self._mbn_parsegen.invalidate_pointers = True

        # Check if the file is encrypted
        if self.is_encrypted():
            self._decrypt_data()
            self.store_debug_data(FILE_DECRYPTED_IN, self._elf_parsegen.get_data())

            # Get the original data
            self._elf_parsegen = elf.ParseGenElf(self._elf_parsegen.get_data(),
                                                 self.debug_dir,
                                                 self.debug_prefix,
                                                 self.debug_suffix)

        # Do any segment checks
        self._validate_segments()

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n'
                'SecElf Properties: ' + '\n'
                '' + properties_repr([('image_type', self.image_type),
                                      ('max_elf_segments', self.max_elf_segments),
                                      ('testsig_serialnum', hex_addr(self.serial_num) if self.serial_num is not None else self.serial_num),]))

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
            # Get the size of the hash segment
            hash_segment_size = self._compute_hash_segment_size(integrity_check, sign, encrypt)
            hash_segment_addr = self._compute_hash_address(self._elf_parsegen)

            # Add the prog & hash entries in phdrs
            # The data in the prog and hash at this time maybe dummy data
            phdr_class = self._elf_parsegen.get_new_phdr_entry()
            prog_phdr_entry = self._get_prog_phdr_entry(phdr_class(),
                                                        self._elf_parsegen.ehdr)
            hash_phdr_entry = self._get_hash_phdr_entry(phdr_class(),
                                                        hash_segment_size,
                                                        hash_segment_addr,
                                                        prog_phdr_entry.p_offset +
                                                        prog_phdr_entry.p_filesz)
            self._elf_parsegen.shift(prog_phdr_entry.p_offset,
                                     hash_phdr_entry.p_offset + hash_phdr_entry.p_filesz -
                                     prog_phdr_entry.p_offset,
                                     hash_phdr_entry.p_align)
            self._elf_parsegen.add_segment(prog_phdr_entry, '')
            self._elf_parsegen.add_segment(hash_phdr_entry, PAD_BYTE_1 * hash_segment_size, index=1)

            # Update the dest pointer for the mbn
            self._mbn_parsegen.header.image_dest_ptr = hash_segment_addr + self._mbn_parsegen.header.get_size()

            try:
                # Generate the hash segment now
                hash_segment = self.get_hash_segment(integrity_check, sign, encrypt)

                # Check here for sizes mismatching just in case
                if len(hash_segment) != hash_segment_size:
                    raise RuntimeError('Estimating the size of the hash table was wrong.')

                # Re-add the hash segment, this time with the real data
                self._elf_parsegen.remove_segment(hash_phdr_entry)
                self._elf_parsegen.remove_segment(prog_phdr_entry)
                self._elf_parsegen.add_segment(prog_phdr_entry, '')
                self._elf_parsegen.add_segment(hash_phdr_entry, hash_segment, index=1)

                # If encrypting, change the process segment data
                if encrypt:
                    parsegen_updater_cls = ParseGenEncDecMap[self.encdec.get_segment_num_scheme()]
                    parsegen_updater = parsegen_updater_cls(self.store_debug_data,
                                                            self.encrypt_segment)
                    parsegen_updater.update_parsegen(self._elf_parsegen)

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
        # Get the size of the hash segment
        hash_segment_size = self._compute_hash_segment_size(self.integrity_check, self.sign, self.encrypt)
        hash_segment_addr = self._compute_hash_address(self._elf_parsegen)

        # Add the prog & hash entries in phdrs
        # The data in the prog and hash at this time maybe dummy data
        phdr_class = self._elf_parsegen.get_new_phdr_entry()
        prog_phdr_entry = self._get_prog_phdr_entry(phdr_class(),
                                                    self._elf_parsegen.ehdr)
        hash_phdr_entry = self._get_hash_phdr_entry(phdr_class(),
                                                    hash_segment_size,
                                                    hash_segment_addr,
                                                    prog_phdr_entry.p_offset +
                                                    prog_phdr_entry.p_filesz)
        self._elf_parsegen.shift(prog_phdr_entry.p_offset,
                                 hash_phdr_entry.p_offset + hash_phdr_entry.p_filesz -
                                 prog_phdr_entry.p_offset,
                                 hash_phdr_entry.p_align)
        self._elf_parsegen.add_segment(prog_phdr_entry, '')
        self._elf_parsegen.add_segment(hash_phdr_entry, PAD_BYTE_1 * hash_segment_size, index=1)

        # Update the dest pointer for the mbn
        self._mbn_parsegen.header.image_dest_ptr = hash_segment_addr + self._mbn_parsegen.header.get_size()

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

    def _compute_hash_segment_size(self, integrity_check, sign, encrypt):
        hash_table_len = ((len(self._get_hash('')) * 2) +
                          len(self.get_hash_table()))
        return (self._mbn_parsegen.get_header().get_size() +        # Mbn Header
                hash_table_len +                                    # Hash of all segments
                (self.sig_size if sign else 0) +                    # Size of signature
                (self.cert_chain_size if sign else 0) +             # Size of cert chain
                (len(self.encryption_params) if encrypt else 0)     # Size of encryption params
               )

    @classmethod
    def _get_hash(cls, data):
        h = hashlib.sha256()
        h.update(data)
        return h.digest()

    def _generate_default_hash_segment(self):
        # Get the hash table
        hash_table = self.get_hash_table()

        # Create the header
        mbn = ParseGenMbn(header_size=MbnHdr40B.get_size())
        mbn.code = hash_table
        header = mbn.header
        header.image_id = self.image_type
        header.flash_parti_ver = FLASH_PARTI_VERSION
        header.image_src = 0
        header.image_dest_ptr = self._compute_hash_address(self._elf_parsegen) + MbnHdr40B.get_size()
        header.image_size = ALIGNED_IMAGE_SIZE(len(hash_table))
        header.code_size = len(hash_table)
        header.sig_ptr = header.image_dest_ptr + header.code_size
        header.sig_size = 0
        header.cert_chain_ptr = header.sig_ptr + header.sig_size
        header.cert_chain_size = 0
        mbn.invalidate_pointers = True

        return mbn.get_data()

    def _get_hash_phdr_entry(self, hash_phdr, size, addr, proghdr_end):
        hash_phdr.p_type = segment.type.PT_NULL
        hash_phdr.p_flags = PF_OS_SEGMENT_HASH_FULL
        hash_phdr.p_align = ELF_BLOCK_ALIGN
        hash_phdr.p_offset = int(math.ceil(float(proghdr_end + 1)/ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        hash_phdr.p_memsz = int(math.ceil(float(size)/ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
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
            raise RuntimeError('Image is encrypted. Cannot proceed without a decryptor.')

        decryptor = self.encdec.get_decryptor(encryption_parameters_blob=self.encryption_params,
                                               key=self.encdec.get_dummy_key())

        parsegen_updater_cls = ParseGenEncDecMap[self.encdec.get_segment_num_scheme()]
        parsegen_updater = parsegen_updater_cls(self.store_debug_data,
                                                self.encrypt_segment)
        parsegen_updater.decrypt(decryptor, self._elf_parsegen)

    @classmethod
    def _insert_serial_in_ht(cls, hash_table, serial):
        hash_len = len(cls._get_hash(''))
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
                 (~ (ELF_BLOCK_ALIGN - 1))) +
                ELF_BLOCK_ALIGN)

    def _validate_segments(self):
        if not POLICY_OVERLAPPING_SEGMENTS.is_ignore():
            # Collect all the phys addr ranges
            seg_ranges = []
            for phdr in self._elf_parsegen.phdrs:
                if self._elf_parsegen._segment_to_put(phdr):
                    seg_ranges.append((phdr.p_paddr, phdr.p_paddr + phdr.p_memsz, phdr.p_memsz))

            # Sort ranges by start address
            seg_ranges.sort(key=lambda x: x[0])

            # Check for overlaps
            overlapping = []
            for idx in range(len(seg_ranges)-1):
                if seg_ranges[idx+1][0] < seg_ranges[idx][1]:
                    overlapping.append((seg_ranges[idx], seg_ranges[idx+1]))

            if overlapping:
                # Create table header
                table = TablePrinter([1])
                table.insert_data(0, 0, 'S.No.')
                table.insert_data(0, 1, 'Segment A', column_end=2)
                table.insert_data(0, 3, 'Segment B', column_end=4)

                # Create sub header
                table.insert_data(1, 1, 'Phys')
                table.insert_data(1, 2, 'Size')
                table.insert_data(1, 3, 'Phys')
                table.insert_data(1, 4, 'Size')

                # Add all the overlapping segments
                for idx, overlap in enumerate(overlapping):
                    table.insert_data(idx + 2, 1, hex(overlap[0][0]))
                    table.insert_data(idx + 2, 2, hex(overlap[0][2]))
                    table.insert_data(idx + 2, 3, hex(overlap[1][0]))
                    table.insert_data(idx + 2, 4, hex(overlap[1][2]))

                # Create the error string
                POLICY_OVERLAPPING_SEGMENTS.run('Following overlapping segments were found: ' + '\n' +
                                                '\n'.join(['  ' + l for l in table.get_data().split('\n')]))

        if not POLICY_NON_LOAD_OUTSIDE_LOAD.is_ignore():
            load_ranges = []
            non_load_ranges = []

            # Collect address ranges
            for idx, phdr in enumerate(self._elf_parsegen.phdrs):
                if self._elf_parsegen._segment_to_put(phdr):
                    load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz))
                elif phdr.p_type != PT_NULL and phdr.p_filesz != 0:
                    non_load_ranges.append((phdr.p_offset, phdr.p_offset + phdr.p_filesz, phdr, idx+3))

            # Sort ranges by start address
            load_ranges.sort(key=lambda x: x[0])
            non_load_ranges.sort(key=lambda x: x[0])

            # Check for non-encapsulated segments
            non_encap = []
            non_encap_sno = []
            for d in range(len(non_load_ranges)):
                in_seg = False
                for s in range(len(load_ranges)):
                    in_rng = load_ranges[s][0] <= non_load_ranges[d][0], non_load_ranges[d][1] <= load_ranges[s][1]
                    if in_rng[0] and in_rng[1]:
                        in_seg = True
                # if non load segment not encapsulated in any segment, add to non_encapsulated list
                if not in_seg:
                    non_encap.append(non_load_ranges[d][2])
                    non_encap_sno.append(non_load_ranges[d][3])

            if non_encap:
                # Create the error string
                POLICY_NON_LOAD_OUTSIDE_LOAD.run("Following non-loadable segments found outside load segments: " + '\n' +
                                                 repr_phdrs(non_encap, non_encap_sno))

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


class ParseGenEncDecBase(object):

    def __init__(self, store_debug_data, encrypt_segment):
        self.store_debug_data = store_debug_data
        self.encrypt_segment = encrypt_segment
        self.segment_number = -1

    def update_parsegen(self, parsegen):
        self.orig_psd = parsegen._process_segment_data
        self.orig_stp = parsegen._segment_to_put
        parsegen._process_segment_data = self.process_segment_data
        parsegen._segment_to_put = self.segment_to_put

    def revert_parsegen(self, parsegen):
        parsegen._process_segment_data = self.orig_psd
        parsegen._segment_to_put = self.orig_stp

    def segment_to_put(self, phdr):
        if not self.increase_if_processing:
            self.segment_number += 1
        return self.orig_stp(phdr)

    def process_segment_data(self, phdr, data):
        if phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH:
            if self.increase_if_processing:
                self.segment_number += 1
            data = self.encrypt_segment(data, self.segment_number)
            self.store_debug_data('encr_' + str(self.segment_number), data)
        return data

    def decrypt(self, decryptor, parsegen):
        seg = self.decrypt_seg_number
        increase_if_processing = self.increase_if_processing
        for phdr in parsegen.phdrs:
            if (parsegen._segment_to_put(phdr) and
                phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH):
                data = decryptor.decrypt_segment(parsegen.segments[phdr], seg)
                self.store_debug_data('decr_' + str(seg), data)
                parsegen.segments[phdr] = data
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
