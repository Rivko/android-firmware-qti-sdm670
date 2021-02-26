
#=================================================================================
#   File Name: build.py
#
#   Python Script to call the QC-SCons build System for ADSP/CDSP
#
#   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#---------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0/build.py#2 $
#  $DateTime: 2017/11/26 20:49:52 $
#  $Change: 14920335 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when     who            what, where, why
# --------   ---        ----------------------------------------------------------
# 03/04/13   Haseeb Khan    Script to call the QC-SCons build System for Badger
# 06/12/13   corinc         Moved Build.py to adsp_proc\build\
# 12/08/13   Haseeb Khan    Added Support to compile using verbose levels,
# 04/15/14   corinc         Add tool version support from command line for both LLVM & gcc
# 05/07/14   corinc         re-architect build system phase 3
# 05/08/14   corinc         fixed the issue for alias
# 07/24/14   corinc         Default Compiler and Q6 version for each chipset
# 08/03/16   Naresh Kumar   Cleanup build scripts
#=================================================================================
import os
import re
import subprocess
import sys
import time
import fnmatch
import shutil, errno
import adspprocess
import OpenDSP
from buildSysInfo import defaultSysInfo
from buildSysInfo import buildSysInfo

#Setting environment
print "Info: Python version: ", sys.version 
if sys.version_info < (2,7) or sys.version_info > (3,0) :
    print "Warning: Default python version is not compatible with the build. Build needs Python2.7"
    print "Info: Searching for other python installations..."
    
    print "Error: Required Python version (2.7) is not found. Exiting..."
    sys.exit(0)
#import pdb; pdb.set_trace()
#Setup QC specific enviroment
import argparse
from argparse import RawTextHelpFormatter
print "Info: Build command used:",' '.join(sys.argv)
# Change directory to root
root_dir = os.path.dirname(sys.argv[0])
if root_dir != '':
    os.chdir(root_dir)
os.chdir('..')
root_dir = os.getcwd()
print "Info: Setting root directory as ", os.getcwd()

# Setting up the image name 
default_pl = 'CDSP.HT.1.0'
manifest_file = os.path.join(root_dir, 'build', 'manifest.xml')
pl_name = ''
if os.path.exists(manifest_file):
    import xml.etree.ElementTree as ET
    config = ET.parse(manifest_file).getroot()
    im_tree = config.find('image_tree')
    pl_name = im_tree.find('name').text
    im_version = im_tree.find('revision').text
    print "Building image for ", pl_name, " with image revision ", im_version 
else:
    print "ERROR: Manifest.xml is not found at <ROOT>/build"#%manifest_file
    print "Please create manifest.xml/copy the file from appropriate CRM build"
    print "\t OR create manifest.xml by runing the below command from root\
            \n\tWindows: \\\\stone\\aswcrm\\CRMTools\\Smart\\NT\\Bin\\Create_Manifest.exe -b <BUILD_ID> \
            \n\tLinux: /pkg/qct/qctss/linux/bin/Create_Manifest -b <BUILD_ID>"
    sys.exit(0)
if pl_name != '' and (pl_name.startswith('ADSP') or pl_name.startswith('CDSP')):
    os.environ['PRODUCT_LINE'] = pl_name
else:
    os.environ['PRODUCT_LINE'] = default_pl
if 'CDSP' in pl_name:
    os.environ['IMAGE'] = 'cdsp'
else:
    os.environ['IMAGE'] = 'adsp'

#Software Paths & other definitions 
#For Windows Environment(Recommended locations) and Hexagon tools path
q6_tools_path_win = r"C:/Qualcomm/HEXAGON_Tools"
python_win = r"C:/CRMapps/Apps/Python276-64"

#For Linux Environment(Recommended locations) and Hexagon tools from Local Linux machine
#Note: Hexagon tools from local Linux machine will be preferred if both(Local and Network paths) are present
q6_tools_path_linux = 'None'
if os.environ.get('HOME', None):
   home_dir = os.environ.get('HOME', None)
   print 'HOME directory is:', home_dir
   q6_tools_path_linux = home_dir + "/Qualcomm/HEXAGON_Tools"

#Hexagon tools from Network Location. Donot specify path till version folder, path will be appended later using 'q6_rtos_tools_version'
q6_tools_netpath_linux = '/pkg/qct/software/hexagon/releases/tools'

