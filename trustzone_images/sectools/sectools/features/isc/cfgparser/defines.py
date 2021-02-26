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

import os

from sectools.common.utils import c_config
from sectools.common.utils import c_path
from sectools.common.utils.datautils.enumerated_types import ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG, NOP, TRUE, \
    FALSE, ENABLE, DISABLE, ENABLE_BOUND, RTL_QTI, OTP_OEM, OTP_QTI
from sectools.common.utils.datautils.enumerated_types import ENUMERATED_ENABLE_DISABLE_HEX64_TAG
from sectools.common.utils.datautils.enumerated_types import ENUMERATED_ENABLE_DISABLE_TAG
from sectools.common.utils.datautils.enumerated_types import ENUMERATED_NOP_ENABLE_DISABLE_TAG
from sectools.common.utils.datautils.enumerated_types import ENUMERATED_ROOT_KEY_TAG
from sectools.common.utils.datautils.enumerated_types import set_enumerated_values
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.defines import MAX_CERT_SIZE
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.parsegen.config.defines import CONFIG_KEY_IMAGE_TYPE
from sectools.features.isc.parsegen.config.defines import CONFIG_STRUCTURE_IMAGE_TYPE

ENUMERATED_DEBUG_MAPPING = {
    NOP: 0,
    DISABLE: {
        SECBOOT_VERSION_1_0: 2,
        SECBOOT_VERSION_2_0: 2,
        SECBOOT_VERSION_3_0: 1,
    },
    ENABLE: {
        SECBOOT_VERSION_1_0: 3,
        SECBOOT_VERSION_2_0: 3,
        SECBOOT_VERSION_3_0: 2,
    }
}
ENUMERATED_DEBUG_TYPE = set_enumerated_values(
    ENUMERATED_NOP_ENABLE_DISABLE_TAG, **ENUMERATED_DEBUG_MAPPING)

ENUMERATED_UIE_ROOT_KEY_TYPE = set_enumerated_values(
    ENUMERATED_ROOT_KEY_TAG, **{RTL_QTI: 0, OTP_OEM: 1, OTP_QTI: 2})

ENUMERATED_ENABLE_DISABLE_TYPE = set_enumerated_values(
    ENUMERATED_ENABLE_DISABLE_TAG, **{DISABLE: 0, ENABLE: 1, FALSE: 0, TRUE: 1})

ENUMERATED_ENABLE_DISABLE_MAPPING = {DISABLE: 0, ENABLE: 1, FALSE: 0, TRUE: 1}
ENUMERATED_ENABLE_DISABLE_HEX64_TYPE = set_enumerated_values(
    ENUMERATED_ENABLE_DISABLE_HEX64_TAG, **ENUMERATED_ENABLE_DISABLE_MAPPING)

ENUMERATED_DISABLE_ENABLE_BOUND_MAPPING = {DISABLE: 0, ENABLE: 1, ENABLE_BOUND: 2}
ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TYPE = set_enumerated_values(
    ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG, **ENUMERATED_DISABLE_ENABLE_BOUND_MAPPING)

DEBUG_TAG = "debug"
ROT_EN_TAG = "rot_en"
CRASH_DUMP_TAG = "crash_dump"
UIE_ROOT_KEY_TYPE_TAG = "UIE_root_key_type"
UIE_KEY_SWITCH_ENABLE_TAG = "uie_key_switch_enable"
ACTIVATION_ENABLEMENT_TAG = "activation_enablement"
REVOCATION_ENABLEMENT_TAG = "revocation_enablement"
ROOT_REVOKE_ACTIVATE_ENABLE_TAG = "root_revoke_activate_enable"
OEM_ID_INDEPENDENT_TAG = "oem_id_independent"
IN_USE_SOC_HW_VERSION_TAG = "in_use_soc_hw_version"
USE_SERIAL_NUMBER_IN_SIGNING_TAG = "use_serial_number_in_signing"

