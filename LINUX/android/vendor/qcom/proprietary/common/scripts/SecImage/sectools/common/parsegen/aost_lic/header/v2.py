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


# Version 2 header specific definitions
AOSTL_MAX_GUID_SIZE = 32
AOSTL_MAX_FEATURE_IDS_SIZE = 256
AOSTL_MAX_CHIP_IDS_SIZE = 16
AOSTL_MAX_RESERVED_FIELD_SIZE = 256


class AOSTLicHdrV2(AOSTLicHdrCommon):
    """Version 2 AOST License Header Class"""

    # Implementing the base APIs
    def initialize(self, version):
        AOSTLicHdrCommon.initialize(self, version)
        self.license_type = df.AOSTLTYPECOMM

    @property
    def size(self):
        return (struct.calcsize(self.get_format()) +
                struct.calcsize(self.get_format_feature_id(self.feature_id_size)) +
                struct.calcsize(self.get_format_chipset_id_size()) +
                struct.calcsize(self.get_format_feature_id(self.chip_id_size)) +
                struct.calcsize(self.get_format_extended()) +
                struct.calcsize(self.get_format_reserved_field(self.reserved_field_size))
               )

    def validate(self):
        # Let base do verification
        AOSTLicHdrCommon.validate(self)

        # Reset the properties to do validations
        self.guid = self._guid
        self.bundle_id = self._bundle_id
        self.party_id = self._party_id
        self.oem_id = self._oem_id
        self.feature_ids = self._feature_ids
        self.chip_ids = self._chip_ids
        self.product_id = self._product_id
        self.license_type = self._license_type
        self.evaluation_use_count = self._evaluation_use_count
        self.reserved_field = self._reserved_field

    # Representation of the object
    def _repr_properties(self):
        properties = [
                      ('Bundle ID', {}, hex(self.bundle_id)),
                      ('GUID', {}, self.guid),
                      ('Party ID', {}, hex(self.party_id)),
                      ('OEM ID', {}, hex(self.oem_id)),
                      ('Feature IDs', {}, str(self.feature_ids)),
                      ('Chip IDs', {}, str(self.chip_ids)),
                      ('Product ID', {}, hex(self.product_id)),
                      (df.AOSTLTYPE_STRING, df.AOSTLTYPE_DESCRIPTION, self.license_type),
                      ('Evaluation Use Count', {}, self.evaluation_use_count),
                      ('Reserved field size', {}, self.reserved_field_size),
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

        # Unpack the feature ids
        fmt = self.get_format_feature_id(self.feature_id_size)
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_feature_ids(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack chip_id_size
        fmt = self.get_format_chipset_id_size()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_chip_id_size(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the chip ids (use same formate as feature id)
        fmt = self.get_format_feature_id(self.chip_id_size)
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_chip_ids(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the extended
        fmt = self.get_format_extended()
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_extended(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Unpack the reserved fields
        fmt = self.get_format_reserved_field(self.reserved_field_size)
        if self.data_is_none:
            data = '\0' * struct.calcsize(fmt)
        self._unpack_reserved_field(struct.unpack_from(fmt, data))
        data = data[struct.calcsize(fmt):]

        # Any top level validations
        if not self.data_is_none:
            self.validate()

    def _unpack_base(self, unpacked):
        AOSTLicHdrCommon._unpack_data_list(self, unpacked)
        self._guid = unpacked[2]
        self._bundle_id = unpacked[3]
        self._party_id = unpacked[4]
        self._oem_id = unpacked[5]
        self._feature_id_size = unpacked[6]

    def _unpack_feature_ids(self, unpacked):
        self._feature_ids = list(unpacked)

    def _unpack_chip_id_size(self, unpacked):
        self._chip_id_size = unpacked[0]

    def _unpack_chip_ids(self, unpacked):
        self._chip_ids = list(unpacked)

    def _unpack_extended(self, unpacked):
        self._product_id = unpacked[0]
        self._license_type = unpacked[1]
        self._evaluation_use_count = unpacked[2]
        self._reserved_field_size = unpacked[3]

    def _unpack_reserved_field(self, unpacked):
        self._reserved_field = unpacked[0]

    # Packing APIs
    def pack(self):
        return (self._pack_base() +
                self._pack_feature_ids() +
                self._pack_chip_id_size() +
                self._pack_chip_ids() +
                self._pack_extended() +
                self._pack_reserved_field())


    def _pack_base(self):
        args_list = (AOSTLicHdrV2._pack_data_list(self) +
                     [self.guid,
                      self.bundle_id,
                      self.party_id,
                      self.oem_id,
                      self.feature_id_size,
                     ])
        return struct.pack(self.get_format(),
                           *args_list)

    def _pack_feature_ids(self):
        if self.feature_id_size == 0:
            return ''
        return struct.pack(self.get_format_feature_id(self.feature_id_size),
                           *self.feature_ids)

    def _pack_chip_id_size(self):
        return struct.pack(self.get_format_chipset_id_size(), self.chip_id_size)

    def _pack_chip_ids(self):
        if self.chip_id_size == 0:
            return ''
        return struct.pack(self.get_format_feature_id(self.chip_id_size),
                           *self.chip_ids)

    def _pack_extended(self):
        return struct.pack(self.get_format_extended(),
                           self.product_id,
                           self.license_type,
                           self.evaluation_use_count,
                           self.reserved_field_size)

    def _pack_reserved_field(self):
        if self.reserved_field_size == 0:
            return ''
        return struct.pack(self.get_format_reserved_field(self.reserved_field_size),
                           self.reserved_field)

    # Formats broken down for various segments
    @classmethod
    def get_format(cls):
        return AOSTLicHdrCommon.get_format() + str(AOSTL_MAX_GUID_SIZE) + 'sIIHH'

    @classmethod
    def get_format_feature_id(cls, count):
        return '>' + ('I' * count)

    @classmethod
    def get_format_chipset_id_size(cls):
        return '>I'

    @classmethod
    def get_format_extended(cls):
        return '>HHHH'

    @classmethod
    def get_format_reserved_field(cls, count):
        return '>' + str(count) + 's'

    # Per property validation logic
    @property
    def guid(self):
        return self._guid

    @guid.setter
    def guid(self, value):
        guid_length = len(value)
        if guid_length != AOSTL_MAX_GUID_SIZE:
            raise RuntimeError('GUID length must be of length: ' + str(AOSTL_MAX_GUID_SIZE) + ', '
                               'length is: ' + str(guid_length))
        self._guid = value

    @property
    def bundle_id(self):
        return self._bundle_id

    @bundle_id.setter
    def bundle_id(self, value):
        self._range_check(value, 4, 'bundle_id')
        self._bundle_id = value

    @property
    def party_id(self):
        return self._party_id

    @party_id.setter
    def party_id(self, value):
        self._range_check(value, 4, 'party_id')
        self._party_id = value

    @property
    def oem_id(self):
        return self._oem_id

    @oem_id.setter
    def oem_id(self, value):
        self._range_check(value, 2, 'oem_id')
        self._oem_id = value

    @property
    def feature_id_size(self):
        return self._feature_id_size

    @property
    def feature_ids(self):
        return self._feature_ids

    @feature_ids.setter
    def feature_ids(self, value):
        # Check the length of the array
        if value is None:
            value = []
        feature_id_size = len(value)
        if feature_id_size > AOSTL_MAX_FEATURE_IDS_SIZE:
            raise RuntimeError('Feature ID array length must be less than: ' + str(AOSTL_MAX_FEATURE_IDS_SIZE) + ', '
                               'length is: ' + str(feature_id_size))

        # Check the length of each feature id
        for feature_id in value:
            self._range_check(feature_id, 4, 'feature_id')

        self._feature_id_size = feature_id_size
        self._feature_ids = value

    @property
    def chip_id_size(self):
        return self._chip_id_size

    @property
    def chip_ids(self):
        return self._chip_ids

    @chip_ids.setter
    def chip_ids(self, value):
        chip_id_size = len(value)
        if chip_id_size > AOSTL_MAX_CHIP_IDS_SIZE:
            raise RuntimeError('Chip ID array length must be less than: ' + str(AOSTL_MAX_CHIP_IDS_SIZE) + ', '
                                'length is: ' + str(chip_id_size))

        # Check the length of each chip id
        for chip_id in value:
            self._range_check(chip_id, 4, 'chip_id')

        self._chip_id_size = chip_id_size
        self._chip_ids = value

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
    def reserved_field_size(self):
        return self._reserved_field_size

    @property
    def reserved_field(self):
        return self._reserved_field

    @reserved_field.setter
    def reserved_field(self, value):
        # Check the length of the array
        reserved_field_size = len(value)
        if reserved_field_size > AOSTL_MAX_RESERVED_FIELD_SIZE:
            raise RuntimeError('Reserved field length must be less than: ' + str(AOSTL_MAX_RESERVED_FIELD_SIZE) + ', '
                               'length is: ' + str(reserved_field_size))

        self._reserved_field_size = reserved_field_size
        self._reserved_field = value
