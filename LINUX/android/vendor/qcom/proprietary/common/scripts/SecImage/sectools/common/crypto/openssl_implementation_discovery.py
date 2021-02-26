# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from collections import namedtuple

from sectools.common.utils.c_logging import logger

# Import the discovery factory
from sectools.common.crypto.discovery import discovery_factory
from sectools.common.crypto.discovery import openssl

# Get the module names
from sectools.common.crypto.discovery.defines import MOD_OPENSSL, \
    MOD_CRYPTO_CBC, MOD_CRYPTO_CCM

def discover_path(module):
    # Discovery logic
    try:
        return discovery_factory.get_impl(module)
    except Exception as e:
        logger.debug(e)
        return None

def get_crypto_paths():
    # Return the discovered paths
    return (discover_path(MOD_OPENSSL),
            discover_path(MOD_CRYPTO_CBC),
            discover_path(MOD_CRYPTO_CCM))

def discover_crypto_implementation():
    import openssl_binary_implementation as obi

    # Import the implementation and set the paths
    obi.openssl_binary_path, obi.cbc_crypto_path, obi.ccm_crypto_path = get_crypto_paths()

    # Set the return object
    openssl_impl = namedtuple('openssl_impl', ['openssl_implementation'])
    openssl_impl.openssl_implementation = obi
    return openssl_impl

def are_available(modules):
    # Check if the modules are available
    module_map = {
                  MOD_OPENSSL: openssl_impl.openssl_implementation.openssl_binary_path != None,
                  MOD_CRYPTO_CBC: openssl_impl.openssl_implementation.cbc_crypto_path != None,
                  MOD_CRYPTO_CCM: openssl_impl.openssl_implementation.ccm_crypto_path != None,
                 }
    missing_modules = [m for m in modules if not module_map[m]]
    if missing_modules:
        if MOD_OPENSSL in missing_modules:
            missing_modules.remove(MOD_OPENSSL)
            missing_modules.insert(0, MOD_OPENSSL + ' v' + openssl.OPENSSL_VERSION_MIN)
        raise RuntimeError(', '.join(missing_modules) + ' ' +
                           ('is' if len(missing_modules) == 1 else 'are') +
                           ' unavailable.')

openssl_impl = discover_crypto_implementation()
OpenSSL_Service = lambda : openssl_impl
