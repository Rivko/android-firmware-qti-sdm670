# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta, abstractmethod

class BaseEncryptionParameters(object):
    '''
    '''
    __metaclass__=ABCMeta

    def __init__(self, encryption_params_blob=None, key=None, config=None, debug_dir=None, validating=False):
        self.encryption_params_blob = encryption_params_blob
        self.key = key
        self.debug_dir = debug_dir
        self.validating = validating

    def get_key(self):
        return self.key

    def get_encryption_params_blob(self):
        return self.encryption_params_blob

    @abstractmethod
    def get_image_encryption_key(self):
        raise NotImplementedError

    @abstractmethod
    def get_image_encryption_iv(self):
        raise NotImplementedError
