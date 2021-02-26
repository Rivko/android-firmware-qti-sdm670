# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created on Mar 18, 2013

@author: francisg

This module provides a rule base class for components to implement their rules.
"""

import abc


class CoreRuleBase(object):
    """
    This is an abstract base class that defines the interface for modules that
    need to implement rules, i.e. configuration rules, feature rules, etc...
    """

    __metaclass__ = abc.ABCMeta

    def __init__(self):
        pass

    @abc.abstractmethod
    def validate(self, data, data_dict):
        pass