#Start of Main build.py
python_linux = "/pkg/python/2.7.5/bin"
print '\nUsage of build.py wrapper script'
usage = " \
                 --Command for build/clean of dsp image \n \
                   cd BUILD_ROOT \n \
                      BUILD_ROOT for ADSP is adsp_proc \n \
                      To Build dsp image:           python build/build.py -c <chipset> -o all (Default)\n \
                      To Build sim-only dsp image:  python build/build.py -c <chipset> -o sim \n \
                      To Clean dsp image:           python build/build.py -c <chipset> -o clean \n\n \
                 --Command to create packages \n \
                   cd BUILD_ROOT \n \
                      To create HK11 package:        python build/build.py -c <chipset> -o pkg_hk11 \n \
                      To create HY11 package:        python build/build.py -c <chipset> -o pkg_oem \n \
                      To create HD11 package:        python build/build.py -c <chipset> -o pkg_hd11 \n \
                      To create HY22 package:        python build/build.py -c <chipset> -o pkg_hy22 \n \
                      To create HCBSP_Test package:  python build/build.py -c <chipset> -o pkg_hcbsp \n\n \
                 --Command to use Verbose levels \n \
                      To compile with verbose level 2:          python build/build.py -c <chipset> -v 2 \n \
                      0[=off] or 1[=limited] or 2[=detailed] or 3[raw, no formatting] \n\n \
                 --Command to specify number of core for building \n \
                      To compile with 16 cores:     python build/build.py -c <chipset> -n 16 \n \
                      parameter depends on the number of cores available on the machine or server \n\n \
                 --Command to add SCons Options \n \
                      To add SCons Options(seperated by comma): python build.py -c <chipset> -s implicit-deps-unchanged,ignore-errors \n\n \
                 --Command to use different tools version for compilation \n \
                      python build/build.py -c <chipset> -tv 5.1.04 \n\n \
                 --For help:  python build/build.py -h"


parser = argparse.ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

parser.add_argument('-b', '--buildid', help='buildid option [Default: 0x8fffffff]', dest='buildid', action='store')
parser.add_argument('-c', '--chipset', help='chipset option, No Default, must specify chipset in ENGG build', dest='chipset', action='store')
parser.add_argument('-p', '--pid', help='protection domain option [Default: mpd]', dest='protection_domain', action='store')

parser.add_argument("-o", "--others", help='other variable number of options(seperated by comma): <ARG1>,<ARG2> ... \n\
Eg: all,sim,clean \n\
    To create packages: pkg_hk11,pkg_oem,pkg_hd11,pkg_hy22,pkg_hcbsp \n\
    all: Build the dsp image.          \n\
    sim: Build the sim-only dsp image.          \n\
    clean: Clean the dsp image.               \n', \
                   dest='other_option', action=adspprocess.other_options_cb)

parser.add_argument('-f', '--flags', help='Flags to pass to the build system(seperated by comma): <FLAG1>,<FLAG2> ... \n\
                   Any new flags, in future, can be added here', \
                   dest='flags', action=adspprocess.other_options_cb)

parser.add_argument('-v', '--verbose' , help='To compile with specified verbose levels [Default 1] \n\
                   0[=off] or 1[=limited] or 2[=detailed] or 3[raw, no formatting]', dest='verbose', action='store')

parser.add_argument('-n', '--numcore' , help='To compile with specified number of core for building', dest='numcore', action='store')

parser.add_argument('-m', '--module', help='To Compile Individual Module (specify path just before \'build\' folder) [Default: Disabled]', dest='module', action='store')

parser.add_argument('-a', '--alias', help='To specify image alias [core_libs, avs_libs,adsp_link, sensor_img, msm8974_MPD] etc. \n\
if not specified, script will assign the target specific image alias based on chipset information', dest='alias', action='store')

parser.add_argument('-s', '--sconsargs', help='To add SCons Options(seperated by comma) \n\
Like keep-going,implicit-deps-unchanged,implicit-cache,ignore-errors etc...', dest='sconsargs', action=adspprocess.other_options_cb)

parser.add_argument('-u', '--userargs', help='To add Custom User Options(seperated by comma) \n\
Like major_version=3,minor_version=5,is_qurt_enabled=yes etc...', dest='userargs', action=adspprocess.other_options_cb)

parser.add_argument('-tv', '--tv', '--toolversion', help='To specify HexagonTools version [Default: -tv 5.1.04], GCC tool version is 5.x.x and before, LLVM tool version is 6.x.x and beyond', dest='toolversion', action='store')

