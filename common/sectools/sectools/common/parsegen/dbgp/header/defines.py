# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

#------------------------------------------------------------------------------
# Debug Policy Header
#------------------------------------------------------------------------------
# magic values
DBGPMAG                  = 'DBGP'

DBGPMAG_STRING           = 'Magic'
DBGPMAG_DESCRIPTION      = \
{
            DBGPMAG      : DBGPMAG,
}

# version values
DBGPVERONE               = 1
DBGPVERTWO               = 2
DBGPVERTHREE             = 3

DBGPVER_STRING           = 'Version'
DBGPVER_DESCRIPTION      = \
{
            DBGPVERONE      : '1',
            DBGPVERTWO      : '2',
            DBGPVERTHREE    : '3',
}

# constants
IMAGEBIT_MAP_MAX_VALUE         = 0xFFFFFFFF
SERNUM_START_END_MATCH  = 1

#------------------------------------------------------------------------------
# File names for debugging
#------------------------------------------------------------------------------
FILE_DATA_IN                    = 'dbgp_in_data'
FILE_ROOTCERTHASH_IN            = 'dbgp_in_rootcerthashdata'