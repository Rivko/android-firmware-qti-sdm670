# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


from sectools.common.parsegen.aost_lic.format import ParseGenAOSTLic
from sectools.features.isc.parsegen.base import SecParseGenBase


AOST_LIC_FILE_TYPE = 'aost_lic'


class SecParseGenAOSTLic(SecParseGenBase):

    def __init__(self, data, imageinfo=None, aost_lic_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

        # Disable cert chain padding
        self.pad_cert_chain = False

        # Initialize the aost license parsegen
        self._aost_lic_parsegen = ParseGenAOSTLic(data,
                                                  self.debug_dir,
                                                  self.debug_prefix,
                                                  self.debug_suffix)

        # Set the security attributes from the license parser
        self.data_signature = self._aost_lic_parsegen.sign
        self.cert_chain = self._aost_lic_parsegen.cert_chain

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'AOST License Properties: ' + '\n' + repr(self._aost_lic_parsegen))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return AOST_LIC_FILE_TYPE

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
        # Get the data to sign (header)
        return self._aost_lic_parsegen.header.pack()

    def contains_integrity_check(self):
        return False

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def _get_data_int(self, sign):
        # Backup the parsegen
        backup = _BackupAOSTLicParsegen(self._aost_lic_parsegen)

        # Update the security attributes per the flags
        if sign:
            self._aost_lic_parsegen.sign = self.data_signature
            self._aost_lic_parsegen.cert_chain = self.cert_chain

        # Get the signed data
        retval = self._aost_lic_parsegen.get_data()

        # Restore the parsegen
        backup.restore(self._aost_lic_parsegen)

        return retval


class _BackupAOSTLicParsegen(object):

    def __init__(self, parsegen):
        self.backup(parsegen)

    def backup(self, parsegen):
        # Backup the original
        self.signature = parsegen.sign
        self.cert_chain = parsegen.cert_chain

        # Clear the original
        parsegen.sign = ''
        parsegen.cert_chain = ''

        return parsegen

    def restore(self, parsegen):
        # Restore the original
        parsegen.sign = self.signature
        parsegen.cert_chain = self.cert_chain

        # Clear self
        self.signature = ''
        self.cert_chain = ''

        return parsegen


