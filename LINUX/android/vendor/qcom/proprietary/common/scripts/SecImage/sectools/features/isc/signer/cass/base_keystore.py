# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta, abstractproperty

from singleton import Singleton


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
