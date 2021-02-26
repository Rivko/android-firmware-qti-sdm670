
#=================================================================================
#   File Name: adspprocess.py
#
#   ADSP build system: build process functions
#
#   Copyright (c) 2014 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#---------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/adspprocess.py#1 $
#  $DateTime: 2018/08/07 05:36:15 $
#  $Change: 16823152 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when     who            what, where, why
# --------   ---        ----------------------------------------------------------
# 09/07/17   sm          Add sdm1000 support.
# 04/24/14   corinc      Move functions from build\build.py to adspprocess.py
# 04/29/14   corinc      Initial check-in and promotion to PW
# 05/08/14   corinc      re-architect and fixed the issue for alias
# 07/24/14   corinc      set_chipset_target() update
#=================================================================================

import os
import re
import subprocess
import sys
import time
import fnmatch
import shutil, errno
import buildSysInfo
import argparse
#=================================================================================
#=================================================================================
#                  Function definitions are below
#=================================================================================
#=================================================================================

# Function definition 'set_chipset_target' is here
def set_chipset_target(str, chipset_param, opts):
# Setting CHIPSET and TARGET here
   opts_chipset = opts.chipset
   
   if str is None or opts_chipset:
      str = chipset_param

#   print "str = ", str
   a = re.compile('\d\w\d+')
   targets = a.findall(str)
   target = targets[0]
   os.environ['CHIPSET'] = str
   os.environ['TARGET'] = target
   
   if target=='8974':
     os.environ['CHIPSET'] = "msm8974"
     os.environ['TARGET'] = "8974"
     q6_version = "v5"
     q6_tools_version = '5.1.04'
   elif target=='8994':
     q6_version = "v55"
     q6_tools_version = '5.1.05'
   elif target=='8992':
     q6_version = "v55"
     q6_tools_version = '5.1.05'
   elif target=='8952':
     q6_version = "v55"
     q6_tools_version = '5.1.05'     
   elif target=='8996':
     q6_version = "v60"
     q6_tools_version = '7.2.09'
   elif target=='8998':
     q6_version = "v62v1"
     q6_tools_version = '7.4.02'
   elif target=='845':
     q6_version = "v65"
     q6_tools_version = '8.1.04'
   elif target=='855':
     q6_version = "v66"
     q6_tools_version = '8.2.01'
   elif target=='1000':
     q6_version = "v66"
     q6_tools_version = '8.2'
   elif target=='660':
     if os.environ.get('IMAGE', 'adsp') == 'cdsp':
        q6_version = "v60"
        q6_tools_version = '8.0.08'
     else:
        q6_version = "v62"
        q6_tools_version = '8.0.09'
   elif target=='670':
     q6_version = "v65"
     q6_tools_version = '8.1.04'
   else:
     print 'CHIPSET not detected'
     sys.exit(0)
   # write the QDSP6 version into OS environments - need to reduce these to just 1
   os.environ['Q6VERSION'] = q6_version
   os.environ['HEXAGON_Q6VERSION'] = q6_version
   os.environ['HEXAGON_REQD_Q6VERSION'] = q6_version
   
   # check the tool version set from the command line, use the one from command line
   if opts.toolversion != "":
      q6_tools_version = opts.toolversion
   
   os.environ['HEXAGON_RTOS_RELEASE'] = q6_tools_version
   # temporary workaround to fix Sensors' scons file

   if int(q6_tools_version[0]) <= 5:
      os.environ['COMPILER'] = "gcc"
   else:
      os.environ['COMPILER'] = "llvm"


   print 'CHIPSET: ', os.environ['CHIPSET']
   print 'TARGET: ', os.environ['TARGET']
   print 'HEXAGON_Q6VERSION: ', os.environ['HEXAGON_Q6VERSION']
   print 'HEXAGON_RTOS_RELEASE:', os.environ['HEXAGON_RTOS_RELEASE']
   print 'Hexagon Tool COMPILER Type: ', os.environ['COMPILER']
   print '\n'
   
   return;


# Function definition 'check_success' is here
def check_success():
# check if success file present
      file_name="success"
      if os.path.exists(file_name):
          try:
              os.remove(file_name)
          except:
              print "Exception: ",str(sys.exc_info())
      else:
          print "File '%s' not found" % file_name


      return;

