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
from sectools.common.parsegen.dbgp.header.v2_header import DbgpHdrv2

ROOTCERTHASH_MAX_COUNT = 4
SERIAL_NUM_MAX_COUNT = 200


class DbgpHdrv3(DbgpHdrv2):

    INVALID_FLAG_BITS = 0xFFFF80FFFFF0

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

        # Unpack number of rootcerthashes_qti
        fmt = self.get_format_rootcerthash_qti_count()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_qti_count(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the hashes_qti
        fmt = self.get_format_rootcerthash_qti_array()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_rootcerthash_qti_array(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Any top level validations
        if not self.data_is_none:
            self.validate()

    def _unpack_rootcerthash_qti_count(self, unpacked):
        self._rootcerthash_qti_count = unpacked[0]

    def _unpack_rootcerthash_qti_array(self, unpacked):
        self._rootcerthash_qti_array = list(unpacked)
        for i in range(self.rootcerthash_qti_count):
            self._rootcerthash_qti_array[i] = binascii.hexlify(self._rootcerthash_qti_array[i])

    def pack(self):

        return (self._pack_base() +
                self._pack_flags() +
                self._pack_imagebit_map() +
                self._pack_rootcerthash_count() +
                self._pack_rootcerthash_array() +
                self._pack_serial_num_info() +
                self._pack_rootcerthash_qti_count() +
                self._pack_rootcerthash_qti_array()
                )

    def _pack_rootcerthash_qti_count(self):
        values = [self.rootcerthash_qti_count]
        return struct.pack(self.get_format_rootcerthash_qti_count(), *values)

    def _pack_rootcerthash_qti_array(self):
        values = []
        for i in range(self.rootcerthash_qti_count):
            values.append(binascii.unhexlify(self.rootcerthash_qti_array[i]))
        values = values + list([''] * (ROOTCERTHASH_MAX_COUNT - self.rootcerthash_qti_count))
        return struct.pack(self.get_format_rootcerthash_qti_array(), *values)

    @classmethod
    def get_format_rootcerthash_qti_count(cls):
        return '<I'

    @classmethod
    def get_format_rootcerthash_qti_array(cls):
        return '<' + ('32s' * ROOTCERTHASH_MAX_COUNT)

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

        if not (0 <= self.rootcerthash_qti_count <= ROOTCERTHASH_MAX_COUNT):
            raise RuntimeError('Invalid number of rootcert hashes supplied in config file. ' +
                               '0 - ' + str(ROOTCERTHASH_MAX_COUNT) + ' hashes supported, ' +
                               str(self.rootcerthash_qti_count) + ' detected.')

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

        properties = properties + [('Root Certificate Hash QTI Count', self.rootcerthash_qti_count)]
        if self.rootcerthash_qti_count != 0:
            for i in self.rootcerthash_qti_array:
                properties = properties + [('Root Certificate Hash QTI', i)]

        return [(attr, val) for attr, val in properties]

    @property
    def rootcerthash_qti_count(self):
        return self._rootcerthash_qti_count

    @property
    def rootcerthash_qti_array(self):
        return self._rootcerthash_qti_array

    @rootcerthash_qti_array.setter
    def rootcerthash_qti_array(self, value):
        for i in range(len(value)):
            if len(value[i]) != 64:
                raise RuntimeError('rootcerthash_qti is not equal to 256 bits, rxd input rootcerthash_qti bit length as '+str(len(value[i])*4))

        self._rootcerthash_qti_count = len(value)
        self._rootcerthash_qti_array = value

    @property
    def data_size(self):
        return (struct.calcsize(self.get_format()) +
                struct.calcsize(self.get_format_flags()) +
                struct.calcsize(self.get_format_imagebit_map()) +
                struct.calcsize(self.get_format_rootcerthash_count()) +
                struct.calcsize(self.get_format_rootcerthash_array()) +
                struct.calcsize(self.get_format_serial_num_info()) +
                struct.calcsize(self.get_format_rootcerthash_qti_count()) +
                struct.calcsize(self.get_format_rootcerthash_qti_array())
               )

    def set_base_info(self):
        self.magic = df.DBGPMAG
        self.version = df.DBGPVERTHREE