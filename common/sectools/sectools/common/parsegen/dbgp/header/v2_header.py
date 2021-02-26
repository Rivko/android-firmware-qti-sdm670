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

ROOTCERTHASH_MAX_COUNT = 4
SERIAL_NUM_MAX_COUNT = 200


class DbgpHdrv2(StructBase):

    INVALID_FLAG_BITS = 0xFFFFFFFFFFF0

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

        # Unpack the flags
        fmt = self.get_format_flags()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_flags(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the image bit map
        fmt = self.get_format_imagebit_map()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_imagebit_map(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack number of rootcerthashes
        fmt = self.get_format_rootcerthash_count()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_count(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the hashes
        fmt = self.get_format_rootcerthash_array()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_array(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack serial number fields
        fmt = self.get_format_serial_num_info()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_serial_num_info(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Any top level validations
        if not self.data_is_none:
            self.validate()

    def _unpack_base(self, unpacked):
        self._magic = unpacked[0]
        self._data_size = unpacked[1]
        self._version = unpacked[2]

    def _unpack_flags(self, unpacked):
        self._flags = unpacked[0]

    def _unpack_imagebit_map(self, unpacked):
        self._imagebit_map = unpacked[0]

    def _unpack_rootcerthash_count(self, unpacked):
        self._rootcerthash_count = unpacked[0]

    def _unpack_rootcerthash_array(self, unpacked):
        self._rootcerthash_array = list(unpacked)
        for i in range(self.rootcerthash_count):
            self._rootcerthash_array[i] = binascii.hexlify(self._rootcerthash_array[i])

    def _unpack_serial_num_info(self, unpacked):
        self._serial_num_count = unpacked[0]
        self._serial_num_array = unpacked[1:SERIAL_NUM_MAX_COUNT+1]

    def pack(self):

        return (self._pack_base() +
                self._pack_flags() +
                self._pack_imagebit_map() +
                self._pack_rootcerthash_count() +
                self._pack_rootcerthash_array() +
                self._pack_serial_num_info()
                )

    def _pack_base(self):
        values = [self.magic,
                  self.data_size,
                  self.version]
        return struct.pack(self.get_format(), *values)

    def _pack_flags(self):
        values = [self.flags]
        return struct.pack(self.get_format_flags(), *values)

    def _pack_imagebit_map(self):
        values = [self.imagebit_map]
        return struct.pack(self.get_format_imagebit_map(), *values)

    def _pack_rootcerthash_count(self):
        values = [self.rootcerthash_count]
        return struct.pack(self.get_format_rootcerthash_count(), *values)

    def _pack_rootcerthash_array(self):
        values = []
        for i in range(self.rootcerthash_count):
            values.append(binascii.unhexlify(self.rootcerthash_array[i]))
        values = values + list([''] * (ROOTCERTHASH_MAX_COUNT - self.rootcerthash_count))
        return struct.pack(self.get_format_rootcerthash_array(), *values)

    def _pack_serial_num_info(self):
        values = [self.serial_num_count] + list(self.serial_num_array) + list([0] * (SERIAL_NUM_MAX_COUNT-len(self.serial_num_array)))
        return struct.pack(self.get_format_serial_num_info(), *values)

    @classmethod
    def get_format(cls):
        return '<4sII'

    @classmethod
    def get_format_flags(cls):
        return '<Q'

    @classmethod
    def get_format_imagebit_map(cls):
        return '<I'

    @classmethod
    def get_format_rootcerthash_count(cls):
        return '<I'

    @classmethod
    def get_format_rootcerthash_array(cls):
        return '<' + ('32s' * ROOTCERTHASH_MAX_COUNT)

    @classmethod
    def get_format_serial_num_info(cls):
        return '<' + ('I' * (SERIAL_NUM_MAX_COUNT + 1))

    def validate(self):
        # Check that the magic is correct
        if self.magic not in df.DBGPMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic in the Debug Policy file: ' + self.magic + '\n'
                               'This is not a Debug Policy file.')

        # Check that the version is correct
        if self.version not in df.DBGPVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported version in the Debug Policy file: ' + hex(self.version))

    def validate_ranges(self):
        if not (0 <= self.rootcerthash_count <= ROOTCERTHASH_MAX_COUNT):
            raise RuntimeError('Invalid number of rootcert hashes supplied in config file. ' +
                               '0 - ' + str(ROOTCERTHASH_MAX_COUNT) + ' hashes supported, ' +
                               str(self.rootcerthash_count) + ' detected.')
        if not (1 <= self.serial_num_count <= SERIAL_NUM_MAX_COUNT):
            raise RuntimeError('Invalid number of serial numbers supplied in config file. ' +
                               '1 - ' + str(SERIAL_NUM_MAX_COUNT) + ' serial numbers supported, ' +
                               str(self.serial_num_count) + ' detected.')
        if self.imagebit_map > df.IMAGEBIT_MAP_MAX_VALUE:
            raise RuntimeError("The <image_bit_map> field only supports 32 bit positions (swid). Bit_pos outside [0, 32] cannot be set.")

    def _repr_properties(self):
        properties = [
                      ('Magic', self.magic),
                      ('Size', self.data_size),
                      ('Revision', self.version),
                      ('Flags', hex(self.flags)[2:].rstrip('L')),
                      ('Image Bit Map', hex(self.imagebit_map)[2:].rstrip('L'))]

        properties = properties + [('Root Certificate Hash Count', self.rootcerthash_count)]
        if self.rootcerthash_count != 0:
            for i in self.rootcerthash_array:
                properties = properties + [('Root Certificate Hash', i)]

        properties = properties + [('Serial Number Count', self._serial_num_count)]
        if self.serial_num_count != 0:
            for s in self.serial_num_array:
                properties = properties + [('Serial Num', s)]

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
    def imagebit_map(self):
        return self._imagebit_map

    @imagebit_map.setter
    def imagebit_map(self, value):
        self._imagebit_map = value

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
    def serial_num_count(self):
        return self._serial_num_count

    @property
    def serial_num_array(self):
        return self._serial_num_array

    @serial_num_array.setter
    def serial_num_array(self, value):
        for i in range(len(value)):
            self._range_check(value[i], 4, 'serial_num_array')

        self._serial_num_count = len(value)
        self._serial_num_array = value

    @property
    def data_size(self):
        return (struct.calcsize(self.get_format()) +
                struct.calcsize(self.get_format_flags()) +
                struct.calcsize(self.get_format_imagebit_map()) +
                struct.calcsize(self.get_format_rootcerthash_count()) +
                struct.calcsize(self.get_format_rootcerthash_array()) +
                struct.calcsize(self.get_format_serial_num_info())
               )

    def set_base_info(self):
        self.magic = df.DBGPMAG
        self.version = df.DBGPVERTWO