opts = parser.parse_args()
args = sys.argv
(opts, args) = adspprocess.postprocess_command_options(opts, args)
if opts.other_option is None:
   opts.other_option = 'all'

if type(opts.other_option) is not list:
   opts.other_option = [opts.other_option]

sconsargs_values = ''
if opts.sconsargs is not None:
    sconsargs_values = opts.sconsargs.split(',')
    opts.sconsargs = ''
    for arg in sconsargs_values:
       opts.sconsargs += '--'+arg+','
    opts.sconsargs = opts.sconsargs.rstrip(',')
    opts.sconsargs = opts.sconsargs.rstrip('--')

if opts.userargs is not None:
    opts.userargs = opts.userargs.rstrip(',')
          
            

alloptions = ['buildid', 'chipset', 'protection_domain', 'other_option', 'flags', 'verbose', 'numcore', 'module', 'alias', 'toolversion', 'sconsargs', 'userargs']

#Initialize the build system info flags
defSysInfo=defaultSysInfo()
bldSysInfo=buildSysInfo()

#If 'hap/default_pack.txt' present then extract default parameters from this file 
#If user options specified, will override these default values

default_params_filelist = adspprocess.find_files('hap', 'default_pack.txt')
if default_params_filelist:
   default_params_file = open( "hap/default_pack.txt", "r" )
   lines = []
   for line in default_params_file:
      if not re.search('#', line):
         default_chipset_match = re.search('--chipset (\w\w\w\w\w\w\w)', line)
         default_pid_match = re.search('--pid (\w\w\w)', line)
         default_buildid_match = re.search('--buildid (\w\w\w\w\w\w\w\w\w\w)', line)
         default_others_match = re.search('--others (.+)', line)
         default_flags_match = re.search('--flags (.+)', line)
         default_verbose_match = re.search('--verbose (\d+)', line)
         default_numcore_match = re.search('--numcore (\d+)', line)
         default_module_match = re.search('--module (.+)', line)
         default_alias_match = re.search('--alias (.+)', line)
         default_sconsargs_match = re.search('--sconsargs (.+)', line)
         default_userargs_match = re.search('--userargs (.+)', line)

         if default_chipset_match:
            defSysInfo.default_chipset = default_chipset_match.group(1)
         if default_pid_match:
            defSysInfo.default_pid = default_pid_match.group(1)
         if default_buildid_match:
            defSysInfo.default_buildid = default_buildid_match.group(1)
         if default_others_match:
            defSysInfo.default_others = default_others_match.group(1)
         if default_flags_match:
            defSysInfo.default_flags = default_flags_match.group(1)
         if default_verbose_match:
            defSysInfo.default_verbose = default_verbose_match.group(1)
         if default_numcore_match:
            defSysInfo.default_numcore = default_numcore_match.group(1)
         if default_module_match:
            defSysInfo.default_module = default_module_match.group(1)
         if default_alias_match:
            defSysInfo.default_alias = default_alias_match.group(1)
         if default_sconsargs_match:
            defSysInfo.default_sconsargs = default_sconsargs_match.group(1)
         if default_userargs_match:
            defSysInfo.default_userargs = default_userargs_match.group(1)

   default_params_file.close()

###############################################################################

sconsargs_values = ''
if defSysInfo.default_sconsargs is not None:
    sconsargs_values = defSysInfo.default_sconsargs.split(',')
    defSysInfo.default_sconsargs = ''
    for arg in sconsargs_values:
       defSysInfo.default_sconsargs += '--'+arg+','
    defSysInfo.default_sconsargs = defSysInfo.default_sconsargs.rstrip(',')
    defSysInfo.default_sconsargs = defSysInfo.default_sconsargs.rstrip('--')

if defSysInfo.default_userargs is not None:
    defSysInfo.default_userargs = defSysInfo.default_userargs.rstrip(',')


# Check all build options and set in the build system information flags
adspprocess.allOptionsCheck(opts, alloptions, defSysInfo, bldSysInfo)

#set_chipset_target() is main function to set target, chipset, compiler version, Q6 version. 
#Try to find BLD_ENV_BUILD_ID enviroment variable

env_bld_id = os.environ.get('BLD_ENV_BUILD_ID', None)
if env_bld_id and not opts.chipset:
   print 'Extracting CHIPSET from BLD_ENV_BUILD_ID'
   adspprocess.set_chipset_target(env_bld_id, bldSysInfo.chipset_param, opts)
   bldSysInfo.chipset_flag = 1
