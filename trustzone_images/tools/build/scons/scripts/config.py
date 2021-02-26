#===============================================================================
#
# Common utils
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/config.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Author: pwbldsvc $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   wd      Create
#
#===============================================================================
import os
import re
import errno
import glob
import sys
import subprocess
import stat
import string
import tempfile
import SCons.Action
import SCons.Errors
import SCons.Tool
from SCons.Script import *
import inspect
#import time

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('external_config')

def generate(env):
   env.Replace(BUILDDATA_ROOT = "${BUILD_ROOT}/tools/build/scons/data")
   env.Replace(BUILDAMSS_ROOT = "${BUILD_ROOT}/build/ms")
   
   env.AddMethod(init_build_config, "InitBuildConfig")
   env.AddMethod(init_usesflags, "InitUsesFlags")
   env.AddMethod(init_incpaths, "InitIncPaths")
   env.AddMethod(init_image_vars, "InitImageVars")
   
   # list public and private APIs
   listapis = env.GetOption('listapis')
   if listapis:
      list_apis(env)
      
   return None

#===============================================================================
# Init External environment header, etc
#===============================================================================
def init_build_config(env, target_files=True):
   #conf = Configure(env)
   #env = conf.Finish()
      
   init_data_fodler(env)
      
   if target_files:
      check_target_config(env)
      if 'TARGET_CONFIG' in env:
         init_target_config(env)
      else:
         init_custheader(env)
         init_usesflags(env)
      init_usesflags_cl(env)
      init_incpaths(env)
   else:
      # special case, sucng as creating documents we don't need any target 
      # specific information such as inc paths nor uses flags.
      env.AddMethod(do_nothing, "UsesInc")
      env.AddMethod(do_nothing, "UsesExternalInc")
      env.AddMethod(do_nothing, "InitAUIncPaths")
      env.AddMethod(do_nothing, "PublishPublicApi")
      env.AddMethod(do_nothing, "PublishRestrictedApi")
      env.AddMethod(do_nothing, "PublishProtectedApi")
      env.AddMethod(do_nothing, "PublishPrivateApi")
      env.AddMethod(do_nothing, "RequirePublicApi")
      env.AddMethod(do_nothing, "RequireRestrictedApi")
      env.AddMethod(do_nothing, "RequireProtectedApi")
      env.AddMethod(do_nothing, "RequirePrivateApi")
      env.AddMethod(do_nothing, "RequireExternalApi")
      env.AddMethod(do_nothing, "GetCoreBspPath")
      env.AddMethod(do_nothing, "GetExternalPath")
   
   # add builders
   env.PrintDebugInfo('scripts', "Loading TOOLs scripts...")
   
   image_tools = env.get('IMAGE_TOOLS', [])
   
   if len(image_tools) > 0:
      #import pdb; pdb.set_trace()
      for tool in image_tools:
         ext = ".py"
         
         (tool_base, tool_ext) = os.path.splitext(tool)
         if tool_ext.lower() == ".py":
            ext = ""
            
         if env.PathExists("".join(["${BUILD_SCRIPTS_ROOT}/", tool , ext])):
            env.LoadToolScript("".join(["${BUILD_SCRIPTS_ROOT}/", tool , ext]))
         elif env.PathExists("".join([tool , ext])):
            env.LoadToolScript("".join([tool , ext]))
         else:
            env.PrintWarning ("Cannot find tool named [%s]!!" % str(tool))

   return None
   
#===============================================================================
# List Public and Restricted APIs
#===============================================================================
def list_apis(env):
   #t_start = time.clock()
   env.PrintStageBanner("Public and Restricted APIs")
   env.InitBuildConfig(target_files=False)
   
   # get valid SCons AU names and create the list of paths
   au_list = env.GetAreaSoftwareUnitNames()
   if type(au_list) is not list:
      au_list = [au_list]

   au_root_dir_patterns = env.get('AU_ROOT_DIR_PATTERNS')
   if type(au_root_dir_patterns) is not list:
      au_root_dir_patterns = [au_root_dir_patterns]
      
   au_pattern_list = []
   au_name_index = 0
   au_pattern_index = 1
   for au in au_list:
      au_pattern_list.append([au, [string.replace(pattern, '*', au ) for pattern in au_root_dir_patterns]])
      
   #import pdb; pdb.set_trace()      
   
   # list Public APIs
   env.PrintStageBanner("Public APIs")   
   env.AddMethod(publish_print_public_api,      "PublishPublicApi")
   env.AddMethod(publish_dummy_restricted_api,  "PublishRestrictedApi")
   env.AddMethod(publish_invalid_protected_api, "PublishProtectedApi")
   env.AddMethod(publish_invalid_private_api,   "PublishPrivateApi")
   
   for au in au_pattern_list:
      env.PrintDebugInfo('any', "")
      env.PrintDebugInfo('any', "================================================================")
      env.PrintDebugInfo('any', "Public APIs for AU: %s" % string.upper(au[au_name_index]))
      env.Replace(AU_NAME = au[au_name_index])
      env.LoadAPIUnits(au[au_pattern_index])
   
   # list Restricted APIs
   env.PrintStageBanner("Restricted APIs")   
   env.AddMethod(publish_dummy_public_api,      "PublishPublicApi")
   env.AddMethod(publish_print_restricted_api,  "PublishRestrictedApi")
   env.AddMethod(publish_invalid_protected_api, "PublishProtectedApi")
   env.AddMethod(publish_invalid_private_api,   "PublishPrivateApi")

   for au in au_pattern_list:
      env.PrintDebugInfo('any', "")
      env.PrintDebugInfo('any', "================================================================")
      env.PrintDebugInfo('any', "Restricted APIs for AU: %s" % string.upper(au[au_name_index]))
      env.Replace(AU_NAME = au[au_name_index])      
      env.LoadAPIUnits(au[au_pattern_index])
   
   #t_end = time.clock()
   #print "** Elapsed time = " + str(round(t_end - t_start, 3))

#===============================================================================
# Do nothing fucntion
#===============================================================================
def do_nothing(env, *args, **kargs):
   return None
   
