# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import subprocess

# Path of the current directory
CUR_DIR = os.path.dirname(os.path.abspath(__file__))

def read_file_contents(file_path):
    """Returns the contents of the file specified by file_path.
    """
    with open(file_path, 'rb') as f:
        return f.read()

def save_file_contents(file_path, data):
    """Stores data in the file specified by file_path.
    """
    with open(file_path, 'wb') as f:
        return f.write(data)

def launch_command(command):
    """Launches the command and returns the output from the command.
    """
    return subprocess.Popen(command, stdout=subprocess.PIPE).communicate()[0]

# Import all the example signers
from plugin.signer.example import hsm_dataprov
from plugin.signer.example import hsm_openssl