else:
   if bldSysInfo.chipset_param:
      #print 'Chipset Parameter:', bldSysInfo.chipset_param
      if not env_bld_id:
         print 'BLD_ENV_BUILD_ID enviroment variable is None'
      else:
         print '-c option provided from command-line over-rides chipset extraction from BLD_ENV_BUILD_ID', bldSysInfo.chipset_param
      adspprocess.set_chipset_target(env_bld_id, bldSysInfo.chipset_param, opts)
   else: # ENGG build and no chipset specified will come here.
      print 'Error: chipset argument missing on command line !!!'
      print '\tPlease use <-c CHIPSET> option'
      sys.exit(0)


#exit the build process, if HEXAGON_Q6VERSION is not set in the set_chipset_target()
if os.environ.get('HEXAGON_Q6VERSION', None) is None:
   print 'Error: HEXAGON_Q6VERSION is not set in set_chipset_target(), exit the build process !!!'
   sys.exit(0)

#update q6_rtos_tools_version, if the compiler is specified in the command line
q6_rtos_tools_version = os.environ.get('HEXAGON_RTOS_RELEASE', None)
if q6_rtos_tools_version is None:
   print 'Error: HEXAGON_RTOS_RELEASE is not set in set_chipset_target(), exit the build process !!!'
   sys.exit(0)

# get the main version number to specify the compiler bin path
q6_tools_major_version = int(q6_rtos_tools_version[0])

# Set the tool version and path info after command parsing is done
q6_tools_path_win = q6_tools_path_win.replace('\\', '/').rstrip('/')
python_win = python_win.replace('\\', '/').rstrip('/')
q6_tools_path_linux = q6_tools_path_linux.rstrip('/')
q6_tools_netpath_linux = q6_tools_netpath_linux.rstrip('/')
# python_linux = python_linux.rstrip('/')


q6_tools_linux = 'None'
if os.environ.get('HOME', None):
   q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version
if not os.path.exists(q6_tools_linux):
   q6_tools_path_linux = q6_tools_netpath_linux
   q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version

if os.name == 'posix':
   if not os.path.exists(q6_tools_linux):
      (q6_tools_path_linux, q6_tools_linux) = adspprocess.search_exe_tool('which')
   # Set up ARM
   os.environ['ARMLMD_LICENSE_FILE']="8224@REDCLOUD"
   os.environ['LLVMTOOLS'] = '/pkg/qct/software/llvm/release/arm/3.9.3/tools/bin'
   os.environ['LLVMHOME'] = '/pkg/qct/software/llvm/release/arm/3.9.3'
   os.environ['LLVMTOOLPATH'] = os.environ['LLVMHOME'] + '/tools/bin'
   os.environ['LLVMBIN'] = os.environ['LLVMHOME'] + '/bin'
   os.environ['LLVMLIB'] = os.environ['LLVMHOME'] + '/lib'
   os.environ['LLVMINCLUDE'] = os.environ['LLVMHOME'] + '/lib/clang/3.9.3/include/'
   #os.environ['LLVM32INC'] = os.environ['LLVMHOME'] + '/lib/clang/3.9.3/include/'
   os.environ['LIBC'] = os.environ['LLVMHOME'] + '/armv7m-none-eabi/libc/lib'
   os.environ['LIBCINCLUDE'] = os.environ['LLVMHOME'] + '/armv7m-none-eabi/libc/include'
   
else:
   where_tools_path = q6_tools_path_win  + "/" + q6_rtos_tools_version
   if not os.path.exists(where_tools_path):      
      q6_tools_path_win = adspprocess.search_exe_tool('where')    
   os.environ['ARMLMD_LICENSE_FILE']='7117@license-wan-arm'
   
   os.environ['LLVMTOOLS'] = 'C:\\Apps\\LLVM\\3.9.3\\tools\\bin'
   os.environ['LLVMHOME'] = 'C:\\Apps\\LLVM\\3.9.3'
   os.environ['LLVMTOOLPATH'] = os.environ['LLVMHOME'] + '\\tools\\bin'
   os.environ['LLVMBIN'] = os.environ['LLVMHOME'] + '\\bin'
   os.environ['LLVMLIB'] = os.environ['LLVMHOME'] + '\\lib'
   os.environ['LLVMINCLUDE'] = os.environ['LLVMHOME'] + '\\lib\\clang\\3.9.3/include/'
   os.environ['LIBC'] = os.environ['LLVMHOME'] + '\\armv7m-none-eabi\\libc\\lib'
   os.environ['LIBCINCLUDE'] = os.environ['LLVMHOME'] + '\\armv7m-none-eabi\\libc\\include'

