#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

'''
Created on Oct 23, 2014

@author: aehrlich
'''
def range_check(name, value, min, max):
    if value > max:
        raise RuntimeError("%r cannot be larger than %d" % (name, max))
    elif value < min:
        raise RuntimeError("%r cannot be less than %d" % (name, min))
