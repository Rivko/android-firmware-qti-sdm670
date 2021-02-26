# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Apr 28, 2014

@author: hraghav
'''
from sectools.common.parsegen.elf.header.format import Elf32_Ehdr, Elf64_Ehdr


def zero_out_sections(elf_header):
    assert isinstance(elf_header, (Elf32_Ehdr, Elf64_Ehdr))
    elf_header.e_shoff = 0
    elf_header.e_shnum = 0
    elf_header.e_shstrndx = 0
