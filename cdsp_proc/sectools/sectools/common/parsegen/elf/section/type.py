# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Aug 30, 2017

@author: coryf
'''
#------------------------------------------------------------------------------
# ELF Section Header - Type (sh_type)
#------------------------------------------------------------------------------
SHT_NULL           = 0
SHT_PROGBITS       = 1
SHT_SYMTAB         = 2
SHT_STRTAB         = 3
SHT_RELA           = 4
SHT_HASH           = 5
SHT_DYNAMIC        = 6
SHT_NOTE           = 7
SHT_NOBITS         = 8
SHT_REL            = 9
SHT_SHLIB          = 10
SHT_DYNSYM         = 11
SHT_INIT_ARRAY     = 14
SHT_FINI_ARRAY     = 15
SHT_LOPROC         = 1879048192
SHT_HIPROC         = 2147483647
SHT_LOUSER         = 2147483648
SHT_HIUSER         = 4294967295
SHT_GNU_HASH       = 1879048182
SHT_VERSYM         = 1879048191
SHT_VERDEF         = 1879048189
SHT_VERNEED        = 1879048190
SHT_LOOS           = 1610612737
SHT_ARM_EXIDX      = 1879048193
SHT_ARM_ATTRIBUTES = 1879048195


ST_STRING       = 'Type'
ST_DESCRIPTION = \
{
    SHT_NULL           : 'NULL',
    SHT_PROGBITS       : 'PROGBITS',
    SHT_SYMTAB         : 'SHT_SYMTAB',
    SHT_STRTAB         : 'STRTAB',
    SHT_RELA           : 'RELA',
    SHT_HASH           : 'HASH',
    SHT_DYNAMIC        : 'DYNAMIC',
    SHT_NOTE           : 'NOTE',
    SHT_NOBITS         : 'NOBITS',
    SHT_REL            : 'REL',
    SHT_SHLIB          : 'SHLIB',
    SHT_DYNSYM         : 'DYNSYM',
    SHT_INIT_ARRAY     : 'INIT_ARRAY',
    SHT_FINI_ARRAY     : 'FINI_ARRAY',
    SHT_LOPROC         : 'LOPROC',
    SHT_HIPROC         : 'HIPROC',
    SHT_LOUSER         : 'LOUSER',
    SHT_HIUSER         : 'HIUSER',
    SHT_GNU_HASH       : 'GNU_HASH',
    SHT_VERSYM         : 'VERSYM',
    SHT_VERDEF         : 'VERDEF',
    SHT_VERNEED        : 'VERNEED',
    SHT_LOOS           : 'LOOS+1',
    SHT_ARM_EXIDX      : 'ARM_EXIDX',
    SHT_ARM_ATTRIBUTES : 'ARM_ATTRIBUTES'


}
