# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Provides an easy way to create a plugin interface. Requires creation of a
manager that will allow plugins to register.

#. The manager class must derive from :class:`SecPluginMgr`
#. The plugin class must derive from :class:`SecPluginIntf_Abs`
"""

import abc

from sectools.common.utils.c_misc import abstractclassmethod


class _SecPluginMgr_META(abc.ABCMeta):
    """Metaclass to run initialization code at class creation. This can be
    used to run cleanup, etc. Overloads the class instantiation method to create
    a map structure to store the plugin references.

     .. data:: _plugin_map

        ({plugin_id : plugin}) Map of id to the plugin.

    """

    def __init__(self, *args, **kwargs):
        """Initialize the class."""
        abc.ABCMeta.__init__(self, *args, **kwargs)
        self._plugin_map = {}


class _SecPluginIntf_META(abc.ABCMeta):
    """Metaclass to register the plugin with the plugin manager at class
    creation. Overloads the class creation method to run
    :meth:`SecPluginIntf_Abs._register`.
    """

    def __new__(cls, name, bases, namespace):
        """Overload the class creation."""
        retval = abc.ABCMeta.__new__(cls, name, bases, namespace)
        retval._register()
        return retval


class SecPluginMgr(object):
    """This is the base class to be derived by the plugin managers. Contains the
    APIs to allow plugins to register.

    .. note:: The derived class must not specify any metaclass. This class
    contains the capabilities of abc (Abstract base classes).
    """

    __metaclass__ = _SecPluginMgr_META

    #--------------------------------------------------------------------------
    # Public methods.
    #--------------------------------------------------------------------------
    def register(self, plugin_id, plugin):
        """Register a class using the plugin_id provided.

        :param plugin_id: ID to be used for the plugin. This may be any type of object.
        :param plugin: Plugin to be registered

        :raises RuntimeError: If the plugin_id is already registered.
        """
        if plugin_id in self._plugin_map:
            raise RuntimeError('Trying to register: ' + str(plugin) + ' for plugin_id: ' + str(plugin_id) + '\n'
                               '    ' + str(self._plugin_map[plugin_id]) + ' is already registered for this plugin_id')
        self._plugin_map[plugin_id] = plugin

    def get_map(self):
        """({id : plugin}) Returns the map of the plugins."""
        return self._plugin_map


class SecPluginIntf_Abs(object):
    """This is the base class to be derived by the plugins. Plugins must
    override the abstract methods for the registration to work.

    .. note:: The derived class must not specify any metaclass. This class
    contains the capabilities of abc (Abstract base classes).
    """

    __metaclass__ = _SecPluginIntf_META

    #--------------------------------------------------------------------------
    # Private methods.
    #--------------------------------------------------------------------------
    @classmethod
    def _register(cls):
        """Registers the class with the plugin manager if :meth:`is_plugin`
        returns true.

        :raises RuntimeError: If returned plugin manager from :meth:`get_plugin_id`
            is not an instance of :class:`SecPluginMgr`.
        """
        if not cls.is_plugin():
            return

        plugin_id = cls.get_plugin_id()
        plugin_manager = cls.get_plugin_manager()

        if not isinstance(plugin_manager, SecPluginMgr):
            raise RuntimeError(cls.__name__ + ': get_plugin_manager() return value must be an instance of SecPluginMgr. Return value is: ' + str(plugin_manager))
        plugin_manager.register(plugin_id, cls)

    #--------------------------------------------------------------------------
    # Methods that must be overridden by the derived classes.
    #--------------------------------------------------------------------------
    @abstractclassmethod
    def is_plugin(cls):
        """(bool) Returns true if the class should be registered with the plugin
        manager.

        Usecase: Any classes which are used as base class for the actual plugins
        will derive from SecPluginIntf_Abs but return False for this method. The
        actual plugins should return True as they need to be registered.
        """
        return False

    @abstractclassmethod
    def get_plugin_id(cls):
        """Returns the id of the class to be used for plugin registration. This
        must not clash between classes.
        """
        raise RuntimeError(cls.__name__ + ': must implement the class method get_plugin_id.')

    @abstractclassmethod
    def get_plugin_manager(cls):
        """Returns the registration manager for this plugin. This must be an
        instance of :class:`SecPluginMgr`.
        """
        raise RuntimeError(cls.__name__ + ': must implement the class method get_plugin_manager.')