# try to find CRM_BUILDID enviroment variable
env_bld_ver = os.environ.get('CRM_BUILDID', None)
if env_bld_ver:
    print 'Extracting QDSP6_BUILD_VERSION from CRM_BUILDID'
    build_ver = os.environ['CRM_BUILDID']
    build_ver_match = re.search(r'([\w\.\d-]+?)(AAAA\w+)', build_ver)
    if not build_ver_match:
       build_ver_match = re.search(r'([\w\.\d-]+)(-\d)', build_ver)
    os.environ['QDSP6_BUILD_VERSION'] = build_ver_match.group(1)
    bldSysInfo.buildid_flag = 1
else:
    if bldSysInfo.buildid_param:
        print 'CRM_BUILDID enviroment variable is None'
        os.environ['QDSP6_BUILD_VERSION'] = bldSysInfo.chipset_param.upper() + "_" + bldSysInfo.buildid_param
        build_ver_match = re.search('(..)\w', os.environ['QDSP6_BUILD_VERSION'])

if bldSysInfo.build_flags or defSysInfo.default_flags:
      print 'flags Parameter:', bldSysInfo.flags_param
      os.environ['BUILD_FLAGS'] = bldSysInfo.flags_param
else:
      os.environ['BUILD_FLAGS'] = ''
      bldSysInfo.flags_param = ''

if bldSysInfo.build_verbose_flag or defSysInfo.default_verbose:
      os.environ['BUILD_VERBOSE'] = bldSysInfo.opts_verbose
else:
      os.environ['BUILD_VERBOSE'] = ''

if bldSysInfo.build_numcore_flag or defSysInfo.default_numcore:
      os.environ['BUILD_NUMCORE'] = bldSysInfo.opts_numcore
else:
      os.environ['BUILD_NUMCORE'] = ''

if bldSysInfo.build_filter_flag or defSysInfo.default_module:
      bldSysInfo.opts_module = bldSysInfo.opts_module.replace('\\', '/')
      os.environ['BUILD_FILTER'] = bldSysInfo.opts_module
      extract_component = bldSysInfo.opts_module.split('/', 1)
      extract_component[0] = extract_component[0].lower()
      if extract_component[0] == 'sensors':
         os.environ['BUILD_COMPONENT'] = 'sensor_img'
      else:
         os.environ['BUILD_COMPONENT'] = extract_component[0]+'_libs'
else:
      os.environ['BUILD_FILTER'] = ''
      os.environ['BUILD_COMPONENT'] = ''
      bldSysInfo.opts_module = ''
      extract_component = ''

#If users use build_filter to specifiy tech area unit without specifying image alias, image alias will be extraced from component path and append a "_libs" or "_img" accordingly
#If the alias generated by extracting from build filter option doesn't work, users need to specify the image alias
if bldSysInfo.image_alias_flag or defSysInfo.default_alias:
      os.environ['BUILD_COMPONENT'] = bldSysInfo.opts_alias

if bldSysInfo.build_sconsargs_flag or defSysInfo.default_sconsargs:
      #opts_sconsargs = opts.sconsargs
      os.environ['BUILD_SCONSARGS'] = bldSysInfo.opts_sconsargs
else:
      os.environ['BUILD_SCONSARGS'] = ''
      bldSysInfo.opts_sconsargs = ''

if bldSysInfo.build_userargs_flag or defSysInfo.default_userargs:
      print bldSysInfo.opts_userargs
      os.environ['BUILD_USERARGS'] = bldSysInfo.opts_userargs
else:   
      os.environ['BUILD_USERARGS'] = ''
      bldSysInfo.opts_userargs = ''


os.environ['BUILD_FLAVOR'] = 'mpd'
os.environ['BUILD_ACT'] = ''
if bldSysInfo.mpd_flag:
   os.environ['BUILD_FLAVOR'] = 'mpd'
if bldSysInfo.spd_flag:
   os.environ['BUILD_FLAVOR'] = 'spd'
