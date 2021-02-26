# ===========================================================================
# FILE: 
#    checkbuild.py
#
# DESCRIPTION:
#    Implementation to verify the below builds
#     1. META BUILD : Default Images, which are integrated by target team in the meta_build
#     2. CDSP BUILD : CRM ADSP BUILD flashed by User, not from meta
#     3. ADSP BUILD : CRM ADSP BUILD flashed by User, not from meta
#
# Copyright (C) 2016 Qualcomm Technologies Incorporated. All rights reserved.
#                    QUALCOMM Proprietary/GTDR
#
# All data and information contained in or disclosed by this document is
# confidential and proprietary information of Qualcomm Technologies Incorporated and all
# rights therein are expressly reserved.  By accepting this material the
# recipient agrees that this material and the information contained therein
# is held in confidence and in trust and will not be used, copied, reproduced
# in whole or in part, nor its contents revealed in any manner to others
# without the express written permission of Qualcomm Technologies Incorporated.
# ===========================================================================
#
# ---------------------------------------------------------------------------
#  NOTE: META Image and CRM Images might be different
# ---------------------------------------------------------------------------

#!/usr/local/bin/python
import os
import sys
import subprocess
import argparse

def runadbcommand(command, shell_val=True):
	try:
		info = subprocess.Popen(command, stdout= subprocess.PIPE, stderr=subprocess.STDOUT, shell=shell_val)
		print info.stdout.read()
	except subprocess.CalledProcessError:
		print "Subprocess called error" # handle errors in the called executable
	except OSError:
		print "os error" # executable not found

def meta_build():
    adb_root = "adb wait-for-device root"
    runadbcommand(adb_root)
    cmd = 'adb shell cat /firmware/verinfo/ver_info.txt'
    Meta_build = subprocess.check_output(cmd.split())
    print "==== Meta Build has following Images ==== \n"
    print Meta_build
    return 1
	
def cdsp_build():
    adb_root = "adb wait-for-device root"
    runadbcommand(adb_root)
    build_no = subprocess.check_output(["adb", "shell", "cd /sys/devices/soc0 && echo 16 > select_image && echo `cat image_version`"])
    print "==== CDSP Build " + build_no[3:].rstrip() + " ==== \n"
    return 1
	
def adsp_build():
    adb_root = "adb wait-for-device root"
    runadbcommand(adb_root)
    build_no = subprocess.check_output(["adb", "shell", "cd /sys/devices/soc0 && echo 12 > select_image && echo `cat image_version`"])
    print "==== ADSP Build " + build_no[3:].rstrip() + " ==== \n"
    print "Done !!!"
    return 1
	
if __name__ == '__main__':
    meta_build()
    cdsp_build()
    adsp_build()
    os.system("pause") 
