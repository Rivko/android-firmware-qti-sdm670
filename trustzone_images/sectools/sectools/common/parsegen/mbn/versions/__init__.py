# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import sys

from . import v3

mbn_versions_path = os.path.realpath(os.path.dirname(__file__))

for module_name in os.listdir(mbn_versions_path):
    if not os.path.isdir(os.path.join(mbn_versions_path, module_name)):
        continue

    if hasattr(sys.modules[__name__], module_name):
        continue

    if module_name in ['__pycache__']:
        continue

    __import__(".".join([__name__, module_name]))
