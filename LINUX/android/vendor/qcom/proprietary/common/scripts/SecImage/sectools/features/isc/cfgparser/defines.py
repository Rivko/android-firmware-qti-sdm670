# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
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
from sectools.common.utils import c_config, c_path
from sectools.features.isc.parsegen.config.defines import CONFIG_KEY_IMAGE_TYPE, \
    CONFIG_STRUCTURE_IMAGE_TYPE


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'secimage'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'


#------------------------------------------------------------------------------
# Auto-generated XML Parser related information
#------------------------------------------------------------------------------
# XML - Basic information
XML_PY_FILE_NAME = 'auto_gen_xml_config.py'
XML_PY_PATH = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME)
XML_NAME_ENDING = '_' + ROOTNODE_NAME + '.xml'
XML_NAME_REGEX = '[^_]+_' + ROOTNODE_NAME + '\.xml'

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_TNS = 'xmlns:tns="http://www.qualcomm.com/secimage"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/secimage ../xsd/secimage.xsd"'
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
# cert_config block
CONFIG_KEY_CERT_CONFIG = 'cert_config'

# Set the signers
def set_signers(signers):
    CONFIG_STRUCTURE_GENERAL_PROPERTIES['selected_signer'] = (tuple(signers),)

# general properties block
CONFIG_KEY_GENERAL_PROPERTIES = 'general_properties'
CONFIG_STRUCTURE_GENERAL_PROPERTIES = \
{
    'selected_signer'               : ((''),),
    'selected_encryptor'            : (('ssd_encryption', 'unified_encryption_2_0', (),),),
    'selected_cert_config'          : '',
    'cass_capability'               : ('',),
    'hash_algorithm'                : (('sha1', 'sha256', 'sha384', (),),),
    'cass_attest_cert'              : (('sha1', 'sha256', (),),),
    'segment_hash_algorithm'        : (('sha256', 'sha384', (),),),
    'max_cert_size'                 : ((2048,),),
    'key_size'                      : ((2048, 4096, ()),),
    'num_certs_in_certchain'        : ((2, 3),),
    'num_root_certs'                : 0,
    'testsig_serialnum'             : ('',),
    'mrc_index'                     : (0,),
    'msm_part'                      : (c_config.HEX,),
    'oem_id'                        : c_config.HEX,
    'model_id'                      : c_config.HEX,
    'sw_id'                         : (c_config.HEX,),
    'app_id'                        : (c_config.HEX,),
    'crash_dump'                    : (c_config.HEX,),
    'rot_en'                        : (c_config.HEX,),
    'soc_hw_version'                : (c_config.HEX,),
    'soc_vers'                      : ('',),
    'mask_soc_hw_version'           : (c_config.HEX,),
    'in_use_soc_hw_version'         : ((0, 1, (),),),
    'use_serial_number_in_signing'  : ((0, 1, (),),),
    'serial_number'                 : (c_config.HEX,),
    'debug'                         : (c_config.HEX,),
    'debug_serials'                 :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'serial' : ([''],),
    },
    'exponent'                      : ((3, 257, 65537, ()),),
    'hash_pageseg_as_segment'       : (False,),
    'oem_id_independent'            : ((0, 1, (),),),
    'secboot_version'               : ('1.0',),
    'qc_sign'                       : (False,),
    'oem_sign'                      : (True,),
    'UIE_key'                       : ('',),
    'UIE_capability'                : ('',),
    'UIE_root_key_type'             : ((0, 1, 2, ()),),
    'revocation_enablement'         : (c_config.HEX,),
    'activation_enablement'         : (c_config.HEX,),
    'object_id' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'name'  : '',
        'min'   : ('',),
        'max'   : ('',),
    },
    'rsa_padding'                   : (('pss', 'pkcs', (),),),
    'hmac'                          : (False,),
    'dsa_type'                      : (('rsa', 'ecdsa', (),),),
    'ecdsa_curve'                   : (('secp256r1', 'secp384r1', (),),),
}

