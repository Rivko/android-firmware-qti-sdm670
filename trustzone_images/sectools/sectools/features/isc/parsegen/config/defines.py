# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the parsegen package.

Constants relate to:

#. Common constants
#. Auto-generated XML Parser - auto_gen_xml_config
#. Auto-generated OBJ - auto_gen_obj_config
#. Config Data Structure
"""

import os

from sectools.common.utils import c_config, c_path


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = os.path.dirname(os.path.abspath(__file__))

# Name of the root node of any config related objects
ROOTNODE_NAME = 'parsegen'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = '.'


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
XML_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/parsegen"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/parsegen ./parsegen.xsd"'
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


#------------------------------------------------------------------------------
# Config Data Structure
#------------------------------------------------------------------------------
# image type block
CONFIG_KEY_IMAGE_TYPE = 'image_type'
CONFIG_STRUCTURE_IMAGE_TYPE = \
{
    'id' : '',
    'file_type' : '',

    'mbn_properties' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'header_size' : 0,
    },

    'elf_properties' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'image_type' : 0,
        'max_elf_segments' : (0,),
        'hash_seg_placement' : (('POST_PHDR_ALIGNED', 'POST_PHDR', 'POST_PHDR_LOAD_ALIGNED', 'END_ALIGNED',
                                 'END_64B_ALIGNED', 'END_128B_ALIGNED', ()),),
        'validate_ph_addrs' : (True,),
        'validate_vir_addrs' : (False,),
        'has_multi_image_segment' : (False,),
        'has_license_manager_segment' : (False,),
        'has_sections' : (False,),
        'persist_encr' : (False),
    },

    'ewm_properties' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'image_entry' : '',
        'relocatable' : False,
    },

    'pmbl_properties' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'preamble_size' : (0,),
        'has_magic_num' : False,
        'ota_enabled' : False,
        'page_size' : (0,),
        'num_of_pages' : (0,),
        'min_size_with_pad' : (0,),
    },
}

# top-level block
CONFIG_KEY = ROOTNODE_NAME
CONFIG_STRUCTURE = \
{
    'image_types_list':
    {
        CONFIG_KEY_IMAGE_TYPE : [CONFIG_STRUCTURE_IMAGE_TYPE],
    }
}