#===============================================================================
# Init cust/targ files module
#===============================================================================
def init_data_fodler(env):
   data_dir = "${BUILDDATA_ROOT}"
   if not env.PathExists(data_dir):
      env.PrintInfo("creating DATA folder for uses flags and ext incpaths.")
      os.mkdir(env.subst(data_dir))
   return None

#===============================================================================
# Init target configuration
#===============================================================================
def check_target_config(env):
   targ_config = env['BUILD_SCRIPTS_TCFG_ROOT'] + "/target_config.py"
   if env.PathExists(targ_config):
      env['TARGET_CONFIG'] = targ_config

#===============================================================================
# Init target configuration
#===============================================================================
def init_target_config(env):
   [targ_config_tool_path, targ_config_tool_name] = os.path.split(env['TARGET_CONFIG'])
   targ_config_tool_name = targ_config_tool_name.rsplit('.', 1)[0]  # Remove the .py suffix
   env.LoadToolScript(targ_config_tool_name, toolpath=[targ_config_tool_path])
   if not env.GenTargetConfig(env['BUILD_ID']):
      env.PrintError("Target Config failed to generate")
      raise RuntimeError, 'Target Config Generation Failed!!!'
   return None

#===============================================================================
# Init cust/targ files module
#===============================================================================
def init_custheader(env):
   generate_cust = False
   custfile_name = env.subst("cust${BUILD_ID}").lower()
   custfile_path = env.subst("${BUILDAMSS_ROOT}/"+ custfile_name + ".h")
   buildsfile_name = env.subst("${CHIPSET}.builds").lower()
   # builds file uses uppercase X in the name
   buildsfile_name = buildsfile_name.replace("x", "X")
   buildsfile_path = env.subst("${BUILDAMSS_ROOT}/"+ buildsfile_name)
   buildsfile_data_name = env.subst("${BUILD_ID}").lower()
   buildsfile_data_base = env.subst("${BUILDDATA_ROOT}/" + buildsfile_data_name)
   buildsfile_data = buildsfile_data_base + ".dat"
   buildsfile_md5 = buildsfile_data_base + ".md5"
   
   # check if builds file exists other wise do nothing; most possibly
   # stip/pack build expect everything to be done.
   if not env.PathExists(buildsfile_path):
      return None
   
   # Check to see if we need to regen any data, 
   if (not env.CompareAndUpdateMD5File(buildsfile_path, buildsfile_md5) \
      or not env.PathExists(custfile_path) or not env.PathExists(buildsfile_data)) \
      and not env.GetOption('frommake'):
      generate_cust = True

   if generate_cust:
      # do not lower the entire build path as AMSS needs to be upper in Linux builds
      data_file = env.NormPath(buildsfile_data)
      
      if sys.platform.startswith("linux"):
         gencust_cmd = "sh linux_build ${BUILD_ID} -m corebsp_create_files"
         #gencust_cmd = "sh linux_build ${BUILD_ID} -m corebsp_uses -m -p > " + data_file
      else:
         gencust_cmd = "build.cmd ${BUILD_ID} -m corebsp_create_files"
         #gencust_cmd = "build.cmd ${BUILD_ID} -m corebsp_uses -m -p > " + data_file
      
      env.PrintInfo("creating AMSS cust and dat files  " + custfile_name)
      data, err, rv = env.ExecCmds(gencust_cmd, "${BUILDAMSS_ROOT}", 
         target=custfile_name, silent=True, shell=True)
      
      if not env.PathExists(data_file):
         env.PrintError("================================================================")
         env.PrintError("Call to MAKE generated no data..")
         env.PrintError("Expected data file not found!! [%s]" % data_file)
         env.PrintError("BLDCMD: %s" % gencust_cmd)
         env.PrintError("STDOUT:\n%s" % data)
         env.PrintError("STDERR:\n%s" % err)
         raise RuntimeError, 'MAKE call failed!!'
         
      if rv or not re.search("COREBSP_CREATE_DATA_INCPATHS_FILE", data):
         env.PrintError("================================================================")
         env.PrintError("MAKE fail to create a data file return value is [%d]" % rv)
         env.PrintError("BLDCMD: %s" % gencust_cmd)
         env.PrintError("STDOUT:\n%s" % data)
         env.PrintError("STDERR:\n%s" % err)
         raise RuntimeError, 'MAKE call failed!!'


   cmdline_targets = map(str, COMMAND_LINE_TARGETS)
   
   return None
   
#===============================================================================
# Init usesflags module
#===============================================================================
def init_usesflags(env):
   # ==========================================================================
   # Decode build ID for FFA/SURF
   # ==========================================================================

   #import pdb; pdb.set_trace()
   STRING_OFFSET = 0
   if len(env['BUILD_ID']) == 10:
      STRING_OFFSET = 1
   elif len(env['BUILD_ID']) == 9:
      STRING_OFFSET = 0
   elif len(env['BUILD_ID']) == 6:
      STRING_OFFSET = 0

   if env['BUILD_ID'][STRING_OFFSET] == "A":
      STRING_OFFSET = STRING_OFFSET + 1
   elif env['BUILD_ID'][STRING_OFFSET] == "T":
      STRING_OFFSET = STRING_OFFSET + 1

   if env['BUILD_ID'][STRING_OFFSET] == "F":
      env.Replace(USES_FFA = 'yes')
   elif env['BUILD_ID'][STRING_OFFSET] == "R":
      env.Replace(USES_SURF = 'yes')
   elif env['BUILD_ID'][STRING_OFFSET] == "S":
      env.Replace(USES_SURF = 'yes')
   elif env['BUILD_ID'][STRING_OFFSET] == "H":
      env.Replace(USES_SURF = 'yes')   
   elif env['BUILD_ID'][STRING_OFFSET] == "A":
      env.Replace(USES_SURF = 'yes')   


   #---------------------------------------------------------------------------
   # Load in external path variables
   #---------------------------------------------------------------------------
   usesflags_fname = env.subst("uses${BUILD_ID}").lower()
   amss_usesflags_generator(env, env.NormPath("${BUILDDATA_ROOT}/" + usesflags_fname))
   env.LoadToolScript(usesflags_fname, toolpath = ['${BUILDDATA_ROOT}'])
   

