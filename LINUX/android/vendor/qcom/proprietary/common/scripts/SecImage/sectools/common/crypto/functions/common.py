# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Oct 26, 2014

@author: hraghav
'''

import binascii
import os

from sectools.common.utils import c_process


def run_command(cmd, expected_retcode=0):
    retcode, output, stderr_data = c_process.CoreSubprocess.simpleExecuteCommand(cmd)
    if retcode != expected_retcode:
        raise RuntimeError(cmd[0] + ' retcode: ' + str(retcode) + '\n'
                           'Output: ' + stderr_data)
    return output

def gen_random_data(n=16):
    return binascii.hexlify(os.urandom(n))
