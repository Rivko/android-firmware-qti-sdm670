#******************************************************************************
#  @file    activity_trigger_stress_test.py
#  @brief   stress tests activity trigger
#
#  ---------------------------------------------------------------------------
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
# ******************************************************************************/

"""Script to launch all the packages followed by one real package to
check the behaviour in real world situations.

"""

import os
import sys
import subprocess as sp
import signal
import time
import shlex
from threading import Thread

def get_packages_from_list():
    pkg_list = []
    tmp_pkg_list = [
    "package:com.google.android.googlequicksearchbox",
    "package:com.android.providers.calendar",
    "package:com.android.providers.media",
    "package:com.android.wallpapercropper",
    "package:com.android.documentsui",
    "package:com.android.htmlviewer",
    "package:com.qualcomm.svi",
    "package:com.android.companiondevicemanager",
    "package:com.android.providers.downloads",
    "package:com.qualcomm.qti.callenhancement",
    "package:com.qualcomm.pipcamera",
    "package:com.android.browser",
    "package:com.google.android.configupdater",
    "package:com.android.defcontainer",
    "package:com.android.providers.downloads.ui",
    "package:com.android.vending",
    "package:org.codeaurora.bluetooth",
    "package:com.android.contacts",
    "package:org.codeaurora.gallery",
    "package:com.qualcomm.qti.callfeaturessetting",
    "package:com.qti.qualcomm.deviceinfo",
    "package:com.android.launcher3",
    "package:org.codeaurora.ims",
    "package:com.android.calendar",
    "package:com.android.providers.settings",
    "package:com.android.printspooler",
    "package:com.android.musicfx",
    "package:com.google.android.webview",
    "package:com.qualcomm.qti.simsettings",
    "package:com.qualcomm.qti.seccamsample",
    "package:com.google.android.syncadapters.contacts",
    "package:com.android.facelock",
    "package:com.android.chrome",
    "package:com.android.dialer",
    "package:com.qualcomm.qti.ims.tests",
    "package:com.cyanogenmod.filemanager",
    "package:com.android.inputmethod.latin",
    "package:com.google.android.feedback",
    "package:com.google.android.printservice.recommendation",
    "package:com.google.android.syncadapters.calendar",
    "package:com.qualcomm.qti.carrierconfigure",
    "package:com.qualcomm.settings",
    "package:com.android.providers.partnerbookmarks",
    "package:com.android.smspush",
    "package:com.qualcomm.qti.ltedirectdiscoverytest",
    "package:com.android.wallpaper.livepicker",
    "package:com.android.bookmarkprovider",
    "package:com.android.settings",
    "package:com.android.calculator2",
    "package:com.qualcomm.simcontacts",
    "package:com.caf.fmradio",
    "package:com.android.email",
    "package:com.android.music",
    "package:com.android.phone",
    "package:com.android.audiosettings",
    "package:com.qualcomm.qti.roamingsettings",
    "package:com.android.deskclock",
    "package:com.android.exchange",
    "package:com.qualcomm.qti.networksetting",
    "package:com.android.MultiplePdpTest",
    "package:com.android.providers.contacts"]

    for x in tmp_pkg_list:
        x = x.strip()
        if x.startswith("package:"):
            pkg_name = x.replace("package:", "")
            pkg_list.append(pkg_name)
    return pkg_list

def get_packages(pkg_list_file):
    pkg_list = []
    with open(pkg_list_file) as fp:
        for x in fp:
            x = x.strip()
            if x.startswith("package:"):
                pkg_name = x.replace("package:", "")
                pkg_list.append(pkg_name)
    return pkg_list


def get_packages_from_device():
    temp_file_name = "temp_pkg_list.txt"
    pkg_list = []
    temp_output = open(temp_file_name, "wb")
    p1 = sp.Popen("adb shell pm list packages",
                  stdout=temp_output, stderr=temp_output, shell=True)
    p1.wait()
    temp_output.close()

    pkg_list = get_packages(temp_file_name)
    return pkg_list


def clear_pkg(pkg_name):
    sp.Popen("adb shell am force-stop " + pkg_name, shell=True).wait()
    return