#===============================================================================
# Init usesflags from the command line
#===============================================================================
def init_usesflags_cl(env):
   #---------------------------------------------------------------------------
   # Load any USES_FALGS pass in the command line
   #---------------------------------------------------------------------------
   #import pdb; pdb.set_trace()      
   uses_flags = env.get('USES_FLAGS')
   uses_flags_cl = env.get('USES_FLAGS_CL')

   if uses_flags_cl is not None:
      uses_flags_cl = uses_flags_cl.split(",")

      if uses_flags is not None:
         env.Replace(USES_FLAGS = uses_flags + uses_flags_cl)
      else:
         env.Replace(USES_FLAGS = uses_flags_cl)
         
      for uf in uses_flags_cl:
         env[uf] = True

   env.PrintDebugInfo('uses_flags', "Top level uses flags are: %s" % str(env.get('USES_FLAGS')))
   
#===============================================================================
# Init incpaths module
#===============================================================================
def init_incpaths(env):
   #---------------------------------------------------------------------------
   # Add methods UsesInclude
   #---------------------------------------------------------------------------
   env.AddMethod(uses_inc, "UsesInc")
   env.AddMethod(uses_external_inc, "UsesExternalInc")

   env.AddMethod(init_au_incpaths, "InitAUIncPaths")

   env.AddMethod(publish_public_api,      "PublishPublicApi")
   env.AddMethod(publish_restricted_api,  "PublishRestrictedApi")
   env.AddMethod(publish_invalid_protected_api, "PublishProtectedApi")
   env.AddMethod(publish_invalid_private_api,   "PublishPrivateApi")
   
   env.AddMethod(get_corebsp_path, "GetCoreBspPath")
   env.AddMethod(get_external_path, "GetExternalPath")
   
   #---------------------------------------------------------------------------
   # Init incpath environment
   #---------------------------------------------------------------------------
   scons_incpaths_env = env.Clone()

   scons_incpaths_env.Replace(SRCROOT = env['BUILD_ROOT'])
   scons_incpaths_env.Replace(ROOT = env['INC_ROOT'])

   # publish/require path init
   scons_incpaths_env.Replace(REQUIRE_LIST = [])

   env.Replace(SCONS_INCPATHS_ENV = scons_incpaths_env)
   scons_incpaths_env.Replace(SCONS_INCPATHS_ENV = scons_incpaths_env)

   incpaths_ext_env = scons_incpaths_env.Clone()
   env.Replace(INCPATHS_EXT_ENV = incpaths_ext_env)
   incpaths_ext_env.Replace(INCPATHS_EXT_ENV = incpaths_ext_env)

   #---------------------------------------------------------------------------
   # Load in internal path variables
   #---------------------------------------------------------------------------

   
   # get valid SCons AU names and create the list of paths
   au_list = env.GetAreaSoftwareUnitNames()
   if type(au_list) is not list:
      au_list = [au_list]

   au_root_dir_patterns = env.get('AU_ROOT_DIR_PATTERNS')
   if type(au_root_dir_patterns) is not list:
      au_root_dir_patterns = [au_root_dir_patterns]
      
   au_pattern_list = []
   au_name_index = 0
   au_pattern_index = 1
   for au in au_list:
      au_pattern_list.append([au, [string.replace(pattern, '*', au ) for pattern in au_root_dir_patterns]])
      
   #import pdb; pdb.set_trace()      
   
   # list Public APIs
   env.PrintDebugInfo('scripts', "Loading public APIs...")
   for au in au_pattern_list:
      env.Replace(AU_NAME = au[au_name_index])
      env.LoadAPIUnits(au[au_pattern_index])

   env.Replace(AU_NAME = 'UNKNOWN')


   #---------------------------------------------------------------------------
   # Load in external path variables
   #---------------------------------------------------------------------------
   
   if 'TARGET_CONFIG' in env:
      incpaths_ext_env.Dictionary().update(env.GetTargetIncpaths())
   else:
      incpaths_fname = env.subst("incpaths${BUILD_ID}").lower()
      amss_incpaths_generator(env, env.NormPath("${BUILDDATA_ROOT}/" + incpaths_fname))
      incpaths_ext_env.LoadToolScript(incpaths_fname, toolpath = ['${BUILDDATA_ROOT}'])

   toolpath = ['${BUILD_SCRIPTS_ROOT}']
   
   incpaths_ext_env.LoadToolScript('externalpaths', toolpath = toolpath)

   #---------------------------------------------------------------------------
   # init publish/require api methods
   #---------------------------------------------------------------------------
   env.AddMethod(publish_invalid_public_api,    "PublishPublicApi")
   env.AddMethod(publish_invalid_restricted_api,"PublishRestrictedApi")
   env.AddMethod(publish_protected_api,         "PublishProtectedApi")
   env.AddMethod(publish_private_api,           "PublishPrivateApi")

   env.AddMethod(require_public_api,            "RequirePublicApi")
   env.AddMethod(require_restricted_api,        "RequireRestrictedApi")
   env.AddMethod(require_protected_api,         "RequireProtectedApi")
   env.AddMethod(require_private_api,           "RequirePrivateApi")

   env.AddMethod(require_external_api,          "RequireExternalApi")
   

   return None