# top-level block
CONFIG_KEY = ROOTNODE_NAME
CONFIG_STRUCTURE = \
{
    'metadata' :
    {
        'chipset' : '',
        'version' :  '',
    },

    CONFIG_KEY_GENERAL_PROPERTIES : CONFIG_STRUCTURE_GENERAL_PROPERTIES,

    'parsegen' :
    {
        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
        'image_types_list' :
        {
            c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
            CONFIG_KEY_IMAGE_TYPE : [CONFIG_STRUCTURE_IMAGE_TYPE],
        }
    },

    'signing' :
    {
        'signer_attributes' :
        {
            'cass_signer_attributes' :
            {
                c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],

                'server' :
                {
                    c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                    'host' : '',
                    'port' : 0,
                    'trust_keystore' : c_config.PATH,
                    'trust_keystore_type' : (('JKS', 'PKCS12',),),
                    'trust_keystore_password' : ('',),
                    'host_validation_mode' : (('TEST', 'STAGE_PROD',),),
                },

                'user_identity' :
                {
                    'token_identity_slot' : (0,),
                    'token_password' : ('',),
                    'keystore_file' : (c_config.PATH,),
                    'keystore_type' : (('PKCS11', 'PKCS12',),),
                },
            },
            'remote_client_signer_attributes' :
            {
                c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                'send_signing_overrides' : False,
            },
            'remote_signer_attributes' :
            {
                c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                'allow_signing_overrides' : False,
            }
        },
    },

    'post_process' :
    {
        'pil_splitter' : '',
    },

    'data_provisioning' :
    {
        'base_path' : c_config.PATH,
    },

    'images_list' :
    {
        'image' :
        [
            {
                'sign_id' : '',
                'name' : ('',),
                'image_type' : '',
                CONFIG_KEY_CERT_CONFIG : ('',),
                'pil_split' : (False,),
                'meta_build_location' : ('',),
                'general_properties_overrides' :
                {
                    'max_cert_size'                 : ((2048, (),),),
                    'key_size'                      : ((2048, 4096, (),),),
                    'num_certs_in_certchain'        : ((2, 3, (),),),
                    'num_root_certs'                : (0,),
                    'testsig_serialnum'             : ('',),
                    'mrc_index'                     : (0,),
                    'msm_part'                      : (c_config.HEX,),
                    'oem_id'                        : (c_config.HEX,),
                    'model_id'                      : (c_config.HEX,),
                    'sw_id'                         : (c_config.HEX,),
                    'app_id'                        : (c_config.HEX,),
                    'crash_dump'                    : (c_config.HEX,),
                    'soc_hw_version'                : (c_config.HEX,),
                    'soc_vers'                      : ('',),
                    'mask_soc_hw_version'           : (c_config.HEX,),
                    'in_use_soc_hw_version'         : ((0, 1, (),),),
                    'use_serial_number_in_signing'  : ((0, 1, (),),),
                    'serial_number'                 : (c_config.HEX,),
                    'rot_en'                        : (c_config.HEX,),
                    'debug'                         : (c_config.HEX,),
                    'debug_serials'                 :
                    {
                        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                        'serial' : ([''],),
                    },
                    'exponent'                      : ((3, 257, 65537, (),),),
                    'hash_algorithm'                : (('sha1', 'sha256', 'sha384', (),),),
                    'cass_attest_cert'              : (('sha1', 'sha256', (),),),
                    'segment_hash_algorithm'        : (('sha256', 'sha384', (),),),
                    'oem_id_independent'            : ((0, 1, (),),),
                    'secboot_version'               : ('',),
                    'qc_sign'                       : (False,),
                    'oem_sign'                      : (True,),
                    'UIE_key'                       : ('',),
                    'UIE_capability'                : ('',),
                    'UIE_root_key_type'             : ((0, 1, 2, ()),),
                    'revocation_enablement'         : (c_config.HEX,),
                    'activation_enablement'         : (c_config.HEX,),
                    'rsa_padding'                   : (('pss', 'pkcs', (),),),
                    'hmac'                          : (False,),
                    'dsa_type'                      : (('rsa', 'ecdsa', (),),),
                    'ecdsa_curve'                   : (('secp256r1', 'secp384r1', (),),),

                    'object_id' :
                    {
                        c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                        'name'  : '',
                        'min'   : ('',),
                        'max'   : ('',),
                    },
                },
                'post_process_commands' : ('',),
            }
        ]
    }
}

JTAGID_SIGN_IDS = \
{
    '8996' : ['xbl', 'mba', 'device_prog_ddr', 'device_prog_lite', 'prog_emmc_ddr', 'prog_emmc_lite', 'prog_ufs_lite', 'prog_ufs_ddr', 'vip'],
}

IOT_MRC_CHIPSETS = \
[
    '4020',
]


ECDSA_CHIPSETS = \
[
    '9x65',
]

SPECIAL_OVERRIDES = \
{
    'cass_password' : ('',),
}