# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common import crypto
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.encryption_service.unified.decryptor import Decryptor
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters_1_0_L2_1_0
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters_1_0_L2_1_1
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters_2_0_L2_1_2
from sectools.features.isc.encryption_service.unified.encryptor import Encryptor


UNIFIED_ENCRYPTION_IDENT = 'unified_encryption_2_0'
UIE_1_0_L2_1_1_IDENT = 'UIE_1_0_L2_1_1'
UIE_2_0_L2_1_2_IDENT = 'UIE_2_0_L2_1_2'
UNIFIED_SEGMENT_NUM_SCHEME = BaseEncdec.SegmentNumberScheme.TWO_BASED


class Encdec(BaseEncdec):

    def __init__(self, imageinfo, debug_dir=None, validating=False, encrypted_segments_indices=None):
        # Check that crypto binaries are available
        try:
            crypto.are_available([crypto.modules.MOD_OPENSSL,
                                  crypto.modules.MOD_CRYPTO_CBC,
                                  crypto.modules.MOD_CRYPTO_CCM])
        except Exception as e:
            raise RuntimeError('Cannot proceed with encryption/decryption: ' + str(e))
        BaseEncdec.__init__(self, imageinfo, debug_dir=debug_dir, validating=validating,
                            encrypted_segments_indices=encrypted_segments_indices)

    @property
    def _encryptor_class(self):
        return Encryptor

    @property
    def _decryptor_class(self):
        return Decryptor

    @property
    def _encryption_parameters_class(self):
        return EncryptionParameters_1_0_L2_1_0

    def get_dummy_key(self):
        return self.encryption_parameters.get_l1_key()

    def extract_encryption_parameters(self, data):
        return self.encryption_parameters.extract_encryption_parameters(data)

    @classmethod
    def ident(self):
        return UNIFIED_ENCRYPTION_IDENT

    @classmethod
    def get_segment_num_scheme(self):
        return UNIFIED_SEGMENT_NUM_SCHEME


class EncdecUIE11(Encdec):

    @property
    def _encryption_parameters_class(self):
        return EncryptionParameters_1_0_L2_1_1

    @classmethod
    def ident(self):
        return UIE_1_0_L2_1_1_IDENT


class EncdecUIE2(Encdec):

    def __init__(self, imageinfo, debug_dir=None, validating=False, encrypted_segments_indices=None):
        try:
            crypto.are_available([crypto.modules.MOD_ECIESAD])
        except Exception as e:
            raise RuntimeError('Cannot proceed with encryption/decryption: ' + str(e))
        super(EncdecUIE2, self).__init__(imageinfo, debug_dir=debug_dir, validating=validating,
                                         encrypted_segments_indices=encrypted_segments_indices)

    @property
    def _encryption_parameters_class(self):
        return EncryptionParameters_2_0_L2_1_2

    @classmethod
    def ident(self):
        return UIE_2_0_L2_1_2_IDENT

    @classmethod
    def supports_decryption(cls):
        return False


Encdec.register()
EncdecUIE11.register()
EncdecUIE2.register()