def clear_all_apps(packages):
    """Kill all the apps in packages list."""
    print("Clearing all the apps which were launched")
    for pkg_name in packages:
        clear_pkg(pkg_name)
    return


def clean_and_start_collecting_adb_logcat(x):
    """Clean adb logcat and start logging in file named x + _log.txt."""
    sp.Popen("adb logcat -c").wait()
    fp_out = open(x + "_log.txt", "wb")
    log_process = sp.Popen(["adb", "logcat"], stdout=fp_out)
    return log_process, fp_out


def launch_pkg(pkg_name):
    launch_cmd = "adb shell monkey -p "
    activity = " -c android.intent.category.LAUNCHER 1"
    result = -1
    try:
        res = sp.check_output(launch_cmd + pkg_name + activity)
        result = 0
    except sp.CalledProcessError as result:
        print "error code", result.returncode, result.output
    return result


def launch_except_one(packages, pkg_name_to_exclude):
    """Launch all the packeges other than pkg_name_to_exclude. """
    for pkg_name in packages:
        time.sleep(1)
        if pkg_name_to_exclude == pkg_name:
            continue
        result = launch_pkg(pkg_name)
        if (result):
            print("Error: failed to launch pkg '%s'" % (pkg_name))
    return


def finish_logging(log_process, fp_out):
    log_process.kill()
    fp_out.close()
    return


def launch_packages(packages):
    """Launch all the packages in list."""
    count = 0
    for pkg_name in packages:
        log_process, fp_out = clean_and_start_collecting_adb_logcat(pkg_name)

        launch_except_one(packages, pkg_name)

        # Wait for 30 seconds to settle down other launches.
        time.sleep(30)

        # Now launch the actual process to see how it behaves under memory
        # pressure and check it's VmRSS values.
        print("Launching pkg: '%s'" % (pkg_name))

        result = launch_pkg(pkg_name)
        if (result):
            print("Error: failed to launch pkg '%s'" % (pkg_name))

        time.sleep(45)
        finish_logging(log_process, fp_out)
        count += 1
        clear_all_apps(packages)
        if (count > 0):
            break
    return


def launch_packages_sequencial(packages):
    for pkg_name in packages:
        result = launch_pkg(pkg_name)
        if (result):
            print("Error: failed to launch pkg '%s'" % (pkg_name))
        else:
            clear_pkg(pkg_name)
    return


def launch_packages_in_group(packages, pkg_launch_group_size=10):
    """Launch packages in size of group. kill them after group size
    is reached and start from next one.
    """
    pkg_group_list = []
    cur_pkg_size = 0
    for pkg_name in packages:
        pkg_group_list.append(pkg_name)
        cur_pkg_size += 1
        result = launch_pkg(pkg_name)
        if (result):
            print("Error: failed to launch pkg '%s'" % (pkg_name))
        if cur_pkg_size >= pkg_launch_group_size:
            clear_all_apps(pkg_group_list)
            pkg_group_list = []
            cur_pkg_size = 0

    clear_all_apps(packages)
    return


def launch_packages_in_threads(packages, total_threads, per_thread_pkgs):
    """Launch packages in threads. Each thread will launch specified
    number of apps and exit.
    """
    thread_list = []
    for i in xrange(total_threads):
        t1 = Thread(target = launch_packages_in_group,
                    args = (
                        packages[i * per_thread_pkgs: (i + 1)*per_thread_pkgs],
                        1000)) # Last argument is to ensure no groping in threads

        thread_list.append(t1)

    for thread in thread_list:
        thread.start()

    for thread in thread_list:
        thread.join()

    return


if __name__ == "__main__":
    pkg_list_file = "pkg_list.txt"
    pkg_launch_group_size = 10
    total_threads = 20
    per_thread_pkgs = 10

    packages = get_packages_from_list()
    #packages = get_packages(pkg_list_file )
    #packages = get_packages_from_device()

    for i in xrange(100000):
        #packages launched sequenctiall (one app open/close)
        launch_packages_sequencial(packages)

        #packages launched sequenctiall (10 app open/10 app close)
        launch_packages_in_group(packages, pkg_launch_group_size)

        launch_packages_in_threads(packages, total_threads, per_thread_pkgs)
