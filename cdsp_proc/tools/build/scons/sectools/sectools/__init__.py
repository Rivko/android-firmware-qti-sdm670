#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================


# Name & version of the tool
SECTOOLS_TOOL_NAME = 'SecTools'
SECTOOLS_TOOL_VERSION = '3.13'

import os
import sys

# Add any external modules to the path
CUR_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(CUR_DIR, '..', 'ext'))

# Initialize the chain
from sectools.common.core import chain
chain.initialize()
