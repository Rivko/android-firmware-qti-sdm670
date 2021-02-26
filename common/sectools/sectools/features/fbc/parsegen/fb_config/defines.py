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
ROOTNODE_NAME = 'fuseblower'

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

XML_NAME_ENDING_OEM = '_' + ROOTNODE_NAME + '_OEM' + '.xml'
XML_NAME_ENDING_QTI = '_' + ROOTNODE_NAME + '_QTI' + '.xml'
XML_NAME_ENDING_UI = '_' + ROOTNODE_NAME + '_UI' + '.xml'
XML_NAME_ENDING_USER = '_' + ROOTNODE_NAME + '_USER' + '.xml'

# Modes of tool operation
FB_MODE_SECBOOT = 'secboot'
FB_MODE_ROT     = 'rot'

XML_NAME_END_OEM = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_OEM' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_OEM' + '.xml',
}

XML_NAME_END_QTI = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_QTI' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_QTI' + '.xml',
}

XML_NAME_END_UI = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_UI' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_UI' + '.xml',
}

XML_NAME_END_USER = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_USER' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_USER' + '.xml',
}

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_FUSEBLOWER = 'xmlns:tns="http://www.qualcomm.com/fuseblower"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'

XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/fuseblower ../xsd/fuseblower.xsd"'
XML_NAMESPACE_SCHEMA_USER = 'xsi:schemaLocation="http://www.qualcomm.com/fuseblower ../xsd/fuseblower_user.xsd"'

XML_NAMESPACE = XML_NAMESPACE_FUSEBLOWER + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA
XML_NAMESPACE_USER = XML_NAMESPACE_FUSEBLOWER + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA_USER

# XML - classname generator
XML_CLASSNAME_GEN = lambda x: 'complex_' + x

# Fuseblower - Secboot Mode & RoT Mode
# mapping of user configurable entries and what Fuse Field they modify
FIELD_ID_MAP = {'oem_hw_id':'OEM_HW_ID',
                'mrc_cert_count':'ROOT_CERT_TOTAL_NUM',
                'oem_product_id':'OEM_PRODUCT_ID',
                'SEC_BOOT1_PK_Hash_in_Fuse' : 'SEC_BOOT1_PK_Hash_in_Fuse',
                'SEC_BOOT1_rom_pk_hash_index':'SEC_BOOT1_ROM_PK_Hash_Index',
                'SEC_BOOT1_use_serial_num':'SEC_BOOT2_Use_Serial_Num',
                'SEC_BOOT2_PK_Hash_in_Fuse' : 'SEC_BOOT2_PK_Hash_in_Fuse',
                'SEC_BOOT2_rom_pk_hash_index':'SEC_BOOT3_ROM_PK_Hash_Index',
                'SEC_BOOT2_use_serial_num':'SEC_BOOT1_Use_Serial_Num',
                'SEC_BOOT3_PK_Hash_in_Fuse' : 'SEC_BOOT3_PK_Hash_in_Fuse',
                'SEC_BOOT3_rom_pk_hash_index':'SEC_BOOT2_ROM_PK_Hash_Index',
                'SEC_BOOT3_use_serial_num':'SEC_BOOT3_Use_Serial_Num',
                'mrc_pk_hash_index':'ROOT_CERT_PK_HASH_INDEX',
                'modem_mrc_pk_hash_index':'MODEM_ROOT_CERT_PK_HASH_INDEX',
                'root_cert_hash' : 'HASH_DATA_ROW0',
                'root_cert_file' : 'HASH_DATA_ROW0',
                'apps_debug_disabled' : 'APPS_DBGEN_DISABLE',
                'allow_bus_dcvs' : 'APPS_NIDEN_DISABLE',
                'watchdog_enable' : 'WDOG_EN',
                'root_cert_hash0' : 'HASH0_DATA_ROW0',
                'root_cert_hash0_file' : 'HASH0_DATA_ROW0',
                'pk_hash1' : 'HASH1_DATA_ROW0',
                'pk_hash1_file':'HASH1_DATA_ROW0',
                'image_encr_key1' : 'KEY_DATA_ROW0',
                'image_encr_key1_file' : 'KEY_DATA_ROW0',
                'disable_rot_transfer': 'DISABLE_ROT_TRANSFER',
                'current_sw_rot_apps': 'CURRENT_SW_ROT_APPS',
                'current_sw_rot_modem': 'CURRENT_SW_ROT_MODEM',
                'root_certhash' : 'HASH_DATA0_ROW0',
                'root_certfile' : 'HASH_DATA0_ROW0',
                'oem_hwid' : 'OEM_HW_ID_9_0',
                'oem_productid' :'OEM_PRODUCT_ID_3_0',
                'SECURE_BOOT0_use_serial_num' : 'SECURE_BOOT0_USE_SERIAL_NUM',
                'total_rot_num': 'TOTAL_ROT_NUM',
                'rot_activation': 'ROT_ACTIVATION',
                'rot_revocation': 'ROT_REVOCATION',
                'root_cert_activate_index': 'ROOT_CERT_ACTIVATION_LIST',
                'root_cert_revoke_index': 'ROOT_CERT_REVOCATION_LIST',
                'sp_nvm_ar_config': 'SP_NVM_AR_CONFIG',
                'wifi_board_id': 'WIFI_BOARD_ID',
                }

ROOT_CERT_PK_HASH_INDEX={'PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED':0x00,
                        #Production device, no certificate selected (Default to certificate 0)
                        'PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0' :0xF0,
                        #Production device fixed to certificate 0
                        'DEVICE_FIXED_TO_CERTIFICATE_1'            :0xE1,
                        #Device fixed to certificate 1
                        'DEVICE_FIXED_TO_CERTIFICATE_2'            :0xD2,
                        #Device fixed to certificate 2
                        'DEVICE_FIXED_TO_CERTIFICATE_3'            :0xC3,
                        #Device fixed to certificate 3
                        'DEVICE_FIXED_TO_CERTIFICATE_4'            :0xB4,
                        #Device fixed to certificate 4
                        'DEVICE_FIXED_TO_CERTIFICATE_5'            :0xA5,
                        #Device fixed to certificate 5
                        'DEVICE_FIXED_TO_CERTIFICATE_6'            :0x96,
                        #Device fixed to certificate 6
                        'DEVICE_FIXED_TO_CERTIFICATE_7'            :0x87,
                        #Device fixed to certificate 7
                        'DEVICE_FIXED_TO_CERTIFICATE_8'            :0x78,
                        #Device fixed to certificate 8
                        'DEVICE_FIXED_TO_CERTIFICATE_9'            :0x69,
                        #Device fixed to certificate 9
                        'DEVICE_FIXED_TO_CERTIFICATE_10'           :0x5A,
                        #Device fixed to certificate 10
                        'DEVICE_FIXED_TO_CERTIFICATE_11'           :0x4B,
                        #Device fixed to certificate 11
                        'DEVICE_FIXED_TO_CERTIFICATE_12'           :0x3C,
                        #Device fixed to certificate 12
                        'DEVICE_FIXED_TO_CERTIFICATE_13'           :0x2D,
                        #Device fixed to certificate 13
                        'DEVICE_FIXED_TO_CERTIFICATE_14'           :0x1E,
                        #Device fixed to certificate 14
                        'DEVICE_FIXED_TO_CERTIFICATE_15'           :0x0F,
                        #Device fixed to certificate 15
                        'DEVICE_FIXED_TO_CERTIFICATE_INVALID'      :0xFF,
                        #Invalid certificate index
                        }
