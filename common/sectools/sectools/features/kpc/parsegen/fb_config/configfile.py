# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Mar 9, 2014

@author: hraghav
'''

import abc
import copy
import os

import defines

from sectools.features.kpc.parsegen.fb_config.rules import _RULES
from sectools.features.fuse_common.parsegen.fb_config.defines import ENTRIES_STRUCTURE, OBJ_STRUCTURE

from sectools.common.utils import datautils
from sectools.common.utils.c_config import CoreConfig
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.datamodel import auto_gen_model, defines as dm_defines


class _BaseParseGen(CoreConfig):

    __metaclass__ = abc.ABCMeta

    __version__ = '1.0'

    def __init__(self, config_path):
        import auto_gen_fb_config
        self.config_module = auto_gen_fb_config
        CoreConfig.__init__(self, auto_gen_fb_config, config_path)
        self.validate()

    @property
    def chipset(self):
        """(str) Name of the chipset.
        """
        return self.root.metadata.chipset

    @property
    def version(self):
        """(str) Version of the config file.
        """
        return self.root.metadata.version

    @property
    def config_data(self):
        """(str) Config object
        """
        return self.root

    def validate(self):
        """Normalizes and validates the data in the xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        # Check that the version matches
        config_version = self.version
        if config_version != self.__version__:
            raise RuntimeError('Version of parser: "' + self.__version__ + '" does not match version from config "' + config_version + '"')

    def generate(self, file_path=None):
        """Generates the config file with the current configuration of the root
        node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.note('Generating config file...')
        CoreConfig.generate(self, file_path,
                            defines.XML_ROOTNODE,
                            defines.XML_NAMESPACE,
                            defines.XML_PREPEND_LINES)
        logger.note('Generated config file at: ' + file_path)

    def update_data_model(self, data_model):
        return CoreConfig.transfer(self, self.root, data_model,
                                   dm_defines.OBJ_STRUCTURE,
                                   dm_defines.ROOTNODE_NAME,
                                   auto_gen_model,
                                   dm_defines.OBJ_CLASSNAME_GEN)

    def update_from_data_model(self, data_model):
        return CoreConfig.transfer(self, data_model, self.root,
                                   dm_defines.OBJ_STRUCTURE,
                                   dm_defines.ROOTNODE_NAME,
                                   self.config_module,
                                   defines.XML_CLASSNAME_GEN)


class _OEMParseGen(_BaseParseGen):

    def validate(self):
        """Normalizes and validates the data in the xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        _BaseParseGen.validate(self)
        CoreConfig.validate(self, OBJ_STRUCTURE)

    def generate(self, file_path):
        """Generates the config file with the current configuration of the root
        node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.note('Generating config file...')
        CoreConfig.generate(self, file_path,
                            defines.XML_ROOTNODE,
                            defines.XML_NAMESPACE,
                            defines.XML_PREPEND_LINES)
        logger.note('Generated config file at: ' + file_path)

    def update_data_model(self, data_model):
        retval = _BaseParseGen.update_data_model(self, None)
        retval.set_fuses_owner(dm_defines.OWNER_OEM)
        return data_model + retval

    def update_from_data_model(self, data_model):
        data_model = auto_gen_model.filter_by_owner(data_model, dm_defines.OWNER_OEM)
        self.root = _BaseParseGen.update_from_data_model(self, data_model)


class _QTIParseGen(_BaseParseGen):

    def validate(self):
        """Normalizes and validates the data in the xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        _BaseParseGen.validate(self)
        CoreConfig.validate(self, OBJ_STRUCTURE)

    def generate(self, file_path):
        """Generates the config file with the current configuration of the root
        node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.note('Generating config file...')
        CoreConfig.generate(self, file_path,
                            defines.XML_ROOTNODE,
                            defines.XML_NAMESPACE,
                            defines.XML_PREPEND_LINES)
        logger.note('Generated config file at: ' + file_path)

    def update_data_model(self, data_model):
        retval = _BaseParseGen.update_data_model(self, None)
        retval.set_fuses_owner(dm_defines.OWNER_QTI)
        return data_model + retval

    def update_from_data_model(self, data_model):
        data_model = auto_gen_model.filter_by_owner(data_model, dm_defines.OWNER_QTI)
        self.root = _BaseParseGen.update_from_data_model(self, data_model)


class _UIParseGen(_BaseParseGen):

    def validate(self):
        """Normalizes and validates the data in the xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        _BaseParseGen.validate(self)
        CoreConfig.validate(self, OBJ_STRUCTURE)

    def generate(self, file_path):
        """Generates the config file with the current configuration of the root
        node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.note('Generating config file...')
        CoreConfig.generate(self, file_path,
                            defines.XML_ROOTNODE,
                            defines.XML_NAMESPACE,
                            defines.XML_PREPEND_LINES)
        logger.note('Generated config file at: ' + file_path)

    def update_data_model(self, data_model):
        retval = _BaseParseGen.update_data_model(self, None)
        return data_model + retval

    def update_from_data_model(self, data_model):
        data_model = copy.deepcopy(data_model)
        data_model.set_fuses_owner(None)
        data_model.set_fuses_values('0x0000000000000000')
        self.root = _BaseParseGen.update_from_data_model(self, data_model)


class _USERParseGen(_BaseParseGen):

    def __init__(self, config_path):
        import auto_gen_fb_user_config
        self.config_module = auto_gen_fb_user_config
        CoreConfig.__init__(self, auto_gen_fb_user_config, config_path)
        self.validate()

    def validate(self):
        """Normalizes and validates the data in the xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        _BaseParseGen.validate(self)
        CoreConfig.validate(self, ENTRIES_STRUCTURE)

    def generate(self, file_path):
        """Generates the config file with the current configuration of the root
        node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.note('Generating config file...')
        CoreConfig.generate(self, file_path,
                            defines.XML_ROOTNODE,
                            defines.XML_NAMESPACE_USER,
                            defines.XML_PREPEND_LINES)
        logger.note('Generated config file at: ' + file_path)

    def update_data_model(self, data_model):
        entries = []
        for entry in self.root.module.entry:
            entries.append({'name': entry.name, 'value': entry.value, 'ignore': entry.ignore, 'description': entry.description})

        for entry in entries:
            if entry['ignore'] == False:
                if entry['name'] == 'L1_encryption_file':
                    dt = datautils.DataHandler(datautils.get_cb_dict(path_basepath=os.path.dirname(self.config_path)))
                    entry['value'] = dt.validate(entry['value'], datautils.path_handler.TypePath())
                elif entry['name'] == 'oem_image_encr_key_file':
                    dt = datautils.DataHandler(datautils.get_cb_dict(path_basepath=os.path.dirname(self.config_path)))
                    entry['value'] = dt.validate(entry['value'], datautils.path_handler.TypePath())
                elif entry['name'] == 'ROT_encryption_file':
                    dt = datautils.DataHandler(datautils.get_cb_dict(path_basepath=os.path.dirname(self.config_path)))
                    entry['value'] = dt.validate(entry['value'], datautils.path_handler.TypePath())

        data_model_filtered = auto_gen_model.filter_by_owner(data_model, dm_defines.OWNER_OEM)
        rules = _RULES(self.chipset)
        rules.process(entries, data_model_filtered)

        return (data_model + data_model_filtered)

    def update_from_data_model(self, data_model):
        rules = _RULES()
        rules.update_from_data_model(data_model, self.root.module.entry)