#===============================================================================
# Init incpaths module
#===============================================================================
def init_au_incpaths(env):
   #root_env.Replace(AU_INCPATHS_ENV = au_incpaths_env)
   env.Replace(AU_INCPATHS_ENV = env.Clone())
   
   # init path publish methods
   env.AddMethod(publish_dummy_public_api,      "PublishPublicApi")
   env.AddMethod(publish_au_restricted_api,     "PublishRestrictedApi")   
   env.AddMethod(publish_invalid_protected_api, "PublishProtectedApi")
   env.AddMethod(publish_invalid_private_api,   "PublishPrivateApi")
   
   # Load in restricted AU path variables
   au_name = env.get('AU_NAME')
   
   env.PrintDebugInfo('scripts', "Loading " + au_name.upper() + " restricted APIs...")   
   env.LoadAPIUnits(env.get('AU_DIR_PATTERNS'))

   # init publish/require methods
   env.AddMethod(publish_invalid_public_api,    "PublishPublicApi")
   env.AddMethod(publish_invalid_restricted_api,"PublishRestrictedApi")
   env.AddMethod(publish_protected_api,         "PublishProtectedApi")
   env.AddMethod(publish_private_api,           "PublishPrivateApi")
   
   env.AddMethod(require_au_restricted_api,  "RequireRestrictedApi")
   
   #---------------------------------------------------------------------------
   # Load in external path variables per AU
   #---------------------------------------------------------------------------
   incpaths_ext_env = env.Clone()
   env.Replace(INCPATHS_EXT_ENV = incpaths_ext_env)
   incpaths_ext_env.Replace(INCPATHS_EXT_ENV = incpaths_ext_env)
   
   if 'TARGET_CONFIG' in env:
      incpaths_ext_env.Dictionary().update(env.GetTargetIncpaths())
   else:
      incpaths_fname = env.subst("incpaths${BUILD_ID}").lower()
      amss_incpaths_generator(env, env.NormPath("${BUILDDATA_ROOT}/" + incpaths_fname))
      incpaths_ext_env.LoadToolScript(incpaths_fname, toolpath = ['${BUILDDATA_ROOT}'])

   externalpath_file_path = "${BUILD_ROOT}/${AU_NAME}/build"
   externalpath_file_name = "externalpaths"
   externalpath_file_ext = ".py"

   #import pdb; pdb.set_trace()      
   if not env.PathExists("".join([externalpath_file_path, "/", externalpath_file_name, externalpath_file_ext])):
      externalpath_file_path = "${BUILD_SCRIPTS_ROOT}"
   
   incpaths_ext_env.LoadToolScript('externalpaths', toolpath = [externalpath_file_path])
   
   return None

#===============================================================================
# Include PATHs / PATHs file builder
#===============================================================================
def inc_paths_builder(env, incpaths_env, api_names, dbg_str=None, fname=None):
   paths_to_add = list()
   # get command line options
   useincfiles = env.GetOption('useincfiles')
   #debugincpaths_str = env.GetOption('debugincpaths').lower()
   debuginfo_opt = env.get('DEBUGINFO_OPT')
   debugincpaths = False
   clean = env.GetOption('clean')
   cwd = os.getcwd()

   if dbg_str == None and fname != None:
      dbg_str = "Require APIs for " + file
   elif dbg_str == None:
      dbg_str = "Require APIs"
      
   if fname == None:
      useincfiles = False

   # turn on debugging
   if ('incpaths' in debuginfo_opt) or ('incpaths-int' in debuginfo_opt) or ('incpaths-ext' in debuginfo_opt): 
      debugincpaths = True
   
   if not useincfiles:
      if debugincpaths:
         env.PrintDebugInfo('any', "================================================================")
         env.PrintDebugInfo('any', "=== " + dbg_str)
         env.PrintDebugInfo('any', "=== from " + cwd)
         env.PrintDebugInfo('any', "----------------------------------------------------------------")
         env.PrintDebugInfo('any', api_names)
         
      # don't use external or internal inlcude files, put incpaths in comand line
      for api_name in api_names:
         
         #import pdb; pdb.set_trace()
         if api_name in incpaths_env:
            incpath_list = incpaths_env[api_name]
            for p in incpath_list:
               if p not in paths_to_add:
                  paths_to_add.append(p)
      for p in paths_to_add:
        exists = os.path.exists(p)
        if debugincpaths:
          env.PrintDebugInfo('any', " {} = {}".format('i' if exists else 'x', p))
        if exists:
          env.AppendUnique(CPPPATH=p)

   else: # useincfiles is True
      # create external or internal include files
      # Little hack, to get the correct location for the file
      # if the target path is the same as the script, scons will return
      # the full path from the File method, but if the target dir is not
      # in CWD it will return just the file with no path
      check_build_path = env.File(fname)

      #print os.path.realpath(fname)
      #print os.path.realpath(str(check_build_path))

      # determine file full path
      if os.path.realpath(str(check_build_path)) == os.path.realpath(fname):
         target_file = os.path.realpath(os.getcwd() + "/" + env.subst('${BUILDPATH}') + "/" + fname + ".inc")
      else:
         target_file = os.path.realpath(os.getcwd() + "/" + fname + ".inc")

      if clean:
         #Delete(target_file)
         if os.path.exists(target_file):
            os.unlink(target_file)
            print env.subst("Removed ") + target_file
      else:
         incpaths_dict = dict()
         incpaths_list = []

         if debugincpaths:
            print "================================================================"
            print "=== " + dbg_str
            print "=== from " + cwd
            print "----------------------------------------------------------------"
            print list
         for incpath in list:
            for i in incpaths_env.subst(incpath).split():
               if os.path.exists(i):
                  i = os.path.realpath(i)
                  if debugincpaths:
                     print " i = " + i
                  #env.AppendUnique(INCPATHS = "${INCPREFIX}"+ i +" ")
                  i = incpaths_env.subst("${INCPREFIX}") + i
                  if i not in incpaths_dict:
                     incpaths_dict[i] = 0
                     incpaths_list.append(i)
               else:
                  if debugincpaths:
                     print " x = " + i
         if debugincpaths:
            print "----------------------------------------------------------------"

         if not os.path.exists(os.path.split(target_file)[0]):
            os.mkdir(os.path.split(target_file)[0])

         incpaths_file = open(target_file, 'a')

         for incpath in incpaths_list:
            # write inc paths file
            incpaths_file.write (incpath + '\n')
         incpaths_file.close()

         env.Append(INCPATHS = ' --via ' + target_file)

   return None