if bldSysInfo.clean_flag:
   os.environ['BUILD_ACT'] = 'clean'
if bldSysInfo.sim_flag:
   os.environ['BUILD_ACT'] = 'SIM'
if bldSysInfo.all_flag:
   os.environ['BUILD_ACT'] = ''

if os.name == 'posix':
   print 'It is Linux environment!!!'
   if os.environ.get('HEXAGON_ROOT', None):
      q6_tools_path_linux = os.environ.get('HEXAGON_ROOT', None)
   else:
      os.environ['HEXAGON_ROOT'] = q6_tools_path_linux

   os.environ['Q6_TOOLS_ROOT'] =  q6_tools_linux
   os.environ['Q6_ROOT'] = os.environ.get('Q6_TOOLS_ROOT', None)
   q6_root = os.environ['Q6_ROOT']

   os.environ['PYTHON_ROOT'] = python_linux
   python_root = os.environ['PYTHON_ROOT']
   # #At this stage Python version is 2.7.x, set PYTHONPATH in os env
   # if os.environ.get('PYTHONPATH') is not None:
      # os.environ['PYTHONPATH'] = python_linux
   # if os.environ.get('PYTHON_PATH') is not None:
      # os.environ['PYTHON_PATH'] = python_linux

else:
   if os.environ.get('HEXAGON_ROOT', None):
      q6_tools_path_win = os.environ.get('HEXAGON_ROOT', None)
      q6_tools_path_win = q6_tools_path_win.replace('\\', '/')
      os.environ['HEXAGON_ROOT'] = q6_tools_path_win
   else:
      q6_tools_path_win = q6_tools_path_win.replace('\\', '/')
      os.environ['HEXAGON_ROOT'] = q6_tools_path_win

   q6_tools_win = q6_tools_path_win + "/" + q6_rtos_tools_version
   os.environ['Q6_TOOLS_ROOT'] = q6_tools_win
   q6_tools_root = os.environ['Q6_TOOLS_ROOT']
   os.environ['Q6_ROOT'] = os.environ.get('Q6_TOOLS_ROOT', None)
   q6_root = os.environ['Q6_ROOT']
   os.environ['PYTHON_ROOT'] = python_win
   python_root = os.environ['PYTHON_ROOT']
   if os.environ.get('PYTHONPATH') is not None:
      os.environ['PYTHONPATH'] = python_win
   if os.environ.get('PYTHON_PATH') is not None:
      os.environ['PYTHON_PATH'] = python_win
	  
if q6_tools_major_version <= 6:
  q6_root_gnu_bin = ''.join([q6_root, '/gnu/bin'])
  q6_root_qc_lib = ''.join([q6_root, '/qc/lib/iss'])
  q6_root_qc_bin = ''.join([q6_root, '/qc/bin'])
else: # LLVM path >= version 7
  q6_root_gnu_bin = ''.join([q6_root, '/Tools/bin'])
  q6_root_qc_lib = ''.join([q6_root, '/Tools/lib/iss'])
  q6_root_qc_bin = ''.join([q6_root, '/Tools/bin'])

print "Current OS Environment:"
print os.environ
bldSysInfo.local_path = os.environ['PATH']
print 'Old PATH is:\n', os.environ['PATH']

if os.name != 'posix':
   path_remove_cmd = ''.join([root_dir + "/hap/remove_cygwin.cmd"])
   if os.path.isfile(path_remove_cmd):
      print path_remove_cmd
      proc = subprocess.Popen(path_remove_cmd, shell=True)
      (out, err) = proc.communicate()
      file_withpath = open("path_with_cygwin_removed.txt", "r")
      line_path = file_withpath.readline()
      bldSysInfo.local_path = line_path.replace('PATH=', '')
      file_withpath.close()
      os.remove('path_with_cygwin_removed.txt')

#AVS_SIM_Config to edit physpoolsize
if bldSysInfo.sim_flag:
  sys.path.insert(0, root_dir+"/avs/main/aDSPTest/build/")
  import avs_sim_build_cfg
  avs_sim_build_cfg.sim_build_replace(root_dir+"/build/chipset/"+ bldSysInfo.chipset_param+"/adsp")
  print("AVS physpool CHANGE Done.")

