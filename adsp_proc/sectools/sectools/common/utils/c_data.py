# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created on Nov 19, 2014

@author: aehrlich

This module provides various helper functions which don't have a parent category
"""

import binascii
import os
import random
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


def get_random_sublist(source_list, max_size=None):
    max_size = max_size if max_size is not None else len(source_list)
    left_index = random.randint(0, max_size-1)
    right_index = random.randint(left_index+1, max_size)
    return source_list[left_index:right_index]


def remove_list_items_and_duplicates(input_list, items_to_remove):
    """
    If duplicates exist in input_list they will be removed.
    If maintaining list duplicates is required, use remove_list_items_and_retain_duplicates instead.
    """
    if input_list is None:
        return []
    elif items_to_remove is None or not items_to_remove:
        return input_list
    else:
        return list(set(input_list) - set(items_to_remove))


def remove_list_items_and_retain_duplicates(input_list, items_to_remove):
    if input_list is None:
        return []
    elif items_to_remove is None or not items_to_remove:
        return input_list
    else:
        return [x for x in input_list if x not in items_to_remove]


def gen_random_data(n=16):
    return binascii.hexlify(os.urandom(n))


def get_bits_from_bytes(int_value, index, size):
    """
    :param int_value: The integer from which to extract bits.
    :param index: Start index of value to extract from data. Least significant bit of data is index zero.
    :param size: Size in bits of the value to extract from data.
    :return: The extracted bits.
    """
    if index < 0 or size < 0:
        raise RuntimeError("Index and size provided to get_bits_from_bytes must be positive. Index: {0}, size: {1}".format(index, size))
    num_bits = 32
    if index + size > num_bits:
        raise RuntimeError("Invalid values provided to get_bits_from_bytes. Size + index must be less than or equal to size in bit of data.\nSize of data: {0} bits, size of field: {1} bits, index: {2}".format(num_bits, size, index))
    int_value >>= index
    mask = (1 << size) - 1
    return int_value & mask


def convert_int_to_bits(int_value, index, size):
    """
    :param int_value: The integer value to convert to shifted bit representation.
    :param index: Start index of value in desired 4-byte output. Least significant bit of output is index zero.
    :param size: Size in bits that integer value should take up in returned data.
    :return: A 4-byte integer containing the int_value at the specified index.
    """
    if index < 0 or size < 1:
        raise RuntimeError("Index provided to convert_int_to_bits must be a positive value. Index: {0}".format(index))
    if size < 1:
        raise RuntimeError("Size provided to convert_int_to_bits must be non-zero. Size: {0}".format(size))
    if int_value > 2**size - 1:
        raise RuntimeError("Value provided to convert_int_to_bits cannot be represented in desired number of bits. Value: {0}, number of bits: {1}".format(int_value, size))
    num_bits = 32
    if index + size > num_bits:
        raise RuntimeError("Invalid values provided to convert_int_to_bits. Size + index must be less than or equal to 32.\nSize: {0} bits, index: {1}".format(size, index))
    return int_value << index