#===============================================================================
# Use Inc
#===============================================================================
def uses_inc(env, list):
   api_env = env['SCONS_INCPATHS_ENV']
   dbg_str="Legacy Use Internal PATHs"
   #import pdb; pdb.set_trace()
   list = [i.replace("${","") for i in list]
   list = [i.replace("}","") for i in list]
   list = [i.replace("API","PUBLIC") for i in list]
   list = [i.replace("LEGACY","RESTRICTED") for i in list]
   inc_paths_builder(env, api_env, list, dbg_str, fname="internal")
   return None

#===============================================================================
# Use External Inc
#===============================================================================
def uses_external_inc(env, list):
   #api_env = env['INCPATHS_EXT_ENV']
   #dbg_str="Legacy Use External PATHs"
   #import pdb; pdb.set_trace()
   list = [i.replace("${","") for i in list]
   list = [i.replace("}","") for i in list]
   list = [i.replace("_INC","") for i in list]
   require_external_api(env, list)
   #inc_paths_builder(env, api_env, list, dbg_str, fname="external")
   return None

#===============================================================================
# amss_incpaths_generator(env, filename)
#   Create incpaths<buildid>.py by scanning and running AMSS make system on incpaths.min
#===============================================================================
def amss_incpaths_generator(env, filename):
   mycwd = os.getcwd()
   output_filename = env.NormPath(filename + '.py')
   output_filename_md5 = env.NormPath(filename + '.md5')
   buildsfile_name = env.subst("${CHIPSET}.builds").lower()
   # builds file uses uppercase X in name
   buildsfile_name = buildsfile_name.replace("x", "X")
   buildsfile_path = env.subst("${BUILDAMSS_ROOT}/"+ buildsfile_name)
   buildsfile_data_name = env.subst("${BUILD_ID}").lower()
   buildsfile_data = env.subst("${BUILDDATA_ROOT}/" + buildsfile_data_name + ".dat")   
   
  
   cmdline_targets = map(str, COMMAND_LINE_TARGETS)

   generate_incpaths = False

   # check if builds file exists other wise do nothing; most possibly
   # stip/pack build expect everything to be done.
   if env.PathExists(buildsfile_data):
      if not env.CompareAndUpdateMD5File(buildsfile_data, output_filename_md5) or \
         not env.PathExists(output_filename):
         generate_incpaths = True
   else:
      env.PrintError("Expected build data file not found!! [%s]" % buildsfile_data)
      raise RuntimeError, 'missing build data file.'

   if generate_incpaths:
      env.PrintInfo("creating AMSS incpaths for " + os.path.basename(filename))
      genpaths_script = env.NormPath('${BUILD_SCRIPTS_ROOT}/genincpaths.py')
      
      if not env.PathExists(buildsfile_data):
         env.PrintError("Expected data file not found!! [%s]" % buildsfile_data)
         raise RuntimeError, 'missing data file.'
         
      cmds = "${PYTHONCMD} %s -f %s -o %s" % (genpaths_script, buildsfile_data, output_filename)
      
      env.ExecCmds(cmds, "${BUILDAMSS_ROOT}", target=os.path.basename(output_filename))
   return None

#===============================================================================
# amss_usesflags_generator(env, filename)
#   Create uses<buildid>.py by scanning and running AMSS make system for USES flags
#===============================================================================
def amss_usesflags_generator(env, filename):
   mycwd = os.getcwd();
   output_filename = env.NormPath(filename + '.py')
   output_filename_md5 = env.NormPath(filename + '.md5')
   buildsfile_name = env.subst("${CHIPSET}.builds").lower()
   # builds file uses uppercase X in name
   buildsfile_name = buildsfile_name.replace("x", "X")
   buildsfile_path = env.subst("${BUILDAMSS_ROOT}/"+ buildsfile_name)
   
   buildsfile_data_name = env.subst("${BUILD_ID}").lower()
   buildsfile_data = env.subst("${BUILDDATA_ROOT}/" + buildsfile_data_name + ".dat")


   cmdline_targets = map(str, COMMAND_LINE_TARGETS)

   generate_uses = False

   # check if builds file exists other wise do nothing; most possibly
   # stip/pack build expect everything to be done.
   if env.PathExists(buildsfile_data):
      if not env.CompareAndUpdateMD5File(buildsfile_data, output_filename_md5) or \
         not env.PathExists(output_filename):
         generate_uses = True
   else:
      env.PrintError("Expected build data file not found!! [%s]" % buildsfile_data)
      raise RuntimeError, 'missing build data file.'

   if generate_uses:
      env.PrintInfo("creating AMSS uses flags for " + os.path.basename(filename))   
      
      genusesflags_script = env.NormPath('${BUILD_SCRIPTS_ROOT}/genuses.py')
      
      if not env.PathExists(buildsfile_data):
         env.PrintError("Expected data file not found!! [%s]" % buildsfile_data)
         raise RuntimeError, 'missing data file.'
      
      cmds = "${PYTHONCMD} %s -f %s -o %s" % (genusesflags_script, buildsfile_data, output_filename)
         
      env.ExecCmds(cmds, "${BUILDAMSS_ROOT}", target=os.path.basename(output_filename))
   return None

#===============================================================================
# publish_api
#
#===============================================================================
def print_api(env, api_name, paths):
   if type(paths) is not list:
      paths = [paths]
      
   def process_api(env, path):
      path = env.RealPath(path)
                  
      if os.path.exists(path):
         env.PrintDebugInfo('any', " i = " + path)
      else:
         env.PrintDebugInfo('any', " x = " + path)

   api_name = "".join([string.upper(env.get('AU_NAME', 'UNKNOWN')), "_", api_name])
   env.PrintDebugInfo('any', "----------------------------------------------------------------")
   env.PrintDebugInfo('any', api_name)

   #import pdb; pdb.set_trace()      
   for path in paths:
      path_list = env.subst(path).split()
      [process_api(env, i) for i in path_list]