if os.path.isfile("./build/build.py"):
   build_cmd = ''.join(['.\\tools\\build\\scons\\SCons\\scons --directory=./build ' , '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None), ' ', \
                     '--build_verbose=', os.environ.get('BUILD_VERBOSE', None), ' ', '--build_component=', os.environ.get('BUILD_COMPONENT', None), ' ', \
                     '--build_numcore=', os.environ.get('BUILD_NUMCORE', None), ' ', '--image_name=', os.environ.get('IMAGE', None), ' ', \
                     '--build_filter=', os.environ.get('BUILD_FILTER', None), ' ', '--build_sconsargs=', os.environ.get('BUILD_SCONSARGS', None),' ',\
                     ])
   linux_build_cmd = ''.join(['./tools/build/scons/SCons/scons --directory=./build ', '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None), ' ', \
                     '--build_verbose=', os.environ.get('BUILD_VERBOSE', None), ' ', '--build_component=', os.environ.get('BUILD_COMPONENT', None), ' ', \
                     '--build_numcore=', os.environ.get('BUILD_NUMCORE', None), ' ', '--image_name=', os.environ.get('IMAGE', None), ' ', \
                     '--build_filter=', os.environ.get('BUILD_FILTER', None), ' ', '--build_sconsargs=', os.environ.get('BUILD_SCONSARGS', None),' ',\
                     ]) 

if os.name != 'posix':
   print "Setting paths now..."
   new_path = ''.join([python_root, ';', q6_root_gnu_bin, ';', q6_root_qc_lib, ';', q6_root_qc_bin])

if os.name == 'posix':
   new_path = ''.join([python_root, ':', q6_root_gnu_bin, ':', q6_root_qc_lib, ':', q6_root_qc_bin, ':'])
   new_engg_path = ''.join([new_path, ':', bldSysInfo.local_path, ':'])
   os.environ['PATH'] = new_engg_path
   print 'New PATH w.r.t Linux:\n', os.environ['PATH']
   if bldSysInfo.all_flag or bldSysInfo.sim_flag or bldSysInfo.clean_flag:
      latestCmdFile = open('./build/latest_build_command.txt', 'w')
      latestCmdFile.write(' '.join(sys.argv))
      latestCmdFile.close()

      adspprocess.summary_build(opts, defSysInfo, bldSysInfo)
      print '\n\nBuild Command on Linux:\n', linux_build_cmd
      print "\n"
      proc = subprocess.Popen(linux_build_cmd, shell=True)
      (out, err) = proc.communicate()
else: #The complete code below till end is for non-linux case
   if bldSysInfo.all_flag or bldSysInfo.sim_flag or bldSysInfo.clean_flag:
      latestCmdFile = open('./build/latest_build_command.txt', 'w')
      latestCmdFile.write(' '.join(sys.argv))
      latestCmdFile.close()

      new_engg_path = ''.join([new_path, ';', bldSysInfo.local_path])
      os.environ['PATH'] = new_engg_path
      print 'New PATH w.r.t engineering build:\n', os.environ['PATH']
      adspprocess.summary_build(opts, defSysInfo, bldSysInfo)

      print '\n\nBuild Command for Engineering build:\n', build_cmd
      print "\n"
      proc = subprocess.Popen(build_cmd, shell=True)
      (out, err) = proc.communicate()

if (bldSysInfo.all_flag or bldSysInfo.sim_flag) and (not bldSysInfo.opts_module) and (not bldSysInfo.opts_alias):
    variant = os.environ.get('VARIANT_NAME')
    # print 'Current working directory', root_dir
    build_artifacts = ''.join([root_dir + "/dsp_%sQ.elf"%variant])
    external_pack = ''.join([root_dir + "/build/ms/tbc_cleanpack.py"])
    if not os.path.exists(external_pack):
       dynamic_modules = ''.join([root_dir + "/build/dynamic_modules"])
       dynamic_mods1 = ''.join([root_dir + '/build/dynamic_modules/%s/map_AVS_SHARED_LIBS_%sQ.txt'%(variant, variant)])
       dynamic_mods2 = ''.join([root_dir + '/build/dynamic_modules/%s/map_SHARED_LIBS_%sQ.txt'%(variant, variant)])
       if os.path.exists(dynamic_mods1) and os.path.exists(dynamic_mods2):
           print 'Shared Libs Generated'
       else:
           print 'Shared Libs NOT Generated'
    if os.path.exists(build_artifacts):
        print 'Compilation SUCCESS'
    else:
        print 'Compilation FAILED'	


# HAP packages creation
OpenDSP.HAPPackagesCreate(bldSysInfo)




