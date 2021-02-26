# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import sys
from glob import glob

from sectools.common.core.plugin import SecPluginMgr


class ParsegenMgr(SecPluginMgr):

    def get_parser(self, file_type):
        parsegens = self.get_map()
        if file_type not in parsegens:
            raise RuntimeError(
                'Parser unavailable for plugin ID: ' + str(file_type) + '\n'
                '    ' + 'Available plugin IDs are: ' + str(parsegens.keys()))
        return parsegens[file_type]


parsegen_mgr = ParsegenMgr()


def import_object(object_name):
    if object_name in ["__init__", '__pycache__']:
        return
    object_path = ".".join([__name__, object_name])
    if not hasattr(sys.modules[__name__], object_name):
        __import__(object_path)
    return sys.modules[object_path]


for py_file_name in glob(os.path.join(os.path.dirname(__file__), "*.py")):
    import_object(os.path.splitext(os.path.basename(py_file_name))[0])

DIR_PATH = os.path.realpath(os.path.dirname(__file__))

for package in os.listdir(DIR_PATH):
    if not os.path.isdir(os.path.join(DIR_PATH, package)):
        continue
    import_object(package)
