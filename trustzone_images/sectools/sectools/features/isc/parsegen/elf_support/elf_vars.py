# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.c_base import ValPolicy
from sectools.common.parsegen.elf.segment.type import PT_NOTE
from sectools.common.parsegen.elf.segment.type import PT_PHDR

# Constants
OFFSET_12K                  = 0x3000
ELF_BLOCK_ALIGN             = 0x1000        # Block alignment size
MAX_PHDR_COUNT              = 100           # Maximum allowable program headers

# Debug files
FILE_PROG_SEG_IN            = 'sec_elf_in_prog_segment'
FILE_HASH_SEG_IN            = 'sec_elf_in_hash_segment'
FILE_DECRYPTED_IN           = 'sec_elf_in_decrypted'
FILE_PROG_HASH_REMOVED_IN   = 'sec_elf_in_prog_hash_removed'

NON_ENCAP_SEGMENTS                   = "NON_ENCAP_SEGMENTS"
NON_HASHABLE_SEGMENTS                = "NON_HASHABLE_SEGMENTS"
POLICY_NON_LOAD_OUTSIDE_LOAD         = "POLICY_NON_LOAD_OUTSIDE_LOAD"
POLICY_OVERLAPPING_SEGMENTS_VIRTUAL  = "POLICY_OVERLAPPING_SEGMENTS_VIRTUAL"
POLICY_OVERLAPPING_SEGMENTS_PHYSICAL = "POLICY_OVERLAPPING_SEGMENTS_PHYSICAL"

# Dictionary for use in addr validation
PHY_ADDR_VALIDATION_DICT = {
    'isPhy': True,
    'policy': lambda: elf_vars[POLICY_OVERLAPPING_SEGMENTS_PHYSICAL],
    'tString': 'Phys',
    'eString': 'Following segments physical address overlap: ' + '\n'}

VIR_ADDR_VALIDATION_DICT = {
    'isPhy': False,
    'policy': lambda: elf_vars[POLICY_OVERLAPPING_SEGMENTS_VIRTUAL],
    'tString': 'Vir',
    'eString': 'Following segments virtual address overlap: ' + '\n'}

elf_vars = {
    # Selected policy for overlapping segments
    POLICY_OVERLAPPING_SEGMENTS_PHYSICAL: ValPolicy(ValPolicy.WARN),
    POLICY_OVERLAPPING_SEGMENTS_VIRTUAL: ValPolicy(ValPolicy.WARN),
    POLICY_NON_LOAD_OUTSIDE_LOAD: ValPolicy(ValPolicy.ERROR),
    # Segments allowed to exist outside the LOAD segments
    NON_ENCAP_SEGMENTS: [PT_NOTE],
    # Segments to ignore for hashing
    NON_HASHABLE_SEGMENTS: [PT_PHDR],
}


# Test API to modify error policy
def set_overlapping_segments_policies(phyPolicy, virPolicy):
    elf_vars[POLICY_OVERLAPPING_SEGMENTS_PHYSICAL] = phyPolicy
    elf_vars[POLICY_OVERLAPPING_SEGMENTS_VIRTUAL]  = virPolicy


# Test API to save error policy
def get_overlapping_segments_policies():
    return (elf_vars[POLICY_OVERLAPPING_SEGMENTS_PHYSICAL],
            elf_vars[POLICY_OVERLAPPING_SEGMENTS_VIRTUAL])


def set_non_encap_segments(segments):
    elf_vars[NON_ENCAP_SEGMENTS] = segments


def set_non_hashable_segments(segments):
    elf_vars[NON_HASHABLE_SEGMENTS] = segments
