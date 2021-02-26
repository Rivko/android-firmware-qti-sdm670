# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.core.plugin import SecPluginMgr, SecPluginIntf_Abs
from sectools.common.utils.c_misc import abstractclassmethod


class FuncNotFound(object):
    pass


class FuncFactory(SecPluginMgr):
    def __init__(self, modules_discovery_mgr):
        self.funcs = {}
        self.modules_discovery_mgr = modules_discovery_mgr

    def supports(self, func):
        return func in self.funcs

    def get_impl(self, func):
        # Check if func is supported for discovery
        if func not in self.get_map():
            raise RuntimeError('"' + str(func) + '" functions cannot be discovered. ' +
                               'Discovery logic is not available.')

        # Check if func impl is available
        func_impl = self.funcs.get(func, None)

        # Discover if func has not already been searched
        if func_impl is None:
            func_impl = self.discover(func)

        # Check if func was found
        if func_impl == FuncNotFound:
            raise RuntimeError('"' + str(func) + '" functions not found.')

        # Return the discovered func
        return func_impl

    def discover(self, func):
        # Discover a func
        for module, impl in self.get_map()[func].modules().items():
            try:
                module_impl = self.modules_discovery_mgr.get_impl(module)
            except Exception:
                continue
            func_impl = impl(module_impl)
            break
        else:
            func_impl = FuncNotFound
        self.funcs[func] = func_impl
        return func_impl


# Initialize to None
func_factory = None


def create_func_factory(discovery_factory):
    # Create instance of the function manager
    global func_factory
    func_factory = FuncFactory(discovery_factory)
    return func_factory


class FuncImplDiscovery(SecPluginIntf_Abs):
    @classmethod
    def get_plugin_id(cls):
        return cls.func()

    # noinspection PyMethodParameters
    @abstractclassmethod
    def func(cls):
        pass

    # noinspection PyMethodParameters
    @abstractclassmethod
    def modules(cls):
        pass

    @classmethod
    def get_plugin_manager(cls):
        """Returns the registration manager for this plugin. This must be an
        instance of :class:`SecPluginMgr`.
        """
        return func_factory
