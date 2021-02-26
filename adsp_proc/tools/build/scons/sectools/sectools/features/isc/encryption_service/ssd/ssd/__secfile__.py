#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os

DIR_PATH = None

def init():
    global DIR_PATH
    DIR_PATH = os.path.dirname(os.path.abspath(__file__))

