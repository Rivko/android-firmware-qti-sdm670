# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc
import re
from struct import Struct
from struct import calcsize

from six import with_metaclass, ensure_binary
from sectools.common.utils.c_data import remove_duplicates
from sectools.common.utils.c_misc import obj_repr


class StructBase(with_metaclass(abc.ABCMeta, Struct)):
    """Wrapper over the python struct object to allow derived classes to
    automatically update internal attributes when pack and unpack are called.

    https://docs.python.org/2/library/struct.html
    """

    def __init__(self, data=None, offset=0):
        super(StructBase, self).__init__(self.get_format())
        self.data_is_none = data is None
        self.unpack(data, offset)

    def pack(self):
        return super(StructBase, self).pack(*self._pack_data_list())

    def unpack(self, data, offset=0):
        data = ('\0' * self.size) if self.data_is_none else data

        offset = 0 if self.data_is_none else offset
        self._unpack_data_list(super(StructBase, self).unpack_from(ensure_binary(data), offset))
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
        return Struct(cls.get_format()).size

    @classmethod
    def _range_check(cls, value, bcnt, tag):
        max_val = int('0x' + 'FF' * bcnt, 16)
        if value < 0 or value > max_val:
            raise RuntimeError(tag + ' value: ' + value + ' is out of range: 0x0-' + hex(max_val))

    def repr_ignore_attr(self):
        return ['data_is_none']

    def __repr__(self):
        return obj_repr(self, self.repr_ignore_attr())


class StructDynamic(Struct):
    """
    Wrapper over the python struct object to allow creation and access of objects with runtime-determined members
    """

    def __init__(self, binary_data, binary_data_format):
        self._data = binary_data
        self._format = binary_data_format[0]
        self._fields = list(binary_data_format[1])
        if len(self.fields) != len(remove_duplicates(self.fields)):
            raise RuntimeError("Fields must not contain duplicates")
        if len(self.fields) != len(re.sub("[@=<>!0-9]", "", self.format)):
            raise RuntimeError("Number of fields must match the number of structures defined by format string")
        if len(self.binary_data) < calcsize(self.format):
            raise RuntimeError("Length of data must be greater than or equal to length specified by format string")
        super(StructDynamic, self).__init__(self.format)

    @property
    def binary_data(self):
        """Data which is parsed"""
        return self._data

    @property
    def format(self):
        """
        A format string of the data which is parsed.
        The format characters match those specified by Python struct library.
        """
        return self._format

    @property
    def fields(self):
        """
        A list of strings identifying the various fields by name in the order they appear in the data which is parsed
        """
        return self._fields

    def __getattr__(self, field):
        """
        Any field present in the "fields" list can be accessed as a traditional member. For example, if "my_field" is
        present in self.fields, then my_field can be access via self.my_field
        """
        if field in self._fields:
            return super(StructDynamic, self).unpack_from(self._data)[self._fields.index(field)]
        else:
            raise RuntimeError(field + " is not a field within data")
