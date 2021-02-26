# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta, abstractmethod


class BaseDecryptor(object):

    __metaclass__=ABCMeta

    def __init__(self, encryption_parameters, debug_dir=None):
        self.encryption_parameters = encryption_parameters
        self.debug_dir = debug_dir

    @abstractmethod
    def decrypt_segment(self, encrypted_binary_segment, segment_num):
        raise NotImplementedError()
