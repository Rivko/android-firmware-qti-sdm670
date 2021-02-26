# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


from sectools.features.isc.parsegen.base import SecParseGenBase


AOST_BIN_LIC_FILE_TYPE = 'aost_bin_lic'


class SecParseGenAOSTBinLic(SecParseGenBase):

    def __init__(self, data, imageinfo=None, aost_bin_lic_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

        # Disable cert chain padding
        self.pad_cert_chain = False

        # Save data for signing
        self.data = data

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return AOST_BIN_LIC_FILE_TYPE

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt)
        if integrity_check:
            raise RuntimeError('AOST Licenses do not support integrity check.')
        if encrypt:
            raise RuntimeError('AOST Licenses do not support encryption.')
        return self._get_data_int(sign)

    @property
    def data_to_sign(self):
        # Get the data to sign (header + metadata)
        return self.data

    def contains_integrity_check(self):
        return False

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def _get_data_int(self, sign):
        # Set the default data
        retval = self.data

        # Get the signed data
        if sign:
            retval += self.data_signature + self.cert_chain

        return retval