# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Nov 25, 2013

@author: hraghav
'''

from sectools.common.parsegen import pmbl
from sectools.features.isc.parsegen.base import SecParseGenBase


PMBL_FILE_TYPE = 'pmbl'


class SecParseGenPmbl(SecParseGenBase):

    def __init__(self, data, imageinfo=None, pmbl_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

        # Check the arguments
        if imageinfo is not None:
            pmbl_properties = imageinfo.image_type.pmbl_properties
        if pmbl_properties is None:
            raise RuntimeError('PMBL properties must not be None.')

        # Initialize the pmbl parsegen
        self._pmbl_parsegen = pmbl.ParseGenPmbl(data,
                                                pmbl_properties.preamble_size,
                                                pmbl_properties.has_magic_num,
                                                pmbl_properties.page_size,
                                                pmbl_properties.num_of_pages,
                                                pmbl_properties.ota_enabled,
                                                pmbl_properties.min_size_with_pad,
                                                self.debug_dir,
                                                self.debug_prefix,
                                                self.debug_suffix)

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'PMBL Properties: ' + '\n' + repr(self._pmbl_parsegen))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return PMBL_FILE_TYPE

    def get_wrapped_data(self):
        return self._pmbl_parsegen.code

    def set_wrapped_data(self, data):
        self._pmbl_parsegen.code = data

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt)
        if integrity_check:
            raise RuntimeError('Pmbl Images do not support integrity check.')
        if sign:
            raise RuntimeError('Pmbl Images do not support signing.')
        if encrypt:
            raise RuntimeError('Pmbl Images do not support encryption.')
        return self._pmbl_parsegen.get_data()

    @property
    def data_to_sign(self):
        raise RuntimeError('Bin Images do not support signing')

    def contains_integrity_check(self):
        return False
