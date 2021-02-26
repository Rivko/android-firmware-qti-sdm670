# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the mbngen package.

Constants relate to:

#. Common constants
#. Auto-generated XML Parser - auto_gen_xml_config
#. Auto-generated OBJ - auto_gen_obj_config
#. Config Data Structure
"""


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'mbngen'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR = c_path.join(c_path.join(PACKAGE_PATH, '../../../../'), CONFIG_DIR_BASENAME)

# Default LTF config directory
DEFAULT_MBNGEN_CONFIG_DIR = c_path.join(DEFAULT_CONFIG_DIR, ROOTNODE_NAME)

MBNGEN_XML_NAME_ENDING = ROOTNODE_NAME + '.xml'

MBNGEN_CONFIG_FILE = c_path.join(DEFAULT_MBNGEN_CONFIG_DIR,MBNGEN_XML_NAME_ENDING)

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
XML_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/mbngen"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/localtf ../xsd/mbngen.xsd"'
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



# top-level block
CONFIG_KEY = ROOTNODE_NAME
CONFIG_STRUCTURE = \
{


    'mbngen_conf' :
    {
        'conf' :
        [
            {
                'tag_name' : '',
                'image_id' : 0,
                'header_version' : 0,
                'image_src' : 0,
                'image_dest_ptr' : 0,
                'header_length' : 0,
            }
         ]
    }
}

