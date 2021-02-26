# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Parses the config file specific to debugpolicy. Provides all the
functionalities from :mod:`sectools.common.utils.c_config`. Once the config is parsed,
a tree representation of the object is created.
"""

import os

import defines as defines
from sectools.common.utils import c_config
from sectools.common.utils.c_logging import logger


class ConfigParser(c_config.CoreConfig):
    """Parses the debugpolicy configuration file using c_config.CoreConfig and
    the module auto-generated using generateDS (auto_gen_xml_config).

    :param str config_path: Path to the config file to be parsed.

    .. data: root

        Root class representing the structure of the config file.

    .. data:: __version__ = 1.0

        Version of the Config Parser

    """

    __version__ = '1.0'

    def __init__(self, config_path):
        """Initializations and checks"""
        import auto_gen_xml_config, auto_gen_obj_config

        assert isinstance(config_path, str)
        self.config_module = auto_gen_xml_config
        self.obj_module = auto_gen_obj_config
        c_config.CoreConfig.__init__(self, self.config_module, config_path)
        self.validate()
        self.normalize()

    def normalize(self):
        """Normalizes the data in the debugpolicy xml."""

        flag_value = 0
        maskbit_one = 1

        for i in range(len(self.root.file_properties.flags.flag)):
            if self.root.file_properties.flags.flag[i].value == 1:
                flag_value = flag_value | (maskbit_one<<self.root.file_properties.flags.flag[i].bit_pos)
            elif self.root.file_properties.flags.flag[i].value == 0:
                flag_value = flag_value & ~(maskbit_one<<self.root.file_properties.flags.flag[i].bit_pos)

        self.root.file_properties.flags = flag_value

        flag_value = 0
        maskbit_one = 1

        if self.root.file_properties.image_bit_map is not None:
            for i in range(len(self.root.file_properties.image_bit_map.swid)):
                if self.root.file_properties.image_bit_map.swid[i].value == 1:
                    flag_value = flag_value | (maskbit_one<<self.root.file_properties.image_bit_map.swid[i].bit_pos)
                elif self.root.file_properties.image_bit_map.swid[i].value == 0:
                    flag_value = flag_value & ~(maskbit_one<<self.root.file_properties.image_bit_map.swid[i].bit_pos)
            self.root.file_properties.image_bit_map = flag_value

        if self.root.file_properties.image_id_list is not None:
            int_list = [int(i, 16) for i in self.root.file_properties.image_id_list.image_id]
            self.root.file_properties.image_id_list.image_id = int_list

        if self.root.file_properties.serial_num_list is not None:
            int_list = [int(i, 16) for i in self.root.file_properties.serial_num_list.serial_num]
            self.root.file_properties.serial_num_list.serial_num = int_list

        self.root.file_properties.elf.elf_class = defines.ELF_CLASS[self.root.file_properties.elf.elf_class]
        self.root.file_properties.elf.phys_addr = int(self.root.file_properties.elf.phys_addr, 16)

    def validate(self):
        """Validates the data in the debugpolicy xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        c_config.CoreConfig.validate(self, defines.CONFIG_STRUCTURE)

        # Check that the version matches
        config_version = self.version
        if config_version != self.__version__:
            raise RuntimeError('Version of parser: "' + self.__version__ + '" does not match version from config "' + config_version + '"')

    def generate(self, file_path):
        """Generates the debugpolicy config file with the current configuration
        of the root node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.debug('Generating config file...')
        c_config.CoreConfig.generate(self, file_path,
                                     defines.XML_ROOTNODE,
                                     defines.XML_NAMESPACE,
                                     defines.XML_PREPEND_LINES)
        logger.debug('Generated config file at: ' + str(file_path))

    def transfer_from_obj(self, obj):
        """Updates the values of the config root node using the attributes of
        the object provided.

        :param Cfg_Debugpolicy obj: Object to be used for obtaining the values.
        """
        assert isinstance(obj, self.obj_module.Cfg_Debugpolicy)
        logger.debug('Updating config contents using object: ' + obj)
        self.root = self.transfer_from_obj_to_node(obj,
                                                   self.root,
                                                   defines.CONFIG_STRUCTURE,
                                                   defines.ROOTNODE_NAME)
        logger.debug('Config contents updated.')

    def transfer_to_obj(self, obj):
        """Updates the attributes of the object provided using the values of the
        root config node.

        :params: same as :meth:`transfer_from_obj`
        """
        assert isinstance(obj, self.obj_module.Cfg_Debugpolicy)
        logger.debug('Updating object using config contents: ' + obj)
        obj = self.transfer_from_node_to_obj(self.root,
                                             obj,
                                             defines.CONFIG_STRUCTURE,
                                             defines.ROOTNODE_NAME,)
        logger.debug('Object updated using the config.')

    def transfer_from_obj_to_node(self, obj, node, structure, structure_parent):
        return self.transfer(obj,
                             node,
                             structure,
                             structure_parent,
                             self.config_module,
                             defines.XML_CLASSNAME_GEN)

    def transfer_from_node_to_obj(self, node, obj, structure, structure_parent):
        return self.transfer(node,
                             obj,
                             structure,
                             structure_parent,
                             self.obj_module,
                             defines.OBJ_CLASSNAME_GEN)

    @property
    def chipset(self):
        """
        :returns: Name of the chipset.
        :rtype: str
        """
        return self.root.metadata.chipset

    @property
    def version(self):
        """
        :returns: Version of the config file.
        :rtype: str
        """
        return self.root.metadata.version

    @property
    def config_data(self):
        """
        :returns: Config object
        :rtype: obj
        """
        return self.root


def get_chipset_mode_config(chipset):
    """Returns a tuple of the configs found for the chipset dir. If any of
    the config files is not found in the dir, its value is returned as None.

    :param str chipset_dir: Chipset to return config files for
    :returns: (oem_config_path, qti_config_path, ui_config_path, user_config_path)
    :rtype: (tuple(str))
    """
    logger.debug('Searching configs corresponding to platform: ' + chipset)
    chipset_dir = defines.c_path.join(defines.DEFAULT_CONFIG_DIR, chipset)

    if defines.c_path.validate_dir(chipset_dir):
        return _get_cpaths(chipset_dir)
    raise RuntimeError('Did not find config for platform: "' + chipset + '"')

def _get_cpaths(chipset_dir):
    """Returns a tuple of the configs found in the chipset dir matching
    the naming conventions. If any of the config files is not found
    in the dir, its value is returned as None.

    :param str chipset_dir: The directory in which to look for config paths.
    :returns: (dbgp_config_path, secimage_config_path)
    :rtype: (tuple(str))
    """
    dpconfig, dp_siconfig = None, None
    chipset_from_dir_name = os.path.basename(chipset_dir)

    for entry in os.listdir(chipset_dir):
        path = defines.c_path.join(chipset_dir, entry)
        if defines.c_path.validate_file(path):
            # Extract the chipset from the file
            try:
                chipset_from_file = ConfigParser(path).chipset
            except Exception as e:
                logger.debug2('Skipping file: ' + entry + '\n'
                                '    ' + 'Failed to load the file: ' + str(e))
                if entry.endswith(defines.XML_NAME_END_DP_SI):
                    dp_siconfig = path
                    continue
                elif entry.endswith(defines.XML_NAME_END_DP):
                    raise
                else:
                    continue

            # Check the naming conventions
            if chipset_from_file == chipset_from_dir_name:
                if entry.endswith(defines.XML_NAME_END_DP):
                    dpconfig = path
                elif entry.endswith(defines.XML_NAME_END_DP_SI):
                    dp_siconfig = path
                else:
                    logger.debug2('Skipping file: ' + entry + '\n'
                                    '    ' + 'Name does not match any of the naming convention patters')
            else:
                logger.debug2('Skipping file: ' + entry + '\n'
                                '    ' + 'Chipset from file: "' + chipset_from_file + '" does not match chipset from dir name: "' + chipset_from_dir_name + '"')


    if dpconfig is None:
        raise RuntimeError('Could not find out the debugpolicy config file from the platform specified directory path: ' + chipset_dir)

    if dp_siconfig is None:
        raise RuntimeError('Could not find out the dbgp secimage config file from the platform specified directory path: ' + chipset_dir)

    logger.debug2('Config paths found for chipset_dir: ' + chipset_dir + '\n'
                    '    ' + 'debugpolicy config file: ' + str(dpconfig) )
    return dpconfig, dp_siconfig


#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
__all__ = ['ConfigDir',
           'ConfigParser',
           'defines',
           'auto_gen_obj_config']
