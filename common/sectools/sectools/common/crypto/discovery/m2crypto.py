# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
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
    def get_discovery_id(cls):
        """See base class documentation.
        """
        return MOD_M2CRYPTO

    def discover(self):
        """Searches for the M2Crypto library in the python package.

        :returns module: M2Crypto python module
        """
        module_name = MODULE_NAME
        try:
            # noinspection PyPackageRequirements
            import M2Crypto
            logger.debug2(module_name + ': Found at - ' + str(M2Crypto))
        except ImportError as e:
            # noinspection PyPep8Naming
            M2Crypto = ModuleNotFound
            logger.debug2(module_name + ': Not Found - ' + str(e))
        module = M2Crypto
        return module
