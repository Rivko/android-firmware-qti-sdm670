# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.utils.datautils import DataHandler
from sectools.common.utils.datautils import get_cb_dict


class CfgOverrides(object):

    def __init__(self, structure, excluded_tags=None):
        assert isinstance(structure, dict)
        self.structure = structure
        self.excluded_tags = [] if excluded_tags is None else excluded_tags
        cb_dict = get_cb_dict(path_basepath=os.path.curdir)
        self.root = DataHandler(cb_dict).detail(self.structure)

    def get_properties(self, has_value=False):
        properties = {}
        for tag, detail in self.root.children.items():
            if not detail.is_simple and tag not in self.excluded_tags:
                continue
            if has_value:
                if detail.value_set:
                    properties[tag] = detail
            else:
                properties[tag] = detail
        return properties
