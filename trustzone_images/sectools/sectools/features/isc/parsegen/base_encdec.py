# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from .base import SecParseGenBase


class SecParseGenBaseEncDec(SecParseGenBase):

    def __init__(self, data,
                 imageinfo=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        self._encryption_params = ''
        self._encrypt = False

        super(SecParseGenBaseEncDec, self).__init__(data,
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

    def _repr_properties(self):
        properties = [
                      ('Integrity Check', self.contains_integrity_check()),
                      ('Signed', self.is_signed()),
                      ('Encrypted', self.is_encrypted()),
                      ('Size of signature', self.sig_size),
                      ('Size of one cert', self.max_cert_size),
                      ('Num of certs in cert chain',  self.num_certs_in_certchain),
                      ('Number of root certs', self.num_root_certs),
                      ('Maximum number of root certs', self.max_num_root_certs),
                      ('Cert chain size', self.get_cert_chain_size(self.authority)),
                     ]
        return properties

    @property
    def encryption_params(self):
        return self._encryption_params

    @encryption_params.setter
    def encryption_params(self, encryption_params):
        if encryption_params:
            self.validate_encryption_params(encryption_params)
        self._encryption_params = encryption_params

    def validate_encryption_params(self, encryption_params):
        if not isinstance(encryption_params, bytes):
            raise RuntimeError(
                'Encryption params must be of type "bytes". Given encryption params type is: "{}"'.format(
                    type(encryption_params)))

    #--------------------------------------------------------------------------
    # Tests to check various attributes of the data
    #--------------------------------------------------------------------------
    def is_encrypted(self):
        """ Returns True if data contains all the encryption metadata """
        return self.contains_encryption_params()

    def contains_encryption_params(self):
        """ Returns True if data contains the encryption params """
        return self.encryption_params != ''

    #--------------------------------------------------------------------------
    # Get, Set functions for 'encrypt'
    #--------------------------------------------------------------------------
    @property
    def encrypt(self):
        return self._encrypt

    @encrypt.setter
    def encrypt(self, value):
        self._encrypt = True if value else False

    #--------------------------------------------------------------------------
    # Helper functions
    #--------------------------------------------------------------------------
    def encrypt_segment(self, segment_data, segment_num):
        return self.encdec.get_encryptor().encrypt_segment(segment_data, segment_num)

    #--------------------------------------------------------------------------
    # Hash segment metadata methods
    #--------------------------------------------------------------------------
    def get_hash_segment_metadata(self, authority=None):
        """
        Return the signing attributes (of the provided authority) contained
            within the hash segment.
        Only applies to parsers that have a hash segment with metadata.
        """
        return None

    def validate_hash_segment_metadata(self, authority=None):
        """
        Raise exception if parsegen's hash segment contains signing attributes
            that do not match the configured values.
        Only applies to parsers that have a hash segment with metadata.
        """
        pass
