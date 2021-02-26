# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import sys
from abc import ABCMeta, abstractproperty

from sectools.common.crypto import utility_functions
from sectools.common.utils import c_path, c_misc
from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer import signerutils
from sectools.features.isc.signer.base_signer import ConfigError
from sectools.features.isc.signer.cass import defines
from singleton import Singleton


class IdentityKeystoreFactory(object):

    @classmethod
    def get_key_store(cls, user_identity):
        keystore_type = user_identity.keystore_type
        if keystore_type == HardwareIdentityKeystore.KEYSTORE_TYPE:
            identity_keystore = HardwareIdentityKeystore(user_identity)
        elif keystore_type == SoftwareIdentityKeystore.KEYSTORE_TYPE:
            identity_keystore = SoftwareIdentityKeystore(user_identity)
        else:
            raise RuntimeError("Unsupported keystore_type = {0}".format(keystore_type))
        return identity_keystore


class BaseKeystore(object):
    __metaclass__ = ABCMeta
    __metaclass__ = Singleton

    def __init__(self, password):
        self.password = password

    @abstractproperty
    def file(self):
        """
        Returns the keystore file location
        """
        raise RuntimeError('Method must be implemented by the derived class')

    @abstractproperty
    def type(self):
        """
        Returns the keystore type
        """
        raise RuntimeError('Method must be implemented by the derived class')

    @property
    def password(self):
        return self._password

    @password.setter
    def password(self, value):
        if value:
            self._password = value
        elif hasattr(self, "_password") is False or not self._password:
            self._password = self._get_user_password()

    def _get_user_password(self):
        import getpass
        user_passphrase = getpass.getpass(self.MESG_PROMPT_PASSWORD)
        return user_passphrase

    def reset_password(self):
        self._password = ""


class HardwareIdentityKeystore(BaseKeystore):
    KEYSTORE_TYPE = "PKCS11"
    PKCS11_CFG_TEMPLATE = c_path.normalize(os.path.join(defines.CASS_CLIENT_REFAPP_INPUT_DIR, 'pkcs11.cfg.template'))
    MESG_PROMPT_PASSWORD = '\nPlease enter the password for the USB token (user identity):'
    MESG_TOKEN_DRIVER_INVALID = "Token driver path {0} is invalid! Please install Safenet driver."

    def __init__(self, user_identity_config):
        self.file = None
        self._user_identity = user_identity_config
        BaseKeystore.__init__(self, user_identity_config.token_password)
        self.token_driver_home = user_identity_config.token_driver_home
        self.file = self._generate_pkcs11_cfg(self.token_driver_home)

    @property
    def file(self):
        return self._file

    @file.setter
    def file(self, value):
        self._file = value

    @property
    def type(self):
        return self.KEYSTORE_TYPE

    @property
    def token_driver_home(self):
        return self._token_driver_home

    @token_driver_home.setter
    def token_driver_home(self, token_driver_home_config):
        if sys.platform.startswith('linux'):
            self._token_driver_home = token_driver_home_config.linux
        else:
            self._token_driver_home = token_driver_home_config.windows

        if c_path.validate_file(self._token_driver_home) is False:
            raise ConfigError(self.MESG_TOKEN_DRIVER_INVALID.format(self._token_driver_home))

    def _generate_pkcs11_cfg(self, token_driver_home):
        pkcs11_cfg_template_data = c_misc.load_data_from_file(self.PKCS11_CFG_TEMPLATE)
        pkcs11_cfg_data = signerutils.macro_replace(pkcs11_cfg_template_data,
                                  "token_driver_home",
                                  token_driver_home,
                                  isMandatory=True)
        return utility_functions.store_data_to_temp_file(pkcs11_cfg_data)

    def release(self):
        if self.file and os.path.isfile(self.file):
            logger.debug("Deleting temporary file: " + self.file)
            os.unlink(self.file)

    def __del__(self):
        self.release()


class SoftwareIdentityKeystore(BaseKeystore):
    KEYSTORE_TYPE = "PKCS12"
    MESG_PROMPT_PASSWORD = '\nPlease enter the password for the softkey (user identity):'

    def __init__(self, user_identity_config):
        self._user_identity = user_identity_config
        BaseKeystore.__init__(self, user_identity_config.token_password)
        self._file = c_path.normalize(user_identity_config.keystore_file)

    @property
    def file(self):
        return self._file

    @property
    def type(self):
        return self.KEYSTORE_TYPE

    @property
    def token_driver_home(self):
        return None


class TrustKeystore(BaseKeystore):
    MESG_PROMPT_PASSWORD = '\nPlease enter the password for the trust keystore (server authentication):'

    def __init__(self, filepath=None, password="", keystoreType="JKS"):
        BaseKeystore.__init__(self, password)
        self._file = c_path.normalize(filepath)
        self._type = keystoreType

    @property
    def file(self):
        return self._file

    @property
    def type(self):
        return self._type
