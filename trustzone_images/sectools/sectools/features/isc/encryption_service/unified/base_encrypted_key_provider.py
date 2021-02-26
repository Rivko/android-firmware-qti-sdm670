# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created Nov 20th, 2015 by coryf
"""
from abc import abstractmethod

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.features.isc.encryption_service.unified.encrypted_key_provider_mgr import encrypted_key_provider_mgr


class BaseEncryptedKeyProvider(SecPluginIntf_Abs):
    """
    This interface provides methods to get encrypted l2 keys, encrypted l3 keys, and plain l3 keys
    for use in UIE. The abstraction allows for any subclass to implement
    its own mechanism for retrieval of the encrypted keys.
    """

    @classmethod
    def is_plugin(cls):
        return False

    @classmethod
    def get_plugin_manager(cls):
        return encrypted_key_provider_mgr

    @abstractmethod
    def get_encrypted_l2_key(self):
        raise NotImplementedError

    @abstractmethod
    def get_encrypted_l3_key(self):
        raise NotImplementedError

    @abstractmethod
    def get_clear_l3_key(self):
        raise NotImplementedError

    @classmethod
    def is_qti(cls):
        return False
