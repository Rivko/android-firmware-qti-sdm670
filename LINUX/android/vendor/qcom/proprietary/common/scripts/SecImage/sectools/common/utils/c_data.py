# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created on Nov 19, 2014

@author: aehrlich

This module provides various helper functions which don't have a parent category
"""

import re


def nice_binary_to_string(bin_data, sep=':', length=30):
    return_data = []
    for i in xrange(0, len(bin_data), length):
        return_data.append(sep.join(hex(ord(b))[2:].rjust(2, '0') for b in bin_data[i:i+length]))

    return '\n'.join(return_data)

def nice_string_to_binary(data, sep=':'):
    return ''.join([chr(int(c, 16)) for c in sep.join(data.strip().split('\n')).split(sep)])

def nice_string_to_list(data, n=2):
    return [data[i:i+n] for i in xrange(0, len(data), n)]

def nice_string_to_multiline(data, length=30):
    return '\n'.join([data[i:i+length] for i in xrange(0, len(data), length)])

def nice_c_hex(data, length=9):
    retdata =  nice_binary_to_string(data, ':', length)
    newdata = []
    for l in retdata.split('\n'):
        l_n = ['0x' + c for c in l.split(':')]
        newdata.append(', '.join(l_n))
    return '   ' + ',\n   '.join(newdata) + ','

def nice_hex_reversed(data):
    data = reversed(nice_binary_to_string(data, ':', len(data)).split(':'))
    return nice_string_to_binary(':'.join(data))

def filter_data(data_list, inclusions, exclusions, cb=None):
    def filter_func(data):
        if cb is not None:
            data = cb(data)
        if inclusions:
            for inc in inclusions:
                if re.match(inc, data):
                    break
            else:
                return False
        if exclusions:
            for exc in exclusions:
                if re.match(exc, data):
                    return False
        return True

    return filter(filter_func, data_list)

def find_regex(regex, search_list, find_first=False):
    retval = []
    for tag in search_list:
        if re.search(regex, tag) is not None:
            retval.append(tag)
            if find_first:
                break
    return retval
	
def format_hex(value, name):
    # converts string values to uppercase string hex values without a leading "0x"
    if not isinstance(value, str):
        raise RuntimeError(name + " is not of type str")
    try:
        int(value, 16)
    except ValueError:
        raise RuntimeError(name + " with value {0} is not a valid hex value".format(value))
    if value[:2] == '0x':
        return value[2:].upper()
    else:
        return value.upper()

def get_duplicates(input_list):
    duplicates = []
    if input_list is None:
        return duplicates
    for item in input_list:
        if input_list.count(item) > 1 and duplicates.count(item) == 0:
            duplicates.append(item)
    return duplicates

def remove_duplicates(input_list):
    if input_list is None:
        return []
    else:
        return list(set(input_list))
