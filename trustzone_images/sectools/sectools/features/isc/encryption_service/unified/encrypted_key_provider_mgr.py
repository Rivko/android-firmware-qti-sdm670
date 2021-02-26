# ===============================================================================
#
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.core.plugin import SecPluginMgr


class EncryptedKeyProviderMgr(SecPluginMgr):

    def get_key_provider(self, key_provider_id):
        key_providers = self.get_map()
        if key_provider_id not in key_providers:
            raise RuntimeError(
                "There is no encrypted key provider with an id matching the selected key provider id: {}.\n"
                "Available key provider ids are: {}".format(key_provider_id, ", ".join(key_providers.keys())))
        return key_providers[key_provider_id]


encrypted_key_provider_mgr = EncryptedKeyProviderMgr()


def get_encrypted_key_provider(key_provider_id):
    return encrypted_key_provider_mgr.get_key_provider(key_provider_id)


def encrypted_key_provider_id_supported(key_provider_id):
    return key_provider_id is not None and key_provider_id in encrypted_key_provider_mgr.get_map()
