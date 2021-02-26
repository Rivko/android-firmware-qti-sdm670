#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os
from sectools.common.data_provisioning.data_prov import DataProvisioner
from sectools.common.utils.c_logging import logger
from sectools.common.utils import c_path

ENCRYPTION_STR = "Encryption"
UNIFIED_STR = "Unified"
L1_KEY_TAG = 'l1_file_name'
L2_KEY_TAG = 'l2_file_name'
L3_KEY_TAG = 'l3_file_name'

class SingletonClass(type):
    _singleton_instance = {}
    def __call__(self, *args, **kwargs):
        if self not in self._singleton_instance:
            self._singleton_instance[self] = super(SingletonClass, self).__call__(*args, **kwargs)
        return self._singleton_instance[self]

class KeyService(object):
    __metaclass__ = SingletonClass

    def log_once(self, msg):
        if msg not in self.message_print_set:
            logger.info(msg)
            self.message_print_set.add(msg)

    def __init__(self, config=None, base_path=None, chipset=None):
        '''config file value will override the rest'''
        if config is not None:
            base_path = config.config.data_provisioning.base_path
            chipset = config.chipset

        if base_path is not None and chipset is not None:
            self.base_path = base_path
            self.chipset = chipset
        else:
            raise RuntimeError("Required parameters are not provided. Base path and chipset.")

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

        return query_result.files[asset_file_name]

    def get_l1_key(self):
        return self.query_data_prov(ENCRYPTION_STR, UNIFIED_STR, self.chipset, L1_KEY_TAG)

    def get_l2_key(self):
        return self.query_data_prov(ENCRYPTION_STR, UNIFIED_STR, self.chipset, L2_KEY_TAG)

    def get_l3_key(self):
        return self.query_data_prov(ENCRYPTION_STR, UNIFIED_STR, self.chipset, L3_KEY_TAG)
