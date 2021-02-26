# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Sep 28, 2015

@author: hraghav
'''

def get_signing_config_overrides():
    retval = None
    try:
        from sectools.common.engg.config_overrides import get_config_overrides
        retval = get_config_overrides()
    except ImportError:
        pass
    return retval

