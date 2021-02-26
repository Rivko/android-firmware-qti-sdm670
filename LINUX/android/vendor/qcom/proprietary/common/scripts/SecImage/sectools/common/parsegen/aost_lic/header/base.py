# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import defines as df
from sectools.common.parsegen.aost_lic.header.defines import AOSTLVERONE, \
    AOSTLVERTWO, AOSTLVER_DESCRIPTION
from sectools.common.utils.c_misc import properties_repr
from sectools.common.utils.struct_base import StructBase


class AOSTLicHdrCommon(StructBase):

    def _unpack_data_list(self, unpacked):
        self._magic_number = unpacked[0]
        self._version_number = unpacked[1]

    def _pack_data_list(self):
        return [self.magic_number,
                self.version_number,]

    @classmethod
    def get_format(cls):
        return ('>4sI')

    def initialize(self, version):
        self.magic_number = df.AOSTLMAG
        self.version_number = version

    def validate(self):
        # Top level validations
        self.magic_number = self._magic_number
        self.version_number = self._version_number

    def _repr_properties(self):
        properties = [
                      (df.AOSTLMAG_STRING, df.AOSTLMAG_DESCRIPTION, self.magic_number),
                      (df.AOSTLVER_STRING, df.AOSTLVER_DESCRIPTION, self.version_number),
                     ]
        return [(attr, desc.get(val, val)) for attr, desc, val in properties]

    def __repr__(self):
        return properties_repr(self._repr_properties())

    def _range_check(self, value, bcnt, tag):
        max_val = int('0x' + 'FF'*bcnt, 16)
        if value < 0 or value > max_val:
            raise RuntimeError(tag + ' value: ' + value + ' is out of range: 0x0-' + hex(max_val))

    @property
    def magic_number(self):
        return self._magic_number

    @magic_number.setter
    def magic_number(self, value):
        if value not in df.AOSTLMAG_DESCRIPTION.keys():
            raise RuntimeError('Invalid magic: ' + value)
        self._magic_number = value

    @property
    def version_number(self):
        return self._version_number

    @version_number.setter
    def version_number(self, value):
        if value not in df.AOSTLVER_DESCRIPTION.keys():
            raise RuntimeError('Unsupported AOST license version: ' + hex(value))
        self._version_number = value


# Add the classes into the map
from sectools.common.parsegen.aost_lic.header.v1 import AOSTLicHdrV1
from sectools.common.parsegen.aost_lic.header.v2 import AOSTLicHdrV2

AOSTL_HDRS = {
                AOSTLVERONE : AOSTLicHdrV1,
                AOSTLVERTWO : AOSTLicHdrV2,
             }


def get_header_class(version):
    header_version = AOSTL_HDRS.get(version, None)
    if header_version is None:
        supported_versions = [str(key) + '-' + str(AOSTLVER_DESCRIPTION[key]) for key in AOSTL_HDRS.keys()]
        raise RuntimeError('Invalid AOST License version: ' + str(version) + '\n'
                           '    ' + 'Supported versions are: ' + str(supported_versions))
    return header_version

def extract_header(data):
    # Ensure data is of common header length
    if len(data) < AOSTLicHdrCommon.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than common AOST License length "' + AOSTLicHdrCommon.get_size() + '"')

    # Get the appropriate version header
    header = AOSTLicHdrCommon(data)
    header_version = get_header_class(header.version_number)

    # Ensure data is of aost header length
    if len(data) < header_version.get_size():
        raise RuntimeError('Data length "' + str(len(data)) + '" is less than ' +
                           str(AOSTLVER_DESCRIPTION[header.version_number]) + ' AOST License length "' + header_version.get_size() + '"')

    # Extract the header
    header = header_version(data)
    data = data[header.size:]
    return data, header

def create_empty_header(version):
    header = get_header_class(version)()
    header.initialize(version)
    return header
