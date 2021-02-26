# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Mar 31, 2014

@author: hraghav
'''

#------------------------------------------------------------------------------
# Magic numbers used in pmbl format
#------------------------------------------------------------------------------
FLASH_CODE_WORD                     = 0x844BDCD1
MAGIC_NUM                           = 0x73D71034
SBL_VIRTUAL_BLOCK_MAGIC_NUM         = 0xD48B54C6
VIRTUAL_BLOCK_SIZE                  = 0x20000

#------------------------------------------------------------------------------
# File names for debugging
#------------------------------------------------------------------------------
FILE_DATA_IN                        = 'pmbl_in_0_data'

FILE_MAGIC_REMOVED                  = 'pmbl_in_1_magic_removed'

FILE_PRMBL_REMOVED                  = 'pmbl_in_2_prmbl_removed'
FILE_PRMBL_IN                       = 'pmbl_in_2_prmbl'
FILE_PRMBL_OUT                      = 'pmbl_out_2_prmbl'

FILE_CODE_IN                        = 'pmbl_in_3_code'
