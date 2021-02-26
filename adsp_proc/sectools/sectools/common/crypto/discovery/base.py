# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Base to be used by any of the discovery modules.
"""

import abc

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.crypto.discovery.factory import discovery_factory
from sectools.common.utils.c_misc import abstractclassmethod


class BaseDiscoveryImpl(SecPluginIntf_Abs):
    """Base class for the discovery logic for various modules.
    """

    # ==========================================================================
    # Mandatory methods to be implemented by derived classes
    # ==========================================================================

    @abstractclassmethod
    def register_to_factory(self):
        """Returns True if this class should be registered with the factory
        to allow discovery of this module.
        """
        pass

    @abstractclassmethod
    def get_discovery_id(self):
        """Returns the ID for discovery for this module.
        """
        pass

    @abc.abstractmethod
    def discover(self):
        """Returns module specific information. This information should be
        sufficient for the user of this module in order to be able to run the
        module.

        For eg, the return value can be:

        #. Handle to a python module.
        #. Path to a binary file.

        Since the return value is module specific, the user of the module
        should know how to use this data.
        """
        pass

    # ==========================================================================
    # Plugin related methods
    # ==========================================================================

    @classmethod
    def get_plugin_manager(cls):
        """Returns the factory for the discovery classes.
        """
        return discovery_factory

    @classmethod
    def get_plugin_id(cls):
        """Returns True if this class should be registered with the factory.
        """
        return cls.get_discovery_id()

    @classmethod
    def is_plugin(cls):
        """Returns True if this class should be registered with the factory.
        """
        return cls.register_to_factory()
