# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Factory module for crypto discovery.
"""

from sectools.common.core.plugin import SecPluginMgr


class ModuleNotFound(object):
    """Class to be used as return value by discovery implementations to specify
    that the module was not found.
    """
    pass


class DiscoveryFactory(SecPluginMgr):
    """Factory class for the discovery classes. Allows plugging in new classes.
    """

    def __init__(self):
        """Initializes modules variable with the IDs that have registered with
        the factory.
        """
        self.modules = {}

    def supports(self, module_id):
        """Returns True if the module ID (str) is supported for discovery.
        """
        return module_id in self.modules

    def get_impl(self, module_id):
        """Returns the module specific information for the module ID (str)
        provided. Tries to discover the module if it hasn't already been
        discovered.

        :param str module_id: module
        :raises KeyError: if the module is not registered for discovery.
        :raises RuntimeError: if the module could not be discovered.
        """
        # Check if module is supported for discovery
        if module_id not in self.get_map():
            raise KeyError('Module "' + str(module_id) + '" cannot be discovered. ' +
                           'Discovery logic is not available.')

        # Check if module impl is available
        module_impl = self.modules.get(module_id, None)

        # Discover if module has not already been searched
        if module_impl is None:
            module_impl = self.discover(module_id)
            self.modules[module_id] = module_impl

        # Check if module was found
        if module_impl == ModuleNotFound:
            raise RuntimeError('Module "' + str(module_id) + '" was not found.')

        # Return the discovered module
        return module_impl

    def discover(self, module_id):
        """Discovers the module based on the module ID (str) provided. If module
        was discovered before, then rediscovers the module and overrides
        previous module info.

        :param str module_id: module
        :returns: Module specific information & data structure. If module is not
            found, returns ModuleNotFound
        """
        # Discover a module
        module_impl = self.get_map()[module_id]().discover()
        return module_impl


# Create instance of the discovery manager
discovery_factory = DiscoveryFactory()
