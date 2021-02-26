#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

'''
Created on Aug 7, 2014

@author: scarmich
'''

import os
from sectools.common.data_provisioning.data_prov import DataProvisioner
from sectools.common.utils.c_logging import logger
from sectools.common.utils import c_path

ASSETS_STR = 'General_Assets'
SIGNING_STR = 'Signing'
ATEST_CA_XTS_TAG = 'attestation_certificate_extensions'
CA_CERT_XTS_TAG = 'ca_certificate_extensions'
OPENSSL_CONFIG_TAG = 'openssl_configfile'
OPENSSL_TARGET = "openssl"

class SingletonClass(type):
    _singleton_instance = {}
    def __call__(self, *args, **kwargs):
        if self not in self._singleton_instance:
            self._singleton_instance[self] = super(SingletonClass, self).__call__(*args, **kwargs)
        return self._singleton_instance[self]

class OpenSSL_DataService(object):
    __metaclass__ = SingletonClass

    def log_once(self, msg):
        if msg not in self.message_print_set:
            logger.info(msg)
            self.message_print_set.add(msg)

    def __init__(self, config=None, base_path=None, target=None):
        '''config file value will override the rest'''
        if config is not None:
            self.base_path = config.data_provisioning.base_path
            self.target = target if target is not None else OPENSSL_TARGET
        else:
            raise RuntimeError("Config file is not provided.")

        self.data_prov = DataProvisioner(self.base_path)
        self.message_print_set = set()

    def query_data_prov(self, level1_namespace, level2_namespace, level3_namespace, asset_tag):
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

        return asset_file_path

    def get_attest_ca_xts(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, ATEST_CA_XTS_TAG)

    def get_ca_cert_xts(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, CA_CERT_XTS_TAG)

    def get_openssl_config(self):
        return self.query_data_prov(ASSETS_STR, SIGNING_STR, self.target, OPENSSL_CONFIG_TAG)


class OpenSSLPathsObject(object):

    def __init__(self, config):
        openssl_temp = OpenSSL_DataService(config)
        self.attest_ca_xts = openssl_temp.get_attest_ca_xts()
        self.ca_cert_xts = openssl_temp.get_ca_cert_xts()
        self.openssl_config = openssl_temp.get_openssl_config()
