# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Parses & generates config files using a config structure. Currently xml file
is supported.

.. data:: PATH

    Object that can be used to specify that the value expected in the parsed
    config file is a path. If the value in the config starts with a './', the
    dot is replaced with the directory of the config file. This ensures that all
    the relative paths specified in the config file are relative to one location
    (the config file path).
    See more documentation: :mod:`datautils.path_handler`

    Example usage:

    ::

        validation_dict = {
                                'type' : '',
                                'path' : c_config.PATH,
                          }

.. data:: DICT

    Object that can be used for providing additional validation options for a
    dictionary.
    See more documentation: :mod:`datautils.dict_handler`

"""

import json
import os
import sys

from c_logging import logger
import c_path
from datautils import dict_handler, hex_handler, path_handler, DataHandler, get_cb_dict
from sectools.common.utils.datautils.data_detail import DataType


# Objects for usage in validation/structure dictionaries
PATH = path_handler.type_path
DICT = dict_handler
HEX  = hex_handler.type_hex


class CoreConfig(object):
    """Parses the configuration file provided using auto-generated xml tree.
    The tree can be generated using the generateDS tool.

    :param obj config_module: The generateDS config module that will parse the
        config file.
    :param str config_path: Path to the config file to be parsed.

    .. data:: root (obj)

        An object created after parsing the config file

    .. data:: config_path (str)

        Path of the config file parsed
    """

    def __init__(self, config_module, config_path):
        assert isinstance(config_path, str)

        self.root = None
        self.config_path = c_path.normalize(config_path)

        logger.debug2('Checking config file existence at ' + self.config_path)
        if not c_path.validate_file(self.config_path):
            raise RuntimeError('Cannot read config file at ' + self.config_path)

        logger.debug2('Using generateDs APIs to parse config and validate syntax')
        try:
            doc = config_module.parsexml_(self.config_path)
            rootNode = doc.getroot()
            rootClass = config_module.get_root_tag(rootNode)[1]
            self.root = rootClass.factory()
            self.root.build(rootNode)
        except Exception as e:
            raise type(e), type(e)('Error parsing config file: ' + str(e) + '\n'
                                   '    ' + 'config_path: ' + self.config_path), sys.exc_info()[2]
        doc = None
        logger.debug2('Parsed config using generateDs')

    def validate(self, validation_data):
        """Normalizes and validates the semantics of the data contained in the
            xml using the given validation_data.

        :param validation_data: Data that specifies how to validate the root
            node. This will in most cases be a dictionary.
        """
        logger.debug2('Normalizing and validating the xml semantics')
        try:
            cb_dict = get_cb_dict(path_basepath=os.path.dirname(self.config_path))
            self.root = DataHandler(cb_dict).validate(self.root, validation_data)
        except Exception as e:
            raise type(e), type(e)('xml->' + str(e)), sys.exc_info()[2]
        logger.debug2('Config data validated')

    def generate(self, config_file, root_name, namespacedef, lines_prepend='', lines_append=''):
        """Generates the config file with the current configuration of the root
        node.

        :param str config_file: config file that should be generated with the
            current data.
        :param str root_name: name of the root node for the xml file.
        :param str namespacedef: namespacedef for the xml file.
        :param list[str] lines_prepend: lines to be written to the xml file
            before the auto generated content.
        :param list[str] lines_append: lines to be written to the xml file
            after the auto generated content.
        """
        assert isinstance(config_file, str)
        assert isinstance(root_name, str)
        assert isinstance(namespacedef, str)
        assert isinstance(lines_prepend, str)
        assert isinstance(lines_append, str)

        logger.debug2('Generating config file at ' + config_file + '\n'
                      '    ' + 'Root node name: ' + root_name + '\n'
                      '    ' + 'XML namespacedef: ' + namespacedef + '\n'
                      '    ' + 'Lines to prepend at start: ' + str(lines_prepend) + '\n'
                      '    ' + 'Lines to append at the end: ' + str(lines_append))
        fd = open(config_file, 'w')
        fd.write(lines_prepend)
        self.root.export(fd, 0, namespace_='', name_=root_name, namespacedef_=namespacedef)
        fd.write(lines_append)
        fd.close()
        logger.debug2('Generated config file')

    def transfer(self, obj_1, obj_2, structure, structure_parent, class_module, classname_gen):
        """Updates the values of the config nodes using the values of the
        attributes of the object provided using the given structure.

        :param obj_1: Object to be used for obtaining the values.
        :param obj_2: Object to be updated.
        :param structure: Structure of obj_1 & obj_2.
        :param str structure_parent: name of the root object for the structure.
        :param mod class_module: Module containing the classes used in the
            obj_2 structure.
        :param classname_gen: Callback function that can update the name of the
            class.
        """
        assert isinstance(structure, dict)
        assert isinstance(structure_parent, str)

        logger.debug2('Transferring data from obj_1 to obj_2\n'
                      '    ' + 'obj_1: ' + str(obj_1) + '\n'
                      '    ' + 'obj_2: ' + str(obj_2) + '\n'
                      '    ' + 'Root name: ' + structure_parent + '\n'
                      '    ' + 'Class module: ' + str(class_module))
        cb_dict = get_cb_dict(base_modules=class_module,
                              base_classname_gen=classname_gen,
                              path_basepath=os.path.dirname(self.config_path))
        return_val = DataHandler(cb_dict).transfer(obj_1, obj_2, structure, structure_parent)
        logger.debug2('Transferred data from obj_1 to obj_2')
        return return_val


def _json_str_hook(data):
    for key, value in data.iteritems():
        if isinstance(value, unicode):
            data[key] = str(value)
            value = value.encode('utf-8')
        elif isinstance(value, dict):
            value = _json_str_hook(value)
    return data


class CoreJSONConfig(CoreConfig):

    class SelfGeneratingClass(object):
        """ Class that automatically generates attributes based on the dictionary given """

        def __init__(self, tag, data):
            """
            Recursively creates new objects to comply with the given dictionary.

            Parameters:
            1. dataDict (str): Dictionary that specifies the structure to be created.
            """
            self.tag = tag
            self.dictionary = data
            for data_tag, data_value in data.items():
                setattr(self, data_tag, CoreJSONConfig.SelfGeneratingClass(data_tag, data_value) if isinstance(data_value, dict) else data_value)

        def get_dict(self):
            ret = {}
            for data_tag, data_value in self.dictionary.items():
                ret[data_tag] = getattr(self, data_tag)
                if isinstance(data_value, dict):
                    ret[data_tag] = ret[data_tag].get_dict()
            return ret

        def __repr__(self):
            return self.tag

    def __init__(self, config_path):
        assert isinstance(config_path, str)

        self.root = None
        self.config_path = c_path.normalize(config_path)

        logger.debug2('Checking config file existence at ' + self.config_path)
        if not c_path.validate_file(self.config_path):
            raise RuntimeError('Cannot read config file at ' + self.config_path)

        logger.debug2('Using JSON module to parse the config file.')
        try:
            with open(config_path, 'r') as fp:
                self.root = self.SelfGeneratingClass('Root', json.load(fp, object_hook=_json_str_hook))
        except Exception as e:
            raise type(e), type(e)('Error parsing config file: ' + str(e) + '\n'
                                   '    ' + 'config_path: ' + self.config_path), sys.exc_info()[2]

    def generate(self, config_file):
        """Generates the config file with the current configuration of the root
        node.

        :param str config_file: config file that should be generated with the
            current data.
        """
        assert isinstance(config_file, str)

        logger.debug2('Generating config file at ' + config_file)
        with open(config_file, 'w') as fp:
            json.dump(self.root.get_dict(), fp, indent=4)
        logger.debug2('Generated config file')


class CfgOverrides(object):

    def __init__(self, structure):
        assert isinstance(structure, dict)
        self.structure = structure
        cb_dict = get_cb_dict(path_basepath=os.path.curdir)
        self.root = DataHandler(cb_dict).detail(self.structure)

    def get_properties(self, has_value=False):
        simple = True
        properties = {}
        for tag, detail in self.root.children.items():
            if simple and not detail.is_simple:
                continue
            if has_value:
                if detail.value_set:
                    properties[tag] = detail
            else:
                properties[tag] = detail
        return properties

