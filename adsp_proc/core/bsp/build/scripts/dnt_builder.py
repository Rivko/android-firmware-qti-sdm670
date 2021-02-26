# vi: tw=128 ts=3 sw=3 et :
# @file dnt_builder.py
# @brief This file contains the API for the DNT/TMS SCONS Tool Loader, API 1.0
#===============================================================================
# NOTE: The @brief description above does not appear in the PDF.
# The tms_mainpage.dox file contains the group/module descriptions that
# are displayed in the output PDF generated using Doxygen and LaTeX. To
# edit or update any of the group/module text in the PDF, edit the
# tms_mainpage.dox file or contact Tech Pubs.
#===============================================================================
#===============================================================================
# Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#===============================================================================
#===============================================================================
# Edit History
# $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/dnt_builder.py#1 $
# $DateTime: 2017/07/21 22:10:47 $
# $Change: 13907704 $
# $Author: pwbldsvc $
#===============================================================================

# Module Version, Python Standard PEP8, 2001, https://www.python.org/dev/peps/pep-0008/
__version__ = "$Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/dnt_builder.py#1 $"

# Shared Library Version, Semantic Versioning 2.0.0, http://semver.org/
__version_info__ = "1.0.0"

#===============================================================================
# Description
#===============================================================================

# This SCONS tool is the generic tool loader for TMS. Tools are supplied in a list
# and if they are found in the PL scripts directory, then they are loaded.

# Debug instruments are exposed by adding to command line as
# build.cmd build_target BUILD_VER=00000 --debug=stacktrace --debuginfo=tms

#===============================================================================
# Module Imports
#===============================================================================

import re
import string
import os
import sys
import socket
from xml.dom import minidom
from SCons.Script import *
import fnmatch

#===============================================================================
# Module Scope Definitions
#===============================================================================

TMS_TOOLS_DIR1 = '${BUILD_ROOT}/core/bsp/build/scripts'
TMS_TOOLS_DIR2 = '${BUILD_ROOT}/core/debugtools/build/scripts'

TMS_TOOLS = [
   'tms_builder_errcb',          # SCONS Tool for ERR Callbacks
   'tms_builder_rcinit',         # SCONS Tool for RCINIT Framework
   'tms_builder_rcinit_info',    # SCONS Tool for RCINIT Framework
]

#===============================================================================
# SCONS Hooks
#===============================================================================

def exists(env):

   # Common Methods

   env.AddMethod(tool_error,              'TmsError')       # env.TmsError([list_of_strings])
   env.AddMethod(tool_warning,            'TmsWarning')     # env.TmsWarning([list_of_strings])
   env.AddMethod(tool_info,               'TmsInfo')        # env.TmsInfo([list_of_strings])
   env.AddMethod(tool_debug,              'TmsDebug')       # env.TmsDebug([list_of_strings])
   env.AddMethod(tool_get_path,           'TmsGetPath')     # env.TmsGetPath(rootDir, fileSpec)
   env.AddMethod(tool_represents_float,   'TmsIsFloat')     # env.TmsIsFloat(object)
   env.AddMethod(tool_represents_int,     'TmsIsInt')       # env.TmsIsInt(object)
   env.AddMethod(tool_represents_str,     'TmsIsStr')       # env.TmsIsStr(object)
   env.AddMethod(tool_str_to_int,         'TmsToInt')       # env.TmsToInt(object)
   env.AddMethod(tool_namehash,           'TmsNameHash')    # env.TmsNameHash(string)

   # python version check
   if 0x02070000 > sys.hexversion:
      env.TmsWarning([ 'Fails Minimum Version Check of Python v2.7, Available at https://www.python.org/downloads/' ])

   build_no = 'N/A'

   # pywin32 version check
   if 'linux2' == sys.platform:
      pass

   elif 'win32' == sys.platform:
      try:
         import distutils
         import distutils.sysconfig
         site_packages = distutils.sysconfig.get_python_lib(plat_specific=1)
         build_no = open(os.path.join(site_packages, "pywin32.version.txt")).read().strip()
         if 219 > int(build_no):
            env.TmsWarning([ 'Fails Minimum Version Check of PyWin32 Build 219, Available at http://sourceforge.net/projects/pywin32/files/pywin32/' ])
      except:
         env.TmsWarning([ 'Fails Installation of PyWin32, Available at http://sourceforge.net/projects/pywin32/files/pywin32/' ])

   image_name = env.get('IMAGE_NAME')

   image_aliases = []
   for image_alias in env.get('IMAGE_ALIASES'):
      if env.IsTargetEnable(image_alias):
         image_aliases.extend([ image_alias ])
      elif env.IsTargetEnable(image_alias.upper()):
         image_aliases.extend([ image_alias.upper() ])
      else:
         image_aliases.extend([ '!' +image_alias ])

   build_tags = []
   for build_tag in env.get('IMAGE_BUILD_TAGS'):
      if env.IsTargetEnable(build_tag):
         build_tags.extend([ build_tag ])
      elif env.IsTargetEnable(build_tag.upper()):
         build_tags.extend([ build_tag.upper() ])
      else:
         build_tags.extend([ '!' +build_tag ])

   for tool_name in TMS_TOOLS:
      if env.PathExists(''.join([ TMS_TOOLS_DIR2, '/', tool_name ])) or env.PathExists(''.join([ TMS_TOOLS_DIR2, '/', tool_name, '.py'])):
         env.LoadToolScript(tool_name, toolpath = [ TMS_TOOLS_DIR2 ])
         env.TmsInfo([ 'Loads SCONS tool', tool_name ])
      elif env.PathExists(''.join([ TMS_TOOLS_DIR1, '/', tool_name ]))or env.PathExists(''.join([ TMS_TOOLS_DIR1, '/', tool_name, '.py'])):
         env.LoadToolScript(tool_name, toolpath = [ TMS_TOOLS_DIR1 ])
         env.TmsInfo([ 'Loads SCONS Tool', tool_name ])

   env.TmsInfo([ 'Host name         ', socket.gethostname() ])
   env.TmsInfo([ 'Host platform     ', sys.platform ])
   env.TmsInfo([ 'Python version    ', sys.version ])
   env.TmsInfo([ 'PyWin32 version   ', build_no ])
   env.TmsInfo([ 'Image name        ', image_name ])
   env.TmsInfo([ 'Image aliases     ', image_aliases ])
   env.TmsInfo([ 'Image build tags  ', build_tags ])
   env.TmsInfo([ 'Target name       ', env.get('TARGET_NAME') ])
   env.TmsInfo([ 'Default targets   ', SConscript.DefaultTargets ])

   return True