ENUMERATED_TAG_MAPPING = {
    DEBUG_TAG: ENUMERATED_NOP_ENABLE_DISABLE_TAG,
    UIE_ROOT_KEY_TYPE_TAG: ENUMERATED_ROOT_KEY_TAG,
    ROT_EN_TAG: ENUMERATED_ENABLE_DISABLE_HEX64_TAG,
    CRASH_DUMP_TAG: ENUMERATED_ENABLE_DISABLE_HEX64_TAG,
    UIE_KEY_SWITCH_ENABLE_TAG: ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG,
    ACTIVATION_ENABLEMENT_TAG: ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG,
    REVOCATION_ENABLEMENT_TAG: ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG,
    ROOT_REVOKE_ACTIVATE_ENABLE_TAG: ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG,
    OEM_ID_INDEPENDENT_TAG: ENUMERATED_ENABLE_DISABLE_TAG,
    IN_USE_SOC_HW_VERSION_TAG: ENUMERATED_ENABLE_DISABLE_TAG,
    USE_SERIAL_NUMBER_IN_SIGNING_TAG: ENUMERATED_ENABLE_DISABLE_TAG,
}

# ------------------------------------------------------------------------------
# Common constants
# ------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = os.path.dirname(os.path.abspath(__file__))

# Name of the root node of any config related objects
ROOTNODE_NAME = 'secimage'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'

# ------------------------------------------------------------------------------
# Auto-generated XML Parser related information
# ------------------------------------------------------------------------------
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

# ------------------------------------------------------------------------------
# Auto-generated OBJ Parser related information
# ------------------------------------------------------------------------------
# OBJ - Basic information
OBJ_PY_FILE_NAME = 'auto_gen_obj_config.py'
OBJ_PY_PATH = c_path.join(PACKAGE_PATH, OBJ_PY_FILE_NAME)

# OBJ - lines at the beginning of the file
OBJ_PREPEND_LINES = '"""\nAuto generated classes\n"""\n\n'

# OBJ - classname generator
OBJ_CLASSNAME_GEN = lambda x: 'Cfg_' + x.replace('_', ' ').title().replace(' ', '_')

# ------------------------------------------------------------------------------
# Config Data Structure
# ------------------------------------------------------------------------------
# cert_config block
CONFIG_KEY_CERT_CONFIG = 'cert_config'


# Set the signers
def set_signers(signers):
    CONFIG_STRUCTURE_GENERAL_PROPERTIES['selected_signer'] = (tuple(signers),)


