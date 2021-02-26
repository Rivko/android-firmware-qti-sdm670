# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import header.defines as df
from sectools.common.parsegen.dbgp.header.defines import DBGPVERONE, DBGPVERTWO, DBGPVERTHREE, DBGPVER_DESCRIPTION
from sectools.common.utils.c_misc import store_debug_data_to_file

# Add the classes into the map
from sectools.common.parsegen.dbgp.header.v1_header import DbgpHdrv1
from sectools.common.parsegen.dbgp.header.v2_header import DbgpHdrv2
from sectools.common.parsegen.dbgp.header.v3_header import DbgpHdrv3

DBGP_HDRS = {
                DBGPVERONE      : DbgpHdrv1,
                DBGPVERTWO      : DbgpHdrv2,
                DBGPVERTHREE    : DbgpHdrv3,
             }

class ParseGenDbgp(object):

    def __init__(self, data=None,
                 version=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 ):
        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix

        # Handle empty dbgp creation
        if data is None:
            data = self._create_empty_dbgp(version)

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = self._extract_header(data)

    def __repr__(self):
        return ('Debug Policy File: ' + '\n' + repr(self.header))

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(str(prefix) + '_' + str(file_name) + str(suffix),
                                     data, self.debug_dir)

    def _create_empty_dbgp(self, version):
        header = get_header_class(version)()
        header.set_base_info()
        return header.pack()

    def _extract_header(self, data):

        import struct
        ver_data = data
        version = struct.unpack('<I', ver_data[8:12])[0]

        header = get_header_class(version)(data)
        data = data[header.size:]
        return data, header

    def get_data(self):
        return self.header.pack()


def get_header_class(version):
    header_version = DBGP_HDRS.get(version, None)
    if header_version is None:
        supported_versions = [str(key) + '-' + str(DBGPVER_DESCRIPTION[key]) for key in DBGP_HDRS.keys()]
        raise RuntimeError('Invalid Debugpolicy version: ' + str(version) + '\n'
                           '    ' + 'Supported versions are: ' + str(supported_versions))
    return header_version