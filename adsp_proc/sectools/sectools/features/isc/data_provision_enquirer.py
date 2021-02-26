# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.data_provisioning.data_prov import DataProvisioner
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger

ENCRYPTION_STR = "Encryption"
UNIFIED_STR = "Unified"
L1_KEY_TAG = 'l1_file_name'
L2_KEY_TAG = 'l2_file_name'
L3_KEY_TAG = 'l3_file_name'
L2_ENC_KEY_TAG = 'l2_enc_file_name'
L3_ENC_KEY_TAG = 'l3_enc_file_name'

ASSETS_STR = 'General_Assets'
SIGNING_STR = 'Signing'
ATEST_CA_XTS_TAG = 'attestation_certificate_extensions'
CA_CERT_XTS_TAG = 'ca_certificate_extensions'
OPENSSL_CONFIG_TAG = 'openssl_configfile'
OPENSSL_TARGET = "openssl"


class SingletonClass(type):
    _singleton_instance = {}

    @classmethod
    def reset(cls):
        """
        Deletes the current instance in order to force a newly constructed instance to be returned the next time the constructor is called.
        """
        cls._singleton_instance = {}

    def __call__(self, *args, **kwargs):
        if self not in self._singleton_instance:
            self._singleton_instance[self] = super(SingletonClass, self).__call__(*args, **kwargs)
        return self._singleton_instance[self]


class DataProvEnquirer(object):
    __metaclass__ = SingletonClass

    def log_once(self, msg):
        if msg not in self.message_print_set:
            logger.info(msg)
            self.message_print_set.add(msg)

    def __init__(self, base_path=None):
        self.data_prov = DataProvisioner(base_path)
        self.message_print_set = set()

    def query_data_prov(self, level1_namespace, level2_namespace, level3_namespace, asset_tag, return_file_path=False):
        query_result = self.data_prov.query(level1_namespace, level2_namespace, level3_namespace)[0]
        '''check if config.xml exists'''
        if not query_result.config:
            raise RuntimeError('DataProvisioner: ' + c_path.join(query_result.path, 'config.xml') + ' is not found')
        '''check if config.xml is valid'''
        try:
            asset_file_name = str(getattr(query_result.config.METACONFIG, asset_tag))
        except Exception:
            raise RuntimeError('DataProvisioner: ' + asset_tag + ' is not found in config.xml')

        '''check if asset file exists'''
        asset_file_path = os.path.join(query_result.path, asset_file_name)
        if not c_path.validate_file(asset_file_path):
            raise RuntimeError('DataProvisioner: ' + asset_file_path + ' is not found')

        self.log_once(asset_tag + " = " + asset_file_path)
        logger.debug("DataProvisioner config: " + str(query_result.config))

        if return_file_path:
            return asset_file_path
        else:
            return query_result.files[asset_file_name]


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
        from sectools.features.isc.encryption_service.unified import encrypted_key_provider_id_supported
        return encrypted_key_provider_id_supported(self.UIE_key)

    def get_encrypted_key_provider_id(self):
        return self.UIE_key

    def _get_key(self, tag):
        try:
            if self.using_encrypted_key_provider():
                return None
            key = self.data_prov_enquirer.query_data_prov(ENCRYPTION_STR, UNIFIED_STR, self.chipset, tag)
        except RuntimeError:
            key = None
        except KeyError:
            raise RuntimeError("Encryption key folder " + "\"" + c_path.join(self.data_prov_base_path, ENCRYPTION_STR, UNIFIED_STR, self.chipset) + "\"" + " was not found.\n"
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


class OpenSSLDataService(DataProvEnquirer):

    def __init__(self, config=None, base_path=None, target=None):
        '''config file value will override the rest'''
        if config is not None:
            base_path = config.data_provisioning.base_path
            self.target = target if target is not None else OPENSSL_TARGET
        else:
            raise RuntimeError("Config file is not provided.")

        DataProvEnquirer.__init__(self, base_path)

    def get_attest_ca_xts(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, ATEST_CA_XTS_TAG, True)

    def get_ca_cert_xts(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, CA_CERT_XTS_TAG, True)

    def get_openssl_config(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, OPENSSL_CONFIG_TAG, True)


class OpenSSLPathsObject(object):

    def __init__(self, config):
        openssl_temp = OpenSSLDataService(config)
        self.attest_ca_xts = openssl_temp.get_attest_ca_xts()
        self.ca_cert_xts = openssl_temp.get_ca_cert_xts()
        self.openssl_config = openssl_temp.get_openssl_config()
