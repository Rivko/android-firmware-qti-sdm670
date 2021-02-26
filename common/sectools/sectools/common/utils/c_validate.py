# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Oct 23, 2014

@author: aehrlich
'''
def range_check(name, value, min, max):
    if value > max:
        raise RuntimeError("%r cannot be larger than %d" % (name, max))
    elif value < min:
        raise RuntimeError("%r cannot be less than %d" % (name, min))