def publish_api(env, api_env, api_name, paths):
   debuginfo_opt = env.get('DEBUGINFO_OPT')
   if 'incpaths' in debuginfo_opt or 'incpaths-int' in debuginfo_opt or 'incpaths-ext' in debuginfo_opt: 
      debugincpaths = True
   else:
      debugincpaths = False
   
   # make sure paths is a list
   if type(paths) is not list:
      paths = [paths]
   paths = [env.RealPath(s) for s in paths]
   non_existant_paths = list(filter(lambda x: not os.path.exists(x),paths))
   if debugincpaths:
     env.PrintDebugInfo('any', "Trying to publish non existant paths to api name {}".format(api_name))
     for p in non_existant_paths:
       env.PrintDebugInfo('any','    {}'.format(p))
   
   paths = list(filter(os.path.exists,paths))
   # check if environment already have that api_name; if this is the case
   # append don't overwrite
   api_list = api_env.get(api_name)
   if api_list is not None:
      if type(api_list) is not list:
         api_list = [api_list]
      api_list.extend(paths)
   else:
      api_env[api_name] = paths   
      
   api_name = "".join([string.upper(env.get('AU_NAME', 'UNKNOWN')), "_", api_name])
   api_list = api_env.get(api_name)
   if api_list is not None:
      api_list.extend(paths)
   else:
      api_env[api_name] = paths

   return None

#===============================================================================
# publish_public_api
#
#===============================================================================
def publish_public_api(env, api_name, paths):
   api_env = env['SCONS_INCPATHS_ENV']   
   publish_api(env, api_env, "".join([api_name, "_PUBLIC"]), paths)
   return None

def publish_invalid_public_api(env, api_name, paths):
   env.PrintWarning("Invalid environment context for PublishPublicApi \n" \
                    "   called from [" + inspect.stack()[2][1] + "]")
   return None
   
def publish_print_public_api(env, api_name, paths):
   api_name = "".join([api_name, "_PUBLIC"])
   print_api(env, api_name, paths)
   return None

def publish_dummy_public_api(env, api_name, paths):
   return None

#===============================================================================
# publish_restricted_api
#
#===============================================================================
def publish_restricted_api(env, api_name, paths):
   api_env = env['SCONS_INCPATHS_ENV']   
   publish_api(env, api_env, "".join([api_name, "_RESTRICTED"]), paths)
   return None

def publish_au_restricted_api(env, api_name, paths):
   api_env = env['AU_INCPATHS_ENV']   
   publish_api(env, api_env, "".join([api_name, "_RESTRICTED"]), paths)
   return None
   
def publish_invalid_restricted_api(env, api_name, paths):
   env.PrintWarning("Invalid environment context for PublishRestrictedApi \n" \
                    "   called from [" + inspect.stack()[2][1] + "]")
   return None

def publish_print_restricted_api(env, api_name, paths):
   api_name = "".join([api_name, "_RESTRICTED"])
   print_api(env, api_name, paths)
   return None
   
def publish_dummy_restricted_api(env, api_name, paths):
   return None

#===============================================================================
# publish_protected_api
#
#===============================================================================
def publish_protected_api(env, api_name, paths):
   publish_api(env, env, "".join([api_name, "_PROTECTED"]), paths)   
   return None

def publish_invalid_protected_api(env, api_name, paths):
   env.PrintWarning("Invalid environment context for PublishProtectedApi \n" \
                    "   called from [" + inspect.stack()[2][1] + "]")
   return None

def publish_dummy_protected_api(env, api_name, paths):
   return None
   
#===============================================================================
# publish_private_api
#
#===============================================================================
def publish_private_api(env, api_name, paths, auto_require=True):
   publish_api(env, env, "".join([api_name, "_PRIVATE"]), paths)   
   
   if auto_require:
      require_private_api(env, api_name)

   return None

def publish_invalid_private_api(env, api_name, paths, auto_require=True):
   env.PrintWarning("Invalid environment context for PublishPrivateApi \n" \
                    "   called from [" + inspect.stack()[2][1] + "]")
   return None

def publish_dummy_private_api(env, api_name, paths, auto_require=True):
   return None
   
#===============================================================================
# require_api
#
#===============================================================================
def require_api(env, api_env, api_prefix, api_names, api_suffix, dbg_str, fname):
   
   # make sure api_names is a list
   if type(api_names) != list:
      api_names = [api_names]
   
   if len(api_names) > 0:      
      #import pdb; pdb.set_trace()
      
      api_list = ["".join([api_prefix, api_name, api_suffix]) for api_name in api_names]
      inc_paths_builder(env, api_env, api_list, dbg_str, fname)
   return None

#===============================================================================
# require_public_api
#
#===============================================================================
def require_public_api(env, api_names, area=None):

   if type(api_names) != list:
      api_names = [api_names]
      
   if len(api_names) > 0:
      if area is None:
         api_prefix = string.upper(env.get('AU_NAME', 'CORE'))
      else:
         api_prefix = string.upper(area)
         
      api_prefix = "".join([api_prefix, "_"])
      api_suffix="_PUBLIC"

      api_env = env['SCONS_INCPATHS_ENV']
      
      # temporary start - check and warning  ===================
      
      api_list = ["".join([api_prefix, api_name, api_suffix]) for api_name in api_names]
      intersection = set(api_list) & set(api_env.Dictionary())
      if len(intersection) == 0:
         env.PrintWarning("API name [%s] not found for API prefix [%s], called from [%s]" % (api_name, api_prefix, inspect.stack()[2][1]))

         #api_prefix = "CORE_"
      # temporary end - check and warning  ===================      
      
      require_api(env, api_env, api_prefix, api_names, api_suffix, 
         dbg_str="Required Public APIs", fname="internal_public")
   return None

#===============================================================================
# require_restricted_api
#
#===============================================================================
def require_restricted_api(env, api_names):
   api_prefix = "CORE_"
   api_env = env['SCONS_INCPATHS_ENV']
   require_api(env, api_env, api_prefix, api_names, api_suffix="_RESTRICTED", 
      dbg_str="Required Restricted APIs", fname="internal_restricted")
   return None

def require_au_restricted_api(env, api_names):
   api_prefix = "".join([string.upper(env.get('AU_NAME', 'UNKNOWN')), "_"])
   api_env = env['AU_INCPATHS_ENV']
   require_api(env, api_env, api_prefix, api_names, api_suffix="_RESTRICTED", 
      dbg_str="Required Restricted APIs", fname="internal_restricted")
   return None

