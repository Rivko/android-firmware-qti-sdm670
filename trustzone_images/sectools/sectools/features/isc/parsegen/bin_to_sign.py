# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc import defines
from sectools.common.parsegen.mbn.versions.v6.headers import MbnHdrV6
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec

# Bin to sign file type
BIN_TO_SIGN_FILE_TYPE = 'bin_to_sign'


class SecParseGenBinToSign(SecParseGenBaseEncDec):

    def __init__(self, data, imageinfo=None,
                 bin_to_sign_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        super(SecParseGenBinToSign, self).__init__(data,
                                                   imageinfo=imageinfo,
                                                   general_properties=general_properties,
                                                   encdec=encdec,
                                                   debug_dir=debug_dir,
                                                   debug_prefix=debug_prefix,
                                                   debug_suffix=debug_suffix,
                                                   validating=validating,
                                                   signing=signing,
                                                   parsegens=parsegens,
                                                   sign_attr=sign_attr)
        self.data = data
        self.header = None

        if imageinfo.general_properties.secboot_version == SECBOOT_VERSION_3_0:
            self.header = MbnHdrV6(self.data)

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

    def get_hash_segment_metadata(self, authority=None):
        if authority is None:
            authority = self.authority
        if (self.header is not None and
                getattr(self.header, "supports_metadata", lambda: False)()):
            if authority == defines.AUTHORITY_OEM:
                return self.header.get_metadata()
            else:
                return self.header.get_metadata_qti()
        return None

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
