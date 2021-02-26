# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Parses the config files specific to fuseblower. Provides all the
functionalities from :mod:`..BaseDataParseGen`.
"""

import __secfile__
__secfile__.init()

import abc
import os

import defines

from sectools.features.fuse_common.parsegen import BaseDataParseGen
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import get_dups_in_list
from sectools.features.fuse_common.datamodel.auto_gen_model import Cfg_Fuseblower
from configfile import _BaseParseGen, _OEMParseGen, _QTIParseGen, _UIParseGen, _USERParseGen


class ConfigDir(object):
    """Processes a given config directory to discover the available fuseblower
    config files in the directory.

    :param str config_dir: Path to the directory containing the config files.
    """

    def __init__(self, config_dir):
        """Initializations and checks"""
        if not c_path.validate_dir(config_dir):
            raise RuntimeError('Directory doesnt exist: ' + config_dir)

        # First level of directory that is expected
        sub_config_dir = c_path.join(config_dir, defines.CONFIG_DIR_BASENAME)
        try:
            c_path.validate_dir(sub_config_dir)
        except Exception as e:
            raise RuntimeError('Directory ' + config_dir + '\n'
                               '    ' + 'must contain configs sub directory: ' + defines.CONFIG_DIR_BASENAME)

        self.config_dir = sub_config_dir

    def _get_config_paths(self, chipset_dir):
        """Returns a tuple of the configs found in the chipset dir matching
        the naming conventions. If any of the config files is not found
        in the dir, its value is returned as None.

        :param str chipset_dir: The directory in which to look for config paths.
        :returns: (oem_config_path, qti_config_path, ui_config_path, user_config_path)
        :rtype: (tuple(str))
        """
        oem, qti, ui, user = None, None, None, None
        chipset_from_dir_name = os.path.basename(chipset_dir)

        for entry in os.listdir(chipset_dir):
            path = c_path.join(chipset_dir, entry)
            if c_path.validate_file(path):
                # Extract the chipset from the file
                try:
                    chipset_from_file = ConfigParser.get_chipset_from_file(path)
                except Exception as e:
                    logger.debug2('Skipping file: ' + entry + '\n'
                                  '    ' + 'Failed to load the file: ' + str(e))
                    continue

                # Check the naming conventions
                if chipset_from_file == chipset_from_dir_name:
                    if entry.endswith(defines.XML_NAME_ENDING_OEM):
                        oem = path
                    elif entry.endswith(defines.XML_NAME_ENDING_QTI):
                        qti = path
                    elif entry.endswith(defines.XML_NAME_ENDING_UI):
                        ui = path
                    elif entry.endswith(defines.XML_NAME_ENDING_USER):
                        user = path
                    else:
                        logger.debug2('Skipping file: ' + entry + '\n'
                                      '    ' + 'Name does not match any of the naming convention patters')
                else:
                    logger.debug2('Skipping file: ' + entry + '\n'
                                  '    ' + 'Chipset from file: "' + chipset_from_file + '" does not match chipset from dir name: "' + chipset_from_dir_name + '"')

        logger.debug2('Config paths found for chipset_dir: ' + chipset_dir + '\n'
                      '    ' + 'oem: ' + str(oem) + '\n'
                      '    ' + 'qti: ' + str(qti) + '\n'
                      '    ' + 'ui: ' + str(ui) + '\n'
                      '    ' + 'user: ' + str(user))
        return oem, qti, ui, user

    @property
    def config_paths(self):
        """(list[tuple(str)]) List of the config paths found in the workspace
        conforming to the naming structure.
        """
        config_dir = self.config_dir
        config_paths = []

        logger.debug('Searching config path sets in dir: ' + config_dir)
        for entry in os.listdir(config_dir):
            path = c_path.join(config_dir, entry)
            if c_path.validate_dir(path):
                oem, qti, ui, user = self._get_config_paths(path)
                if oem or qti or ui or user:
                    config_paths.append((oem, qti, ui, user))
                else:
                    logger.debug2('Skipping dir: ' + entry + '\n'
                                  '    ' + 'Does not contain any configs')
            else:
                logger.debug2('Skipping file in first level: ' + entry)
        logger.debug('Config paths found from the config dir: ' + str(config_paths))
        return config_paths

    @property
    def configs(self):
        """(list[obj]) List of objects of :class:`ConfigParser` generated by
        loading the config files in the config_dir.
        """
        configs = []
        for oem, qti, ui, user in self.config_paths:
            logger.debug2('Loading configs: ' + '\n'
                          '    ' + 'oem: ' + str(oem) + '\n'
                          '    ' + 'qti: ' + str(qti) + '\n'
                          '    ' + 'ui: ' + str(ui) + '\n'
                          '    ' + 'user: ' + str(user))

            try:
                config_parser = ConfigParser()
                config_parser.read(oem, qti, ui, user)

                configs.append(config_parser)
            except Exception as e:
                logger.warning('Failed to load configs.'
                               '    ' + 'Error: ' + str(e))
        return configs

    @property
    def chipsets(self):
        """(list[str]) List of chipset names supported by the configs in the
        config dir.
        """
        chipsets = []
        for config in self.configs:
            chipsets.append(config.chipset)
        logger.debug('Chipsets found from the configs: ' + str(chipsets))
        return chipsets

    @config_paths.setter
    def config_paths(self, value):
        raise RuntimeError('Config_Paths in a config dir cannot be set.')

    @configs.setter
    def configs(self, value):
        raise RuntimeError('Configs in a config dir cannot be set.')

    @chipsets.setter
    def chipsets(self, value):
        raise RuntimeError('Chipsets in a config dir cannot be set.')

    def get_chipset_dir(self, chipset):
        """Returns the expected path within the config dir for the chipset

        :param str chipset: The chipset for which directory path is queried.
        """
        return c_path.join(self.config_dir, chipset)

    def create_chipset_dir(self, chipset):
        """Creates the directory in the config dir for the chipset

        :param str chipset: The chipset for which directory needs to be
            created
        :raises: RuntimeError - If directory creation fails
        """
        try:
            c_path.create_dir(self.get_chipset_dir(chipset))
        except Exception as e:
            raise RuntimeError('Failed to create directory for chipset: ' + chipset + '\n'
                               '    ' + 'Error: ' + str(e))

    def get_chipset_config(self, chipset):
        """Returns a tuple of the configs found for the chipset dir. If any of
        the config files is not found in the dir, its value is returned as None.

        :param str chipset_dir: Chipset to return config files for
        :returns: (oem_config_path, qti_config_path, ui_config_path, user_config_path)
        :rtype: (tuple(str))
        """
        logger.debug('Searching configs corresponding to platform: ' + chipset)
        chipset_dir = c_path.join(self.config_dir, chipset)

        if c_path.validate_dir(chipset_dir):
            return self._get_cpaths(chipset_dir)
        raise RuntimeError('Did not find config for platform: "' + chipset + '"')

    def _get_cpaths(self, chipset_dir):
        """Returns a tuple of the configs found in the chipset dir matching
        the naming conventions. If any of the config files is not found
        in the dir, its value is returned as None.

        :param str chipset_dir: The directory in which to look for config paths.
        :returns: (oem_config_path, qti_config_path, ui_config_path, user_config_path)
        :rtype: (tuple(str))
        """
        oem, qti, ui, user = None, None, None, None
        chipset_from_dir_name = os.path.basename(chipset_dir)

        for entry in os.listdir(chipset_dir):
            path = c_path.join(chipset_dir, entry)
            if c_path.validate_file(path):
                # Extract the chipset from the file
                try:
                    chipset_from_file = ConfigParser.get_chipset_from_file(path)
                except Exception as e:
                    logger.debug2('Skipping file: ' + entry + '\n'
                                  '    ' + 'Failed to load the file: ' + str(e))
                    continue

                # Check the naming conventions
                if chipset_from_file == chipset_from_dir_name:
                    if entry.endswith(defines.XML_NAME_ENDING_OEM):
                        oem = path
                    elif entry.endswith(defines.XML_NAME_ENDING_USER):
                        user = path
                    else:
                        logger.debug2('Skipping file: ' + entry + '\n'
                                      '    ' + 'Name does not match any of the naming convention patters')
                else:
                    logger.debug2('Skipping file: ' + entry + '\n'
                                  '    ' + 'Chipset from file: "' + chipset_from_file + '" does not match chipset from dir name: "' + chipset_from_dir_name + '"')

        logger.debug2('Config paths found for chipset_dir: ' + chipset_dir + '\n'
                      '    ' + 'keymap: ' + str(oem) + '\n'
                      '    ' + 'user: ' + str(user))
        return oem, qti, ui, user

    def get_chipset_config_paths(self, chipset):
        """Returns a tuple of the configs found for the chipset dir. If any of
        the config files is not found in the dir, its value is returned as None.

        :param str chipset_dir: Chipset to return config files for
        :returns: (oem_config_path, qti_config_path, ui_config_path, user_config_path)
        :rtype: (tuple(str))
        """
        logger.debug('Searching configs corresponding to chipset: ' + chipset)
        chipset_dir = c_path.join(self.config_dir, chipset)

        if c_path.validate_dir(chipset_dir):
            return self._get_config_paths(chipset_dir)
        raise RuntimeError('Did not find config for chipset: "' + chipset + '"')

    def has_configs(self, chipset):
        """Returns booleans stating if the config files exist for the chipset
        in the config dir"""
        try:
            oem, qti, ui, user = self.get_chipset_config_paths(chipset)
        except Exception:
            oem, qti, ui, user = None, None, None, None

        has_oem = True if oem else False
        has_qti = True if qti else False
        has_ui = True if ui else False
        has_user = True if user else False
        return (has_oem, has_qti, has_ui, has_user)


class WorkspaceDir(ConfigDir):

    def __init__(self, workspace_path):
        """Initializations and checks"""
        # Create the workspace dir
        try:
            c_path.create_dir(workspace_path)
        except Exception as e:
            raise RuntimeError('No write access to workspace dir: ' + workspace_path + '\n'
                               '    ' + 'Error: ' + e)

        # Create the config folder
        config_dir = c_path.join(workspace_path, defines.CONFIG_DIR_BASENAME)
        try:
            c_path.create_dir(config_dir)
        except Exception as e:
            raise RuntimeError('No write access to config dir: ' + config_dir + '\n'
                               '    ' + 'Error: ' + e)

        # Pass the workspace dir to ConfigDir obj
        ConfigDir.__init__(self, workspace_path)

class ConfigParser(BaseDataParseGen):
    """Parses the FuseBlower configuration files. Allows the user to set the
    path to the config files. All config files should be for the same chipset.
    """

    def __init__(self):
        """Initializations and checks"""
        BaseDataParseGen.__init__(self)
        self._oem_config = None
        self._qti_config = None
        self._ui_config = None
        self._user_config = None

    def reset(self):
        """Resets the config parser by detaching from any config files.
        """
        self._oem_config = None
        self._qti_config = None
        self._ui_config = None
        self._user_config = None

    @property
    def oem_config_path(self):
        """(str) Path to the oem config file. 'None' when path is not set.
        Cannot be set directly. Use :meth:`read` for setting paths.
        """
        return None if self._oem_config is None else self._oem_config.config_path

    @oem_config_path.setter
    def oem_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def qti_config_path(self):
        """(str) Path to the qti config file. 'None' when path is not set.
        Cannot be set directly. Use :meth:`read` for setting paths.
        """
        return None if self._qti_config is None else self._qti_config.config_path

    @qti_config_path.setter
    def qti_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def ui_config_path(self):
        """(str) Path to the ui config file. 'None' when path is not set.
        Cannot be set directly. Use :meth:`read` for setting paths.
        """
        return None if self._ui_config is None else self._ui_config.config_path

    @ui_config_path.setter
    def ui_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def user_config_path(self):
        """(str) Path to the user config file. 'None' when path is not set.
        Cannot be set directly. Use :meth:`read` for setting paths.
        """
        return None if self._user_config is None else self._user_config.config_path

    @user_config_path.setter
    def user_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def chipset(self):
        """(str) Chipset that the parser is currently configured to. 'None' if
        no config files have been provided.
        """
        for config in [self._oem_config, self._qti_config, self._ui_config, self._user_config]:
            if config is not None:
                config_chipset = config.chipset
                if config_chipset is not None:
                    return config_chipset
        return None

    def update(self):
        """(str) Updates the data model from the config files. Also checks
        that all config files are for the same chipset.
        """
        chipset = None
        self.data_model.reset()
        for config in [self._ui_config, self._qti_config, self._oem_config, self._user_config]:
            if config is not None:
                # Check that the chipset matches
                config_chipset = config.chipset
                if config_chipset is not None:
                    if chipset is not None:
                        if config_chipset != chipset:
                            raise RuntimeError('Config file chipset "' + config.chipset +
                                               '" does not match the chipset "' + chipset + '" that parser is configured with.')
                    else:
                        chipset = config_chipset

                # Update the data model using the config
                self.data_model = config.update_data_model(self.data_model)

    def read(self, oem=None, qti=None, ui=None, user=None):
        # Read the files
        self.reset()
        self._oem_config = None if oem is None else _OEMParseGen(oem)
        self._qti_config = None if qti is None else _QTIParseGen(qti)
        self._ui_config = None if ui is None else _UIParseGen(ui)
        self._user_config = None if user is None else _USERParseGen(user)
        self.update()
        return self.data_model

    def write(self, dm=None, oem=None, qti=None, ui=None, user=None):
        # Check the paths
        if dm is None:
            dm = self.data_model
        if oem is None:
            oem = self.oem_config_path
        if qti is None:
            qti = self.qti_config_path
        if ui is None:
            ui = self.ui_config_path
        if user is None:
            user = self.user_config_path

        # Generate the files
        for config, config_path in [(self._ui_config, ui),
                                    (self._qti_config, qti),
                                    (self._oem_config, oem),
                                    (self._user_config, user)]:
            if config is not None:
                config.update_from_data_model(dm)
                config.generate(config_path)

        # Update self
        self.read(oem, qti, ui, user)

    @classmethod
    def get_chipset_from_file(cls, file_path):
        return _BaseParseGen(file_path).chipset

    def get_secdat_file_info(self):
        return self._user_config.root.secdat.file_info

    def get_secdat_file_version(self):
        return self._user_config.root.secdat.file_version

    def get_secdat_fuse_version(self):
        return self._user_config.root.secdat.fuse_version



#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
__all__ = ['ConfigDir',
           'ConfigParser',
           'defines']