#===============================================================================
# require_protected_api
#
#===============================================================================
def require_protected_api(env, api_names):
   api_prefix = "".join([string.upper(env.get('AU_NAME', 'UNKNOWN')), "_"])
   require_api(env, env, api_prefix, api_names, api_suffix="_PROTECTED", 
      dbg_str="Required Protected APIs", fname="internal_protected")
   return None

#===============================================================================
# require_private_api
#
#===============================================================================
def require_private_api(env, api_names):
   api_prefix = "".join([string.upper(env.get('AU_NAME', 'UNKNOWN')), "_"])
   require_api(env, env, api_prefix, api_names, api_suffix="_PRIVATE",
      dbg_str="Required Private APIs", fname="internal_private")
   return None

#===============================================================================
# require_external_api
#
#===============================================================================
def require_external_api(env, api_names):
   api_env = env['INCPATHS_EXT_ENV']
   api_prefix = ""
   require_api(env, api_env, api_prefix, api_names, api_suffix="_EXTERNAL", 
      dbg_str="Required External APIs", fname="external")
   return None

#===============================================================================
# get_corebsp_path
#
#===============================================================================
def get_corebsp_path (env, path):
   api_env = env['SCONS_INCPATHS_ENV']
   path = api_env.subst(path)
   return path
   
#===============================================================================
# get_external_path
#
#===============================================================================
def get_external_path (env, path):
   api_env = env['INCPATHS_EXT_ENV']
   path = api_env.subst(path)
   return path
   
#===============================================================================
# init_filter_options
# init the filter options for the image to decide what to build or not.
#===============================================================================
def init_filter_options(env):
   #import pdb; pdb.set_trace()
   #------------------------------------------------------------------------------
   # Decide build steps
   #------------------------------------------------------------------------------
   # regular build steps (no filter) is build everything, once a user start 
   # using filters we have to make decisions depending on user input.
   # The LoadAUSoftwareUnits function will take care of filtering subsystem, units, 
   # etc. This is to take care of what steps to enable disable from the top level
   # script, such as building files specify in this script i.e. quatz, stubs, etc.
   do_local_files = True
   do_link = True
   do_post_link = True

   # get user input from command line
   filter_opt = env.get('FILTER_OPT')
   elf_file = ARGUMENTS.get('process_elf', None)
   
   # make decisions
   if filter_opt is not None:
      do_link = False
      
      if env.FilterMatch(os.getcwd()):
         if elf_file is None:
            do_post_link = False
         else:
            image_elf = env.File(elf_file)
      else:
         do_local_files = False
         do_post_link = False
         
   if do_local_files:
      env.Replace(IMAGE_BUILD_LOCAL_FILES = True)

   if do_link:
      env.Replace(IMAGE_BUILD_LINK = True)

   if do_post_link:
      env.Replace(IMAGE_BUILD_POST_LINK = True)
      

