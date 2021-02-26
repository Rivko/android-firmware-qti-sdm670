#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

from sectools.features.isc.encryption_service.base.encdec import BaseEncdec
from sectools.features.isc.encryption_service.unified_1_0.encryptor import Encryptor
from sectools.features.isc.encryption_service.unified_1_0.decryptor import Decryptor
from sectools.features.isc.encryption_service.unified_1_0.encryption_parameters import EncryptionParameters


UNIFIED_ENCRYPTION_IDENT = 'unified_encryption'
UNIFIED_SEGMENT_NUM_SCHEME = BaseEncdec.SEGMENT_NUMBER_SCHEME.ZERO_BASED

class Encdec(BaseEncdec):

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
        return self.encryption_parameters._get_l1_key()

    def extract_encryption_parameters(self, data):
        return self.encryption_parameters.extract_encryption_parameters(data)

    @classmethod
    def ident(self):
        return UNIFIED_ENCRYPTION_IDENT

    @classmethod
    def get_segment_num_scheme(self):
        return UNIFIED_SEGMENT_NUM_SCHEME

Encdec.register()