# general properties block
SHARED_PROPERTIES = {
    'cass_attest_cert': (('sha1', 'sha256', ()),),
    'hash_algorithm': (('sha1', 'sha256', 'sha384', ()),),
    'segment_hash_algorithm': (('sha1', 'sha256', 'sha384', ()),),
    'exponent': ((3, 257, 65537, ()),),
    'rsa_padding': (('pss', 'pkcs', ()),),
    'hmac': (True,),
    'dsa_type': (('rsa', 'ecdsa', ()),),
    'ecdsa_curve': (('secp256r1', 'secp384r1', ()),),
    'max_cert_size': ((MAX_CERT_SIZE, ()),),
    'key_size': ((2048, 4096, ()),),
    'num_certs_in_certchain': ((2, 3, ()),),
    'max_num_certs_in_certchain': ((2, 3, ()),),
    'num_root_certs': (0, ()),
    'max_num_root_certs': ((1, 2, 3, 4, ()),),
    'testsig_serialnum': ('',),
    'mrc_index': (0, ()),
    'msm_part': (c_config.HEX32,),
    'oem_id': (c_config.HEX16,),
    'model_id': (c_config.HEX16,),
    'sw_id': (c_config.HEX64,),
    'anti_rollback_version': (c_config.HEX32,),
    'app_id': (c_config.HEX64,),
    'soc_hw_version': (c_config.HEX32,),
    'soc_vers': (c_config.HEX16LIST,),
    'mask_soc_hw_version': (c_config.HEX16,),
    'serial_number': (c_config.HEX32,),
    'multi_serial_numbers': {
        c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
        'serial': ([c_config.HEX32],),
    },
    'hash_pageseg_as_segment': (False,),
    'secboot_version': ((SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0, ()),),
    'qti_sign': (False,),
    'oem_sign': (True,),
    'UIE_key': ('',),
    'UIE_capability': ('',),
    'UIE_server_cert_path': (c_config.PATH,),
    'UIE_server_url': ('',),
    UIE_KEY_SWITCH_ENABLE_TAG: (ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TYPE,),
    DEBUG_TAG: (ENUMERATED_DEBUG_TYPE,),
    ROT_EN_TAG: (ENUMERATED_ENABLE_DISABLE_HEX64_TYPE,),
    CRASH_DUMP_TAG: (ENUMERATED_ENABLE_DISABLE_HEX64_TYPE,),
    REVOCATION_ENABLEMENT_TAG: (ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TYPE,),
    ACTIVATION_ENABLEMENT_TAG: (ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TYPE,),
    ROOT_REVOKE_ACTIVATE_ENABLE_TAG: (ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TYPE,),
    'multi_image_segment_addr': (c_config.HEX64,),
    'client_id': (c_config.HEX32,),
    'lib_id': (c_config.HEX32,),
    'UIE_feature_id': (c_config.HEX32,),
    'object_id': {
        c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
        'name': '',
        'min': ('',),
        'max': ('',),
    },
}
CONFIG_KEY_GENERAL_PROPERTIES = 'general_properties'
CONFIG_STRUCTURE_GENERAL_PROPERTIES_SPECIFIC = {
    'selected_signer': ((''),),
    'selected_encryptor': (('ssd_encryption', 'unified_encryption_2_0', 'UIE_1_0_L2_1_1', 'UIE_2_0_L2_1_2',
                            ()),),
    'selected_cert_config': '',
    'image_entry': (c_config.HEX32,),
    'cass_capability': ('',),
    IN_USE_SOC_HW_VERSION_TAG: ENUMERATED_ENABLE_DISABLE_TYPE,
    USE_SERIAL_NUMBER_IN_SIGNING_TAG: ENUMERATED_ENABLE_DISABLE_TYPE,
    OEM_ID_INDEPENDENT_TAG: ENUMERATED_ENABLE_DISABLE_TYPE,
    UIE_ROOT_KEY_TYPE_TAG: ENUMERATED_UIE_ROOT_KEY_TYPE,
}
CONFIG_STRUCTURE_GENERAL_PROPERTIES = dict(CONFIG_STRUCTURE_GENERAL_PROPERTIES_SPECIFIC)
CONFIG_STRUCTURE_GENERAL_PROPERTIES.update(SHARED_PROPERTIES)

CONFIG_STRUCTURE_GENERAL_PROPERTIES_OVERRIDES = {
    IN_USE_SOC_HW_VERSION_TAG: (ENUMERATED_ENABLE_DISABLE_TYPE,),
    USE_SERIAL_NUMBER_IN_SIGNING_TAG: (ENUMERATED_ENABLE_DISABLE_TYPE,),
    OEM_ID_INDEPENDENT_TAG: (ENUMERATED_ENABLE_DISABLE_TYPE,),
    UIE_ROOT_KEY_TYPE_TAG: (ENUMERATED_UIE_ROOT_KEY_TYPE,),
}
CONFIG_STRUCTURE_GENERAL_PROPERTIES_OVERRIDES.update(SHARED_PROPERTIES)

