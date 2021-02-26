# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from . import encdec
# Import built-in encrypted key providers
from sectools.features.isc.encryption_service.unified.encrypted_key_provider import ServerEncryptedKeyProvider

# Try to import oem-defined encrypted key providers
try:
    from plugin import encrypted_key_provider
except ImportError:
    pass
