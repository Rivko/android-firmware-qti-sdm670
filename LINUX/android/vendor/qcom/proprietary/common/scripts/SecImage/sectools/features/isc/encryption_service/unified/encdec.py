# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.crypto import crypto_functions
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.encryption_service.unified.decryptor import Decryptor
from sectools.features.isc.encryption_service.unified.encryption_parameters import EncryptionParameters
from sectools.features.isc.encryption_service.unified.encryptor import Encryptor


UNIFIED_ENCRYPTION_IDENT = 'unified_encryption_2_0'
UNIFIED_SEGMENT_NUM_SCHEME = BaseEncdec.SEGMENT_NUMBER_SCHEME.TWO_BASED

class Encdec(BaseEncdec):

    def __init__(self, imageinfo, debug_dir=None):
        # Check that crypto binaries are available
        try:
            crypto_functions.are_available([crypto_functions.MOD_OPENSSL,
                                            crypto_functions.MOD_CRYPTO_CBC,
                                            crypto_functions.MOD_CRYPTO_CCM])
        except Exception as e:
            raise RuntimeError('Cannot proceed with encryption/decryption: ' + str(e))
        BaseEncdec.__init__(self, imageinfo, debug_dir=debug_dir)

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

Encdec.register()
