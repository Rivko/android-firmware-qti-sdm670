# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
This module provides several methods to query an android device.
"""
import os
import re
import subprocess
import time

from six import ensure_str

from sectools.common.utils.c_logging import logger

class AndroidDevice():
    """
    Class to provide an interface and some useful commands to communicate
    with the phone.
    """

    def __init__(self):
        """ Initializes internal variables """
        pass

    @classmethod
    def getDevicesList(self):
        """ Returns list of device serial number of all android devices. """
        process = subprocess.Popen('adb devices', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()

        slist = re.findall("List of devices attached(.*)", ensure_str(stdout) , re.M | re.S)
        devices = re.findall("(\S+).*?\n", slist[0], re.M | re.S)

        return devices

    @classmethod
    def getDeviceStatus(self):
        """Return device status, return True if at least one device is connected
        """
        """
        devices = self.getDevicesList()
        if len(devices) == 0:
            retval = False
        else:
            retval = True

        return retval"""
        retcode = os.system('adb root && adb wait-for-device')
        if retcode != 0:
            retval = False
        else:
            retval = True

        return retval

    @classmethod
    def restartDevice(self):
        """ Returns true if device restarted and connected
        """
        logger.info('Sending reboot command')
        if self.getDeviceStatus():
            process = subprocess.Popen('adb reboot && adb wait-for-device', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        elif self.checkFastboot():
            process = subprocess.Popen('fastboot reboot', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        else:
            raise RuntimeError('device is not boot up and not in fastboot mode, please reboot the device manually')
        stdout, stderr = process.communicate()
        retcode = process.returncode
        if retcode != 0:
            logger.error(ensure_str(stderr))
            return False
        else:
            logger.info('reboot command completed, wait for 30 seconds to check if device boot up')
            time.sleep(30)
            return self.getDeviceStatus()

    @classmethod
    def checkFastboot(self):
        process = subprocess.Popen('fastboot devices', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr  = process.communicate()
        if process.returncode == 0 and stdout !='':
            return True
        else:
            logger.error(ensure_str(stderr))
            return False

    @classmethod
    def fastbootDevice(self):
        """ Returns true if device is in fastboot mode
        """
        process = subprocess.Popen('adb reboot bootloader', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr  = process.communicate()
        if process.returncode == 0:
            time.sleep(5)
            return self.checkFastboot()
        else:
            logger.error(ensure_str(stderr))
            return False

