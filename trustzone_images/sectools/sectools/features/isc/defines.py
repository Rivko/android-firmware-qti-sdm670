# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the isc package."""

import os
from sectools.common.utils import c_path

DIR_PATH = os.path.dirname(os.path.abspath(__file__))


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Default config directory (relative path)
DEFAULT_CONFIG_DIR_REL = '../../..'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR = c_path.join(PACKAGE_PATH, DEFAULT_CONFIG_DIR_REL)

#------------------------------------------------------------------------------
# ImageInfo Dest Dir constants
#------------------------------------------------------------------------------
DEST_DEBUG_DIR = 'debug'
DEST_DEBUG_DIR_PARSEGEN = 'parsegen'
DEST_DEBUG_DIR_SIGNER = 'signer'

DEST_DEBUG_FILE_PARSEGEN_UNSIGNED = '_unsigned'
DEST_DEBUG_FILE_PARSEGEN_TOSIGN = '_tosign'
DEST_DEBUG_FILE_PARSEGEN_CERT_CHAIN = '_cert_chain'
DEST_DEBUG_FILE_PARSEGEN_SIGNATURE = '_signature'
DEST_DEBUG_FILE_PARSEGEN_INTEGRITY_CHECK = '_integrity_check'
DEST_DEBUG_FILE_PARSEGEN_SIGNED = '_signed'

DEST_DEBUG_FILE_SIGNER_ROOT_CERT = 'root.cer'
DEST_DEBUG_FILE_SIGNER_ROOT_KEY = 'root.key'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_CERT = 'attestation_ca.cer'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_KEY = 'attestation_ca.key'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CERT = 'attestation.cer'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_KEY = 'attestation.key'
DEST_DEBUG_FILE_SIGNER_SIGNATURE = 'signature.dat'
DEST_DEBUG_FILE_SIGNER_CERT_CHAIN = 'cert_chain.cer'

DEST_FILE_TO_SIGN = '_tosign'
DEST_DIR_CERT_FOLDER = 'cert'

ALIGNED_IMAGE_SIZE = lambda x: x + ((4 - (x % 4)) if x % 4 else 0)
MBN_PTR_MAX = 0xFFFFFFFF
AUTHORITY_OEM = 'OEM'
MAX_SIG_SIZE = 512
MAX_CERT_SIZE = 2048

SECBOOT_VERSION_1_0 = '1.0'

RETURN_CODE_SUCCESS = 0
RETURN_CODE_ERROR = 1
RETURN_CODE_FAILURE = 2