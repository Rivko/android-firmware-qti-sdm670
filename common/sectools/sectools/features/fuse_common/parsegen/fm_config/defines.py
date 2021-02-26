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
#. Auto-generated FM XML Parser - auto_gen_xml_config
"""

from __secfile__ import DIR_PATH
from sectools.common.utils import c_path


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'fusemaster'


#------------------------------------------------------------------------------
# Auto-generated XML Parser related information
#------------------------------------------------------------------------------
# XML - Basic information
XML_PY_FILE_NAME = 'auto_gen_fm_config.py'
XML_PY_PATH = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME)
XML_NAME_ENDING = '_' + ROOTNODE_NAME + '.xml'

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/fusemaster"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/fusemaster ../xsd/fusemaster.xsd"'
XML_NAMESPACE = XML_NAMESPACE_TNS + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA

# XML - classname generator
XML_CLASSNAME_GEN = lambda x: 'complex_' + x


#------------------------------------------------------------------------------
# Auto-generated Conf XML Parser related information
#------------------------------------------------------------------------------
CONF_ROOTNODE_NAME = 'fusemaster_filter'

# XML - Basic information
XML_CONF_PY_FILE_NAME = 'auto_gen_fmconf_config.py'
XML_CONF_PY_PATH = c_path.join(PACKAGE_PATH, XML_CONF_PY_FILE_NAME)
XML_CONF_NAME_ENDING = '_' + CONF_ROOTNODE_NAME + '.xml'

# XML - rootnode related constants
XML_CONF_ROOTNODE_NAME = CONF_ROOTNODE_NAME
XML_CONF_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_CONF_ROOTNODE_NAME

# XML - namespace related constants
XML_CONF_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/fusemaster_filter"'
XML_CONF_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/fusemaster_filter ../xsd/fusemaster_filter.xsd"'
XML_CONF_NAMESPACE = XML_CONF_NAMESPACE_TNS + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_CONF_NAMESPACE_SCHEMA


