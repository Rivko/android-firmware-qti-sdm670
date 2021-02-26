# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import math
import os
import re

from six import ensure_binary, ensure_str

from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen import elf
from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.header.ident import ELFCLASS64
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.parsegen.mbn.versions.v3 import MbnHdr40B
from sectools.common.parsegen.mbn.versions.v3.parsegen_mbn import ParseGenMbnV3
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import create_mismatch_table
from sectools.common.utils.c_misc import properties_repr
from sectools.features.isc.defines import ALIGNED_IMAGE_SIZE, MAX_SIG_SIZE
from sectools.features.isc.parsegen.base import SecParseGenBase
from sectools.features.isc.parsegen.elf_support.elf_delegate import ElfDelegate
from sectools.features.isc.parsegen.elf_support.elf_vars import ELF_BLOCK_ALIGN
from sectools.features.isc.parsegen.elf_support.elf_vars import FILE_HASH_SEG_IN
from sectools.features.isc.parsegen.elf_support.elf_vars import FILE_PROG_HASH_REMOVED_IN
from sectools.features.isc.parsegen.elf_support.elf_vars import FILE_PROG_SEG_IN
from sectools.features.isc.parsegen.elf_support.elf_vars import MAX_PHDR_COUNT
from sectools.features.isc.parsegen.elf_support.elf_vars import NON_HASHABLE_SEGMENTS
from sectools.features.isc.parsegen.elf_support.elf_vars import OFFSET_12K
from sectools.features.isc.parsegen.elf_support.elf_vars import elf_vars

# Full flags for specific segments
PF_OS_SEGMENT_HASH_FULL = 0x02200000
PF_OS_SEGMENT_PHDR_FULL = 0x07000000


