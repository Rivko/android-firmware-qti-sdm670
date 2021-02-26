
#=================================================================================
#   File Name: OpenDSP.py
#
#   ADSP build system: build process OpenDSP functions
#
#   Copyright (c) 2014 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#---------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0/OpenDSP.py#1 $
#  $DateTime: 2017/09/03 21:39:31 $
#  $Change: 14261189 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when     who            what, where, why
# --------   ---        ----------------------------------------------------------
# 05/09/14   corinc      Moved code from build\build.py to OpenDSP.py, part of re-architect effort
# 05/09/14   corinc      Initial check-in and promotion to PW
#=================================================================================

import os
import re
import subprocess
import sys
import time
import fnmatch
import shutil, errno
import buildSysInfo

#=================================================================================
#=================================================================================
#                  Function definitions are below
#=================================================================================
#=================================================================================

      
#=================================================================================
# HAP packages creation
def HAPPackagesCreate(bldSysInfo):
   # import pdb; pdb.set_trace()
   if 'CDSP' in bldSysInfo.flags_param.upper():
      pack_script = "cdsp_pack"
   else:
      pack_script = "adsp_pack"
   if bldSysInfo.pkg_all_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p all" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
      
   if bldSysInfo.pkg_hy22_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hy22" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()

   if bldSysInfo.pkg_hk11_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hk11" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()

   if bldSysInfo.pkg_hk22_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hk22" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
       
   if bldSysInfo.pkg_oem_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_oem" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()

   if bldSysInfo.pkg_hd11_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hd11" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
      
   if bldSysInfo.pkg_isv_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_isv" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()      
      
   if bldSysInfo.pkg_hcbsp_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hcbsp" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
      
   if bldSysInfo.pkg_hd22_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join(["python " + cwd_dir + "/build/pack/scripts/%s.py -p pkg_hd22" %pack_script])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()      

#=================================================================================
#=================================================================================
#                  Function definitions ends here
#=================================================================================
#=================================================================================

