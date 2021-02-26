# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.aost_lic import defines as df
from sectools.common.parsegen.aost_lic.header.base import extract_header,\
    create_empty_header
from sectools.common.utils.c_misc import store_debug_data_to_file


class ParseGenAOSTLic(object):

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

        # Handle empty license creation
        if data is None:
            if version is None:
                raise RuntimeError('Either data or version must be given.')
            data = self.create_empty_lic(version)

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = extract_header(data)

        # Extract the signing attributes
        data, self.sign = extract_sign(data)
        self.store_debug_data(df.FILE_SIGN_IN, self.sign)
        data, self.cert_chain = extract_cert_chain(data)
        self.store_debug_data(df.FILE_CERT_CHAIN_IN, self.cert_chain)

    def __repr__(self):
        return ('Header: ' + '\n' + repr(self.header))

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def create_empty_lic(self, version):
        header = create_empty_header(version)
        return header.pack()

    def get_data(self):
        return self.header.pack() + self.sign + self.cert_chain

def extract_sign(data):
    data_len = len(data)
    sign = ''
    if data_len == 0:
        pass
    elif data_len < df.AOSTLSIGNSIZE:
        raise RuntimeError('Partial data of length ' + str(data_len) + ' detected in place of signature')
    else:
        sign = data[:df.AOSTLSIGNSIZE]
        data = data[df.AOSTLSIGNSIZE:]
    return data, sign

def extract_cert_chain(data):
    return '', data
