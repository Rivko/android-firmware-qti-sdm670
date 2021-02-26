# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Apr 27, 2015
@author: hraghav
'''

from sectools.features.isc.parsegen.base import SecParseGenBase


# Bin to sign file type
BIN_TO_SIGN_FILE_TYPE = 'bin_to_sign'


class SecParseGenBinToSign(SecParseGenBase):

    def __init__(self, data, imageinfo=None, bin_to_sign_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)
        self.data = data

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return BIN_TO_SIGN_FILE_TYPE

    def get_wrapped_data(self):
        return self.data

    def set_wrapped_data(self, data):
        self.data = data

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        if integrity_check:
            raise RuntimeError('Bin Images do not support integrity check.')
        if encrypt:
            raise RuntimeError('Bin Images do not support encryption.')

        data = self.data
        if sign:
            data += self.data_signature + self.cert_chain
        return data

    @property
    def data_to_sign(self):
        return self.data

    def contains_integrity_check(self):
        return False
