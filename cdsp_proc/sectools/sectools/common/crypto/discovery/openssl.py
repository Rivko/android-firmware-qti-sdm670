# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import os
import re

from sectools.common.crypto.discovery.base import BaseDiscoveryImpl
from sectools.common.crypto.discovery.defines import MOD_OPENSSL
from sectools.common.crypto.discovery.factory import ModuleNotFound
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_platform import bin_names, packaged_bin_folder
from sectools.common.utils.c_process import CoreSubprocess

BINARY_NAME = MOD_OPENSSL               # Name of the openssl binary
OPENSSL_ENV_DIR_TAG = 'OPENSSL_DIR'     # Env variable for the openssl directory
OPENSSL_ENV_CONF_TAG = 'OPENSSL_CONF'   # Env variable for the openssl config file
OPENSSL_CONF_FILE = 'openssl.cfg'       # Default openssl config file

OPENSSL_VERSION_MIN = (1, 0, 1)
OPENSSL_PKEYUTL_FIXED_VERSION_MIN = (1, 0, 2, 'l')


class OpenSSLDiscoveryImpl(BaseDiscoveryImpl):
    """Registers with the factory to allow discovery of the openssl binary.
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
        return MOD_OPENSSL

    @classmethod
    def get_version(cls, openssl_bin):
        """Returns a tuple (major, minor, patch) containing the version info
        for the given openssl bin.
        """
        retcode, output, error = CoreSubprocess.simpleExecuteCommand(str(openssl_bin) + ' version')
        if retcode != 0:
            raise RuntimeError(BINARY_NAME.title() + ': Could not get version. ' + error + '\n' +
                               '' + BINARY_NAME.title() + ' path: ' + openssl_bin)
        version = output.split()[1]
        match = re.match('([\d]+)\.([\d]+)\.([\d]+)([a-z]*)', version)
        if not match:
            raise RuntimeError(BINARY_NAME.title() + ': Version string is incorrect: ' + output + '\n' +
                               '' + BINARY_NAME.title() + ' path: ' + openssl_bin)
        return int(match.group(1)), int(match.group(2)), int(match.group(3)), match.group(4)

    @classmethod
    def is_supported_version(cls, openssl_bin, min_version=OPENSSL_VERSION_MIN):
        """Returns True if the version of the openssl is supported
        """
        # Get the version info
        try:
            openssl_version = cls.get_version(openssl_bin)
        except Exception as e:
            logger.debug2(str(e))
            return False

        # Check if version is less than minimum
        return min_version <= openssl_version

    def discover(self):
        """Searches for the openssl binary in:

        #. The environment using the openssl tag
        #. Prepackaged binary folder
        #. Current path
        #. System path

        :returns str: Path to the openssl binary.
        """
        module_name = BINARY_NAME.title()
        filenames = bin_names(BINARY_NAME)
        module = ModuleNotFound
        logger.debug2('module_name: ' + str(module_name) + ', filenames: ' + str(filenames))

        for filename in filenames:
            # Using the environment
            if OPENSSL_ENV_DIR_TAG in os.environ:
                logger.debug2(str(OPENSSL_ENV_DIR_TAG) + ' tag found in environment')
                env_module = c_path.join(os.environ[OPENSSL_ENV_DIR_TAG], filename)
                logger.debug2('Looking for: ' + str(env_module))
                if not c_path.validate_file(env_module):
                    logger.warning(module_name + ': File from environment does not exist at - ' + env_module)
                elif not self.is_supported_version(env_module):
                    logger.warning(module_name + ': File from environment is not the correct version - ' + env_module)
                else:
                    module = env_module
                    logger.debug2(module_name + ': Found from environment at - ' + env_module)
                    break

            # Searching in prepacked dir, current dir and system paths
            else:
                folder = packaged_bin_folder
                logger.debug2('Looking for: ' + str(filename) + ' in folder: ' + str(folder))
                for module_found in c_path.which_generator(filename, paths=[folder], find_one=False):
                    if not self.is_supported_version(module_found):
                        logger.debug2('Incorrect version: ' + str(module_found))
                        continue
                    module = module_found
                    conf_file = c_path.join(folder, OPENSSL_CONF_FILE)
                    if c_path.validate_file(conf_file):
                        os.environ[OPENSSL_ENV_CONF_TAG] = conf_file
                    logger.debug2(module_name + ': Found at - ' + module)
                    break
                # Check if module is found
                if module != ModuleNotFound:
                    break
        else:
            logger.error(module_name + ': Not Found')

        # Log if permissions are not correct
        if module != ModuleNotFound and not os.access(module, os.X_OK):
            logger.error(module_name + ': Cannot execute. Missing execution permission.')

        return module
