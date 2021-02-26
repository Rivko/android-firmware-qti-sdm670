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
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters_1_1
from sectools.features.isc.encryption_service.unified.encryptor import Encryptor


UNIFIED_ENCRYPTION_IDENT = 'unified_encryption_2_0'
UIE_1_0_L2_1_1_IDENT = 'UIE_1_0_L2_1_1'
UNIFIED_SEGMENT_NUM_SCHEME = BaseEncdec.SEGMENT_NUMBER_SCHEME.TWO_BASED

class Encdec(BaseEncdec):

    def __init__(self, imageinfo, debug_dir=None, validating=False):
        # Check that crypto binaries are available
        try:
            crypto.are_available([crypto.modules.MOD_OPENSSL,
                                  crypto.modules.MOD_CRYPTO_CBC,
                                  crypto.modules.MOD_CRYPTO_CCM])
        except Exception as e:
            raise RuntimeError('Cannot proceed with encryption/decryption: ' + str(e))
        BaseEncdec.__init__(self, imageinfo, debug_dir=debug_dir, validating=validating)

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
        return self.encryption_parameters.get_l1_key()

    def extract_encryption_parameters(self, data):
        return self.encryption_parameters.extract_encryption_parameters(data)

    @classmethod
    def ident(self):
        return UNIFIED_ENCRYPTION_IDENT

    @classmethod
    def get_segment_num_scheme(self):
        return UNIFIED_SEGMENT_NUM_SCHEME

class Encdec_UIE_1_1(Encdec):

    @property
    def _encryption_parameters_class(self):
        return EncryptionParameters_1_1

    @classmethod
    def ident(self):
        return UIE_1_0_L2_1_1_IDENT

Encdec.register()
Encdec_UIE_1_1.register()
