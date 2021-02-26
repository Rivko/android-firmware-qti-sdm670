# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Nov 25, 2013

@author: hraghav
'''

import math
import re
import struct

from sectools.common.parsegen import elf, PAD_BYTE_1, PAD_BYTE_0
from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.format import ParseGenElfDelegate
from sectools.common.parsegen.elf.header.ident import ELFCLASS64
from sectools.common.parsegen.elf.segment.format import repr_phdrs
from sectools.common.parsegen.elf.segment.license_manager import LicenseManagerSegment
from sectools.common.parsegen.elf.segment.multi_image import MultiImageSegment
from sectools.common.parsegen.elf.segment.type import PT_NULL, PT_NOTE, \
    PT_LOAD, PT_PHDR
from sectools.common.parsegen.mbn.format import ALIGNED_IMAGE_SIZE, ParseGenMbn
from sectools.common.parsegen.mbn.header import MbnHdr40B, MBN_HDR_VERSION_3, MBN_HDR_VERSION_5, MBN_HDR_VERSION_6
from sectools.common.utils.c_attribute import Attribute
from sectools.common.utils.c_base import ValPolicy
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import properties_repr, hex_addr, TablePrinter, multi_image_string
from sectools.features.isc.defines import SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0, AUTHORITY_QTI
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.encryption_service.unified import encrypted_key_provider_id_supported
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
get_last_seg = lambda x: sorted([s for s in x], key=lambda s: s.p_offset + s.p_filesz)[-1]

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
                POLICY_NON_LOAD_OUTSIDE_LOAD.run("Following non-loadable segments are not completely encapsulated by a load segment: " + '\n' +
                                                 repr_phdrs(non_encap, non_encap_sno))


class SecParseGenElf(SecParseGenMbn):

    HASH_SIZE_20 = 20
    HASH_SIZE_32 = 32
    HASH_SIZE_48 = 48

    HASH_SIZE_TO_ALGO_MAP = {
        HASH_SIZE_20: "sha1",
        HASH_SIZE_32: "sha256",
        HASH_SIZE_48: "sha384",
    }

    HASH_ALGO_TO_SIZE_MAP = dict((algo, size) for size, algo in HASH_SIZE_TO_ALGO_MAP.iteritems())
    HASH_ALGO_TO_SIZE_MAP[ParseGenMbn.UNKNOWN_ALGORITHM] = HASH_SIZE_32
    HASH_ALGO_TO_SIZE_MAP[ParseGenMbn.NONE_ALGORITHM] = HASH_SIZE_32

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

        SecParseGenBase.__init__(self,
                                 data,
                                 imageinfo,
                                 general_properties,
                                 encdec,
                                 debug_dir,
                                 debug_prefix,
                                 debug_suffix,
                                 validating,
                                 signing,
                                 parsegens,
                                 sign_attr)

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
        self.has_multi_image_segment = False
        self.has_license_manager_segment = False
        self.lib_id = general_properties.lib_id
        self.client_id = general_properties.client_id
        self.image_type = elf_properties.image_type
        self.hash_seg_placement = elf_properties.hash_seg_placement
        self.serial_num = general_properties.testsig_serialnum
        self.signer = general_properties.selected_signer
        if elf_properties.max_elf_segments is not None:
            self.max_elf_segments = elf_properties.max_elf_segments
        if elf_properties.validate_ph_addrs is not None:
            self.validate_ph_addrs = elf_properties.validate_ph_addrs
        if elf_properties.validate_vir_addrs is not None:
            self.validate_vir_addrs = elf_properties.validate_vir_addrs
        if elf_properties.has_multi_image_segment is not None:
            self.has_multi_image_segment = elf_properties.has_multi_image_segment
            self.multi_image_segment_phys_addr = self.imageinfo.multi_image_segment_phys_addr
        self._multi_image_segment = None
        if elf_properties.has_license_manager_segment is not None:
            self.has_license_manager_segment = elf_properties.has_license_manager_segment
        self._license_manager_segment = None

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

        # Extract the hash table from the extracted hash segment
        if hash_segment:
            extracted_hash_table_size = SecParseGenMbn(data=hash_segment,
                                                       imageinfo=None,
                                                       mbn_properties=self._get_sec_parsegen_mbn_properties(),
                                                       general_properties=general_properties,
                                                       encdec=self.encdec,
                                                       debug_dir=debug_dir,
                                                       debug_prefix=debug_prefix,
                                                       debug_suffix=debug_suffix,
                                                       validating=validating,
                                                       signing=signing).code_size
        else:
            extracted_hash_table_size = 0
            # If hash_segment is empty, create dummy hash_segment
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
                                debug_suffix=debug_suffix,
                                validating=validating,
                                signing=signing,
                                parsegens=parsegens,
                                sign_attr=sign_attr)

        if self._elf_parsegen.ehdr.e_ident_class == ELFCLASS64 or self.secboot_version in [SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0]:
            self._mbn_parsegen.invalidate_pointers = True

        # Extract the hash algorithm that was used to generate the extracted hash table
        self._extract_hash_segment_algorithm(extracted_hash_table_size)

        # Set the elf parsegen delegate
        delegate = SecParseGenElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
        self._elf_parsegen.delegate = delegate

        # Check if the file is encrypted
        if self.is_encrypted():
            if encrypted_key_provider_id_supported(general_properties.UIE_key):
                raise RuntimeError("Cannot decrypt an encrypted image when encrypted key provider is configured as UIE server")

            self._decrypt_data()
            self.store_debug_data(FILE_DECRYPTED_IN, self._elf_parsegen.get_data())

            # Get the original data
            self._elf_parsegen = elf.ParseGenElf(self._elf_parsegen.get_data(),
                                                 self.debug_dir,
                                                 self.debug_prefix,
                                                 self.debug_suffix)

            # Ensure that hashes match. If they don't we can assume that decryption failed
            if self._mbn_parsegen.code[SecParseGenElf.HASH_ALGO_TO_SIZE_MAP[self._mbn_parsegen.extracted_segment_hash_algorithm]*2:] != self.get_hash_table():
                raise RuntimeError("Decryption of image failed. This can be caused by:"
                                   "\n\t1) use of an invalid L3 key"
                                   "\n\t2) use of an incorrect segment hash algorithm")

            # Set the elf parsegen delegate
            delegate = SecParseGenElfDelegate(self._elf_parsegen, self.validate_ph_addrs, self.validate_vir_addrs)
            self._elf_parsegen.delegate = delegate

        # Perform license manager segment operations
        self.add_or_update_license_manager_segment()

        # Perform multi image segment operations
        self.add_or_update_multi_image_segment()

    def __repr__(self):
        def segment_properties(seg, segment_name):
            if seg:
                return segment_name + ' Segment Properties: ' + '\n' + repr(seg) + '\n'
            else:
                return ''
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'ELF Properties: ' + '\n' + repr(self._elf_parsegen) + '\n'
                'Hash Segment Properties: ' + '\n' + repr(self._mbn_parsegen) + '\n' +
                segment_properties(self._multi_image_segment, multi_image_string()) +
                segment_properties(self._license_manager_segment, 'License Manager') +
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

    @classmethod
    def supports_multi_image(cls):
        return True

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt
        integrity_check = True if (integrity_check or sign or encrypt) else False

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt)

        hash_segment = None

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
                    raise RuntimeError('Estimated hash table size was wrong. Estimate - ' +
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

    # --------------------------------------------------------------------------
    # Mult Image Helper methods
    # --------------------------------------------------------------------------
    def add_or_update_multi_image_segment(self):
        if self.has_multi_image_segment:
            multi_image_segment_data = self._extract_existing_multi_image_segment()
            self._multi_image_segment = MultiImageSegment(self.segment_hash_algorithm, data=multi_image_segment_data, images=self.dependent_parsegens)
            self._add_multi_image_segment()

    def _extract_existing_multi_image_segment(self):
        multi_image_segment, multi_image_entry = None, None
        # Find the phdr entry for the multi image segment
        for phdr in self._elf_parsegen.phdrs:
            if phdr.p_type == PT_LOAD:
                if multi_image_entry is None:
                    multi_image_entry = phdr
                else:
                    raise RuntimeError(multi_image_string() + " image contains multiple LOAD segments.\n       "
                                       "Only 1 LOAD segment is allowed to exist.")
        # Extract segment data and remove segment
        if multi_image_entry:
            multi_image_segment = self._elf_parsegen.get_segment_data(multi_image_entry)
            self._elf_parsegen.remove_segment(multi_image_entry)
        return multi_image_segment

    def _add_multi_image_segment(self):
        # Get the size and physical address of the multi image segment
        multi_image_segment_size = self._multi_image_segment.get_size()
        multi_image_segment_addr = int(self.multi_image_segment_phys_addr, 16)
        multi_image_phdr_entry = self._get_multi_image_phdr_entry(multi_image_segment_size,
                                                                  multi_image_segment_addr)
        # Add segment
        self._elf_parsegen.add_segment(multi_image_phdr_entry, self._multi_image_segment.pack())

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
        multi_image_phdr.p_memsz = int(math.ceil(float(size) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        multi_image_phdr.p_filesz = size
        multi_image_phdr.p_vaddr = 0
        multi_image_phdr.p_paddr = addr
        return multi_image_phdr

    # --------------------------------------------------------------------------
    # License Manager Segment Helper methods
    # --------------------------------------------------------------------------
    def add_or_update_license_manager_segment(self):
        if self.has_license_manager_segment:
            error = list()
            if self.client_id is None:
                error.append("client_id missing from config. Add client_id value to config.")
            if self.lib_id is None:
                error.append("lib_id missing from config. Add lib_id value to config.")
            if error:
                raise RuntimeError("\n       ".join(error))
            license_manager_segment_data, paddr = self._extract_existing_license_manager_segment()
            signer = self.signer if self.signing else None
            self._license_manager_segment = LicenseManagerSegment(self.client_id, self.lib_id, signer=signer, data=license_manager_segment_data, validating=self.validating)
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
        license_manager_phdr_entry = self._get_license_manager_phdr_entry(self._license_manager_segment.get_size(), addr)
        # Add the license manager segment
        self._elf_parsegen.add_segment(license_manager_phdr_entry, self._license_manager_segment.pack(), index=len(self._elf_parsegen.phdrs), segment_to_put=True)

    def _get_license_manager_address(self, addr):
        if addr is not None:
            return addr
        else:
            return self._compute_hash_address(self._elf_parsegen)

    def _get_license_manager_offset(self):
        last_phdr_entry = get_last_seg(self._elf_parsegen.phdrs)
        return int(math.ceil(float(last_phdr_entry.p_offset + last_phdr_entry.p_filesz + 1) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)

    def _get_license_manager_phdr_entry(self, size, addr):
        license_manager_phdr = self._elf_parsegen.get_new_phdr_entry()()
        license_manager_phdr.p_type = segment.type.PT_NOTE
        license_manager_phdr.p_align = ELF_BLOCK_ALIGN
        license_manager_phdr.p_offset = self._get_license_manager_offset()
        license_manager_phdr.p_memsz = int(math.ceil(float(size) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        license_manager_phdr.p_filesz = size
        license_manager_phdr.p_paddr = self._get_license_manager_address(addr)
        return license_manager_phdr

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

    def get_hash_table(self, get_hash_count=False):
        # Check the number of headers that can be hashed
        phdr_count = len(self._elf_parsegen.phdrs)
        if phdr_count > self.max_elf_segments:
            raise RuntimeError('Number of prog header entries in ELF "' + str(phdr_count) + '" '
                               'is more than the maximum supported "' + str(self.max_elf_segments) + '"')

        hash_table = ''
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
                if phdr.p_type not in NON_HASHABLE_SEGMENTS:
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

        # Insert testsig at this point if its needed
        if not get_hash_count and self.serial_num is not None:
            hash_table = self._insert_serial_in_ht(hash_table, self._serial_num)

        if get_hash_count:
            return hash_count
        else:
            return hash_table

    def _add_phdr_and_hash_segs(self, integrity_check, sign, encrypt):
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
        hash_segment_size = self._compute_hash_segment_size(integrity_check, sign, encrypt)
        hash_segment_addr = self._compute_hash_address(self._elf_parsegen)

        align_func = lambda x: int(math.ceil(float(x + 1) / ELF_BLOCK_ALIGN) * ELF_BLOCK_ALIGN)
        end_byte_aligned = re.search('END_([0-9]+)B_ALIGNED', self.hash_seg_placement) if self.hash_seg_placement else None
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
        elif end_byte_aligned is not None:
            # Get the byte alignment
            alignment = int(end_byte_aligned.group(1))

            # Get size of the hash seg minus sign, certs, etc
            size_of_hash_seg = self._compute_hash_segment_size(False, False, False)

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
        if self.oem_signing_enabled and self.qti_signing_enabled:
            signatures += 1

        encrypt_params = len(self.encdec.get_encryption_parameters_blob()) if (sign and signatures == 2 and self.oem_encryption_enabled) or encrypt else 0

        cert_chain_size = 0
        if sign:
            if signatures == 1:
                cert_chain_size = self.get_cert_chain_size(self.authority)
            else:
                cert_chain_size = self.get_cert_chain_size(self.authority) + self.get_cert_chain_size(self.get_other_authority(self.authority))

        sig_size = MAX_SIG_SIZE * (signatures - 1) if sign else 0
        if sign:
            if ((self.authority != AUTHORITY_QTI and self._data_signature_qti) or
                    (self._data_signature_qti and self._data_signature)):
                sig_size += len(self._data_signature_qti)
            else:
                sig_size += self.sig_size

        # Sum
        return (self._mbn_parsegen.get_header(self.authority,
                                              self.imageinfo,
                                              self.validating,
                                              self.signing,
                                              self.add_signing_attr_to_hash_seg)
                .get_size() + hash_table_len + sig_size + cert_chain_size +
                encrypt_params)

    def _get_hash(self, data):
        from sectools.features.isc.signer.utils.hmac import HMAC
        h = HMAC.HASH_ALGO_MAP[self.segment_hash_algorithm]()
        h.update(data)
        return h.digest()

    def _extract_hash_segment_algorithm(self, hash_table_size=0):
        if hash_table_size > 0:
            hash_count = self.get_hash_table(get_hash_count=True)
            hash_size = hash_table_size / (hash_count + 2)  # add 2: +1 for Hash Table Entry 0 (ELF + Program Header). +1 for Hash Table Entry 1 (Dummy Entry)
            logger.debug("Number of hash entries: " + str(hash_count))
            logger.debug("Hash table size: " + str(hash_table_size))
            logger.debug("Hash size: " + str(hash_size))

            if hash_size in SecParseGenElf.HASH_SIZE_TO_ALGO_MAP.keys():
                self._mbn_parsegen.extracted_segment_hash_algorithm = SecParseGenElf.HASH_SIZE_TO_ALGO_MAP[hash_size]
            else:
                self._mbn_parsegen.extracted_segment_hash_algorithm = ParseGenMbn.UNKNOWN_ALGORITHM

            logger.debug("Determined hash table algorithm: " + self._mbn_parsegen.extracted_segment_hash_algorithm)

            # Determine if parsegen was created during validation (-a) and print error if hash algorithm mismatch is present
            extracted_segment_hash_algorithm = self._mbn_parsegen.extracted_segment_hash_algorithm
            segment_hash_algorithm = self.segment_hash_algorithm if self.segment_hash_algorithm is not None else "sha256"
            if extracted_segment_hash_algorithm != segment_hash_algorithm and (self.validating or self.is_encrypted()):
                from sectools.common.utils.c_misc import create_mismatch_table
                errstr = list()
                mismatches = list()
                mismatches.append(("Hash Segment Algorithm", extracted_segment_hash_algorithm, segment_hash_algorithm))
                create_mismatch_table(mismatches, errstr, data_type_to_compare="Attribute", image_region="Hash Segment")
                logger.warning('Following validations failed for the image:\n       ' +
                               '\n       '.join([(str(i + 1) + '. ' + e) for i, e in enumerate(errstr)]))

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
        image_dest_ptr = self._compute_hash_address(self._elf_parsegen) + mbn.header_size
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
        elif mbn_version in [MBN_HDR_VERSION_5, MBN_HDR_VERSION_6]:
            header.version = mbn_version
            header.sig_size_qti = 0
            header.cert_chain_size_qti = 0
            if mbn_version == MBN_HDR_VERSION_6:
                header.metadata_size_qti = 0
                header.metadata_size = 0
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
                if phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_SWAPPED:
                    max_phdr = phdr
        if max_phdr is None:
            return ELF_BLOCK_ALIGN
        else:
            return ((max_phdr.p_paddr + max_phdr.p_memsz - 1) & (~(ELF_BLOCK_ALIGN - 1))) + ELF_BLOCK_ALIGN

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
