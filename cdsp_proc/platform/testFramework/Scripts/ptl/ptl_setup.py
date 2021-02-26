"""
   Executes dsp tests from provided adsp_proc location
   Script assumes that the test framework has already been compiled and ready to run.
"""
import re
import os
import os.path
import sys
import shutil
import platform
from subprocess import Popen, PIPE

"""
Some fixed string 
"""
DEFAULT_FWK_FOLDER   =  "TFWK"
DEFAULT_APPS_FOLDER  =  "tests"
DEFAULT_DSP_FOLDER   =  "dspso"

DEFAULT_WIN_DATA_FOLDER    =  "C:\\data\\"
DEFAULT_LINUX_DATA_FOLDER  =  "/data/"

LIST_RELEASE_VARIANT =  0
LIST_DATA_FOLDER     =  1
LIST_SLASH           =  2

windows_list = [
      "WinNT_Debug_msvc9",
      "C:\\data\\",
      "\\",
      ]

android_list = [
      "android_ReleaseG",
      "/data/",
      "/",
      ]

os_supported = [
      "Windows",
      "Android",
      ]

def verify_path_exists(path):
   if not if_dir_exists(path):
      print ("Location " + path + " does not exist")
      return -1

   return 0


def err_log(msg):
   print ("\tERROR ==> " + msg + "\n")


def msg_log(msg):
   print ("\n\t" + msg + "\n")

def if_file_exists(fileName):
   if not os.path.isfile(fileName):
      err_log("File [ " + fileName + " ] does not exist!")
      return False
   else:
      return True
   
def if_dir_exists(dirName):
   if not os.path.isdir(dirName):
      err_log("Directory [ " + dirName + " ] does not exist!")
      return False
   else:
      return True

      

def get_folder_name(args, index, defaultName):
   argLen = len(args) - 1

   if (argLen >= index):
      data_default = args[index]
   else:
      data_default = defaultName 

   return data_default

def usage_android():
   print("Usage: ptl_setup.py <device_os> <adsp_proc_root> [tfw_root] [apps_root] [dsp_root]")
   print("       <device_os>      -  OS running on device")
   print("       <adsp_proc_root> -  DSP root directory")
   print("       [tfw_root]       -  Test framework name, this folder will be created under /data")
   print("                           All folders/files will be created/copied into this [tfw_root]")
   print("                           If no [tfw_root] is provided, folder name defaults to [ " + DEFAULT_FWK_FOLDER + " ]")
   print("       [apps_root]      _  Apps folder where apps SOs will be copied to. This folder will be created under /data")
   print("                           If no [apps_root] is provided, folder name defaults to '" + DEFAULT_APPS_FOLDER + "'")
   print("       [dsp_root]       _  Apps folder where dsp SOs will be copied to. This folder will be created under /data")
   print("                           If no dsp_root is provided, folder name defaults to [ " + DEFAULT_DSP_FOLDER + " ]")
   sys.exit()

def usage_windows():
   print("Usage: ptl_setup.py <device_os> <adsp_proc_root> [tfw_root] [apps_root] [dsp_root]")
   print("       <device_os>      -  OS running on device")
   print("       <adsp_proc_root> -  DSP root directory")
   print("       [tfw_root]       -  Test framework name, this folder will be created under C:\data")
   print("                           All folders/files will be created/copied into this [tfw_root]")
   print("                           If no [tfw_root] is provided, folder name defaults to '" + DEFAULT_FWK_FOLDER + "'")
   print("       [apps_root]      _  Apps folder where apps SOs will be copied to. This folder will be created under C:\data")
   print("                           If no [apps_root] is provided, folder name defaults to '" + DEFAULT_APPS_FOLDER + "'")
   print("       [dsp_root]       _  Apps folder where dsp SOs will be copied to. This folder will be created under C:\data")
   print("                           If no [dsp_root] is provided, folder name defaults to '" + DEFAULT_DSP_FOLDER + "'")
   print(" ")
   print("       Sample directory structure")
   print(" ")
   print("       [tfw_root]")
   print("           [apps_root]")
   print("           [dsp_root]")
   sys.exit()

def print_help(op_sys):
   if ("Windows" == platform.system()):
      usage_windows()
   else:
      usage_android()

