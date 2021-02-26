# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import struct

import defines as df
from sectools.common.parsegen.aost_lic.header.base import AOSTLicHdrCommon
from sectools.common.utils.c_misc import properties_repr


# Version 1 header specific definitions
AOSTL_MAX_META_SIZE = 0x0800


class AOSTLicHdrV1(AOSTLicHdrCommon):
    """Version 1 AOST License Header Class"""

    # Implementing the base APIs
    def initialize(self, version):
        AOSTLicHdrCommon.initialize(self, version)
        self.license_type = df.AOSTLTYPECOMM

    @property
    def size(self):
        return (struct.calcsize(self.get_format()) +
                struct.calcsize(self.get_format_metadata(self.metadata_size))
                )

    def validate(self):
        # Let base do verification
        AOSTLicHdrCommon.validate(self)

        # Reset the properties to do validations
        self.feature_id = self._feature_id
        self.oem_id = self._oem_id
        self.chip_id = self._chip_id
        self.product_id = self._product_id
        self.license_type = self._license_type
        self.evaluation_use_count = self._evaluation_use_count
        self.metadata = self._metadata

    # Representation of the object
    def _repr_properties(self):
        properties = [
                      ('Feature ID', {}, self.feature_id),
                      ('OEM ID', {}, hex(self.oem_id)),
                      ('Chip ID', {}, hex(self.chip_id)),
                      ('Product ID', {}, hex(self.product_id)),
                      (df.AOSTLTYPE_STRING, df.AOSTLTYPE_DESCRIPTION, self.license_type),
                      ('Evaluation Use Count', {}, self.evaluation_use_count),
                      ('Metadata Size', {}, self.metadata_size),
                     ]
        properties = [(attr, desc.get(val, val)) for attr, desc, val in properties]
        return AOSTLicHdrCommon._repr_properties(self) + properties

    def __repr__(self):
        return properties_repr(self._repr_properties())

    # Unpacking APIs
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

        # Unpack the metadata
        fmt = self.get_format_metadata(self.metadata_size)
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_metadata(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Any top level validations
        if not self.data_is_none:
            self.validate()

    def _unpack_base(self, unpacked):
        AOSTLicHdrCommon._unpack_data_list(self, unpacked)
        self._feature_id = unpacked[2]
        self._oem_id = unpacked[3]
        self._chip_id = unpacked[4]
        self._product_id = unpacked[5]
        self._license_type = unpacked[6]
        self._evaluation_use_count = unpacked[7]
        self._metadata_size = unpacked[8]

    def _unpack_metadata(self, unpacked):
        self._metadata = unpacked[0]

    # Packing APIs
    def pack(self):
        return (self._pack_base() +
                self._pack_metadata())

    def _pack_base(self):
        args_list = (AOSTLicHdrCommon._pack_data_list(self) +
                     [self.feature_id,
                      self.oem_id,
                      self.chip_id,
                      self.product_id,
                      self.license_type,
                      self.evaluation_use_count,
                      self.metadata_size,]
                    )
        return struct.pack(self.get_format(),
                           *args_list)


    def _pack_metadata(self):
        if self.metadata_size == 0:
            return ''
        return struct.pack(self.get_format_metadata(self.metadata_size),
                           self.metadata)

    # Formats broken down for various segments
    @classmethod
    def get_format(cls):
        return AOSTLicHdrCommon.get_format() + 'IHIHHHH'

    @classmethod
    def get_format_metadata(cls, count):
        return '>' + str(count) + 's'

    # Per property validation logic
    @property
    def feature_id(self):
        return self._feature_id

    @feature_id.setter
    def feature_id(self, value):
        self._range_check(value, 4, 'feature_id')
        self._feature_id = value

    @property
    def oem_id(self):
        return self._oem_id

    @oem_id.setter
    def oem_id(self, value):
        self._range_check(value, 2, 'oem_id')
        self._oem_id = value

    @property
    def chip_id(self):
        return self._chip_id

    @chip_id.setter
    def chip_id(self, value):
        if value is not None:
            self._range_check(value, 4, 'chip_id')
        else:
            value = 0
        self._chip_id = value

    @property
    def product_id(self):
        return self._product_id

    @product_id.setter
    def product_id(self, value):
        if value is not None:
            self._range_check(value, 2, 'product_id')
        else:
            value = 0
        self._product_id = value

    @property
    def license_type(self):
        return self._license_type

    @license_type.setter
    def license_type(self, value):
        if value not in df.AOSTLTYPE_DESCRIPTION.keys():
            raise RuntimeError('Invalid license type: ' + hex(value))
        self._license_type = value

    @property
    def evaluation_use_count(self):
        return self._evaluation_use_count

    @evaluation_use_count.setter
    def evaluation_use_count(self, value):
        if value is not None:
            self._range_check(value, 2, 'evaluation_use_count')
        else:
            value = 0
        self._evaluation_use_count = value

    @property
    def metadata_size(self):
        return self._metadata_size

    @property
    def metadata(self):
        return self._metadata

    @metadata.setter
    def metadata(self, value):
        # Check the length of the array
        metadata_size = len(value)
        if metadata_size > AOSTL_MAX_META_SIZE:
            raise RuntimeError('Length of metadata ' + hex(metadata_size) + ' '
                               'is more than the allowed value ' + hex(AOSTL_MAX_META_SIZE))
        self._metadata_size = metadata_size
        self._metadata = value
