# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
:Authors: sectools

Main interface to the fuseblower functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

from sectools.common.core.feature import FeatureBase_Abs
from sectools.common.parsegen.secdat.segment.defines import SEG_TYPE_EFUSE,\
    SEG_TYPE_ROT
from sectools.common.utils.c_logging import logger
from sectools.features.fbc.parsegen import fb_config
from sectools.features.fuse_common.fcc import FuseCommonCore


FUSEBLOWER_ID = 'fuseblower'


class FuseBlowerCore(FeatureBase_Abs, FuseCommonCore):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).
    """
    FEATURE_DIR         = 'fuseblower_output'
    TOOL_NAME           = 'fbc'
    SECDAT_SEGMENT_TYPE = SEG_TYPE_EFUSE
    CONFIG_MODULE       = fb_config

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None):

        FuseCommonCore.set_config_paths(self, oem_config_path, qti_config_path, ui_config_path, user_config_path)
        # Log the path update
        logger.info('Config paths set to: ' + '\n'
                    '    ' + 'OEM: ' + str(oem_config_path) + '\n'
                    '    ' + 'QTI: ' + str(qti_config_path) + '\n'
                    '    ' + 'UI: ' + str(ui_config_path) + '\n'
                    '    ' + 'USER: ' + str(user_config_path))

    @classmethod
    def _feature_id(cls):
        return FUSEBLOWER_ID

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def _ui_interface(cls):
        from sectools.features.fbc.ui.intf import fuse_blower_ui_intf
        return fuse_blower_ui_intf


class RootOfTrustCore(FuseCommonCore):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).
    """
    FEATURE_DIR         = 'rootoftrust_output'
    TOOL_NAME           = 'rot'
    SECDAT_SEGMENT_TYPE = SEG_TYPE_ROT
    CONFIG_MODULE       = fb_config

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None):

        FuseCommonCore.set_config_paths(self, oem_config_path, qti_config_path, ui_config_path, user_config_path)
        # Log the path update
        logger.info('Config paths set to: ' + '\n'
                    '    ' + 'ROT_OEM: ' + str(oem_config_path) + '\n'
                    '    ' + 'UI: ' + str(ui_config_path) + '\n'
                    '    ' + 'ROT_USER: ' + str(user_config_path))
