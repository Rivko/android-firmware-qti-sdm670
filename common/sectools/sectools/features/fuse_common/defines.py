# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the fbc & kpc package."""


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path


#------------------------------------------------------------------------------
# Common constants - Path
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH                        = DIR_PATH

# Default config directory (relative path)
DEFAULT_CONFIG_DIR_REL              = '../../..'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR                  = c_path.join(PACKAGE_PATH, DEFAULT_CONFIG_DIR_REL)

#------------------------------------------------------------------------------
# Common constants - Feature
#------------------------------------------------------------------------------
COMMON_DIR                          = 'common_output'
SEC_DAT                             = 'sec.dat'

#------------------------------------------------------------------------------
# Debug log paths
#------------------------------------------------------------------------------
DEST_DEBUG_DIR                      = 'debug'

DEST_DEBUG_FILE_DATA_MODEL_REPR     = 'data_model_repr_{0}.txt'
DEST_DEBUG_FILE_SECDAT_REPR         = 'secdat_repr.txt'
