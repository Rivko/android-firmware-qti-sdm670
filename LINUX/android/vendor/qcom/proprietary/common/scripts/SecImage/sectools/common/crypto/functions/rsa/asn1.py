# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Nov 25, 2014

@author: hraghav
'''

import json
import math


class AsnObject(object):

    # Offsets
    OFFSET_TYPE = 0
    OFFSET_SHORT_LEN = 1
    OFFSET_LONG_LEN = 2

    # Masks
    MASK_LONG_LEN = 0x80

    # Types
    TYPE_EOC = 0x00
    TYPE_INT = 0x02
    TYPE_BIT = 0x03
    TYPE_NULL = 0x05
    TYPE_OID = 0x06
    TYPE_SEQ = 0x30
    TYPES_PRIMITIVE = [TYPE_INT, TYPE_NULL, TYPE_OID]

    def __init__(self):
        self.type = -1
        self.short_length = -1
        self.long_length = None
        self.value = ''

    def set_data(self, asn_type, value, use_eoc=False):
        self.type = asn_type
        self.value = value
        length = len(value)
        if asn_type in [self.TYPE_SEQ]:
            length = sum([len(v.pack())/2 for v in value])
        elif asn_type in [self.TYPE_BIT]:
            if use_eoc:
                self.value = ['00']
                length += 1
        elif asn_type not in self.TYPES_PRIMITIVE:
            raise RuntimeError('Unknown type: ' + str(asn_type))
        self.short_length, self.long_length = self.get_length(length)

    def pack(self):
        def bin_str(value):
            retval = hex(value)[2:]
            if len(retval) % 2:
                retval = '0' + retval
            return retval
        retval = bin_str(self.type) + bin_str(self.short_length)
        if self.long_length is not None:
            retval += bin_str(self.long_length)
        value = self.value
        if self.type in [self.TYPE_SEQ]:
            value = ''.join([v.pack() for v in value])
        return retval + ''.join(value)

    def apply_rules(self, asn_type, value, real_length):
        # Parse further
        if asn_type in [self.TYPE_SEQ]:
            # Recurse for more objects
            value = parse(value)
        elif asn_type in [self.TYPE_BIT]:
            # Check for EOC
            if int(value[0], 16) == self.TYPE_EOC:
                real_length = 1
                value = value[0]
        elif asn_type not in self.TYPES_PRIMITIVE:
            raise RuntimeError('Unknown type: ' + str(asn_type))
        return value, real_length

    def unpack(self, data):
        """Parses based on the ASN.1 format and returns a list of AsnObject.
        """
        # Get the type & short length
        asn_type = int(data[self.OFFSET_TYPE], 16)
        short_length = int(data[self.OFFSET_SHORT_LEN], 16)

        # Get the long length
        long_length = None
        real_length = short_length
        offset_value = self.OFFSET_LONG_LEN
        if short_length & self.MASK_LONG_LEN:
            long_length_bytes = short_length & ~self.MASK_LONG_LEN
            long_length = int(''.join(data[self.OFFSET_LONG_LEN:
                                           self.OFFSET_LONG_LEN + long_length_bytes]), 16)
            real_length = long_length
            offset_value += long_length_bytes

        # Get the value
        value = data[offset_value:
                     offset_value + real_length]

        # Parse further
        if asn_type in [self.TYPE_SEQ]:
            # Recurse for more objects
            value = parse(value)
        elif asn_type in [self.TYPE_BIT]:
            # Check for EOC
            if int(value[0], 16) == self.TYPE_EOC:
                real_length = 1
                value = value[:1]
        elif asn_type not in self.TYPES_PRIMITIVE:
            raise RuntimeError('Unknown type: ' + str(asn_type))

        # Save the values
        self.type = asn_type
        self.short_length = short_length
        self.long_length = long_length
        self.value = value

        # Check for EOF
        remaining_data = data[offset_value + real_length:]
        if len(remaining_data) and remaining_data[0] == '00':
            remaining_data = remaining_data[1:]

        # Return remaining data
        return remaining_data

    def _dict_repr_(self):
        retval = {}
        retval['type'] = hex(self.type)
        retval['short_length'] = hex(self.short_length)
        retval['long_length'] = hex(self.long_length) if self.long_length is not None else None
        retval['value'] = [v._dict_repr_() if isinstance(v, AsnObject) else str(v)
                           for v in self.value]
        return retval

    @classmethod
    def get_length(cls, length):
        short_length, long_length = length, None
        if length >= cls.MASK_LONG_LEN:
            byte = int(math.ceil(float(len(hex(length)[2:]))/2))
            short_length, long_length = cls.MASK_LONG_LEN | byte, length
        return short_length, long_length


def parse(data):
    retval = []
    while len(data):
        obj = AsnObject()
        data = obj.unpack(data)
        retval.append(obj)
    return retval

def dump(asn_objects):
    return json.dumps([a._dict_repr_() for a in asn_objects], indent=2)