# Function definition 'verify_args' is here
def verify_args(str, array_var):
      arg_flag = 0         
      for each_element in array_var:         
         match = re.search(str, each_element, re.I)
         if match: arg_flag = 1         

      return arg_flag;      

# Class definition 'other_options_cb' is here
class other_options_cb(argparse.Action):
        def __call__(self, parser, namespace, values, option_string=None):
           #print '%r %r %r' % (namespace, values, option_string)
           #setattr(namespace, self.dest, values)           
           
           args=[]
           temp_arg=0           
           for arg in values:
              if arg[0] != "-" or temp_arg != " ":                             
                 args.append(arg)                           
              else:
                 del values[:len(args)]
                 break
              temp_arg = arg[0]        
           
           if getattr(namespace, self.dest):                    
              args.extend(getattr(namespace, self.dest))
           setattr(namespace, self.dest, values)


# Function definition 'summary_build' is here
def summary_build(opts, defSysInfo, bldSysInfo):
      print '\n\n********************************************************'
      print '*************** Summary Build Environment **************'
      print '********************************************************'      
      print 'Command Given on Console: ', ' '.join(sys.argv)      
      if os.name == 'posix':
         print "Operating System:: Linux"
      else:
         print "Operating System:: Windows"
      
      if sys.version:
         print "Python Version::", sys.version_info[0:3]

         if sys.version_info[0] != 2:
             print "ERROR:: You are not using Python 2.x. Please use 2.7.6"
             sys.exit(0)
      else:
         print '\n\nERROR: Python not installed!!!'
         print 'If installed already, please verify if respective path added to PATH environment variable!!!\n\n'
         sys.exit(0)
      
      tool_version = ''.join(['perl -v'])
      proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
      (out, err) = proc.communicate()
      if out:
         print "Perl Version::", out
      else:
         print 'WARNING: Perl required to execute Post Build Step (Memory Profiling Scripts)!!!'

      if  os.environ.get('COMPILER') is 'llvm':
         print 'COMPILER= llvm'
      else:
         print 'COMPILER= gcc'	  
        
      tool_version = ''.join(['hexagon-sim --version'])
      proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)

      (out, err) = proc.communicate()      
      if out:         
         tool_version = ''.join(['hexagon-sim --version > ./build/toolsver.txt 2>&1'])
         proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
         (out, err) = proc.communicate()
         toolsver_filelist = find_files('build', 'toolsver.txt')
	 if toolsver_filelist:
	    toolsver_file = open( "build/toolsver.txt", "r" )
	    lines = []
	    for line in toolsver_file:	       
	       toolsver_match = re.search('Hexagon Build Version \(*(\d.\d.\d\d)', line)
	       if toolsver_match:
	          tools_ver = toolsver_match.group(1)
	          print 'You are using Hexagon Tools Version: ', tools_ver
	    toolsver_file.close()
	    os.remove('build/toolsver.txt')
      
      print 'Q6VERSION=', os.environ['Q6VERSION']
      print 'Q6_TOOLS_ROOT=', os.environ['Q6_TOOLS_ROOT']
      if bldSysInfo.buildid_flag:
         print 'QDSP6_BUILD_VERSION=', os.environ.get('QDSP6_BUILD_VERSION', None)
      elif defSysInfo.default_buildid:
         print "QDSP6_BUILD_VERSION= [Value Taken from file 'adsp_proc/hap/default_pack.txt']:", os.environ.get('QDSP6_BUILD_VERSION', None)         
      else:
         print 'QDSP6_BUILD_VERSION= [Default Value Taken]', os.environ.get('QDSP6_BUILD_VERSION', None)
      
      # Corin: This chipset_flag is kind of redundant if setting the default chipset is not allowed, remove this later
      if bldSysInfo.chipset_flag:
         print 'CHIPSET=', os.environ.get('CHIPSET', None)
         print 'TARGET=',  os.environ.get('TARGET', None)
      
      if bldSysInfo.protection_domain_flag:
         print 'BUILD_FLAVOR=',  os.environ.get('BUILD_FLAVOR', None)
      elif defSysInfo.default_pid:
         print "BUILD_FLAVOR= [Value Taken from file 'adsp_proc/hap/default_pack.txt']:", os.environ.get('BUILD_FLAVOR', None)
      else:   
         print 'BUILD_FLAVOR= [Default Value Taken]',  os.environ.get('BUILD_FLAVOR', None)   
      
      if bldSysInfo.other_option_flag:
         if os.environ.get('BUILD_ACT', None) == '':
            print 'BUILD_ACT= all'
         else:   
            print 'BUILD_ACT=',  os.environ.get('BUILD_ACT', None)
      elif defSysInfo.default_others:
         print "BUILD_ACT= [Value Taken from file 'adsp_proc/hap/default_pack.txt']:",  os.environ.get('BUILD_ACT', None)
      
      arg_flag = 0
            
      if bldSysInfo.build_verbose_flag:
         if os.environ['BUILD_VERBOSE'] == '0': 
            print 'Custom Verbose level taken is 0(=off)'
         elif os.environ['BUILD_VERBOSE'] == '1': 
            print 'Custom Verbose level taken is 1(=limited)'            
         elif os.environ['BUILD_VERBOSE'] == '2': 
            print 'Custom Verbose level taken is 2(=detailed)'
         elif os.environ['BUILD_VERBOSE'] == '3':
            print 'Custom Verbose level taken is 3(=raw, no formatting)'
         else:
            print 'WARNING: Specified Custom Verbose level NOT Supported!!!: ', os.environ['BUILD_VERBOSE']
            print '         So, Default Verbose level is taken i.e., 1(=limited)'
      elif defSysInfo.default_verbose:
         if os.environ['BUILD_VERBOSE'] == '0': 
            print "Custom Verbose level taken is 0(=off) [Value Taken from file 'adsp_proc/hap/default_pack.txt']"
         elif os.environ['BUILD_VERBOSE'] == '1': 
            print "Custom Verbose level taken is 1(=limited) [Value Taken from file 'adsp_proc/hap/default_pack.txt']"
         elif os.environ['BUILD_VERBOSE'] == '2': 
            print "Custom Verbose level taken is 2(=detailed) [Value Taken from file 'adsp_proc/hap/default_pack.txt']"
         elif os.environ['BUILD_VERBOSE'] == '3':
            print "Custom Verbose level taken is 3(=raw, no formatting) [Value Taken from file 'adsp_proc/hap/default_pack.txt']"
         else:
            print "WARNING: Specified Custom Verbose level NOT Supported [Value Taken from file 'adsp_proc/hap/default_pack.txt'] !!!: ", os.environ['BUILD_VERBOSE']                     
            print '         So, Default Verbose level is taken i.e., 1(=limited)'  
      else:
            print 'Default Verbose level is taken i.e., 1(=limited) !!!' 

      if bldSysInfo.build_numcore_flag:
         print 'number of cores: ', os.environ['BUILD_NUMCORE']
      elif defSysInfo.default_numcore:
         print "number of cores  [Value Taken from file 'adsp_proc/hap/default_pack.txt']: ", os.environ['BUILD_NUMCORE']  
      else:
            print 'Default number of cores is taken i.e., 8 !!!'

      if bldSysInfo.build_filter_flag:
         print 'Individual Module Compilation Enabled for: ', os.environ['BUILD_FILTER']
      elif defSysInfo.default_flags:            
         print "Individual Module Compilation Enabled for [Value Taken from file 'adsp_proc/hap/default_pack.txt']: ", os.environ['BUILD_FILTER']         
      else:
         print 'Individual Module Compilation NOT Enabled !!!'
            
      if bldSysInfo.image_alias_flag:      
         print 'Image alias is: ', os.environ['BUILD_COMPONENT']
      elif defSysInfo.default_alias:            
         print "Image alias is [Value Taken from file 'adsp_proc/hap/default_pack.txt']: ", os.environ['BUILD_COMPONENT']                  
      else:
         print 'Image alias NOT assigned (By Default, script will assign target specific image aliases based on chipset information) !!!'         

      if bldSysInfo.build_sconsargs_flag:
         print 'SCons Options Enabled are:', bldSysInfo.opts_sconsargs
      elif defSysInfo.default_sconsargs:
         print "SCons Options Enabled are [Value Taken from file 'adsp_proc/hap/default_pack.txt']: ", bldSysInfo.opts_sconsargs
      else:
         print 'Any SCons Options NOT Enabled !!!'         

      if bldSysInfo.build_userargs_flag:
         print 'Custom user Options Enabled are:', bldSysInfo.opts_userargs
      elif defSysInfo.default_userargs:
         print "Custom user Options Enabled are [Value Taken from file 'adsp_proc/hap/default_pack.txt']: ", bldSysInfo.opts_userargs
      else:
         print 'Any custom user Options NOT Enabled !!!'  
      
      if bldSysInfo.build_flags:
         print 'Build Flags Enabled are:', bldSysInfo.flags_param
      elif defSysInfo.default_flags:
         print "Build Flags Enabled are [Value Taken from file 'adsp_proc/hap/default_pack.txt']:", bldSysInfo.flags_param
      else:
         print 'Any custom Build flags NOT Enabled !!!'   
         
      
      print "\nFor Build Command help, use -h option: python build.py -h"
      print '********************************************************'      
      print '************** End of Build Environment ****************'
      print '********************************************************'
      print '\n'
      

