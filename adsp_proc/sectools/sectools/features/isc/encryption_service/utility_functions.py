# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import hashlib

def compute_segment_iv_uie(segment_num, image_iv):
    seg_1 = segment_num & 0x000000ff
    seg_2 = (segment_num >> 8) & 0x000000ff
    seg_3 = (segment_num >> 16) & 0x000000ff
    seg_4 = (segment_num >> 24) & 0x000000ff
    computed_segment_iv = hashlib.sha256(image_iv +
                                         chr(seg_4) +
                                         chr(seg_3) +
                                         chr(seg_2) +
                                         chr(seg_1)).digest()[16:]
    return computed_segment_iv


def compute_segment_iv_ssd(segment_num, image_iv):
    seg_1 = segment_num & 0x000000ff
    seg_2 = (segment_num >> 8) & 0x000000ff
    seg_3 = (segment_num >> 16) & 0x000000ff
    seg_4 = (segment_num >> 24) & 0x000000ff
    computed_segment_iv = hashlib.sha256(image_iv +
                                         chr(seg_4) +
                                         chr(seg_3) +
                                         chr(seg_2) +
                                         chr(seg_1)).digest()[:16]
    return computed_segment_iv


def compute_segment_iv_uie_1_0(segment_num, image_iv):
    seg_1 = segment_num & 0x000000ff
    seg_2 = (segment_num >> 8) & 0x000000ff
    seg_3 = (segment_num >> 16) & 0x000000ff
    seg_4 = (segment_num >> 24) & 0x000000ff
    computed_segment_iv = hashlib.sha256(image_iv +
                                         chr(seg_4) +
                                         chr(seg_3) +
                                         chr(seg_2) +
                                         chr(seg_1)).digest()[:16]
    return computed_segment_iv