class ElfV3Base(object):

    HASH_SIZE_20 = 20
    HASH_SIZE_32 = 32
    HASH_SIZE_48 = 48

    HASH_SIZE_TO_ALGO_MAP = {
        HASH_SIZE_20: "sha1",
        HASH_SIZE_32: "sha256",
        HASH_SIZE_48: "sha384",
    }

    def __init__(self,
                 data,
                 parsegen_mbn_class=ParseGenMbnV3,
                 imageinfo=None,
                 elf_properties=None,
                 general_properties=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False, **kwargs):

        # Check the arguments
        if imageinfo is not None:
            elf_properties = imageinfo.image_type.elf_properties
            general_properties = imageinfo.general_properties
        if elf_properties is None:
            raise RuntimeError('ELF properties must not be None.')

        self.secboot_version = general_properties.secboot_version
        self.parsegen_mbn_class = parsegen_mbn_class
        self.HASH_ALGO_TO_SIZE_MAP = dict(
            (algo, size) for size, algo in self.HASH_SIZE_TO_ALGO_MAP.items())
        self.HASH_ALGO_TO_SIZE_MAP[self.parsegen_mbn_class.UNKNOWN_ALGORITHM] = self.HASH_SIZE_32
        self.HASH_ALGO_TO_SIZE_MAP[self.parsegen_mbn_class.NONE_ALGORITHM] = self.HASH_SIZE_32

        # Initialize internal properties
        self._image_type = 0
        self._max_elf_segments = MAX_PHDR_COUNT
        self._validate_ph_addrs = True
        self._validate_vir_addrs = False
        self._hash_padding_size = 0
        self._has_additional_padding = False
        # Following is needed to ensure LOAD segments are not shifted for
        # 4k alignment from previous segment offset; needed for adding prog_hdr and hash segment
        self._align = True
        # Following is needed to keep track of segments which are enclosed within other segments
        self.encapsulated_segments = []

        # Set properties from the config file
        self.image_type = elf_properties.image_type
        self.hash_seg_placement = elf_properties.hash_seg_placement
        self.signer = general_properties.selected_signer
        self.segment_hash_algorithm = general_properties.segment_hash_algorithm
        if elf_properties.max_elf_segments is not None:
            self.max_elf_segments = elf_properties.max_elf_segments
        if elf_properties.validate_ph_addrs is not None:
            self.validate_ph_addrs = elf_properties.validate_ph_addrs
        if elf_properties.validate_vir_addrs is not None:
            self.validate_vir_addrs = elf_properties.validate_vir_addrs

        # Initialize the elf parsegen
        self._elf_parsegen = elf.ParseGenElf(data,
                                             debug_dir=debug_dir,
                                             debug_prefix=debug_prefix,
                                             debug_suffix=debug_suffix,
                                             elf_properties=elf_properties)

        _, hash_seg_entry = self.get_phdr_and_hash_entry(self._elf_parsegen)
        # Populating hash segment address from the extracted hash segment entry from phdr
        if imageinfo and hash_seg_entry:
            imageinfo.hash_seg_address = hash_seg_entry.p_paddr

        self._elf_parsegen.stabilize_phdrs()

        if self.hash_seg_placement == 'POST_PHDR_LOAD_ALIGNED':
            # Below is needed so that segments are not shifted for 4k alignment
            # from offset of previous segment
            self._align = False
            # Get the list of segments which are encapsulated within other segments
            sorted_phdrs, self.encapsulated_segments = self._elf_parsegen.get_sorted_phdrs_and_encapsulated_phdrs(
                self._elf_parsegen.phdrs)
            # pad LOAD segments with 0s if there are any gaps between two consecutive LOAD segments
            self._elf_parsegen.phdrs = sorted_phdrs
            self._update_segments(self._elf_parsegen.phdrs)
            if validating:
                self._validate_filesz(self._elf_parsegen.phdrs,
                                      os.path.getsize(imageinfo.image_under_operation))

        # Remove the prog header and hash segment
        phdr_segment, hash_segment = self.extract_phdr_hash(self._elf_parsegen)
        self.store_debug_data(FILE_PROG_SEG_IN, phdr_segment,
                              debug_dir, debug_prefix, debug_suffix)
        self.store_debug_data(FILE_HASH_SEG_IN, hash_segment,
                              debug_dir, debug_prefix, debug_suffix)
        self.store_debug_data(FILE_PROG_HASH_REMOVED_IN, self._elf_parsegen.get_data(),
                              debug_dir, debug_prefix, debug_suffix)

        self.hash_segment = hash_segment
        self.physical_address_hash_seg = None  # This will be populated with hash segment address present in double signed images for oem signing/re-signing and populated during validation for all kind of images
        # Construct a MBN object
        super(ElfV3Base, self).__init__(hash_segment if hash_segment else self._generate_default_hash_segment(),
                                        imageinfo=None,
                                        mbn_properties=self._get_sec_parsegen_mbn_properties(),
                                        general_properties=general_properties,
                                        debug_dir=debug_dir,
                                        debug_prefix=debug_prefix,
                                        debug_suffix=debug_suffix,
                                        validating=validating,
                                        signing=signing,
                                        parsegens=parsegens,
                                        sign_attr=sign_attr, **kwargs)

        # Controls whether hash table entries should be created for each segment page
        self.hash_pageseg_as_segment = False

        # Extract the hash table from the extracted hash segment
        extracted_hash_table_size = self.code_size if hash_segment else 0

        # By now, self should have already had a MBN object, which in turn has
        # self._mbn_parsegen or an instance of ParseGenMbn class. So, update
        # self._mbn_parsegen.invalidate_pointers state to either True or False.
        self.update_invalidate_pointers_state(
            self._elf_parsegen.ehdr.e_ident_class == ELFCLASS64)

        # Compute hash table size from the extracted hash segment if any.
        # This needs to wait until the self object is created.
        self._extract_hash_segment_algorithm(extracted_hash_table_size)

        # Set the elf parsegen delegate
        delegate = ElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
        self._elf_parsegen.delegate = delegate

    def _repr_properties(self):
        return SecParseGenBase._repr_properties(self)

    def __repr__(self):
        return ('Base Properties: ' + '\n' + properties_repr(self._repr_properties()) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n')

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return cls.__name__

    @classmethod
    def file_type(cls):
        # cls.file_type() must return "elf". It applies to all ELF-typed parsegens.
        # This is used for internal checking.
        return "elf"

    @property
    def data_to_sign(self):
        # Add the header and hash segment
        prog_phdr_entry, hash_phdr_entry, _ =\
            self._add_phdr_and_hash_segs(self.sign)

        # Get the data to sign from the hash segment
        data = self.get_hash_segment_data_to_sign()

        # Remove the prog and hash
        self._elf_parsegen.remove_segment(prog_phdr_entry)
        self._elf_parsegen.remove_segment(hash_phdr_entry)

        return data

    @property
    def data_to_sign_format(self):
        # Add the header and hash segment
        prog_phdr_entry, hash_phdr_entry, _ =\
            self._add_phdr_and_hash_segs(self.sign)

        # Get the format of the data to sign
        fmt = self.get_hash_segment_format()

        # Remove the prog and hash
        self._elf_parsegen.remove_segment(prog_phdr_entry)
        self._elf_parsegen.remove_segment(hash_phdr_entry)

        return fmt

    def contains_integrity_check(self):
        return self.hash_segment != ''

    def get_data(self, integrity_check=None, sign=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        integrity_check = True if (integrity_check or sign) else False

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign)

        if not (integrity_check or sign):
            data = self._elf_parsegen.get_data()
        else:
            # Add the header and hash segment
            prog_phdr_entry, hash_phdr_entry, hash_segment_size =\
                self._add_phdr_and_hash_segs(sign)

            try:
                # Generate the hash segment now
                hash_segment = self.get_hash_segment(sign)

                hash_segment += PAD_BYTE_1 * self._hash_padding_size

                # Check here for sizes mismatching just in case
                if len(hash_segment) != hash_segment_size:
                    raise RuntimeError('Estimated hash table size was wrong. Estimate - ' +
                                       str(hash_segment_size) + ', Actual - ' + str(len(hash_segment)))

                # Re-add the hash segment, this time with the real data
                self._elf_parsegen.remove_segment(hash_phdr_entry)
                self._elf_parsegen.remove_segment(prog_phdr_entry)
                self._elf_parsegen.add_segment(prog_phdr_entry, '', toalign=self._align)
                self._elf_parsegen.add_segment(hash_phdr_entry, hash_segment, index=1, toalign=self._align)

                try:
                    # Get the elf data
                    data = self._elf_parsegen.get_data()
                finally:
                    pass
            finally:
                # Remove the prog
                try:
                    self._elf_parsegen.remove_segment(prog_phdr_entry)
                except Exception as e:
                    logger.warning(str(e))

                # Remove the hash
                try:
                    self._elf_parsegen.remove_segment(hash_phdr_entry)
                except Exception as e:
                    logger.warning(str(e))
        return data

    def _update_segments(self, phdrs):
        for indx, phdr in enumerate(phdrs):
            if phdr in self.encapsulated_segments:
                continue
            try:
                next_phdr = phdrs[indx + 1]
                if phdr.p_type == PT_LOAD:
                    # Current segment is loadable.
                    padding_size = next_phdr.p_offset - (phdr.p_offset + phdr.p_filesz)
                    if padding_size > 0:
                        phdr.p_filesz += padding_size
                        self._elf_parsegen.segments[phdr] += PAD_BYTE_0 * padding_size
                    phdr.p_memsz = phdr.p_filesz
                else:
                    # Current segment is not loadable
                    if phdr.f_os_segment_type == segment.flags.PF_OS_SEGMENT_HASH:
                        # The current program header is for the hash segment.
                        # Compute the padding size, so that the next loadable segment
                        # is aligned at offset 0x3000, without any gap in between.
                        self._hash_padding_size = OFFSET_12K - phdr.p_offset - phdr.p_filesz
                        phdr.p_filesz += self._hash_padding_size
                # Update next segment's offset
                next_phdr.p_offset = phdr.p_offset + phdr.p_filesz
            except IndexError:
                # For the last segment, make sure the memsz == filesz.
                if phdr.p_type == PT_LOAD:
                    phdr.p_memsz = phdr.p_filesz

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def extract_phdr_hash(self, elf_parsegen):
        phdr_segment, hash_segment = '', ''
        phdr_entry, hash_entry = self.get_phdr_and_hash_entry(elf_parsegen)
        # Remove the phdr entries
        if phdr_entry:
            phdr_segment = elf_parsegen.get_segment_data(phdr_entry)
            elf_parsegen.remove_segment(phdr_entry)
        if hash_entry:
            hash_segment = elf_parsegen.get_segment_data(hash_entry)
            elf_parsegen.remove_segment(hash_entry)

        return phdr_segment, hash_segment

    def get_phdr_and_hash_entry(self, elf_parsegen):
        phdr_entry, hash_entry = None, None
        # Find the phdr entries
        for phdr in elf_parsegen.phdrs:
            if phdr.f_os_segment_type == segment.flags.PF_OS_SEGMENT_PHDR:
                phdr_entry = phdr
            elif phdr.f_os_segment_type == segment.flags.PF_OS_SEGMENT_HASH:
                hash_entry = phdr
            if phdr_entry and hash_entry:
                break
        return phdr_entry, hash_entry

    def get_hash_segment(self, sign):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        return self._get_data_int(sign)

    def get_hash_segment_data_to_sign(self):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        # Return data_to_sign of MBN hash segment, instead of that of ELF.
        return super(ElfV3Base, self).data_to_sign

    def get_hash_segment_format(self):
        hash_table = self.get_hash_table()
        self._mbn_parsegen.code = hash_table
        # Return format of the Hash Table Segment
        return super(ElfV3Base, self).data_to_sign_format

    def get_hash_table(self, get_hash_count=False):
        # Check the number of headers that can be hashed
        phdr_count = len(self._elf_parsegen.phdrs)
        if phdr_count > self.max_elf_segments:
            raise RuntimeError('Number of prog header entries in ELF "' + str(phdr_count) + '" '
                               'is more than the maximum supported "' + str(self.max_elf_segments) + '"')

        hash_table = b''
        hash_count = 0

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
            if (page_mode == non_paged) or (page_mode == paged and self.hash_pageseg_as_segment):
                if phdr.p_type not in elf_vars[NON_HASHABLE_SEGMENTS]:
                    if get_hash_count:
                        hash_count += 1
                    else:
                        data_to_hash = self._elf_parsegen.get_segment_data(phdr)
                        hash_table += get_hash(data_to_hash)
            # Hash as pages
            elif page_mode == paged:
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
                    if get_hash_count:
                        hash_count += 1
                    else:
                        data_to_hash = data[:ELF_BLOCK_ALIGN]
                        hash_table += get_hash(data_to_hash)
                    data = data[ELF_BLOCK_ALIGN:]
            # Invalid page mode flag
            else:
                raise RuntimeError('Invalid page mode encountered.')

        hash_table = self.insert_testsig(hash_table, get_hash_count)

        if get_hash_count:
            return hash_count
        else:
            return hash_table

    def insert_testsig(self, hash_table, get_hash_count):
        return hash_table

    def _add_phdr_and_hash_segs(self, sign, **kwargs):
        """
        Add the prog hdr and hash_seg in the right places according to
        elf_properties.hash_seg_placement
        """
        #### Add the prog phdr entry first
        # The data in the prog and hash at this time may be dummy data
        phdr_class = self._elf_parsegen.get_new_phdr_entry()
        prog_phdr_entry = self._get_prog_phdr_entry(phdr_class(),
                                                    self._elf_parsegen.ehdr)
        proghdr_end = prog_phdr_entry.p_offset + prog_phdr_entry.p_filesz
        # Get the size of the hash segment
        hash_segment_size = self._compute_hash_segment_size(sign, **kwargs)
        hash_segment_addr = self._compute_hash_address(self._elf_parsegen)
        align_func = lambda x: int(math.ceil(float(x + 1) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        end_byte_aligned = re.search('END_([0-9]+)B_ALIGNED', ensure_str(self.hash_seg_placement)) if self.hash_seg_placement else None
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
                          align_func(hash_segment_offset + hash_segment_size - prog_phdr_entry.p_offset),
                          OFFSET_12K)
            logger.debug('Adding hash seg directly after phdrs. Placing next segment at 12k boundary.')
            self._hash_padding_size = OFFSET_12K - hash_segment_offset - hash_segment_size
            hash_segment_size += self._hash_padding_size
        elif self.hash_seg_placement == 'END_ALIGNED':
            last_phdr_entry = get_last_seg(self._elf_parsegen.phdrs)
            hash_segment_offset = align_func(last_phdr_entry.offset + last_phdr_entry.filesz)
            shift_data = None
            logger.debug('Adding hash seg after last segment, aligned to 0x1000.')
        elif end_byte_aligned is not None:
            # Get the byte alignment
            alignment = int(end_byte_aligned.group(1))

            # Get size of the hash seg minus sign, certs, etc
            size_of_hash_seg = self._compute_hash_segment_size(False, **kwargs)

            # Shift the last load segment so the end of hash table is aligned
            last_phdr_entry = get_last_seg(self._elf_parsegen.phdrs)
            last_seg_remainder = (last_phdr_entry.p_offset + last_phdr_entry.p_filesz + size_of_hash_seg) % alignment
            if last_seg_remainder > 0:
                self._elf_parsegen.shift(last_phdr_entry.p_offset, alignment - last_seg_remainder, after=True)

            # Get the offset for hash segment
            hash_segment_offset = last_phdr_entry.p_offset + last_phdr_entry.p_filesz
            shift_data = (prog_phdr_entry.p_offset,
                          prog_phdr_entry.p_filesz,
                          None)
            logger.debug('Adding hash seg after last segment with end of hash table {0}B aligned'.format(alignment))
        else:
            raise RuntimeError('Unknown hash_seg_placement value: %s' % self.hash_seg_placement)

        hash_phdr_entry = self._get_hash_phdr_entry(phdr_class(),
                                                    hash_segment_size,
                                                    hash_segment_addr,
                                                    hash_segment_offset
                                                    )
        # shift to make room for the phdr table
        if shift_data:
            self._elf_parsegen.shift(shift_data[0],
                                     shift_data[1],
                                     shift_data[2])

        # Add segment
        self._elf_parsegen.add_segment(prog_phdr_entry, '', toalign=self._align)

        # hash seg contains dummy data for now
        self._elf_parsegen.add_segment(hash_phdr_entry, PAD_BYTE_1 * int(hash_segment_size), index=1,
                                       toalign=self._align)

        # Update the dest pointer for the mb
        if hasattr(self._mbn_parsegen.header, 'image_dest_ptr'):
            self._mbn_parsegen.header.image_dest_ptr = hash_segment_addr + self._mbn_parsegen.header.get_size()

        return prog_phdr_entry, hash_phdr_entry, hash_segment_size

    def _get_hash(self, data):
        from sectools.features.isc.signer.utils.hmac import HMAC
        h = HMAC.HASH_ALGO_MAP[self.segment_hash_algorithm]()
        h.update(ensure_binary(data))
        return h.digest()

    def _generate_default_hash_segment(self):
        # Get the hash table
        hash_table = self.get_hash_table()

        # Create the header
        mbn_version = self.secboot_to_mbn_header_version
        mbn = self.parsegen_mbn_class(
            header_size=MbnHdr40B.get_size(), version=mbn_version)
        mbn.code = hash_table
        header = mbn.header
        header.image_id = self.image_type
        image_dest_ptr = self._compute_hash_address(self._elf_parsegen) + mbn.header_size
        header.image_size = ALIGNED_IMAGE_SIZE(len(hash_table))
        header.code_size = len(hash_table)
        header.sig_ptr = image_dest_ptr + header.code_size
        header.sig_size = 0
        header.cert_chain_ptr = header.sig_ptr + header.sig_size
        header.cert_chain_size = 0
        mbn.invalidate_pointers = True
        self._update_mbn_header_(header, image_dest_ptr, mbn_version)
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

    def _compute_hash_address(self, elf_parsegen):
        if self.physical_address_hash_seg:
            return self.physical_address_hash_seg
        max_phdr = None
        # Added logic to compute hash segment's physical address with encapsulated segments
        _, encapsulated_phdrs = elf_parsegen.get_sorted_phdrs_and_encapsulated_phdrs(elf_parsegen.phdrs)
        for phdr in elf_parsegen.phdrs:
            if max_phdr is None or phdr.p_paddr > max_phdr.p_paddr:
                if phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_SWAPPED and phdr not in encapsulated_phdrs:
                    max_phdr = phdr
        if max_phdr is None:
            return ELF_BLOCK_ALIGN
        else:
            return ((max_phdr.p_paddr + max_phdr.p_memsz - 1) & (~(ELF_BLOCK_ALIGN - 1))) + ELF_BLOCK_ALIGN

    def _compute_hash_segment_size(self, sign, **kwargs):
        hash_table_len = ((len(self._get_hash('')) * 2) +
                          len(self.get_hash_table()))

        # Check configuration
        signatures = 1
        cert_chain_size = 0
        if sign:
            if signatures == 1:
                cert_chain_size = self.get_cert_chain_size(self.authority)
            else:
                cert_chain_size = (self.get_cert_chain_size(self.authority) +
                                   self.get_cert_chain_size(
                                       self.get_other_authority(self.authority)))

        sig_size = MAX_SIG_SIZE * (signatures - 1) if sign else 0
        if sign:
            sig_size += self.sig_size

        # Sum
        return (self._mbn_parsegen.get_header(self.authority,
                                              self.imageinfo,
                                              self.validating,
                                              self.signing,
                                              self.add_signing_attr_to_hash_seg)
                .get_size() + hash_table_len + sig_size + cert_chain_size)

    def _extract_hash_segment_algorithm(self, hash_table_size=0):
        if hash_table_size > 0:
            hash_count = self.get_hash_table(get_hash_count=True)

            # add 2: +1 for Hash Table Entry 0 (ELF + Program Header).
            #        +1 for Hash Table Entry 1 (Dummy Entry)
            hash_size = int(hash_table_size / (hash_count + 2))

            logger.debug("Number of hash entries: " + str(hash_count))
            logger.debug("Hash table size: " + str(hash_table_size))
            logger.debug("Hash size: " + str(hash_size))

            if hash_size in self.HASH_SIZE_TO_ALGO_MAP.keys():
                self._mbn_parsegen.extracted_segment_hash_algorithm =\
                    self.HASH_SIZE_TO_ALGO_MAP[hash_size]
            else:
                self._mbn_parsegen.extracted_segment_hash_algorithm =\
                    self.parsegen_mbn_class.UNKNOWN_ALGORITHM

            logger.debug("Determined hash table algorithm: " +
                         self._mbn_parsegen.extracted_segment_hash_algorithm)

            # Determine if parsegen was created during validation (-a) and
            # print error if hash algorithm mismatch is present
            extracted_segment_hash_algorithm =\
                self._mbn_parsegen.extracted_segment_hash_algorithm
            segment_hash_algorithm = self.segment_hash_algorithm if\
                self.segment_hash_algorithm is not None else "sha256"
            if (extracted_segment_hash_algorithm != segment_hash_algorithm and
                    self.validating):
                errstr = list()
                mismatches = list()
                mismatches.append(
                    ("Hash Segment Algorithm", extracted_segment_hash_algorithm,
                     segment_hash_algorithm))
                create_mismatch_table(
                    mismatches, errstr, data_type_to_compare="Attribute",
                    image_region="Hash Segment")
                logger.warning('Following validations failed for the image:\n'
                               '       ' +
                               '\n       '.join([(str(i + 1) + '. ' + e) for
                                                 i, e in enumerate(errstr)]))

    def _get_sec_parsegen_mbn_properties(self):
        class MbnProperties(object):
            """ Empty class definition to host header_size attribute. """
        properties = MbnProperties()
        properties.header_size = MbnHdr40B.get_size()
        return properties

    #--------------------------------------------------------------------------
    # Method to validate sum of segment sizes equals to total output file size
    #--------------------------------------------------------------------------
    def _validate_filesz(self, phdrs, ref_size):
        segments_size = 0
        for phdr in phdrs:
            if phdr not in self.encapsulated_segments:
                segments_size += phdr.p_filesz

        if ref_size != segments_size:
            raise RuntimeError(
                "Output ELF validation failed. Sum of all segment sizes [" +
                str(segments_size) +
                "] is not equal to output signed file size [" + str(ref_size) + "]")

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


def get_last_seg(x):
    # Returns the last segment in a list of phdrs
    return sorted([s for s in x], key=lambda s: s.p_offset + s.p_filesz)[-1]