def find_files(base, pattern):
    '''Return list of files matching pattern in base folder.'''
    return [n for n in fnmatch.filter(os.listdir(base), pattern) if
        os.path.isfile(os.path.join(base, n))]


def postprocess_command_options(opts, args):
   #For windows: hypen('-') or double-dash('--') is converted to character 0x96 if build command is copy-pasted from outlook.
   #For Linux: hypen('-') or double-dash('--') is removed if build command is copy-pasted from outlook.
   
   #This function avoids script taking default value for the above cases by:
   #   a. replacing character 0x96 back to '-'.
   #   b. recognizing option without '-' or '--'.
   j = 0
   for i in args:
      temp = i.replace('\x96', '-')      
      args[j] = temp      
      if args[j] == '-b' or args[j] == 'b' or args[j] == '-buildid':
         opts.buildid = args[j+1]   
      if args[j] == '-c' or args[j] == 'c' or args[j] == '-chipset':
         opts.chipset = args[j+1]
      if args[j] == '-p' or args[j] == 'p' or args[j] == '-pid':
         opts.protection_domain = args[j+1]   
      if args[j] == '-o' or args[j] == 'o' or args[j] == '-others':
         opts.other_option = args[j+1]         
      if args[j] == '-f' or args[j] == 'f' or args[j] == '-flags':
         opts.flags = args[j+1]                    
      if args[j] == '-v' or args[j] == 'v' or args[j] == '-verbose':
         opts.verbose = args[j+1]
      if args[j] == '-n' or args[j] == 'n' or args[j] == '-numcore':
         opts.numcore = args[j+1]
      if args[j] == '-m' or args[j] == 'm' or args[j] == '-module':
         opts.module = args[j+1]   
      if args[j] == '-a' or args[j] == 'a' or args[j] == '-alias':
         opts.alias = args[j+1]
      if args[j] == '-s' or args[j] == 's' or args[j] == '-sconsargs':
         opts.sconsargs = args[j+1]    
      if args[j] == '-u' or args[j] == 'u' or args[j] == '-userargs':
         opts.userargs = args[j+1]         
      if args[j] == '-tv' or args[j] == 'tv' or args[j] == '-toolversion':
         opts.toolversion = args[j+1]
      if args[j] == '-h' or args[j] == 'h' or args[j] == '-help':         
         parser.print_help()
         print "\n\n"
         sys.exit(0)
      j = j+1
   return (opts, args)