def get_release_folder (op_sys):
   if ("Windows" == op_sys):
      return windows_list[LIST_RELEASE_VARIANT]
   else:
      return android_list[LIST_RELEASE_VARIANT]
   
def get_cmd_list (op_sys):
   if ("windows" == op_sys):
      return [
            # THIS SCRIPT HAS NOT BEEN TESTED ON A WINDOWS DEVICE
            # CHANGES TO BELOW COMMANDS MIGHT BE NECESSARY
         #"adb root",
         #"adb wait-for-device devices"
         "adb shell mkdir " + fwk_dir,
         "adb shell mkdir " + apps_dir,
         "adb shell mkdir " + dsp_dir,
         "adb push " + ptl_path + " " + fwk_dir,
         "adb push " + apps_so_path + " " + apps_dir,
         "adb push " + dsp_so_path + " " + dsp_dir,
         ]
   else:
      return [
         "adb wait-for-device root",
         "adb wait-for-device remount",
         "adb push " + ptl_path + " " + fwk_dir,
         "adb push " + apps_so_path + " " + apps_dir,
         "adb push " + dsp_so_path + " " + dsp_dir,
         "adb shell export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:" + apps_dir + " ; export ADSP_LIBRARY_PATH=" + dsp_dir + " ; chmod 755 " + fwk_dir +  "/libadsprpc ; cd " + fwk_dir + " ; ./libadsprpc",
         ]

def get_data_root_folder(op_sys):
   if ("Windows" == op_sys):
      return windows_list[LIST_DATA_FOLDER]
   else:
      return android_list[LIST_DATA_FOLDER]

def get_slash(op_sys):
   if ("Windows" == op_sys):
      return windows_list[LIST_SLASH] 
   else:
      return android_list[LIST_SLASH] 

def verify_os(op_sys):
   os_found = 0
   device_os = sys.argv[1]
   for os_sel in os_supported:
      if op_sys == os_sel:
         return op_sys

   print ("HLOS " + device_os + " is not supported")
   print ("HLOS suported are")
   for os_sel in os_supported:
      print (os_sel)

   sys.exit(0)

def verify_dsp_path(path):
   if 0 != verify_path_exists(path):
      sys.exit(1)

   return path

""" 
Script start
"""
if len(sys.argv) < 2:
   print ("Enter at least ptl_setup.py <Windows|Linux> to see more help")
   sys.exit(1)
else:
   if len(sys.argv) < 3:
      print_help(sys.argv[1])
      sys.exit(1)

device_os = verify_os(sys.argv[1])
adsp_proc =  verify_dsp_path(sys.argv[2])

ptl_path = adsp_proc + "/platform/testframework/" + get_release_folder(device_os)  
dsp_so_path = adsp_proc + "/platform/media"
apps_so_path = adsp_proc + "/platform/testframework/tests/" + get_release_folder(device_os)

SLASH = get_slash(device_os)
fwk_dir = get_data_root_folder(device_os) + get_folder_name(sys.argv, 3, DEFAULT_FWK_FOLDER)
apps_dir = fwk_dir + SLASH + get_folder_name(sys.argv, 4, DEFAULT_APPS_FOLDER)
dsp_dir = fwk_dir + SLASH + get_folder_name(sys.argv, 5, DEFAULT_DSP_FOLDER)

print ("fwk_dir is " + fwk_dir)
print ("apps_dir is " +apps_dir)
print ("dsp_dir is " + dsp_dir)

cmd_list = get_cmd_list(device_os)

for cmd in cmd_list:
   print ("Executing cmd " + cmd)
   os.system(cmd)

print ("\n\nNOTE:")
print ("\tRemember to set LD_LIBRARY_PATH and ADSP_LIBRARY_PATH")
print ("\tNOTE!!!  LD_LIBRARY_PATH uses colons (:) as delimeter")
print ("\tNOTE!!!  ADSP_LIBRARY_PATH uses semi-colons (;) as delimeter\n")
print ("Copy and paste the below two lines in your device's shell\n")
print ("export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:" + apps_dir)
print ("export ADSP_LIBRARY_PATH=" + dsp_dir)

