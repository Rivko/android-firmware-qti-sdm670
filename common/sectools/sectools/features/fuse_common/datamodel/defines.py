# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the datamodel.

Constants relate to:

#. Common constants.
#. Data model auto generation objects
#. Dict representation of the data model.
"""


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'fuseblower'


#------------------------------------------------------------------------------
# Auto-generated OBJ Parser related information
#------------------------------------------------------------------------------
# OBJ - Basic information
OBJ_PY_FILE_NAME = 'auto_gen_model.py'
OBJ_PY_PATH = c_path.join(PACKAGE_PATH, OBJ_PY_FILE_NAME)

# OBJ - lines at the beginning of the file
OBJ_PREPEND_LINES = '"""\nAuto generated classes\n"""\n\n'

# OBJ - classname generator
OBJ_CLASSNAME_GEN = lambda x: 'Cfg_' + x.replace('_', ' ').title().replace(' ', '_')


#------------------------------------------------------------------------------
# Data Model Structure
#------------------------------------------------------------------------------
OBJ_STRUCTURE = \
{
    'module' :
    [
        {
            'id' : '',
            'base_address' : '',
            'description' : ('',),
            'fuse_region' :
            [
                {
                    'id' : '',
                    'description' : ('',),
                    'fuse' :
                    [
                        {
                            'n' : (0,),
                            'ignore' : (True,),
                            'address' : '',
                            'operation' : '',
                            'field' :
                            [
                                {
                                    'id' : '',
                                    'owner' : ('',),
                                    'description' : ('',),
                                    'value' : '',
                                    'bits' : '',
                                }
                            ],
                        }
                    ]
                }
            ],
         }
    ],
}


#------------------------------------------------------------------------------
# Data Model Specific constants
#------------------------------------------------------------------------------
# Supported owners
OWNER_OEM = 'OEM'
OWNER_QTI = 'QTI'
SUPPORTED_OWNERS = [OWNER_OEM,
                    OWNER_QTI]

# Supported operations
OPERATION_BLOW = 'BLOW'
OPERATION_VERIFYMASK0 = 'VERIFYMASK0'
SUPPORTED_OPERATIONS = [OPERATION_BLOW,
                        OPERATION_VERIFYMASK0]
