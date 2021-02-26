#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
"""Crypto CBC discovery implementation.
"""

from sectools.common.crypto.discovery.base import BaseDiscoveryImpl
from sectools.common.crypto.discovery.defines import MOD_CRYPTO_CBC
from sectools.common.crypto.discovery.factory import ModuleNotFound
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_platform import bin_names, packaged_bin_folder


BINARY_NAME = 'crypto_cbc'


class CryptoCbcDiscoveryImpl(BaseDiscoveryImpl):
    """Registers with the factory to allow discovery of crypto cbc binary.
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
        return MOD_CRYPTO_CBC

    def discover(self):
        """Searches for the crypto cbc binary in the predefined packaged path.

        :returns str: Path to the crypto cbc binary.
        """
        module_name = BINARY_NAME.title()
        filenames = bin_names(BINARY_NAME)
        module = ModuleNotFound

        for filename in filenames:
            file_path = c_path.join(packaged_bin_folder, filename)
            if c_path.validate_file(file_path):
                module = file_path
                logger.debug2(module_name + ': Found at - ' + module)
                break
        else:
            logger.debug2(module_name + ': Not Found')

        return module
