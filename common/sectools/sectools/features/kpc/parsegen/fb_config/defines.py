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
#. Auto-generated FB XML Parser - auto_gen_xml_config
"""

from __secfile__ import DIR_PATH
from sectools.common.utils import c_path
from sectools.features.fuse_common.datamodel import defines as dm_def


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'keyprovision'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'


#------------------------------------------------------------------------------
# Auto-generated XML Parser related information
#------------------------------------------------------------------------------
# XML - Basic information
XML_PY_FILE_NAME = 'auto_gen_fb_config.py'
XML_PY_FILE_NAME_USER = 'auto_gen_fb_user_config.py'
XML_PY_PATH = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME)
XML_PY_PATH_USER = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME_USER)

XML_NAME_ENDING_OEM = '_' + ROOTNODE_NAME + '_KEYMAP' + '.xml'
XML_NAME_ENDING_QTI = '_' + ROOTNODE_NAME + '_QTI' + '.xml'
XML_NAME_ENDING_UI = '_' + ROOTNODE_NAME + '_UI' + '.xml'
XML_NAME_ENDING_USER = '_' + ROOTNODE_NAME + '_USER' + '.xml'

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_KEYPROVISION = 'xmlns:tns="http://www.qualcomm.com/keyprovision"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'

XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/keyprovsion ../xsd/keyprovision.xsd"'
XML_NAMESPACE_SCHEMA_USER = 'xsi:schemaLocation="http://www.qualcomm.com/keyprovision ../xsd/keyprovision_user.xsd"'

XML_NAMESPACE = XML_NAMESPACE_KEYPROVISION + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA
XML_NAMESPACE_USER = XML_NAMESPACE_KEYPROVISION + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA_USER

# XML - classname generator
XML_CLASSNAME_GEN = lambda x: 'complex_' + x

# mapping of user configurable entries and what Fuse Field they modify
FIELD_ID_MAP = {'L1_encryption_key' : 'CUSTKEY1_L1_ROW0',
                'L1_encryption_file' : 'CUSTKEY1_L1_ROW0',
                'ROT_encryption_key' : 'CUSTKEY1_ROT_ROW0',
                'ROT_encryption_file' : 'CUSTKEY1_ROT_ROW0',
                'oem_image_encr_key' : 'ENCR_KEY_ROW0',
                'oem_image_encr_key_file' : 'ENCR_KEY_ROW0',
               }