def process_array(array):
      n = 0
      for m in array:         
         temp = m.replace('\x96', '-')         
         match = re.search('(-)', temp)         
         if match:            
            break
         else:            
            array[n] = temp
            n = n + 1
      array = array[:n]      
      return array;


def search_exe_tool(search_tool):
   if search_tool == 'which':
     q6_tools_path_linux = 'None'
     q6_tools_linux = 'None'
     tools_find = ''.join([search_tool, ' hexagon-sim'])
     proc = subprocess.Popen(tools_find, stdout=subprocess.PIPE, shell=True)
     (out, err) = proc.communicate()
     tools_match = re.search('(.*)(\d.\d.\d\d)', out)
     if tools_match:
        tools_path = tools_match.group(1).replace('\\', '/').rstrip('/')        
        if os.path.exists(tools_path):
           print 'For Linux: Hexagon tools taken from local path and not from recommended path: ', tools_path
           q6_tools_path_linux = tools_path
           q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version
     return (q6_tools_path_linux, q6_tools_linux)
   
   if search_tool == 'where':
     q6_tools_path_win = 'None'
     tools_find = ''.join(['where hexagon-sim'])
     proc = subprocess.Popen(tools_find, stdout=subprocess.PIPE, shell=True)
     (out, err) = proc.communicate()     
     tools_match = re.search('(.*)(\d.\d.\d\d)', out)   
     if tools_match:
        tools_path = tools_match.group(1).replace('\\', '/').rstrip('/')        
        if os.path.exists(tools_path):
           print 'For Windows: Hexagon tools taken from local path and not from recommended path: ', tools_path
           q6_tools_path_win = tools_path          
     return (q6_tools_path_win)
   
