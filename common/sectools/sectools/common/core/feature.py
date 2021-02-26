# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Classes to manage features under sectools. Provides the base class that all
features must inherit from.
"""

from sectools.common.core.auth import AuthParams
from sectools.common.core.plugin import SecPluginMgr, SecPluginIntf_Abs
from sectools.common.utils.c_misc import abstractclassmethod


class FeatureManager(SecPluginMgr):
    """
    #. Provides clients the ability to query for the sectools features that are
    available.
    #. Acts as endpoint for registration of the pluggable features.
    """

    #--------------------------------------------------------------------------
    # Public Methods/Properties
    #--------------------------------------------------------------------------
    def get_available_features(self):
        """Returns a list of feature classes that are registered.
        """
        return self.get_map().values()

# Global feature manager instance.
feature_manager = FeatureManager()


class FeatureBase_Abs(SecPluginIntf_Abs):
    """Base class for all the feature classes. Automatically registers the
    derived class with the feature manager. The derived classes must implement
    the abstract methods.
    """

    #--------------------------------------------------------------------------
    # Public Methods/Properties
    #--------------------------------------------------------------------------
    def authenticate(self, auth_params):
        if not isinstance(auth_params, AuthParams):
            raise RuntimeError('Auth Params must be of type "AuthParams". Auth Params type is ' + str(type(auth_params)))
        return True

    @classmethod
    def get_feature_id(cls):
        """(str) Returns the feature id corresponding to this feature. Performs
        sanity on the feature id provided by the derived class.
        """
        feature_id = cls._feature_id()
        if not isinstance(feature_id, str):
            raise RuntimeError(cls.__name__ + ': Feature ID must be of type "str". Feature id type is ' + str(type(feature_id)))
        elif not feature_id:
            raise RuntimeError(cls.__name__ + ': Feature ID must not be empty')
        return feature_id

    @classmethod
    def get_ui_interface(cls):
        """Returns the ui interface corresponding to this feature. If the
        feature does not implement ui interface, the return value is None.
        Performs sanity on the ui interface provided by the derived class.
        """
        ui_interface = cls._ui_interface()
        if ui_interface is not None:
            from sectools.intf.ui.base import FeatureUIIntf_Abs
            if not isinstance(ui_interface, FeatureUIIntf_Abs):
                raise RuntimeError(cls.__name__ + ': UI Interface must be an instance of "FeatureUIIntf_Abs". UI Interface is: ' + str(ui_interface))
        return ui_interface

    @abstractclassmethod
    def is_plugin(cls):
        """(bool) Returns true if the class should be registered with the
        feature manager. If the derived class is a feature, it must return True.
        If the derived class is a base class for other derived feature classes,
        it must return False.
        """
        return False

    @classmethod
    def get_plugin_id(cls):
        """(str) Returns the plugin ID as required by :class:`SecPluginIntf_Abs`.
        This is the same as the feature id. This should not be modified.
        """
        return cls._feature_id()

    @classmethod
    def get_plugin_manager(cls):
        """Returns the plugin manager instance as required by
        :class:`SecPluginIntf_Abs` against which all the features will register.
        This should not be modified.
        """
        return feature_manager

    #--------------------------------------------------------------------------
    # Private Methods/Properties
    #--------------------------------------------------------------------------
    @abstractclassmethod
    def _feature_id(cls):
        """(str) Returns the feature id. This must be a non empty string.
        """
        raise RuntimeError('Class ' + cls.__name__ + ' must implement the class method _feature_id.')

    @classmethod
    def _ui_interface(cls):
        """Returns the ui interface if implemented by the feature. If
        implemented, the return value should be an instance of FeatureUIIntf_Abs.
        """
        return None

