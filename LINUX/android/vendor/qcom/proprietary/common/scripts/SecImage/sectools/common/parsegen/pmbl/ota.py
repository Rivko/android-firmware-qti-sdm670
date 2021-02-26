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

from sectools.common.parsegen import PAD_BYTE_0


def remove_pad(data):
    raise RuntimeError('Removing of pad logic is not known')

def insert_pad(data, page_size, num_pages, min_size):
    pad_len = page_size * num_pages
    if pad_len > 0:
        import math
        pad_len = int((math.ceil(len(data) / float(pad_len)))) * pad_len
    data = data.ljust(max(min_size, pad_len), PAD_BYTE_0)
    return data
