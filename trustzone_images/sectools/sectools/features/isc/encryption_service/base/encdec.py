# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from abc import abstractproperty, ABCMeta, abstractmethod

from six import add_metaclass


@add_metaclass(ABCMeta)
class BaseEncdec(object):

    class SegmentNumberScheme(object):
        ZERO_BASED = 0x0
        TWO_BASED = 0x1

    def __init__(self, imageinfo, debug_dir=None, validating=False, encrypted_segments_indices=None):
        self.debug_dir = debug_dir
        self.config = imageinfo
        self.validating = validating
        self.encryption_parameters = self._encryption_parameters_class(config=self.config,
                                                                       debug_dir=self.debug_dir,
                                                                       encrypted_segments_indices=encrypted_segments_indices)

    def get_encryptor(self, encryption_parameters_blob=None, key=None):
        encryption_parameters = self.encryption_parameters
        if encryption_parameters_blob:
            encryption_parameters = self._encryption_parameters_class(encryption_parameters_blob,
                                                                      key,
                                                                      self.debug_dir)
        return self._encryptor_class(encryption_parameters, debug_dir=self.debug_dir)

    def update_encryption_parameters(self, encryption_parameters_blob):
        self.encryption_parameters = self._encryption_parameters_class(encryption_params_blob=encryption_parameters_blob,
                                                                       l3_key_blob=self.encryption_parameters.get_image_encryption_key(),
                                                                       config=self.config,
                                                                       debug_dir=self.debug_dir)

    def get_decryptor(self, encryption_parameters_blob=None, key=None):
        # A blob was not provided so don't construct a new instance of EncryptionParameters
        if encryption_parameters_blob is None:
            return self._decryptor_class(self.encryption_parameters, debug_dir=self.debug_dir)
        # L3 key is provided in place of L1 or SSD key
        elif key is None:
            encryption_parameters = self._encryption_parameters_class(encryption_params_blob=encryption_parameters_blob,
                                                                      l3_key_blob=self.encryption_parameters.get_image_encryption_key(),
                                                                      config=self.config,
                                                                      debug_dir=self.debug_dir,
                                                                      validating=self.validating)
            return self._decryptor_class(encryption_parameters, debug_dir=self.debug_dir)
        # L1 or SSD key is provided
        else:
            encryption_parameters = self._encryption_parameters_class(encryption_params_blob=encryption_parameters_blob,
                                                                      private_key=key,
                                                                      config=self.config,
                                                                      debug_dir=self.debug_dir,
                                                                      validating=self.validating)
            return self._decryptor_class(encryption_parameters, debug_dir=self.debug_dir)

    def get_encryption_parameters_blob(self):
        return self.encryption_parameters.get_encryption_params_blob()

    @abstractproperty
    def _encryptor_class(self):
        raise NotImplementedError

    @abstractproperty
    def _decryptor_class(self):
        raise NotImplementedError

    @abstractproperty
    def _encryption_parameters_class(self):
        raise NotImplementedError

    @abstractmethod
    def get_dummy_key(self):
        raise NotImplementedError

    @abstractmethod
    def extract_encryption_parameters(self, data):
        raise NotImplementedError

    @classmethod
    def ident(cls):
        raise NotImplementedError

    @classmethod
    def register(cls):
        from sectools.features.isc.encryption_service import _register_encdec
        _register_encdec(cls.ident(), cls)

    @classmethod
    def get_segment_num_scheme(cls):
        raise NotImplementedError

    @classmethod
    def supports_decryption(cls):
        return True
