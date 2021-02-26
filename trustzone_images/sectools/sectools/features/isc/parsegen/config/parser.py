# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils import c_config
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import get_dups_in_list


class ParsegenCfgParser(c_config.CoreConfig):
    """Parses the Parsegen configuration file using c_config.CoreConfig and
    the module auto-generated using generateDS (auto_gen_xml_config).

    :param str config_path: Path to the config file to be parsed.

    .. data: root

        Root class representing the structure of the config file.

    .. data:: __version__ = 1.3

        Version of the Config Parser

    """

    __version__ = '1.3'

    def __init__(self, config_path, defines=None,
                 auto_gen_xml_config=None, auto_gen_obj_config=None):
        """Initializations and checks"""
        assert isinstance(config_path, str)
        self.config_module = auto_gen_xml_config
        self.obj_module = auto_gen_obj_config
        super(ParsegenCfgParser, self).__init__(self.config_module, config_path)
        self.defines = defines
        self.validate()

    def validate(self):
        """Normalizes and validates the data in the secimage xml.

        :raises: RuntimeError if config is determined to be invalid
        """
        c_config.CoreConfig.validate(self, self.defines.CONFIG_STRUCTURE)

        # Check that the id list contains at least 1 image_type
        image_type_id_list = [image_type.id for image_type in self._get_parsegen_image_type_list()]
        if len(image_type_id_list) == 0:
            raise RuntimeError('Config file must contain at least one image_type under parsegen->image_types_list')

        # Check that the ids of the image_type objects are unique
        dup_image_type_id_list = get_dups_in_list(image_type_id_list)
        if len(dup_image_type_id_list):
            raise RuntimeError('image_type ids must be unique. Found multiple image_types with same id: ' +
                               str(dup_image_type_id_list))

    def generate(self, file_path):
        """Generates the parsegen config file with the current configuration
        of the root node.

        :param str file_path: path to the config file that should be generated
            with the current data
        """
        logger.debug('Generating config file...')
        c_config.CoreConfig.generate(self, file_path,
                                     self.defines.XML_ROOTNODE,
                                     self.defines.XML_NAMESPACE,
                                     self.defines.XML_PREPEND_LINES)
        logger.debug('Generated config file at: ' + str(file_path))

    def transfer_from_obj(self, obj):
        """Updates the values of the config root node using the attributes of
        the object provided.

        :param Cfg_Parsegen obj: Object to be used for obtaining the values.
        """
        assert isinstance(obj, self.obj_module.Cfg_Parsegen)
        logger.debug('Updating config contents using object: ' + obj)
        self.root = self.transfer_from_obj_to_node(obj,
                                                   self.root,
                                                   self.defines.CONFIG_STRUCTURE,
                                                   self.defines.ROOTNODE_NAME)
        logger.debug('Config contents updated.')

    def transfer_to_obj(self, obj):
        """Updates the attributes of the object provided using the values of the
        root config node.

        :params: same as :meth:`transfer_from_obj`
        """
        assert isinstance(obj, self.obj_module.Cfg_Parsegen)
        logger.debug('Updating object using config contents: ' + obj)
        obj = self.transfer_from_node_to_obj(self.root,
                                             obj,
                                             self.defines.CONFIG_STRUCTURE,
                                             self.defines.ROOTNODE_NAME,)
        logger.debug('Object updated using the config.')

    def transfer_from_obj_to_node(self, obj, node, structure, structure_parent):
        return self.transfer(obj,
                             node,
                             structure,
                             structure_parent,
                             self.config_module,
                             self.defines.XML_CLASSNAME_GEN)

    def transfer_from_node_to_obj(self, node, obj, structure, structure_parent):
        return self.transfer(node,
                             obj,
                             structure,
                             structure_parent,
                             self.obj_module,
                             self.defines.OBJ_CLASSNAME_GEN)


    @property
    def config_data(self):
        """
        :returns: Config object
        :rtype: obj
        """
        return self.root

    def _get_parsegen_image_type_list(self):
        """
        :returns: list of image_type objects from the config file
        :rtype: list[obj]
        """
        return self.root.image_types_list.image_type
