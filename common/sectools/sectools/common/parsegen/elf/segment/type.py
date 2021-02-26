# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Apr 2, 2014

@author: hraghav
'''
#------------------------------------------------------------------------------
# ELF Program Header - Type (p_type)
#------------------------------------------------------------------------------
PT_NULL         = 0
PT_LOAD         = 1
PT_DYNAMIC      = 2
PT_INTERP       = 3
PT_NOTE         = 4
PT_SHLIB        = 5
PT_PHDR         = 6
PT_TLS          = 7
PT_GNU_EH_FRAME = 1685382480
PT_GNU_STACK    = 1685382481
PT_EXIDX        = 1879048193

PT_STRING       = 'Type'
PT_DESCRIPTION = \
{
    PT_NULL         : 'NULL',
    PT_LOAD         : 'LOAD',
    PT_DYNAMIC      : 'DYNAMIC',
    PT_INTERP       : 'INTERP',
    PT_NOTE         : 'NOTE',
    PT_SHLIB        : 'SHLIB',
    PT_PHDR         : 'PHDR',
    PT_TLS          : 'TLS',
    PT_GNU_EH_FRAME : 'GNU_EH_FRAME',
    PT_GNU_STACK    : 'GNU_STACK',
    PT_EXIDX        : 'EXIDX'
}

