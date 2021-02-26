# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
This module provides a rule base class for components to implement their rules.
"""

import abc

from six import add_metaclass


@add_metaclass(abc.ABCMeta)
class CoreRuleBase(object):
    """
    This is an abstract base class that defines the interface for modules that
    need to implement rules, i.e. configuration rules, feature rules, etc...
    """

    @abc.abstractmethod
    def validate(self, data, data_dict):
        pass
