# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import struct
import binascii

import defines as df
from sectools.common.utils.c_misc import properties_repr
from sectools.common.utils.struct_base import StructBase

IMG_ID_MAX_COUNT = 32


class DbgpHdrv1(StructBase):

    INVALID_FLAG_BITS = 0xFFFFFFFFFFEC

    def unpack(self, data, offset=0):
        # Take into account the offset
        if not self.data_is_none:
            data = data[offset:]

        # Unpack the base
        fmt = self.get_format()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_base(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack serial segments
        fmt = self.get_format_serial_segment()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_serial_segment(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the reserved field
        fmt = self.get_format_reserved()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_reserved(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the flags
        fmt = self.get_format_flags()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_flags(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the image id fields
        fmt = self.get_format_imgid_info()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_imgid_info(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack number of rootcerthashes
        fmt = self.get_format_rootcerthash_count()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_count(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Use rootcerthash count to unpack the hashes
        fmt = self.get_format_rootcerthash_array(self.rootcerthash_count)
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_array(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Any top level validations
        if not self.data_is_none:
            self.validate()

    def _unpack_base(self, unpacked):
        self._magic = unpacked[0]
        self._data_size = unpacked[1]
        self._version = unpacked[2]

    def _unpack_serial_segment(self, unpacked):
        self._sernum_start = unpacked[0]
        self._sernum_end = unpacked[1]

    def _unpack_reserved(self, unpacked):
        self.reserved = unpacked[0]

    def _unpack_flags(self, unpacked):
        self._flags = unpacked[0]

    def _unpack_imgid_info(self, unpacked):
        self._imgid_count = unpacked[0]
        self._imgid_array = unpacked[1:IMG_ID_MAX_COUNT+1]

    def _unpack_rootcerthash_count(self, unpacked):
        self._rootcerthash_count = unpacked[0]

    def _unpack_rootcerthash_array(self, unpacked):
        self._rootcerthash_array = list(unpacked)
        for i in range(len(self._rootcerthash_array)):
            self._rootcerthash_array[i] = binascii.hexlify(self._rootcerthash_array[i])

    def pack(self):

        return (self._pack_base() +
                self._pack_serial_segment() +
                self._pack_reserved() +
                self._pack_flags() +
                self._pack_imgid_info() +
                self._pack_rootcerthash_count() +
                self._pack_rootcerthash_array()
                )

    def _pack_base(self):
        values = [self.magic,
                  self.data_size,
                  self.version]
        return struct.pack(self.get_format(), *values)

    def _pack_serial_segment(self):
        values = [self.sernum_start, self.sernum_end]
        return struct.pack(self.get_format_serial_segment(), *values)

    def _pack_reserved(self):
        values = [self.reserved]
        return struct.pack(self.get_format_reserved(), *values)

    def _pack_flags(self):
        values = [self.flags]
        return struct.pack(self.get_format_flags(), *values)

    def _pack_imgid_info(self):
        values = [self.imgid_count] + list(self.imgid_array) + list([0] * (IMG_ID_MAX_COUNT-len(self.imgid_array)))
        return struct.pack(self.get_format_imgid_info(), *values)

    def _pack_rootcerthash_count(self):
        values = [self.rootcerthash_count]
        return struct.pack(self.get_format_rootcerthash_count(), *values)

    def _pack_rootcerthash_array(self):
        if self.rootcerthash_count == 0:
            return ''

        values = []
        for i in range(len(self.rootcerthash_array)):
            values.append(binascii.unhexlify(self.rootcerthash_array[i]))
        return struct.pack(self.get_format_rootcerthash_array(self.rootcerthash_count), *values)

    @classmethod
    def get_format(cls):
        return '<4sII'

    @classmethod
    def get_format_serial_segment(cls):
        return '<II'

    @classmethod
    def get_format_reserved(cls):
        return '<I'

    @classmethod
    def get_format_flags(cls):
        return '<Q'

    @classmethod
    def get_format_imgid_info(cls):
        return '<' + ('I' * (IMG_ID_MAX_COUNT+1))

    @classmethod
    def get_format_rootcerthash_count(cls):
        return '<I'

    @classmethod
    def get_format_rootcerthash_array(cls, hashcount):
        return '<' + ('32s' * hashcount)

    def validate(self):
        # Check that the magic is correct
        if self.magic not in df.DBGPMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic in the Debug Policy file: ' + self.magic + '\n'
                               'This is not a Debug Policy file.')

        # Check that the version is correct
        if self.version not in df.DBGPVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported version in the Debug Policy file: ' + hex(self.version))

    def validate_ranges(self):
        # Check number of elements in config falls within range
        if not (0 <= self.imgid_count <= IMG_ID_MAX_COUNT):
            raise RuntimeError('Invalid number of image IDs supplied in config file. ' +
                               '0 - ' + str(IMG_ID_MAX_COUNT) + ' IDs supported, ' +
                               str(self.imgid_count) + ' detected.')

    def _repr_properties(self):
        properties = [
                      ('Magic', self.magic),
                      ('Size', self.data_size),
                      ('Revision', self.version),
                      ('Serial Number Start', self.sernum_start),
                      ('Serial Number End', self.sernum_end),
                      ('Reserved', self.reserved),
                      ('Flags', hex(self.flags)[2:].rstrip('L')),
                      ('Image ID Count', self.imgid_count)]

        if self.imgid_count!=0:
            for i in self.imgid_array:
                properties = properties + [('Image ID', hex(i)[2:].rstrip('L'))]

        properties = properties + [('Root Certificate Hash Count', self.rootcerthash_count)]

        if self.rootcerthash_count!=0:
            for i in self.rootcerthash_array:
                properties = properties + [('Root Certificate Hash', i)]
        return [(attr, val) for attr, val in properties]

    def __repr__(self):
        return properties_repr(self._repr_properties())

    def _range_check(self, value, bcnt, tag):
        max_val = int('0x' + 'FF'*bcnt, 16)
        if value < 0 or value > max_val:
            raise RuntimeError(tag + ' value: ' + value + ' is out of range: 0x0-' + hex(max_val))

    @property
    def magic(self):
        return self._magic

    @magic.setter
    def magic(self, value):
        if value not in df.DBGPMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic: ' + value)
        self._magic = value

    @property
    def version(self):
        return self._version

    @version.setter
    def version(self, value):
        if value not in df.DBGPVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported version: ' + hex(value))
        self._version = value

    @property
    def flags(self):
        return self._flags

    @flags.setter
    def flags(self, value):
        self._flags = value

    @property
    def sernum_start(self):
        return self._sernum_start

    @property
    def sernum_end(self):
        return self._sernum_end

    def set_sernum(self, sernum_start, sernum_end):

        self._range_check(sernum_start, 4, 'sernum_start')
        self._range_check(sernum_start, 4, 'sernum_end')

        if df.SERNUM_START_END_MATCH:
            if sernum_start != sernum_end:
                raise RuntimeError('Serial Number Start is not same as Serial Number End' + '\nSerial Number Start = ' + str(sernum_start) + '\nSerial Number End = ' + str(sernum_end))
        else:
            if sernum_start > sernum_end:
                raise RuntimeError('Serial Number Start is greater than Serial Number End'+ '\nSerial Number Start = ' + str(sernum_start) + '\nSerial Number End = ' + str(sernum_end))

        self._sernum_start = sernum_start
        self._sernum_end = sernum_end

    @property
    def imgid_count(self):
        return self._imgid_count

    @property
    def imgid_array(self):
        return self._imgid_array

    @imgid_array.setter
    def imgid_array(self, value):
        for i in range(len(value)):
            self._range_check(value[i], 4, 'imgid_array')

        self._imgid_count = len(value)
        self._imgid_array = value

    @property
    def rootcerthash_count(self):
        return self._rootcerthash_count

    @property
    def rootcerthash_array(self):
        return self._rootcerthash_array

    @rootcerthash_array.setter
    def rootcerthash_array(self, value):
        for i in range(len(value)):
            if len(value[i]) != 64:
                raise RuntimeError('rootcerthash is not equal to 256 bits, len = '+str(len(value[i])))

        self._rootcerthash_count = len(value)
        self._rootcerthash_array = value

    @property
    def data_size(self):
        return (struct.calcsize(self.get_format()) +
                struct.calcsize(self.get_format_serial_segment()) +
                struct.calcsize(self.get_format_reserved()) +
                struct.calcsize(self.get_format_flags()) +
                struct.calcsize(self.get_format_imgid_info()) +
                struct.calcsize(self.get_format_rootcerthash_count()) +
                struct.calcsize(self.get_format_rootcerthash_array(self.rootcerthash_count))
               )

    def set_base_info(self):
        self.magic = df.DBGPMAG
        self.version = df.DBGPVERONE