# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Sep 8, 2017

@author: coryf
'''

from sectools.common.parsegen.elf.segment.flags import PF_PERM_VALUE

PERM_VALUE = PF_PERM_VALUE

SHF_WRITE     = 0x1
SHF_ALLOC     = 0x2
SHF_EXECINSTR = 0x4

PERM_DESCRIPTION = \
{
    SHF_WRITE: 'W',
    SHF_ALLOC: 'A',
    SHF_EXECINSTR: 'X',
    SHF_WRITE | SHF_ALLOC: 'WA',
    SHF_WRITE | SHF_EXECINSTR: 'WX',
    SHF_ALLOC | SHF_EXECINSTR: 'AX',
    SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR: 'WAX'
}

