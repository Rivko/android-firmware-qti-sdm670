# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the cfgparser package.

Constants relate to:

#. Common constants
#. Auto-generated XML Parser - auto_gen_xml_config
#. Auto-generated OBJ - auto_gen_obj_config
#. Config Data Structure
"""


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path, c_config


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'debugpolicy'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'

# Default config directory (relative path)
DEFAULT_CONFIG_DIR_REL      = '../../../../'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR          = c_path.join(c_path.join(PACKAGE_PATH, DEFAULT_CONFIG_DIR_REL), CONFIG_DIR_BASENAME)

XML_NAME_END_DP             = '_' + ROOTNODE_NAME + '.xml',
XML_NAME_END_DP_SI          = '_' + 'dbgp_secimage' + '.xml',


#------------------------------------------------------------------------------
# Auto-generated XML Parser related information
#------------------------------------------------------------------------------
# XML - Basic information
XML_PY_FILE_NAME = 'auto_gen_xml_config.py'
XML_PY_PATH = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME)
XML_NAME_ENDING = '_' + ROOTNODE_NAME + '.xml'

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/debugpolicy"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/debugpolicy ../xsd/debugpolicy.xsd"'
XML_NAMESPACE = XML_NAMESPACE_TNS + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA

# XML - classname generator
XML_CLASSNAME_GEN = lambda x: 'complex_' + x


#------------------------------------------------------------------------------
# Auto-generated OBJ Parser related information
#------------------------------------------------------------------------------
# OBJ - Basic information
OBJ_PY_FILE_NAME = 'auto_gen_obj_config.py'
OBJ_PY_PATH = c_path.join(PACKAGE_PATH, OBJ_PY_FILE_NAME)

# OBJ - lines at the beginning of the file
OBJ_PREPEND_LINES = '"""\nAuto generated classes\n"""\n\n'

# OBJ - classname generator
OBJ_CLASSNAME_GEN = lambda x: 'Cfg_' + x.replace('_', ' ').title().replace(' ', '_')


# elf properties block
CONFIG_ELF_PROPERTIES = 'elf'
CONFIG_STRUCTURE_ELF_PROPERTIES = \
{
    'elf_class' : 0,
    'phys_addr' : '',
}

# image_id list block
CONFIG_IMAGE_ID_LIST = 'image_id_list'
CONFIG_STRUCTURE_IMAGE_ID_LIST = \
{
    c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
    'image_id': ([''],),
}

# rootcerthash list block
CONFIG_ROOTCERTHASH_LIST = 'root_cert_hash_list'
CONFIG_STRUCTURE_ROOTCERTHASH_LIST = \
{
    'root_cert_hash': [''],
}

# serial number list block
CONFIG_SERIAL_NUM_LIST = 'serial_num_list'
CONFIG_STRUCTURE_SERIAL_NUM_LIST = \
{
    c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
    'serial_num': ([''],),
}

# flag block
CONFIG_FLAG = 'flag'
CONFIG_STRUCTURE_FLAG = \
{
    'bit_pos': 0,
    'value'  : 0,
}

# flags block
CONFIG_FLAGS = 'flags'
CONFIG_STRUCTURE_FLAGS = \
{
    CONFIG_FLAG: [CONFIG_STRUCTURE_FLAG],
}

# swid block
CONFIG_SWID = 'swid'
CONFIG_STRUCTURE_SWID = \
{
    'bit_pos': 0,
    'value'  : 0,
}

# image bit map block
CONFIG_IMAGE_BIT_MAP = 'image_bit_map'
CONFIG_STRUCTURE_IMAGE_BIT_MAP = \
{
    c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
    CONFIG_SWID: [CONFIG_STRUCTURE_SWID],
}

# rootcerthash_qti list block
CONFIG_ROOTCERTHASH_QTI_LIST = 'root_cert_hash_qti_list'
CONFIG_STRUCTURE_ROOTCERTHASH_QTI_LIST = \
{
    c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
    'root_cert_hash_qti': ([''],),
}

# debugpolicy block
CONFIG_STRUCTURE = \
{
    'metadata' :
    {
        'chipset' : '',
        'version' : '',
    },

    'file_properties' :
    {
        'revision' : 0,
        'serial_number_start' : (0,),
        'serial_number_end' : (0,),
        CONFIG_FLAGS : CONFIG_STRUCTURE_FLAGS,
        CONFIG_IMAGE_ID_LIST : CONFIG_STRUCTURE_IMAGE_ID_LIST,
        CONFIG_IMAGE_BIT_MAP : CONFIG_STRUCTURE_IMAGE_BIT_MAP,
        CONFIG_ROOTCERTHASH_LIST : CONFIG_STRUCTURE_ROOTCERTHASH_LIST,
        CONFIG_SERIAL_NUM_LIST : CONFIG_STRUCTURE_SERIAL_NUM_LIST,
        CONFIG_ROOTCERTHASH_QTI_LIST : CONFIG_STRUCTURE_ROOTCERTHASH_QTI_LIST,
        CONFIG_ELF_PROPERTIES : CONFIG_STRUCTURE_ELF_PROPERTIES,
    },

}

# elf class
ELF_CLASS32 = 32
ELF_CLASS64 = 64

ELF_CLASS = \
{
    ELF_CLASS32 : 1,
    ELF_CLASS64 : 2,
}
