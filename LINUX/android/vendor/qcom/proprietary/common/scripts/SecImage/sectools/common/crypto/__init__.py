# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import __secfile__
__secfile__.init()

from sectools.common.utils.c_logging import logger

# Import the discovery factory
from sectools.common.crypto.discovery import discovery_factory

# Import all the discovery implementations
from sectools.common.crypto.discovery import openssl
from sectools.common.crypto.discovery import m2crypto
from sectools.common.crypto.discovery import crypto_cbc
from sectools.common.crypto.discovery import crypto_ccm

# Import the functions factory
from sectools.common.crypto import functions
from sectools.common.crypto.functions import create_func_factory

# Initialize and get the function factory
func_factory = create_func_factory(discovery_factory)

# Set utils if supported
from sectools.common.crypto.functions.utils import openssl as _utils_openssl
try:
    def _get_utils_functions():
        '''Returns the utils functions class.
        @rtype: UtilsBase
        '''
        return func_factory.get_impl(functions.FUNC_UTILS)
    utils = _get_utils_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set RSA if supported
from sectools.common.crypto.functions.rsa import openssl as _rsa_openssl
try:
    def _get_rsa_functions():
        '''Returns the rsa functions class.
        @rtype: RsaBase
        '''
        return func_factory.get_impl(functions.FUNC_RSA)
    rsa = _get_rsa_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set ECDSA if supported
from sectools.common.crypto.functions.ecdsa import openssl as _ecdsa_openssl
try:
    def _get_ecdsa_functions():
        '''Returns the ecdsa functions class.
        @rtype: EcdsaBase
        '''
        return func_factory.get_impl(functions.FUNC_ECDSA)
    ecdsa = _get_ecdsa_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set AES_CBC if supported
from sectools.common.crypto.functions.aes_cbc import openssl as _aes_cbc_openssl
try:
    def _get_aes_cbc_functions():
        '''Returns the aes cbc functions class.
        @rtype: AesCbcBase
        '''
        return func_factory.get_impl(functions.FUNC_AES_CBC)
    aes_cbc = _get_aes_cbc_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set AES_CBC_CTS if supported
from sectools.common.crypto.functions.aes_cbc_cts import crypto_cbc as _aes_cbc_cts_crypto_cbc
try:
    def _get_aes_cbc_cts_functions():
        '''Returns the aes cbc cts functions class.
        @rtype: AesCbcCtsBase
        '''
        return func_factory.get_impl(functions.FUNC_AES_CBC_CTS)
    aes_cbc_cts = _get_aes_cbc_cts_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set AES_CCM if supported
from sectools.common.crypto.functions.aes_ccm import crypto_ccm as _aes_ccm_crypto_ccm
try:
    def _get_aes_ccm_functions():
        '''Returns the aes ccm functions class.
        @rtype: AesCcmBase
        '''
        return func_factory.get_impl(functions.FUNC_AES_CCM)
    aes_ccm = _get_aes_ccm_functions()
except Exception as e:
    logger.debug(e)
    pass

# Set cert if supported
from sectools.common.crypto.functions.cert import openssl as _cert_openssl
try:
    def _get_cert_functions():
        '''Returns the cert functions class.
        @rtype: CertBase
        '''
        return func_factory.get_impl(functions.FUNC_CERT)
    cert = _get_cert_functions()
except Exception as e:
    logger.debug(e)
    pass

# Public functions
def supports(functions):
    unsupported = []
    for func in functions:
        try:
            func_factory.get_impl(func)
        except Exception:
            unsupported.append(func)
    if unsupported:
        return False, ', '.join(unsupported) + ' not supported.'