#===============================================================================
# init_image_vars
# init image enviroment variables
#===============================================================================
def init_image_vars (env, alias_list, proc, config='none', plat='l4', 
   buildpath=None, flashtype='nand', target='${IMAGE_NAME}_${BUILD_ID}', 
   build_kcfg=None, build_tags=[], deprecated_build_tags=[], tools=[]):

   env.Replace(ROOT_ENV = env)

   image_name = str(alias_list[0]).upper()
   
   #----------------------------------------------------------------------------   
   # init image specific variables
   env.Replace(IMAGE_ENV = env)
   env.Replace(IMAGE_ALIASES = alias_list)
   env.Replace(IMAGE_NAME = image_name)
   env.Replace(IMAGE_BUILD_TAGS = build_tags)
   env.Replace(IMAGE_DEPRECATED_BUILD_TAGS = deprecated_build_tags)
   env.Replace(IMAGE_LIBS = [])
   env.Replace(IMAGE_OBJS = [])   
   env.Replace(IMAGE_DOCS = [])
   env.Replace(IMAGE_LISTFILES = [])
   env.Replace(IMAGE_ARTIFACTS = [])
   env.Replace(IMAGE_ROOT = env.Dir('.').path)   
   
   env.Replace(PROC = proc)
   env.Replace(PROC_CONFIG = config)
   
   env.Replace(PLAT = plat)
   env.Replace(INSTALL_LIBPATH = '.')
   #env.Replace(INSTALL_LIBPATH = '${BUILD_ID}')
   env.Replace(TARGET_NAME = target)
   env.Replace(TARGET_BLD_KCFG = build_kcfg)   
   
   
   #----------------------------------------------------------------------------   
   # init build tags
   env.Replace(BUILD_TAGS = build_tags)
   for build_tag in build_tags:
      env[build_tag] = proc
      
   env.Replace(DEPRECATED_BUILD_TAGS = deprecated_build_tags)
   for build_tag in deprecated_build_tags:
      env[build_tag] = proc
      
   #----------------------------------------------------------------------------   
   # init tools var
   if type(tools) is not list:
      tools = [tools]
   
   env.Replace(IMAGE_TOOLS = tools)   
   
   #----------------------------------------------------------------------------
   # init build path
   if buildpath is None:
      #qc_buildpath =  env.subst(image_name.lower() + '/${PROC}')
      qc_buildpath = env.subst("%s/${PROC}/${BUILD_ID}" % image_name.lower() )
   else:
      qc_buildpath = env.subst(buildpath)
      
   qc_short_buildpath = env.subst("${BUILD_ID}")
      
   env.Replace(QC_BUILDPATH = qc_buildpath)
   env.Replace(QC_SHORT_BUILDPATH = qc_short_buildpath)
   
   # needed for use on binary deliverables modules that create
   # their own BUILDPATH substitutes.
   env.Replace(BINARY_BUILDPATH = qc_buildpath)
      
   # take into account possible additional build path from OEMs
   oem_buildpath = env.get('OEM_BUILDPATH')

   # OS environment overrides any other settings.
   oem_buildpath = os.environ.get('SCONS_OEM_BUILDPATH', oem_buildpath)
   
   oem_bid = env.get('OEM_BUILD_ID')
   
   if oem_buildpath is None:
      if oem_bid is not None:
         env.Replace(OEM_BUILDPATH = oem_bid)
   else:
      oem_buildpath = str(oem_buildpath)
      if oem_bid is not None:   
         oem_buildpath = "".join([oem_buildpath, "/", oem_bid])
         
      env.Replace(OEM_BUILDPATH = oem_buildpath)

   if 'OEM_BUILDPATH' in env:
      env.Replace(BUILDPATH = env.subst("${OEM_BUILDPATH}/${QC_BUILDPATH}"))
      env.Replace(SHORT_BUILDPATH = env.subst("${OEM_BUILDPATH}/${QC_SHORT_BUILDPATH}"))
      env.Replace(MBN_ROOT = env.subst("${BUILD_ROOT}/build/ms/bin/${OEM_BUILDPATH}/${QC_SHORT_BUILDPATH}"))
      env.Replace(PIL_BUILDPATH = env.subst("${OEM_BUILDPATH}/PIL_IMAGES/SPLITBINS_${QC_SHORT_BUILDPATH}"))
   else:
      env.Replace(BUILDPATH = qc_buildpath)
      env.Replace(SHORT_BUILDPATH = qc_short_buildpath)
      env.Replace(PIL_BUILDPATH = env.subst("PIL_IMAGES/SPLITBINS_${QC_SHORT_BUILDPATH}"))
      
   #----------------------------------------------------------------------------      
   # Init command line filter options
   init_filter_options(env)
      
   #----------------------------------------------------------------------------   
   # Init variables based on configuration

   if config == 'modem': 
      # Modem PROC configuration
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}M'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
      
      env.Replace(TARGET_IMAGE = 'MODEM_PROC')
      env.Replace(TARGET_FLASH_TYPE = flashtype)
      env.Replace(MODEM_PROC = env.subst('${PROC}'))

      env.Replace(PROC_NAME = 'MODEM')
      
      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = 'M')
         
   elif config == 'apps':
      # Apps PROC configuration (QNP, BMP, QNK, etc)
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}A'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
   
      env.Replace(TARGET_IMAGE = 'APPS_PROC')
      env.Replace(TARGET_FLASH_TYPE = flashtype)
      env.Replace(APPS_PROC = env.subst('${PROC}'))      
      
      env.Replace(PROC_NAME = 'APPS')

      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = 'A')

   elif config == 'single':
      # Single PROC configuration i.e. SCMM
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
   
      env.Replace(TARGET_IMAGE = 'MODEM_PROC')
      env.Replace(TARGET_FLASH_TYPE = flashtype)
      env.Replace(SINGLE_PROC = env.subst('${PROC}'))      
      
      env.Replace(PROC_NAME = 'UNIPROC')

      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = 'U')
   
   elif config == 'qdsp6_sw' or config == 'qdsp6_fw':
      # QDSP6 PROC Configuration
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}Q'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
   
      env.Replace(TARGET_IMAGE = 'MODEM_PROC')
      env.Replace(TARGET_FLASH_TYPE = flashtype)

      env.Replace(MODEM_PROC = env.subst('${PROC}'))      
      env.Replace(QDSP6_PROC = env.subst('${PROC}'))
      
      if config == 'qdsp6_sw':
         env.Replace(PROC_NAME = 'QDSP_SW')
         env.Replace(PIL_SPLITTER_PREFIX = 'modem')
      elif config == 'qdsp6_fw':
         env.Replace(PROC_NAME = 'QDSP_FW')
         env.Replace(PIL_SPLITTER_PREFIX = 'modem_fw')

      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = 'Q')

      RTOS_RELEASE = os.environ.get('HEXAGON_RTOS_RELEASE', '1.1')
      if env['PLATFORM'] in ['windows', 'win32', 'cygwin'] and RTOS_RELEASE =='1.1':
         env.Replace(PLATFORM = 'cygwin')
         
   elif config == 'sps':
      # Smart peripheral system PROC configuration
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
      
      env.Replace(PROC_NAME = 'SPS')
   
      env.Replace(TARGET_IMAGE = 'SPS_PROC')
      env.Replace(TARGET_FLASH_TYPE = flashtype)
      env.Replace(SPS_PROC = env.subst('${PROC}'))      
      
   elif config == 'wcn':
      # WCONNECT PROC aka RIVA PROC configuration
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}M'))
      env.Replace(CUST_H = env.subst('CUST${BUILD_ID}.H').lower())
      
      # TARGET_IMAGE variable is used in SCL and xml names. 
      # TODO: FIX ME
      #env.Replace(TARGET_IMAGE = 'WCN_PROC')
      env.Replace(TARGET_IMAGE = 'MODEM_PROC') 
      env.Replace(TARGET_FLASH_TYPE = flashtype)
      env.Replace(WCN_PROC = env.subst('${PROC}'))      
      
      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = 'M')
         
   else:
      # Here because 'config' is 'none' or 'config' is unknown
      if config != 'none':
         env.PrintWarning("InitImageVars - config parameter not recognized")
         env.PrintWarning("   config = %s; image_name = %s" % (config, image_name))
         env.PrintWarning("   Defaulting to config = none")
      
      # No processor defined configuration.  Setup only the required
      # flags and variables to satisfy scripts.  This config ('none') 
      # is used to build things like documents where modem/apps, etc., 
      # don't make any sense, but some scripts still look for the 
      # definitions.
      
      env.Replace(BUILD_ID = env.subst('${BUILD_ID}'))
      env.Replace(CUST_H = string.lower(env.subst('CUST${BUILD_ID}.H')))
      
      env.Replace(TARGET_IMAGE = image_name)
      env.Replace(TARGET_FLASH_TYPE = flashtype)

      env.Replace(PROC_NAME = image_name)
      
      if build_kcfg is None:
         env.Replace(TARGET_BLD_KCFG = "")
         
   #----------------------------------------------------------------------------
   # deprecated to be removed soon      
   env.Replace(LIBS = [])
         
   return None