#=================================================================================
#================================================================================= 
def allOptionsCheck(opts, alloptions, defSysInfo, bldSysInfo):

    image_name = os.environ['IMAGE']
    for m in alloptions:
        if m == 'chipset':
           if not opts.chipset:
              bldSysInfo.chipset_param = ''                 
              if defSysInfo.default_chipset:
                 bldSysInfo.chipset_param = defSysInfo.default_chipset
                 print "chipset option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % bldSysInfo.chipset_param
              else:
                 print 'chipset option not specified, chipset_param is: NULL'
           else:  
              bldSysInfo.chipset_param = getattr(opts, m)
              bldSysInfo.chipset_flag = 1
           bldSysInfo.chipset_param = bldSysInfo.chipset_param.lower()       
        if m == 'buildid':
           if not opts.buildid:
              bldSysInfo.buildid_param = '0x8fffffff'
              if defSysInfo.default_buildid:
                 bldSysInfo.buildid_param = defSysInfo.default_buildid
                 print "buildid option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % bldSysInfo.buildid_param
              else:
                 print 'buildid option not specified, setting default:', bldSysInfo.buildid_param
           else:
              bldSysInfo.buildid_param = getattr(opts, m)
              bldSysInfo.buildid_flag = 1
           bldSysInfo.buildid_param = bldSysInfo.buildid_param.lower()       
        if m == 'protection_domain':  
           if not opts.protection_domain:
              protection_domain_param = 'mpd'
              if defSysInfo.default_pid:
                 protection_domain_param = defSysInfo.default_pid
                 print "protection domain option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % protection_domain_param
              else:          
                 print 'protection domain option not specified, setting default:', protection_domain_param
           else:
              protection_domain_param = getattr(opts, m)
              bldSysInfo.protection_domain_flag = 1
           protection_domain_param = protection_domain_param.lower()       
           if protection_domain_param == 'mpd':
              bldSysInfo.mpd_flag = 1
           if protection_domain_param == 'spd':
              bldSysInfo.spd_flag = 1
        if m == 'other_option':
           print 'other options:', opts.other_option
           if opts.other_option:

                 bldSysInfo.other_option_flag = 1
                 bldSysInfo.all_flag = verify_args('\Aall\Z', opts.other_option)
                 bldSysInfo.clean_flag = verify_args('\Aclean\Z', opts.other_option)
                 bldSysInfo.sim_flag = verify_args('\ASIM\Z', opts.other_option)
                 bldSysInfo.pkg_all_flag = verify_args('\Apkg_all\Z', opts.other_option)
                 bldSysInfo.pkg_hy22_flag = verify_args('\Apkg_hy22\Z', opts.other_option)
                 bldSysInfo.pkg_hk11_flag = verify_args('\Apkg_hk11\Z', opts.other_option)
                 bldSysInfo.pkg_hk22_flag = verify_args('\Apkg_hk22\Z', opts.other_option)
                 bldSysInfo.pkg_oem_flag = verify_args('\Apkg_oem\Z', opts.other_option)
                 bldSysInfo.pkg_hd11_flag = verify_args('\Apkg_hd11\Z', opts.other_option)
                 bldSysInfo.pkg_isv_flag = verify_args('\Apkg_isv\Z', opts.other_option)
                 bldSysInfo.pkg_hcbsp_flag = verify_args('\Apkg_hcbsp\Z', opts.other_option)
                 bldSysInfo.pkg_hd22_flag = verify_args('\Apkg_hd22\Z', opts.other_option)
           
           elif defSysInfo.default_others:             
                 bldSysInfo.all_flag = verify_args('\Aall\Z', defSysInfo.default_others.split(','))
                 bldSysInfo.clean_flag = verify_args('\Aclean\Z', defSysInfo.default_others.split(','))             
                    
           else:

                 bldSysInfo.all_flag = 1          
        
        opts_flags = []
        if m == 'flags':
           if opts.flags or defSysInfo.default_flags:
              bldSysInfo.flags_param = ''          
              if defSysInfo.default_flags:
                 opts_flags = defSysInfo.default_flags.split(',')
              if opts.flags:   
                 opts_flags.extend(''.join(opts.flags).split(','))
                 bldSysInfo.build_flags = 1
              for n in opts_flags:    
                  #print "flags options are:", n
                  targ = bldSysInfo.chipset_param[3:]
                  if n == 'ADSP' or n == 'adsp':
                     image_name = 'adsp'
                     os.environ['VARIANT_NAME'] = targ+'.adsp.prod'
                  elif n == 'CDSP' or n == 'cdsp':
                     image_name = 'cdsp'
                     os.environ['VARIANT_NAME'] = targ+'.cdsp.prod'
                  bldSysInfo.flags_param = ''.join([n, ',' , bldSysInfo.flags_param])          
              bldSysInfo.flags_param = bldSysInfo.flags_param.rstrip(',')
              
        if m == 'verbose':
           if opts.verbose:
              bldSysInfo.build_verbose_flag = 1
              bldSysInfo.opts_verbose = opts.verbose
              print 'build verbose: ', opts.verbose
           elif defSysInfo.default_verbose:          
              bldSysInfo.opts_verbose = defSysInfo.default_verbose
              print "Verbose default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_verbose

        if m == 'numcore':
           if opts.numcore:
              bldSysInfo.build_numcore_flag = 1
              bldSysInfo.opts_numcore = opts.numcore
              print 'build numcore: ', opts.numcore
           elif defSysInfo.default_numcore:          
              bldSysInfo.opts_numcore = defSysInfo.default_numcore
              print "number of cores for building default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_numcore
      
        if m == 'module':
           if opts.module:
              bldSysInfo.build_filter_flag = 1
              bldSysInfo.opts_module = opts.module
              print 'build filter: ', opts.module
           elif defSysInfo.default_module:
              bldSysInfo.opts_module = defSysInfo.default_module
              print "Module compilation value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_module          

        if m == 'alias':
           if opts.alias:
              bldSysInfo.image_alias_flag = 1
              bldSysInfo.opts_alias = opts.alias
              print 'image alias: ', opts.alias
           elif defSysInfo.default_alias:          
              bldSysInfo.opts_alias = defSysInfo.default_alias
              print "Module Alias value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_alias
                      
        if m == 'sconsargs':
           bldSysInfo.opts_sconsargs = ''
           if opts.sconsargs:
              bldSysInfo.build_sconsargs_flag = 1 
              bldSysInfo.opts_sconsargs = opts.sconsargs
              print 'build scons arguments: ', opts.sconsargs
           elif defSysInfo.default_sconsargs:
              bldSysInfo.opts_sconsargs = defSysInfo.default_sconsargs
              print "Scons arguments value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_sconsargs          

        if m == 'userargs':
           bldSysInfo.opts_userargs = ''
           if opts.userargs:
              bldSysInfo.build_userargs_flag = 1 
              bldSysInfo.opts_userargs = opts.userargs
              print 'build custom user arguments: ', opts.userargs
           elif defSysInfo.default_userargs:
              bldSysInfo.opts_userargs = defSysInfo.default_userargs
              print "Custom user arguments value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % defSysInfo.default_userargs 

        if m == 'toolversion':
           if opts.toolversion:
              print 'Hexagon Tool version set from command line: ', opts.toolversion
           else: # no toolversion specified, set to NULL, this will be set in set_chipset_target()
              opts.toolversion = ''
              print 'No Hexagon Tool version specified from command line, will use chipset default!'

    print 'Building for Image name: %s!!!' %image_name
    os.environ['IMAGE'] = image_name
    os.environ['VARIANT_NAME'] = targ+'.'+image_name.lower()+'.prod'
    print "\n\n"



   
#=================================================================================
#=================================================================================
#                  Function definitions ends here
#=================================================================================
#=================================================================================

