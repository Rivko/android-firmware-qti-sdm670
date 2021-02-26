# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

# used to get MBN header object for all MBN header versions
get_header = {}

"""
Map of: (MbnHeaderSize, MbnHeaderVersion) -> Mbn Header Class

Dictionary containing a mapping of the class to use for parsing the MBN header
based on the header type specified
"""
MBN_HDRS = {}
