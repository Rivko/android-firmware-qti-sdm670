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


import abc
import struct

from sectools.common.utils.c_misc import obj_repr


class StructBase(struct.Struct, object):
    """Wrapper over the python struct object to allow derived classes to
    automatically update internal attributes when pack and unpack are called.

    https://docs.python.org/2/library/struct.html
    """

    __metaclass__ = abc.ABCMeta

    def __init__(self, data=None, offset=0):
        struct.Struct.__init__(self, self.get_format())
        self.data_is_none = data is None
        self.unpack(data, offset)

    def pack(self):
        return struct.Struct.pack(self, *self._pack_data_list())

    def unpack(self, data, offset=0):
        data = ('\0' * self.size) if self.data_is_none else data
        offset = 0 if self.data_is_none else offset
        self._unpack_data_list(struct.Struct.unpack_from(self, data, offset))
        if not self.data_is_none:
            self.validate()

    def validate(self):
        pass

    @abc.abstractmethod
    def _pack_data_list(self):
        return []

    @abc.abstractmethod
    def _unpack_data_list(self, unpacked):
        pass

    @classmethod
    def get_format(cls):
        return ''

    @classmethod
    def get_size(cls):
        return struct.Struct(cls.get_format()).size

    @classmethod
    def _range_check(cls, value, bcnt, tag):
        max_val = int('0x' + 'FF' * bcnt, 16)
        if value < 0 or value > max_val:
            raise RuntimeError(tag + ' value: ' + value + ' is out of range: 0x0-' + hex(max_val))

    def repr_ignore_attr(self):
        return ['data_is_none']

    def __repr__(self):
        return obj_repr(self, self.repr_ignore_attr())
