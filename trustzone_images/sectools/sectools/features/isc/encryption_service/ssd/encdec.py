# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common import crypto
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.encryption_service.ssd.decryptor import Decryptor
from sectools.features.isc.encryption_service.ssd.encryption_parameters import EncryptionParameters
from sectools.features.isc.encryption_service.ssd.encryptor import Encryptor


SSD_ENCRYPTION_IDENT = 'ssd_encryption'
SSD_SEGMENT_NUM_SCHEME = BaseEncdec.SegmentNumberScheme.ZERO_BASED


class Encdec(BaseEncdec):

    def __init__(self, imageinfo, debug_dir=None, validating=False, encrypted_segments_indices=None):
        # Check that crypto binaries are available
        try:
            crypto.are_available([crypto.modules.MOD_OPENSSL])
        except Exception as e:
            raise RuntimeError('Cannot proceed with encryption/decryption: ' + str(e))
        BaseEncdec.__init__(self, imageinfo, debug_dir=debug_dir, validating=validating, encrypted_segments_indices=encrypted_segments_indices)

    @property
    def _encryptor_class(self):
        return Encryptor

    @property
    def _decryptor_class(self):
        return Decryptor

    @property
    def _encryption_parameters_class(self):
        return EncryptionParameters

    def get_dummy_key(self):
        return self.encryption_parameters._get_private_key_from_config_file()

    def extract_encryption_parameters(self, data):
        return self.encryption_parameters.extract_encryption_parameters(data)

    @classmethod
    def ident(self):
        return SSD_ENCRYPTION_IDENT

    @classmethod
    def get_segment_num_scheme(self):
        return SSD_SEGMENT_NUM_SCHEME


Encdec.register()
