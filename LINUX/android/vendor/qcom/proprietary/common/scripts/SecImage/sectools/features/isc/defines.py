# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the isc package."""


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path


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
DEST_DEBUG_DIR_ENCDEC = 'encdec'

DEST_DEBUG_FILE_PARSEGEN_UNSIGNED = '_unsigned'
DEST_DEBUG_FILE_PARSEGEN_TOSIGN = '_tosign'
DEST_DEBUG_FILE_PARSEGEN_CERT_CHAIN = '_cert_chain'
DEST_DEBUG_FILE_PARSEGEN_SIGNATURE = '_signature'
DEST_DEBUG_FILE_PARSEGEN_INTEGRITY_CHECK = '_integrity_check'
DEST_DEBUG_FILE_PARSEGEN_SIGNED = '_signed'
DEST_DEBUG_FILE_PARSEGEN_ENCRYPTED = '_encrypted'

DEST_DEBUG_FILE_SIGNER_ROOT_CERT = 'root.cer'
DEST_DEBUG_FILE_SIGNER_ROOT_KEY = 'root.key'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_CERT = 'attestation_ca.cer'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_KEY = 'attestation_ca.key'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_CERT = 'attestation.cer'
DEST_DEBUG_FILE_SIGNER_ATTESTATION_KEY = 'attestation.key'
DEST_DEBUG_FILE_SIGNER_SIGNATURE = 'signature.dat'
DEST_DEBUG_FILE_SIGNER_CERT_CHAIN = 'cert_chain.cer'
DEST_DEBUG_FILE_SIGNER_QC_SIGNATURE = 'signature_qc.dat'
DEST_DEBUG_FILE_SIGNER_QC_CERT_CHAIN = 'cert_chain_qc.cer'

DEST_FILE_DECRYPTED = '_decrypted'
DEST_FILE_TO_SIGN = '_tosign'
DEST_DIR_CERT_FOLDER = 'cert'

DEST_DEBUG_FILE_ENCRYPTION_PARAMETERS = "enc_param.debug"
DEST_DEBUG_FILE_ENCRYPTED_L2_KEY = "l2_enc.bin"
DEST_DEBUG_FILE_ENCRYPTED_L3_KEY = "l3_enc.bin"
DEST_DEBUG_FILE_L2_IMAGE_IV = "l2_img_iv.bin"
DEST_DEBUG_FILE_L3_IMAGE_IV = "l3_img_iv.bin"
DEST_DEBUG_FILE_L2_ADD = "l2_aad.bin"
DEST_DEBUG_FILE_L3_ADD = "l3_aad.bin"

AUTHORITY_QC = 'QC'
AUTHORITY_OEM = 'OEM'
AUTHORITIES_SUPPORTED = [AUTHORITY_OEM, AUTHORITY_QC]  # Highest authority @ index 0 -> Lowest authority @ index -1


def highest_authority(auth_enabled_list):
    auths = [k for i, k in enumerate(AUTHORITIES_SUPPORTED) if auth_enabled_list[i]]
    return auths[0] if auths else ''

SECBOOT_VERSION_1_0 = '1.0'
SECBOOT_VERSION_2_0 = '2.0'