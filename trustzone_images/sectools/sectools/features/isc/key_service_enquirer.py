# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from six import ensure_str

from sectools.common.utils import c_path
from sectools.features.isc.data_provision_enquirer import DataProvEnquirer
from sectools.features.isc.encryption_service.unified.encrypted_key_provider_mgr import \
    encrypted_key_provider_id_supported

ENCRYPTION_STR = "Encryption"
UNIFIED_STR = "Unified"
L1_KEY_TAG = 'l1_file_name'
L2_KEY_TAG = 'l2_file_name'
L3_KEY_TAG = 'l3_file_name'
L2_ENC_KEY_TAG = 'l2_enc_file_name'
L3_ENC_KEY_TAG = 'l3_enc_file_name'
L1_PUBLIC_KEY_TAG = 'l1_public_file_name'


class KeyService(object):

    def __init__(self, config=None, base_path=None, chipset=None, chipset_override=None):
        """config file value will override the rest"""
        self.UIE_key = None
        if config is not None:
            base_path = config.config.data_provisioning.base_path
            chipset = config.chipset
            self.UIE_key = config.signing_attributes.UIE_key

        if base_path is not None and (chipset is not None or self.UIE_key is not None):
            if self.UIE_key is not None:
                self.chipset = self.UIE_key
            else:
                self.chipset = chipset
        else:
            raise RuntimeError("Required KeyService parameters (base path and chipset) were not provided.")

        self.data_prov_enquirer = DataProvEnquirer(base_path)
        self.data_prov_base_path = base_path

        # get all the keys
        self.l1_key = self._get_key(L1_KEY_TAG)
        self.l2_key = self._get_key(L2_KEY_TAG)
        self.l3_key = self._get_key(L3_KEY_TAG)
        self.encrypted_l2_key = self._get_key(L2_ENC_KEY_TAG)
        self.encrypted_l3_key = self._get_key(L3_ENC_KEY_TAG)

    def using_encrypted_key_provider(self):
        return encrypted_key_provider_id_supported(self.UIE_key)

    def get_encrypted_key_provider_id(self):
        return self.UIE_key

    def _get_key(self, tag):
        try:
            if self.using_encrypted_key_provider():
                return None
            key = self.data_prov_enquirer.query_data_prov(ENCRYPTION_STR, UNIFIED_STR, ensure_str(self.chipset), tag)
        except RuntimeError:
            key = None
        except KeyError as e:
            raise RuntimeError("Encryption key folder " + "\"" +
                               c_path.join(self.data_prov_base_path, ENCRYPTION_STR, UNIFIED_STR, ensure_str(self.chipset)) +
                               "\"" + " was not found.\n"
                               "Ensure that the specified key folder is placed at this location.")
        return key

    def get_l1_key(self):
        return self.l1_key

    def get_l2_key(self):
        return self.l2_key

    def get_l3_key(self):
        return self.l3_key

    def get_encrypted_l2_key(self):
        return self.encrypted_l2_key

    def get_encrypted_l3_key(self):
        return self.encrypted_l3_key

    def all_clear_keys_provided(self):
        return None not in (self.l1_key, self.l2_key, self.l3_key)

    def no_clear_keys_provided(self):
        return (None, None, None) == (self.l1_key, self.l2_key, self.l3_key)

    def all_enc_keys_provided(self):
        return None not in (self.encrypted_l2_key, self.encrypted_l3_key)

    def no_enc_keys_provided(self):
        return (None, None) == (self.encrypted_l2_key, self.encrypted_l3_key)

    @staticmethod
    def get_new_random_clear_key():
        return os.urandom(16)


class AsymmetricKeyService(KeyService):

    def __init__(self, config=None, base_path=None, chipset=None, chipset_override=None):
        KeyService.__init__(self, config, base_path, chipset, chipset_override)
        self.l1_public_key = self._get_key(L1_PUBLIC_KEY_TAG)

    def get_l1_public_key(self):
        return self.l1_public_key
