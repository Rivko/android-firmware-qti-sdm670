# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

DIR_PATH = None

def init():
    global DIR_PATH
    DIR_PATH = os.path.dirname(os.path.abspath(__file__))