# top-level block
CONFIG_KEY = ROOTNODE_NAME
CONFIG_STRUCTURE = \
    {
        'metadata':
            {
                'chipset': '',
                'version': '',
            },

        CONFIG_KEY_GENERAL_PROPERTIES: CONFIG_STRUCTURE_GENERAL_PROPERTIES,

        'parsegen':
            {
                c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
                'image_types_list':
                    {
                        c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
                        CONFIG_KEY_IMAGE_TYPE: [CONFIG_STRUCTURE_IMAGE_TYPE],
                    }
            },

        'signing':
            {
                'signer_attributes':
                    {
                        'cass_signer_attributes':
                            {
                                c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],

                                'server':
                                    {
                                        c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
                                        'host': '',
                                        'port': 0,
                                        'trust_keystore': c_config.PATH,
                                        'trust_keystore_type': (('JKS', 'PKCS12',),),
                                        'trust_keystore_password': ('',),
                                        'host_validation_mode': (('TEST', 'STAGE_PROD',),),
                                    },

                                'user_identity':
                                    {
                                        'token_identity_slot': (0,),
                                        'token_password': ('',),
                                        'keystore_file': (c_config.PATH,),
                                        'keystore_type': (('PKCS11', 'PKCS12',),),
                                    },
                            },
                        'remote_client_signer_attributes':
                            {
                                c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
                                'send_signing_overrides': False,
                            },
                        'remote_signer_attributes':
                            {
                                c_config.DICT.DictHandlerKey_Attr: [c_config.DICT.DictHandlerAttr_Optional],
                                'allow_signing_overrides': False,
                            }
                    },
            },

        'post_process':
            {
                'pil_splitter': '',
            },

        'data_provisioning':
            {
                'base_path': c_config.PATH,
            },

        'images_list':
            {
                'image':
                    [
                        {
                            'sign_id': '',
                            'name': ('',),
                            'image_type': '',
                            CONFIG_KEY_CERT_CONFIG: ('',),
                            'pil_split': (False,),
                            'meta_build_location': ('',),
                            'general_properties_overrides': CONFIG_STRUCTURE_GENERAL_PROPERTIES_OVERRIDES,
                            'post_process_commands': ('',),
                        }
                    ]
            }
    }

JTAGID_SIGN_IDS = {
    '8996': ('xbl', 'mba', 'device_prog_ddr', 'device_prog_lite', 'prog_emmc_ddr',
             'prog_emmc_lite', 'prog_ufs_lite', 'prog_ufs_ddr', 'vip')
}

# These are chipsets that support revocation_enablement and activation_enablement as separate values
MRC_1_0_CHIPSETS = (
    '4020',
)

# These are chipsets that support revocation_enablement and activation_enablement as a combined
# root_revoke_activate_enable value
MRC_2_0_CHIPSETS = (
    'sdm845', '807x', '6290', 'sm8150', 'sdx24', 'sc8180x', 'default', 'sdm670', 'sm6150', 'qcs405',
    'sm8250', 'sdx55', 'sm7150', 'nicobar', '9205', '6390', '7605', 'saipan', 'rennell', 'ipq6018',
    'lahaina', 'kamorta', 'bitra', 'qcn90xx', 'agatti', 'chitwan', 'makena', 'ipq5018', 'olympic', 'cedros', 'mannar',
    'fraser', 'kodiak'
)

SECBOOT_1_IMAGE_TYPES = (
    'mbn_80b',
)

SECBOOT_2_0_SINGLE_SIGN_CHIPSETS = (
    '6290', '6390', '7605', 'qcn90xx'
)

SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS = (
    'sdm845', 'default', 'sdm670'
)

SECBOOT_2_0_CHIPSETS = SECBOOT_2_0_SINGLE_SIGN_CHIPSETS + SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS

SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS = (
    'sm8150', 'sdx24', 'sc8180x', 'sm6150', 'sm8250', 'sdx55', 'sm7150', 'nicobar', '9205', 'qcs405', 'saipan',
    'rennell', 'ipq6018', 'lahaina', 'kamorta', 'bitra', 'agatti', 'chitwan', 'makena', 'olympic', 'cedros',
    'mannar', 'moselle', 'sc7180', 'fraser', 'qtang2', 'kodiak'
)

SECBOOT_3_0_CHIPSETS = SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS

ECDSA_CHIPSETS = (
    '9x65', 'sdx24', 'sm8150', 'sc8180x', 'default', 'sm6150', 'sm8250', 'sdx55', '9205', 'nicobar', 'sm7150', 'saipan',
    'lahaina', 'bitra', 'agatti', 'chitwan', 'makena', 'olympic', 'cedros', 'mannar', 'fraser', 'kodiak'
)

SPECIAL_OVERRIDES = {
    'cass_password': ('',)
}
