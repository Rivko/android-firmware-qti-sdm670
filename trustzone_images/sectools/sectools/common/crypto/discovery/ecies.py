# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""ECIES discovery implementation.
"""

import os

from sectools.common.crypto.discovery.base import BaseDiscoveryImpl
from sectools.common.crypto.discovery.defines import MOD_ECIESAD
from sectools.common.crypto.discovery.factory import ModuleNotFound
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_platform import bin_names, alternate_bin_names, packaged_bin_folder

BINARY_NAME = 'ecies'


class EciesDiscoveryImpl(BaseDiscoveryImpl):
    """Registers with the factory to allow discovery of ecies binary.
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
        return MOD_ECIESAD

    def discover(self):
        """Searches for the ecies binary in the predefined packaged path.

        :returns str: Path to the ecies binary.
        """
        module_name = BINARY_NAME.title()
        filenames = bin_names(BINARY_NAME)
        filenames += alternate_bin_names(BINARY_NAME)
        module = ModuleNotFound

        for filename in filenames:
            file_path = c_path.join(packaged_bin_folder, filename)
            if c_path.validate_file(file_path):
                module = file_path
                logger.debug2(module_name + ': Found at - ' + module)
                break
        else:
            logger.debug2(module_name + ': Not Found')

        # Log if permissions are not correct
        if module != ModuleNotFound and not os.access(module, os.X_OK):
            logger.error(module_name + ': Cannot execute. Missing execution permission.')

        return module