def generate(env):

   # Pathing to Configuration Files by Tool

   if env.PathExists('${BUILD_ROOT}/build/scripts/build_utils.py'):
      env.LoadToolScript('build_utils', toolpath = [ '${BUILD_ROOT}/build/scripts' ])
      CUST_CONFIG_PATH = env.FindConfigFiles('cust_config.xml')[0]
      xmldoc = minidom.parse(CUST_CONFIG_PATH)
      itemlist = xmldoc.getElementsByTagName('max_threads')
      for s in itemlist :
         env['TMS_MAX_THREADS'] = str(s.attributes['value'].value)
         env.TmsInfo([ 'TMS_MAX_THREADS configured value', env['TMS_MAX_THREADS'] ])

#===============================================================================

def tool_error(env, Info):
   ''' FATAL Information, Stops Build '''
   if isinstance(Info, list):
      env.PrintError(' '.join(Flatten(['TMS', caller_name(), caller_line(), map(str, Flatten(Info))])))
   else:
      env.PrintError(' '.join(['TMS', caller_name(), caller_line(), str(Info)]))
   raise Exception(' '.join(['TMS', caller_name(), caller_line(), 'EXPLICIT TERMINATION', socket.gethostname()]))

def tool_warning(env, Info):
   ''' Non-FATAL Information, Build Continues '''
   if isinstance(Info, list):
      env.PrintWarning(' '.join(Flatten(['TMS', caller_name(), caller_line(), map(str, Flatten(Info))])))
   else:
      env.PrintWarning(' '.join(['TMS', caller_name(), caller_line(), str(Info)]))

def tool_info(env, Info):
   ''' Informative Information, Build Continues '''
   if isinstance(Info, list):
      env.PrintInfo(' '.join(Flatten(['TMS', map(str, Flatten(Info))])))
   else:
      env.PrintInfo(' '.join(['TMS', str(Info)]))

def tool_debug(env, Info):
   ''' Diagnostic Information, Build Continues '''
   if isinstance(Info, list):
      env.PrintDebugInfo('tms', ' '.join(Flatten(['TMS', caller_name(), caller_line(), map(str, Flatten(Info))])))
   else:
      env.PrintDebugInfo('tms', ' '.join(['TMS', caller_name(), caller_line(), str(Info)]))

def caller_name():
   ''' Caller Module, Name '''
   import sys
   return sys._getframe(3).f_code.co_name

def caller_line():
   ''' Caller Module, Line '''
   import sys
   return str(sys._getframe(3).f_lineno)

def tool_get_path(env, rootDir, pattern):
   ''' Tool Utility Function '''
   matches = []
   for root, dirs, files in os.walk(rootDir):
      for filename in fnmatch.filter(files, pattern):
         matches.append(os.path.join(root, filename).replace(os.sep, '/'))
   return matches

def tool_represents_float(env, s):
   ''' Tool Utility Function '''
   try:
      float(s)
      return True
   except ValueError:
      return False

def tool_represents_int(env, s):
   ''' Tool Utility Function '''
   try:
      int(s)
      return True
   except ValueError:
      return False

def tool_represents_str(env, s):
   ''' Tool Utility Function '''
   try:
      str(s)
      return True
   except ValueError:
      return False

def tool_str_to_int(env, s):
   ''' Tool Utility Function '''
   if tool_represents_int(env, s):
      return int(s)
   env.TmsError([RCINIT_POC_IO, 'Fails!!'])

def tool_namehash(env, s):
   ''' Tool Utility Function '''
   tmp = 0
   hash = 0
   for i in range(len(s)):
      hash = (hash << 4) + ord(s[i])
      tmp = hash & 0xf0000000
      if tmp != 0:
         hash ^= (tmp >> 24)
      hash &= ~tmp
   return hash
