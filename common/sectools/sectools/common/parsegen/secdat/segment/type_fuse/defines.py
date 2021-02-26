# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

# Segment Version Values
FUSEVER1                    = 1

FUSEVER_DESCRIPTION = \
{
    FUSEVER1      : 'SegmentFuseVersion1',
}

#------------------------------------------------------------------------------
# Fuse segment constants from tzbsp_fuseprov.h
#------------------------------------------------------------------------------

FUSEPROV_REGION_ETYPE_DICT = \
{
    0x00000000 : 'OEM_SEC_BOOT',
    0x00000001 : 'OEM_PK_HASH',
    0x00000002 : 'SEC_HW_KEY',
    0x00000003 : 'OEM_CONFIG',
    0x00000004 : 'READ_WRITE_PERM',
    0x00000005 : 'SPARE_REG19',
    0x00000006 : 'GENERAL',
    0x00000007 : 'FEC_EN',
    0x00000008 : 'ANTI_ROLLBACK_2',
    0x00000009 : 'ANTI_ROLLBACK_3',
    0x0000000A : 'PK_HASH1',
    0x0000000B : 'IMAGE_ENCR_KEY1',
    0x0000000C : 'OEM_SECURE',
    0x0000000D : 'MRC_2_0',
}

FUSEPROV_OPERATION_ETYPE_DICT = \
{
    0x00000000 : 'BLOW',
    0x00000001 : 'VERIFYMASK0',
}

#------------------------------------------------------------------------------
# File names for debugging
#------------------------------------------------------------------------------
FILE_DATA_IN                        = 'seg_fuse_in_data'

FILE_HDR_IN                         = 'seg_fuse_in_hdr'
FILE_HDR_IN_REPR                    = 'seg_fuse_in_hdr_repr'
FILE_HDR_IN_REPR_SUFFIX             = '.txt'

FILE_FUSE_ENTRIES_IN                = 'seg_fuse_in_fuses'
FILE_FUSE_ENTRIES_IN_REPR           = 'seg_fuse_in_fuse_repr'
FILE_FUSE_ENTRIES_IN_REPR_SUFFIX    = '.txt'
