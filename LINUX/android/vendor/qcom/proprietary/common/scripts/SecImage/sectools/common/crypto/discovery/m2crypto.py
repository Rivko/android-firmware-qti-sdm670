# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""M2Crypto discovery implementation.
"""

from sectools.common.crypto.discovery.base import BaseDiscoveryImpl
from sectools.common.crypto.discovery.defines import MOD_M2CRYPTO
from sectools.common.crypto.discovery.factory import ModuleNotFound
from sectools.common.utils.c_logging import logger


MODULE_NAME = 'M2Crypto'


class M2CryptoDiscoveryImpl(BaseDiscoveryImpl):
    """Registers with the factory to allow discovery of M2Crypto module.
    """

    @classmethod
    def register_to_factory(cls):
        """See base class documentation.
        """
        return True

    @classmethod
    def get_discovery_id(self):
        """See base class documentation.
        """
        return MOD_M2CRYPTO

    def discover(self):
        """Searches for the M2Crypto library in the python package.

        :returns module: M2Crypto python module
        """
        module_name = MODULE_NAME
        module = ModuleNotFound
        try:
            import M2Crypto as module
            logger.debug2(module_name + ': Found at - ' + str(module))
        except ImportError as e:
            logger.debug2(module_name + ': Not Found - ' + str(e))
        return module
