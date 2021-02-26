#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
'''OpenSSL discovery implementation.
'''

import os
import re

from sectools.common.crypto.discovery.base import BaseDiscoveryImpl
from sectools.common.crypto.discovery.defines import MOD_OPENSSL
from sectools.common.crypto.discovery.factory import ModuleNotFound
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_platform import bin_names, packaged_bin_folder
from sectools.common.utils.c_process import CoreSubprocess


BINARY_NAME = MOD_OPENSSL  # Name of the openssl binary
OPENSSL_ENV_DIR_TAG = 'OPENSSL_DIR'  # Env variable for the openssl directory
OPENSSL_ENV_CONF_TAG = 'OPENSSL_CONF'  # Env variable for the openssl config file
OPENSSL_CONF_FILE = 'openssl.cfg'  # Default openssl config file

OPENSSL_VERSION_MIN = '1.0.1'

class OpenSSLDiscoveryImpl(BaseDiscoveryImpl):
    '''Registers with the factory to allow discovery of the openssl binary.
    '''

    @classmethod
    def register_to_factory(cls):
        '''See base class documentation.
        '''
        return True

    @classmethod
    def get_discovery_id(self):
        '''See base class documentation.
        '''
        return MOD_OPENSSL

    @classmethod
    def get_version(cls, openssl_bin):
        '''Returns a tuple (major, minor, patch) containing the version info
        for the given openssl bin.
        '''
        retcode, output, error = CoreSubprocess.simpleExecuteCommand(str(openssl_bin) + ' version')
        if retcode != 0:
            raise RuntimeError(BINARY_NAME.title() + ': Could not get version. ' + error + '\n'
                               '' + BINARY_NAME.title() + ' path: ' + openssl_bin)
        version = output.split()[1]
        match = re.match('([\d]+)\.([\d]+)\.([\d]+)', version)
        if not match:
            raise RuntimeError(BINARY_NAME.title() + ': Version string is incorrect: ' + output + '\n'
                               '' + BINARY_NAME.title() + ' path: ' + openssl_bin)
        return (match.group(1), match.group(2), match.group(3))

    @classmethod
    def is_supported_version(cls, openssl_bin):
        '''Returns True if the version of the openssl is supported
        '''
        # Get the version info
        try:
            version = cls.get_version(openssl_bin)
        except Exception as e:
            logger.debug2(str(e))
            return False

        # Check if version is less than minimum
        openssl_version_min = OPENSSL_VERSION_MIN.split('.')
        if (int(openssl_version_min[0]) > int(version[0]) or  # Major
            int(openssl_version_min[1]) > int(version[1]) or  # Minor
            int(openssl_version_min[2]) > int(version[2])):   # Patch
            return False
        return True

    def discover(self):
        '''Searches for the openssl binary in:

        #. The environment using the openssl tag
        #. Prepackaged binary folder
        #. Current path
        #. System path

        :returns str: Path to the openssl binary.
        '''
        module_name = BINARY_NAME.title()
        filenames = bin_names(BINARY_NAME)
        module = ModuleNotFound

        for filename in filenames:
            # Using the environment
            if OPENSSL_ENV_DIR_TAG in os.environ:
                env_module = c_path.join(os.environ[OPENSSL_ENV_DIR_TAG], filename)
                if not c_path.validate_file(env_module):
                    logger.debug2(module_name + ': File from environment does not exist at - ' + env_module)
                elif not self.is_supported_version(env_module):
                    logger.debug2(module_name + ': File from environment is not the correct version - ' + env_module)
                else:
                    module = env_module
                    logger.debug2(module_name + ': Found from environment at - ' + env_module)
                    break

            # Searching in prepacked dir, current dir and system paths
            else:
                folder = packaged_bin_folder
                modules_found = c_path.which(filename, paths=[folder])
                for module_found in modules_found:
                    if not self.is_supported_version(module_found):
                        continue
                    module = module_found
                    conf_file = c_path.join(folder, OPENSSL_CONF_FILE)
                    if c_path.validate_file(conf_file):
                        os.environ[OPENSSL_ENV_CONF_TAG] = conf_file
                    logger.debug2(module_name + ': Found at - ' + module)
                    break
        else:
            logger.debug2(module_name + ': Not Found')

        return module
