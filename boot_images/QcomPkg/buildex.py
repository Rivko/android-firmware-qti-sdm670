#!/usr/bin/env python

#============================================================================
#
# @file buildex.py
#
# GENERAL DESCRIPTION
#   Unified script for building any/all targets available
#
# Copyright 2017,2018 by Qualcomm Technologies, Incorporated.
#                 All Rights Reserved.
#               QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 19/09/17	 mt		  Added support for -V and -b flags with alias flag
# 10/20/17	 c_vkuppa Generating XBLLoader Memory Usage as XBLL_Memoy_Usage.txt
# 07/07/17   c_vkuppa Build time statistic for the whole process
# 05/16/17   vk       Fix typo in filename
# 03/17/17   yps      Fixed buildex deploy issues
# 03/13/17   yps      remove case sensitive for input arguments,added -C components compile
# 02/21/17   yps      Optimized process flow
# 01/26/17   yps      Initial release

import shutil
import stat
import subprocess
import glob
import re
from optparse import OptionParser
from collections import defaultdict
import platform
import os
import sys
import fnmatch
import json
import delcommonts
import time
DEVDEBUG = False
MINIMUM_PYTHON_VERSION = 34014192
DEFAULT_RELEASE_MODES = "DEBUG,RELEASE"
GENERAL_ERROR_CODE = 1
ENVIRONMENT_SETUP_ERROR_CODE = 2
EDK_TOOL_ERROR_CODE = 3
SEARCH_ALL_TARGET_PATH_ERROR_CODE = 4
SEARCH_BUILD_JSONS_PATH_ERROR_CODE = 5
MATCH_IMAGES_ERROR_CODE = 6
FIND_COMPONENTS_ERROR_CODE = 7
Tool_chain_json_config = ""
DEFAULT_ARM_LICENSE_FILE = "7117@license-wan-arm1"
DEFAULT_TARGET_REGEX = "^([a-zA-Z]{3,4})(\d([\d|x|X]{2,3}))Pkg$"
DEFAULT_VARIANT_REGEX = '^[A-Z][A-Z]$'
BUILDIT_BUILDINFO_TAG = "UserExtensions.Buildit.BuildInfo"
BOOT_IMAGES_DIR = "boot_images"
QCOM_DIR = "QcomPkg"
DEFAULT_COMPILER_ARCH = "AARCH64"
COPY_IMAGES_AS_IS = ["jtagprogrammer"]

ARGUMENTS = {"$BUILDROOT":"" , 
             "$REL":"" , 
             "$PRODMODE":"" , 
             "$VAR":"",
             "$COMPLIER":"",
             "$FIRSTDLL":"",
             "$SECONDDLL":"",
             "$XBLSEC":"",
             "$TARGETROOT":"",
             "$TARGETID":""}
Componets_status = {}

conf_files = ["build_rule", "target", "tools_def"]
# merge_book - Acts as a wrapper around a dictionary of merge_entry's
class merge_book:
  def __init__(self):
    self.merges = dict()
  def add_merge_item(self, dll_path):
    # create merge_entry if it doesn't exist
    if merge_name not in self.merges:
      new_merge = merge_entry(merge_name, sign_type, xbl_sec)
      self.merges[merge_name] = new_merge
    # add the dll to merge_entry
    self.merges[merge_name].add_merge_item(dll_path, xbl_sec)
  
  # Same as above, but with a base to merge
  def add_merge_base(self, merge_name, base_dll_path, sign_type, xbl_sec=""):
    if merge_name not in self.merges:
      new_merge = merge_entry(merge_name, sign_type, xbl_sec)
      self.merges[merge_name] = new_merge
    self.merges[merge_name].set_merge_base_dll(base_dll_path, xbl_sec)

class merge_entry:
  def __init__(self, output_name, sign_type, xbl_sec=""):
    self.output_name = output_name
    self.sign_type = sign_type
    self.xbl_sec = xbl_sec
    self.dll_list = []
    self.merge_base_dll = ""
    
    if output_name == "":
      print "Error: # MERGE value is invalid"
      sys.exit(GENERAL_ERROR_CODE)
    
  def add_merge_item(self, dll_path):
    self.dll_list.append(dll_path)

  
  def set_merge_base_dll(self, base_dll_path, xbl_sec=""):
    if self.merge_base_dll:
      print "Error: Multiple MERGE_BASE tags found for " + self.output_name
      sys.exit(GENERAL_ERROR_CODE)
    self.merge_base_dll = base_dll_path
    if xbl_sec:
      self.xbl_sec = xbl_sec

  def modify_merge_entry_xbl_sec(self, xbl_sec):
    self.xbl_sec = xbl_sec

  def modify_merge_entry_dll_list(self, dll_list):
    self.dll_list = dll_list

  def print_merge_entry(self):
    print "[buildex.py] [MERGE DETAILS] output name: {0}".format(self.output_name)
    print "[buildex.py] [MERGE DETAILS] sign_type: {0}".format(self.sign_type)
    print "[buildex.py] [MERGE DETAILS] xbl sec: {0}".format(self.xbl_sec)
    print "[buildex.py] [MERGE DETAILS] dll list: {0}".format(self.dll_list)
    print "[buildex.py] [MERGE DETAILS] merge base dll: {0}".format(self.merge_base_dll)


class build_flavor:
  def __init__(self, target,image = "",variant="", release_mode="",aarch="",compiler="",merged_output_elf="",output_name=""):
    self.target = target
    self.image = image
    self.variant = variant
    self.release_mode = release_mode
    self.aarch = aarch
    self.compiler = compiler
    self.merged_output_elf = merged_output_elf
    self.output_name = output_name

  def to_string(self):
    desc = "Target: " + self.target
    desc += " image: " + self.image
    desc += " Varaint: " + self.variant if self.variant else " Variant: --"
    desc += " release: " + self.release_mode
    return desc

##############################################################################
# display_build_summary
##############################################################################
def display_build_summary(all_flavors):
  if all_flavors:
    print "\n[buildex.py] Successfully built following flavors:\n" \
         "###################################################"
    for flavor in all_flavors:
      print flavor.to_string()
  else:
    print "\n[buildex.py] Nothing built\n"

def main():
  target_paths= []
  json_paths = []
  componets = []
  variants = []
  all_flavors = []
  
  build_start_time = time.time()
  
  if sys.hexversion < MINIMUM_PYTHON_VERSION:
      print "\n[buildex.py] Error: Incorrect version of Python detected: " + sys.version
      print "[buildex.py]        Requires use of Python 2.7.3 or later."
      sys.exit(GENERAL_ERROR_CODE)
  if re.search("\.[sS]", os.getcwd()): 
      print "\n[buildit.py] Error: Directory name containing \".[sS]\" detected: " + os.getcwd() 
      print "[buildit.py]        Please rename directory or use a different vce workspace name" 
      print "[buildit.py]        that does not start with an 's' or 'S'." 
      sys.exit(GENERAL_ERROR_CODE) 

  desc = "python buildex.py [options]\n" \
         "\nExample invocations:\n" \
         "  python buildex.py -t Msm8998Pkg\n" \
         "  python buildex.py -t Msm8998Pkg --variant LA\n" \
         "  python buildex.py -t Msm8998Pkg --variant LA,WP -r DEBUG\n"

  parser = OptionParser(usage=desc)

  parser.add_option('-i', '--image', default="",
                    help="image(s) that will be built, separated by comma. " + \
                         "Example: \n" + \
                         "python buildex.py -i XBL  # that will build all components like XBL, XBL_loader, DDI ...\n" + \
                         "python buildex.py -i DEV_PROG\n" + \
                         "python buildex.py -i XBL_Loader\n" + \
                         "If not specified, all targets will be built", )
                         
  parser.add_option('-t', '--target', default="",
                    help="Target(s) that will be built, separated by comma. " + \
                         "Example: -t Msm8998Pkg. " + \
                         "If not specified, all targets will be built", )
                         
  parser.add_option('-a', "--alias",
                    action="store", default="",
                    help="Specify argument alias from config file QcomPkg/aliasconfig.json " + \
                         "Example: -a 98ladbg",)
                         
  parser.add_option('-V', "--version",
                    action="store", default="0",
                    help="Specify build version. Default is 0. Example: -v 123.")

  parser.add_option('-v', '--variant', default="",
                    help="Variant(s) that will be built, separated by comma. " + \
                         "Example: -v la,wp " + \
                         "If not specified, all variants will be built per target")
                         
  parser.add_option('-b', '--build_flags',
                      action="store", default="",
                      help="Flags to be passed to the build invocation. " + \
                            "Specify multiple arguments separated by a comma or enclosed in double quotes. " + \
                            'Example: -b -v,-n,1')
  
  parser.add_option('-c', '--compiler_flags', default="",
                      help="Extra flags to be passed to the build compiler directly. " + \
                            "Specify multiple arguments enclosed in double quotes. " + \
                            'Example: "-c -DPRE_SIL -DANOTHER_FLAG",')

  parser.add_option('-C', '--components', default="",
                      help="Will only compile the components input from this option " + \
                            "Example: -C XBL_Loader,XBL_Ramdump",)  
                            
  parser.add_option('-r', "--release",
                    action="store", default=DEFAULT_RELEASE_MODES,
                    help="Release mode for building, default is 'DEBUG,RELEASE'." \
                         " Both modes will be built.")

  (options, args) = parser.parse_args()

  init_conf_folder()
  try:
      setup_environment()
  except Exception as error:
      print error
      sys.exit(ENVIRONMENT_SETUP_ERROR_CODE)
  
  if options.alias :
    Alias_config =  delcommonts.remove_json_comments(os.path.join(os.environ["WORKSPACE"],
                                               QCOM_DIR,"aliasconfig.json"),None)
    Alias_dirt =  json.loads(Alias_config)       
    alias_args = options.alias.lower()
    if alias_args in Alias_dirt:
      version = options.version
      build_flags = options.build_flags
      (options, args) = parser.parse_args(Alias_dirt[alias_args])
      options.version = version
      options.build_flags = build_flags
    else:
       raise NameError("Error: Alias arguments %s is not correct.",options.alias)
    
  #delete comments from toolchainconfig.json file
  Tool_chain_json_config = delcommonts.remove_json_comments(os.path.join(os.environ["WORKSPACE"],
                                               QCOM_DIR,"toolchainconfig.json"),None)

  tools_dirt = json.loads(Tool_chain_json_config)
  
  # make sure release_mode is valid
  options.release  = options.release.upper()
  for mode in options.release.split(','):
      if mode not in DEFAULT_RELEASE_MODES:
          print 'Error: Release mode "' + mode + '" is invalid. Please see --help for more information'
          sys.exit(BAD_RELEASE_TYPE_ERROR_CODE)

  create_build_version(options.version)
  # Check that tools exist, otherwise make them
  try:
    make_edk_tools()
  except Exception as error:
    print error
    sys.exit(EDK_TOOL_ERROR_CODE)
  # Delete old build products file
  build_products_file = os.path.join(os.environ["WORKSPACE"],
                        "..", "BuildProducts.txt")
  if os.path.isfile(build_products_file):
    os.remove(build_products_file)

  # find all the targets
  try:
    target_paths=search_all_target_path(options.target)
  except Exception as error:
    print error
    sys.exit(SEARCH_ALL_TARGET_PATH_ERROR_CODE)

  for target_path in target_paths:
    target = os.path.basename(target_path)
    os.environ["TARGETPKG"]  = target
    os.environ["TARGETMSM"]  = target.split("Pkg")[0]
    variants = []
    variants = match_variants(target_path,options.variant)
    matches = re.search(DEFAULT_TARGET_REGEX, target)
    if matches:
      os.environ["TARGETID"] = matches.group(2)
    else:
      # Handles app packages (e.g. QcomToolsPkg)
      os.environ["TARGETID"] = os.environ["TARGETMSM"]
    os.environ['TARGETROOT'] = os.path.join(QCOM_DIR, os.environ["TARGETPKG"])

    ARGUMENTS["$TARGETROOT"] = os.environ["TARGETROOT"]
    ARGUMENTS["$TARGETID"] = os.environ["TARGETID"]
    
    # find buildconfig.json in target folder
    try:
      json_path = search_build_json_path(target_path)
    except Exception as error:
      print error
      sys.exit(SEARCH_BUILD_JSONS_PATH_ERROR_CODE)
      
    build_dirt = pre_parser_json_file(json_path)
    
    # find the image which we should process in the targets
    try:
      images = match_images(options.image,build_dirt)

    except Exception as error:
      print error
      sys.exit(MATCH_IMAGES_ERROR_CODE)
      
    ARGUMENTS['$BUILDROOT'] = os.environ["WORKSPACE"]

    index = 0
    while (index < len(variants) ) or (index == 0):
      if variants:
        variant = variants[index].upper()
      else:
        variant = ""
      image_version_setup(os.environ["TARGETMSM"],variant)
      ARGUMENTS['$VAR'] = variant
      for release_mode in options.release.split(','):
        if release_mode.upper() == "RELEASE":
            ARGUMENTS['$REL'] = 'RELEASE'
            ARGUMENTS['$PRODMODE'] = 'PRODMODE'
        elif release_mode.upper() == "DEBUG":
            ARGUMENTS['$REL'] = 'DEBUG'
            ARGUMENTS['$PRODMODE'] = 'DEBUGMODE'
        else:
            raise NameError("Error:: Opiton -r \""+ \
                                     release_mode + \
                                     "\" does not support !!!")
        
        # loop to process images
        for image in images:
          variant_flavor = build_flavor(target,
                                        image,
                                        variant.upper(),
                                        release_mode.upper(),
                                        "",
                                        "",
                                        "",
                                        "")
          all_flavors.append(variant_flavor)
          create_image(options,variant_flavor,build_dirt,tools_dirt)
          if("SDM670Pkg" == variant_flavor.target):
	      print "[buildit.py] Generating XBLLoader Memory Usage as XBLL_Memoy_Usage.txt... ",
	      Mapit_script_root = os.path.join(os.environ["WORKSPACE"], "QcomPkg", "Tools")
	      Mapit_script = os.path.join(Mapit_script_root, 'Mapit.py')	      
	      XBL_loader_map_path = os.path.join(os.environ["WORKSPACE"],
									"Build",
									os.environ["TARGETMSM"] + str(variant) + "_Loader",
									release_mode + "_" + arch_to_compiler("AARCH64"),
									"AARCH64","QcomPkg","XBLLoader","XBLLoader","DEBUG")
	      map_file = os.path.join(XBL_loader_map_path ,"XBLLoader.map")
	      op_file = os.path.join(XBL_loader_map_path ,"XBLL_Memoy_Usage.txt")
	      
	      if os.path.exists(Mapit_script) is True:
		      print "Done [Memory Usage - Section wise Summary]"              
		      cmds = "python" + ' ' + Mapit_script + " -c CLANG" + " -t " + os.path.join(Mapit_script_root,"SDM670 ") + map_file + ' > ' + op_file
		      subprocess.call(cmds,shell=True)

      index+=1 
  if "clean" in options.build_flags:
     print "[buildex.py] Clean successfully."
  else:
     build_end_time = time.time()
     delta = build_end_time - build_start_time
     minutes = delta//60
     seconds = delta - 60*minutes
     print "\n[buildex.py] Successfully built following flavors in ", int(minutes), ":", int(seconds), "min\n"
     display_build_summary(all_flavors)
  print "\n[buildex.py] Now exiting...\n"
  return

##############################################################################
# arch_to_compiler
##############################################################################
def arch_to_compiler(arch, dsc=""):
  
  # Look for COMPILER_<platform> tag if dsc was provided
  if dsc:
	if platform.system().lower() == "windows":
	  compiler_flag = "COMPILER_WIN"
	else:
	  compiler_flag = "COMPILER_LINUX"
	compiler = find_tag_in_descriptor_file(dsc, BUILDIT_BUILDINFO_TAG,compiler_flag)
	if compiler: return compiler
  
  # Otherwise, determine 
  if arch == "AARCH64":
	if platform.system() == "Windows":
	  return "CLANG39WIN"
	else:
	  return "CLANG39LINUX"
  elif arch == "ARM":
	if platform.system() == "Windows":
	  return "RVCT501"
	else:
	  return "RVCT501LINUX"

def match_variants(target_path,option_variants):
  variant_dirs = []
  available_variants = []
  if option_variants:
    for variant in option_variants.split(","):
      regex = variant.upper()
      if re.search(DEFAULT_VARIANT_REGEX,regex):
         variant_dirs += search_dir(target_path,regex)
      else:
         raise NameError("Error:: Variant: \""+ \
                               variant + \
                               "\" does not support!!!!\n"  + \
                               "Please refer -h to get help.")
  else:
    regex = DEFAULT_VARIANT_REGEX
    variant_dirs =(search_valid_dirs(target_path,regex))
  #print  "variant_dirs is ",variant_dirs
  if variant_dirs:
    for variant_dir in variant_dirs:
      #print "current dir is ", variant_dir
      if os.path.isdir(variant_dir):
        available_variants.append(os.path.basename(variant_dir))
  return available_variants

def find_tool_dirt(tools_dirt,tool_name):
  if tool_name in tools_dirt['ToolChain']:
    if platform.system() is "Windows":
      if 'Windows' in tools_dirt['ToolChain'][tool_name]:
        return tools_dirt['ToolChain'][tool_name]['Windows']
      else:
        return tools_dirt['ToolChain'][tool_name]
    else:
      if 'Linux' in tools_dirt['ToolChain'][tool_name]:
        return tools_dirt['ToolChain'][tool_name]['Linux']
      else:
        return tools_dirt['ToolChain'][tool_name]
     
  else:
    return ""
      
def create_dirs(dir_lists):
 for dir_path in dir_lists:
   dir_path = replace_arguments (dir_path)
   if not os.path.isdir(dir_path):
     os.makedirs(dir_path)
     
#####################################################################
# perform create image
#####################################################################
def create_image(options,variant_flavor,build_dirt,tools_dirt):
  tool_cmds = []
  cmd_params = []
  for i in range(len(build_dirt['CreateImages'])):
    if build_dirt['CreateImages'][i]['Name'] == unicode(variant_flavor.image):
      merge_dict = merge_book()
      for j in range(len(build_dirt['CreateImages'][i]['Process'])):
        process_action = build_dirt['CreateImages'][i]['Process'][j]
        if "Createfolders" in process_action:
          create_dirs(process_action["Createfolders"])
        if "Components" in process_action:
          dll_paths=[]
          dll_paths=process_componets(options,variant_flavor,process_action,build_dirt,tools_dirt,merge_dict)
        if "clean" in options.build_flags:
          continue
        if process_action['ToolChain'] == "Merge":
          # and collect the items(elfs) that were created
          merge_tool_dirt = find_tool_dirt(tools_dirt,"Merge")
          built_items = []
          build_product = perform_merges(options,
                                         dll_paths,
                                         variant_flavor,
                                         process_action,
                                         merge_tool_dirt)
          if build_product:
             built_items.append(build_product)
  
          # Write out after merging for nicer looking output
          try:
            append_build_products(built_items)
          except Exception as error:
            raise NameError("ERROR: buildex::append_build_products: " + \
                              "Failed to append to build products file.")
          continue
       
        tool_dirt = find_tool_dirt(tools_dirt,process_action['ToolChain'])
        tool_cmds = get_tool_cmd(tool_dirt)
        cmd_params = get_cmd_params(options,variant_flavor,process_action,tool_dirt)
        if process_action['ToolChain'] == "Copy":
           shutil.copy2(cmd_params[0],cmd_params[1])
           continue
              
        try:
          subprocess.check_output(tool_cmds + cmd_params)
        except Exception as error:
         raise NameError("ERROR: buildex::excute command failed with error: \n"  \
                                + str(error))
      #break;
  return
#####################################################################
# Check_componets_status
#####################################################################
def check_componets_status(dsc_path,release_mode):
  if release_mode in Componets_status:
    if dsc_path in Componets_status[release_mode]:
      if  Componets_status[release_mode][dsc_path] == "processed":
        return 1
  else:
      Componets_status.setdefault(release_mode,{})
  Componets_status[release_mode].setdefault(dsc_path,"")
  return 0

#####################################################################
# set_componets_status
#####################################################################
def set_componets_status(dsc_path,release_mode):
  if dsc_path in Componets_status[release_mode]:
     Componets_status[release_mode][dsc_path] = "processed"
  return 1
#####################################################################
# process_componets
#####################################################################
def process_componets(options,variant_flavor,process_dirt,build_dirt,tools_dirt,merge_dict):
  dll_paths=[]
  for componet in process_dirt['Components']:

    dsc_path=process_componet(options,variant_flavor,componet,process_dirt,build_dirt,tools_dirt)
    # Determine what to merge
    dll_paths.append(generate_merge_item(merge_dict,dsc_path,variant_flavor,process_dirt))
  return dll_paths

#####################################################################
# build componet
#####################################################################
def process_componet(options,variant_flavor,component,process_dirt,build_dirt,tools_dirt):
  component_dirt = None
  build_cmd = []
  for i in range(len(build_dirt['Components'])):
    if build_dirt['Components'][i]['Name'] == unicode(component):
      component_dirt = build_dirt['Components'][i]
      break
  if component_dirt is None:
    print "There is no this "+ component 
    raise NameError("ERROR: buildex::build_componet: " + \
                    "can't find "+component+"  Please ensure it exist.")
 
  for i in range(len(component_dirt['Process'])):
    cprocess_dirt = component_dirt['Process'][i]
    tool_dirt = find_tool_dirt(tools_dirt,component_dirt['Process'][i]['ToolChain'])
    build_cmd = get_cmd_params(options,variant_flavor,cprocess_dirt,tool_dirt)
    if build_cmd[0] == "build":
      # Make a full flavor; Requried for finding all merge dependencies
      dsc_path = find_dsc_path_from_configuration(build_cmd)
      variant_flavor.aarch = component_dirt['Process'][i]['AArch'].encode('ascii')
      ARGUMENTS['$AARCH']= variant_flavor.aarch
      if ((options.components and component_dirt['Name'].encode('ascii').upper() in options.components.upper()) or options.components == "") and not check_componets_status(dsc_path, variant_flavor.release_mode):
        set_compiler_path(tool_dirt,component_dirt['Process'][i]['ToolChain'])
        # Add build.exe specific OptionParser
        if options.build_flags:
          build_cmd += options.build_flags.strip('"').split(',')
        print "[buildex.py] Building with arguments...\n", build_cmd, '\n'
        try:
           subprocess.check_call(build_cmd)
        except Exception as error:
           raise NameError("ERROR: buildex::build: " + \
                                  "Building failed with error... " + str(error))
        set_componets_status(dsc_path, variant_flavor.release_mode)
      else:
         return dsc_path
      
      if "clean" in options.build_flags:
        return dsc_path
    elif cprocess_dirt['ToolChain'] == "Copy":
       shutil.copy2(build_cmd[0],build_cmd[1])
       continue
    else:
      if tool_dirt != "":
        tool_cmd = get_tool_cmd(tool_dirt)
      else:
        tool_cmd.append(cprocess_dirt['ToolChain'].encode('ascii'))
    #  print "Run ",tool_cmd,build_cmd
      try:
         subprocess.check_output(tool_cmd + build_cmd)
      except Exception as error:
         raise NameError("ERROR: buildex::build: " + \
                                "Building failed with error... " + str(error))
  
  return dsc_path

def replace_arguments(string):
  arg_regex = "(\$[A-Z]+)+"
  matches = []
  matches = re.findall(arg_regex, string)
  str1 = string.encode('ascii')
  if matches:
#    print "find arguments "
#    print matches
    for match in matches:
      if match in ARGUMENTS:
        str1=str1.replace(match,ARGUMENTS[match])
      else:
        raise NameError("ERROR::Buidex: argument "+match+" doesn't exist!!!" )
  if platform.system() is "Windows":
    str1 = str1.replace("/","\\",100)
  else:
    str1 = str1.replace("\\","/",100)
  return str1
#####################################################################
# get build cmd params
#####################################################################
def get_cmd_params(options,variant_flavor,cprocess_dirt,tool_dirt):
  build_cmd = []
  #find component params
  for i in range(len(cprocess_dirt['Params'])):
    str2 = cprocess_dirt['Params'][i]
    str1 = str2.encode('ascii')
    str1 = replace_arguments(str1)
    build_cmd.append(str1)
    
  if 'Params' in tool_dirt and tool_dirt:
    for i in range(len(tool_dirt['Params'])):
      str1 = replace_arguments(tool_dirt['Params'][i].encode('ascii')) 
      build_cmd.append(str1)
      if tool_dirt['Params'][i].encode('ascii') == "-t":
        variant_flavor.compiler = tool_dirt['Params'][i+1].encode('ascii')
        ARGUMENTS['$COMPILER'] = variant_flavor.compiler
    
   # Set FEATURE_FLAGS environment variable, dsc will pass to compiler
  if options.compiler_flags:
     os.environ["FEATURE_FLAGS"] = options.compiler_flags
  return build_cmd

#####################################################################
# get tool path
#####################################################################
def get_tool_cmd(tool_dirt):
  tool_cmd = []
  #find tool path
  if 'Path' in tool_dirt:
      str1=( tool_dirt['Path'].encode('ascii'))
      str1 = replace_arguments(str1)
      if os.path.exists( str1 ):
        tool_cmd.append(str1)
      else:
        raise NameError ("ERROR:: tool path:" +\
                                   str1+ \
                                   " doesn't exist")
      #if tool is py script add python to the command line
      if os.path.splitext(tool_cmd[0])[1]  == ".py":
        tool_cmd.insert(0,"python")
  return tool_cmd

#####################################################################
# get xbl_sec path
#####################################################################
def get_xbl_sec_path(process_action):
  xbl_sec_path = ""
  #find tool path
  if 'SecImage' in process_action:
      str1 = ( process_action['SecImage'].encode('ascii'))
      str1 = replace_arguments(str1)
      xbl_sec_path=str1
  return xbl_sec_path

#####################################################################
# pre parser json file
# remove comments from json file
#####################################################################
def pre_parser_json_file(json_file):
  json_content = delcommonts.remove_json_comments(json_file,None)
  try:
    json_dirt = json.loads(json_content)
  except Exception as error:
    raise NameError("ERROR: pre_parser_json_file::paser: " + \
                            json_file + str(error))
  return json_dirt

##############################################################################
# search_valid_dirs with regex
##############################################################################
def search_valid_dirs(search_path, regex):
  valid_dirs = []
  for f in os.listdir(search_path):
    if re.search(regex, f):
      valid_dirs += [os.path.join(search_path, f)]
  return valid_dirs

##############################################################################
# search_dir
##############################################################################
def search_dir(search_path, target):
  valid_dirs = []
  for f in os.listdir(search_path):
    if target.upper()== f.upper():
      valid_dirs += [os.path.join(search_path, f)]
  return valid_dirs
  
##############################################################################
# compare_files(file1_path,file2_path)
##############################################################################
def compare_files(file1_path,file2_path):
  f1, f2 = None, None
  file_compare = 0
  try:
    size1 = os.path.getsize(file1_path)
  except Exception as err:
    raise NameError(err)
    
  try:
    size2 = os.path.getsize(file2_path)
  except Exception as err:
    raise NameError(err)
    
  if size1 != size2:
    file_compare = -1
    
  if file_compare == 0:   
    try:
      f1 = open(file1_path,"r")
    except IOError:
      print file1_path + "does not exist!"
      sys.exit(2)
    try:
      f2 = open(file2_path,"r")
    except IOError:
      f1.close()
      print file1_path + "does not exist!"
      sys.exit(2)
    while 1:
      line1 = f1.readline()
      line2 = f2.readline()
      if line1 != line2:
         file_compare = -1
         break
      if not line1 or not line2:
         break
    f1.close()
    f2.close()
  return file_compare
  
##############################################################################
# init_conf
##############################################################################
def init_conf_folder():
  Conf_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], "..","Conf")
  Conf_def_path = os.path.join(os.path.split(os.path.realpath(__file__))[0],"..","BaseTools","Conf")
  for conf_file in conf_files:
    if os.path.exists(os.path.join(Conf_path,conf_file+".txt") ):
      if compare_files(os.path.join(Conf_path,conf_file+".txt"),os.path.join(Conf_def_path,conf_file+".template")) != 0:
        print "Buildex:: delete file ",os.path.join(Conf_path,conf_file+".txt")
        os.remove(os.path.join(Conf_path,conf_file+".txt"))

##############################################################################
# setup_environment
##############################################################################
def setup_environment():
  
  print "[buildex.py] Building on " + platform.system()
  make_name = ""
  if platform.system() is "Windows":
    make_name = "nmake.exe"
    EDK_ENV_FLAG = 'EDK_ENV_STARTS_HERE'
    os.chdir(os.path.dirname(os.path.realpath(__file__)) + os.sep + "..")
    edk_output = subprocess.check_output('edksetup.bat --nt32 && echo ' + EDK_ENV_FLAG + ' && set', shell=True)
    parse_and_set_edk_env(edk_output, EDK_ENV_FLAG)

  else:
    make_name = "make"
    os.environ["ARMLMD_LICENSE_FILE"] = DEFAULT_ARM_LICENSE_FILE
    curr_dir = os.getcwd()
    os.chdir(os.path.dirname(os.path.realpath(__file__)) + os.sep + "..")
    edk_command = ['bash', '-c', 'source edksetup.sh BaseTools && env']
    proc = subprocess.Popen(  edk_command, stdout=subprocess.PIPE)
    for line in proc.stdout:
      (key, _, value) = line.partition("=")
      os.environ[key] = value.strip()
    proc.communicate()
    os.chdir(curr_dir)
    # Check if nmake.exe/make.exe is in PATH
    make_found = False
    for path in os.environ["PATH"].split(os.pathsep):
      if os.path.exists(os.path.join(path, make_name)):
        make_found = True
        break
    if not make_found:
      raise NameError("ERROR: buildex::setup_environment: " + make_name +\
                      " not found in PATH")
  if "WORKSPACE" not in os.environ:
    raise NameError("ERROR: buildex::setup_environment: " + \
                    "Failed to setup environment variable WORKSPACE.")
  
  # Check that boot_images folder exists, there are dependencies on this
  if BOOT_IMAGES_DIR not in os.environ["WORKSPACE"]:
    raise NameError("ERROR: buildex::setup_environment: " + \
                    "Build root folder 'boot_images' is missing. Please ensure this folder exist.")
  
  # Enable sectools and set sectools_policy
  os.environ["USE_SECTOOLS"] = "1"
  os.environ["SECTOOLS_POLICY"] = "USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN"
  os.environ["USES_PLATFORM_IMAGE_INTEGRITY_CHECK"] = "1"
  os.environ[os.environ["SECTOOLS_POLICY"]] = "1"
  os.environ["SECTOOLS_CFG_PATH"] = os.path.join(os.environ["WORKSPACE"], QCOM_DIR,"Tools","sectools", "config","integration")
  os.environ["SECTOOLS_SCRIPTS_PATH"] = os.path.join(os.environ["WORKSPACE"], QCOM_DIR,"Tools","sectools")
  
  
  if DEVDEBUG:
    print "[buildex.py] WORKSPACE:" + os.environ["WORKSPACE"]
    print "[buildex.py] Environment\n"
    print os.environ

#####################################################################
# set compiler path
#####################################################################
def set_compiler_path(tool_dirt,tool_name):
   COMPILER_NAME = tool_name.encode('ascii');
   COMPILER_BIN_NAME = COMPILER_NAME+"_BIN"
   COMPILER_PREFIX  = COMPILER_NAME+"_ARM_PREFIX"
   if os.path.exists(tool_dirt['Path']):
     os.environ[COMPILER_BIN_NAME] = tool_dirt['Path']
   else:
     print tool_dirt['Path'] +"does not exist"
     raise NameError("ERROR: buildex::build_single_flavor: " + \
                        "Missing LLVM tool chain." + \
                        "Expected location " + os.environ[COMPILER_BIN_NAME])
     
   if os.path.exists(tool_dirt['Tool_path']):
     os.environ[COMPILER_PREFIX] = tool_dirt['Tool_path']
   else:
     print tool_dirt['Tool_path'] + " does not exist"
     raise NameError("ERROR: buildex::build_single_flavor: " + \
                        "Missing LLVM tool chain." + \
                        "Expected location " + os.environ[COMPILER_PREFIX])
   if platform.system() is "Windows":
     os.environ["MAKE_PATH"] = "nmake"
     os.environ["MAKE_FLAGS"] = "/C"
   else:
     os.environ["MAKE_PATH"] = "make"
     os.environ["MAKE_FLAGS"] = ""
     
   is_bin_exist = False
   for path in os.environ["PATH"].split(os.pathsep):
      if path == os.getenv(COMPILER_BIN_NAME):
        is_bin_exist = True
        break
   if not is_bin_exist:
      os.environ["PATH"] = os.environ[COMPILER_BIN_NAME]+ os.pathsep + os.environ["PATH"]
      
##############################################################################
# parse_and_set_edk_env
##############################################################################    
def parse_and_set_edk_env(edk_output, EDK_ENV_FLAG):
  
  # Go through every line
  # Skip until 'ENDFLAG'
  # Then add everything in env to current env
  skipping = True
  for line in edk_output.splitlines():
    if not skipping:
      envvar = line[0:line.index('=')]
      envval = line[line.index('=')+1:]
      os.environ[envvar] = envval
    elif re.search(EDK_ENV_FLAG, line):
      skipping = False
  return
  
##############################################################################
# create_build_version
############################################################################## 
def create_build_version(build_version):
  build_version_file = os.path.join(os.environ["WORKSPACE"],
                                               QCOM_DIR, 
                                               "Include",
                                               "BuildVersion.h" )
  
  if os.path.isfile(build_version_file) and build_version == "0":
    print "[buildex.py] QcomPkg\Include\BuildVersion.h already exists."
    print "[buildex.py] No build version specified. Will use existing version."
    return
  
  print "[buildex.py] Creating QcomPkg\Include\BuildVersion.h with version: " + build_version
  try:
    with open(build_version_file, 'w') as vf:
      vf.write("/* DO NOT EDIT: This file is autogenerated */\n")
      vf.write("#ifndef __BUILDVERSION_H__\n")
      vf.write("#define __BUILDVERSION_H__\n\n")
      vf.write("#ifndef UEFI_BUILD_VERSION\n")
      vf.write("  #define UEFI_BUILD_VERSION        \"" + build_version + "\"\n")
      vf.write("#endif\n\n")
      vf.write("#endif /* __BUILDVERSION_H__ */\n")
  except:
    raise NameError("ERROR: buildex::create_build_version: " + \
                    "Unable to build version file")

##############################################################################
# make_edk_tools
##############################################################################
def make_edk_tools():
  if platform.system() == "Windows":
    return
    
  workspace_file = os.path.join(os.environ["WORKSPACE"], "BaseTools", "workspace.txt")
  
  # Skip building if file exists and current workspace matches
  if os.path.isfile(workspace_file) and \
     os.environ["WORKSPACE"] == open(workspace_file).read().strip():
    print "[buildex.py] Using prebuilt tools"
    return
  
  tools_path = os.path.join(os.environ["WORKSPACE"], "BaseTools")
  
  # Clean build tools if it already exists
  if not os.path.isdir(os.path.join(os.environ["WORKSPACE"],"BaseTools","Source","C", "bin")):
    subprocess.call(["make", "-C", tools_path, "clean"])
  
  print "[buildex.py] Building tools..."
  subprocess.call(["make", "-C", tools_path])
  # write workspace to file
  with open(workspace_file, 'w') as wsf:
    wsf.write(os.environ["WORKSPACE"])
  print "Done"

##############################################################################
# image_version_setup
##############################################################################
def image_version_setup(target, variant):
  image_version_dir = os.path.join(os.environ['WORKSPACE'], 'QcomPkg', 'Library', 'ImageVersionLib')
  image_version_builder = os.path.join(os.environ['WORKSPACE'], 'QcomPkg', 'Tools', 'scripts', 'version_builder.py')
  
  version_files = [os.path.join(image_version_dir, 'oem_version.c'),
                   os.path.join(image_version_dir, 'oem_uuid.c'),
                   os.path.join(image_version_dir, 'qc_version.c')]
                   
  for vf in version_files:
    if os.path.isfile(vf): os.remove(vf)
  
  version_build_cmd = ['python', image_version_builder, 
                       '-t', image_version_dir,
                       '-b', target + variant]
  curr_dir = os.getcwd()
  os.chdir(os.environ["WORKSPACE"])
  print "[buildex.py] Generating image version file..."
  
  if platform.system() == "Windows":
    print "\tPlease be patient. This step may take a while on Windows machines..."
  
  try:
    subprocess.check_call(version_build_cmd)
  except Exception as error:
    if error.returncode == 1:
      raise NameError("ERROR: buildex::image_version_setup:" + \
                      "image version setup failed.")
    elif error.returncode == 2:
      raise NameError("ERROR: buildex::image_version_setup:" + \
                      "image version setup failed with following error: " + \
                      "manifest.xml not found!")
  
  for vf in version_files:
    if not os.path.isfile(vf):
      print "\nError: Image Version File not generated"
      raise NameError("ERROR buildex::image_version_setup:" + \
                      "image version setup failed to genearte file: " + vf)
  print "Done"

#####################################################################
# search all target path
# if option_target == None, it will return all the folders matched regex "*pkg"
#####################################################################
def search_all_target_path(option_target = None):
  target_paths = []
  boot_dir = os.environ["WORKSPACE"]
  search_path = os.path.join(boot_dir, QCOM_DIR)
  # Set Target regex
  if option_target:
    for target in option_target.split(","):
      target_regex = target
      target_match = search_dir(search_path, target_regex);
      if target_match:
        target_paths.append(target_match[0])
      else:
        raise NameError("ERROR: buildex::search_all_target_path: " + target +\
                      " not found in "+search_path)
  else:
    target_regex = DEFAULT_TARGET_REGEX
    for target_match in search_valid_dirs(search_path, target_regex):
      target_paths.append(target_match)

    if len(target_paths) == 0:
      print "!!!!!!!!!!!!!!!no pkg folder!!!!!!!!!!!!"
      raise NameError("ERROR: buildex::search_all_target_path: "\
                      " not found *pkg folder in PATH")
                      
  return target_paths

#####################################################################
# find componets
#####################################################################
def find_componets(image,json_file):
  componets = [] 
  build_json = delcommonts.remove_json_comments(json_file,None)
  build_obj=json.loads(build_json)
  if image:
    for i in range(len(build_obj['CreateImages'])):
      if build_obj['CreateImages'][i]['Name']== unicode(image):
        componets =  build_obj['CreateImages'][i]['Process'][0]['Components']
        break
  else:
    for i in range(len(build_obj['CreateImages'])):
      componets.extend( build_obj['CreateImages'][i]['Process'][0]['Components'])
  if len(componets) == 0:
    raise NameError("ERROR: buildex::find_componets: "\
                      " not found componets in build_json file")
  return componets


#####################################################################
# find componets
#####################################################################
def find_dsc_path_from_configuration(cmd):
  componets = []
  for i in range(len(cmd)):
    if cmd[i] == "-p":
      return (cmd[i+1]) 

#####################################################################
# match images
# find all the images
#####################################################################
def match_images(matchimages,json_libs):
  images = []
  if matchimages:
    for matchimage in matchimages.split(","):
      for i in range(len(json_libs['CreateImages'])):
        if json_libs['CreateImages'][i]['Name'].upper() == unicode(matchimage).upper():
          images.append(json_libs['CreateImages'][i]['Name'])
  else:
    for i in range(len(json_libs['CreateImages'])):
      images.append(json_libs['CreateImages'][i]['Name'])
  if len(images) == 0:
    raise NameError("ERROR: buildex::match_images: "\
                      " not found images in build_json file")
  return images

#####################################################################
# search all build json path
#####################################################################
def search_all_build_json_path(target_paths):
  build_json_paths = []
  build_json_regex = "buildconfig.json"
#  print "/***************search_build_json_path*********************/"
  
  for target_path in target_paths:
    current_build_json_paths = search_valid_dirs(target_path,build_json_regex)
    if len(current_build_json_paths):
      build_json_paths.append(current_build_json_paths[0])
    else:
      print "There is no build json file "+target_path+ " in this path"

  if len(build_json_paths) == 0:
   print "!!!!!!!!!!!!!!!no build json file found!!!!!!!!!!!!"
   raise NameError("ERROR: buildex::search_all_build_json_path: "\
                      " not json file found in PATH")
  return build_json_paths

#####################################################################
# search build json path
#####################################################################
def search_build_json_path(target_path):
  build_json_regex = "buildconfig.json"

  current_build_json_paths = search_valid_dirs(target_path,build_json_regex)
  if len(current_build_json_paths) == 0:
    print "!!!!!!!!!!!!!!!no build json file found!!!!!!!!!!!!"
    raise NameError("ERROR: buildex::search_all_build_json_path: "\
                      " not json file found in PATH")

  return current_build_json_paths[0]


##############################################################################
# find_tag_in_descriptor_file
##############################################################################
def find_tag_in_descriptor_file(dsc, section, tag):
  with open(os.path.join(os.environ["WORKSPACE"], dsc), 'r') as f:
    
    if section:
      in_desired_section = False
    else:
      in_desired_section = True
      
    for line in f.readlines():
      # Check if entering new section, only if section is provided
      if section:
        section_search = re.search("\[(\S+)\]", line)
        if section_search:
          # Check if entering desired section
          if section_search.group(1) == section:
            in_desired_section = True
          elif in_desired_section:
            # Hit new section, and already hit desired
            # No tag was found, return early
            return ""
      
      if in_desired_section or not section:
        expr = tag + "\s*=\s*(\S+)"
        tag_search = re.search(expr, line)
        if tag_search:
          return tag_search.group(1)
  
  # Not found anywhere
  return ""

def find_next_param_from_list(parms_list,param_name):
  if param_name in parms_list :
    index = parms_list.index(param_name)
    return parms_list[index+1].encode('ascii')
  else:
    return ""
##############################################################################
# perform_merges
##############################################################################
def perform_merges(options,dll_paths, variant_flavor,process_action,merge_tool_dirt):
  xbl_sec = ""
  dlls = dll_paths
  cxbl_cmd = []
  output_name = os.path.basename(find_next_param_from_list(process_action['Params'],'-o'))
  if 'SecImage' in process_action:
    xbl_sec = process_action['SecImage']

  if merge_tool_dirt:
   cxbl_tool_cmd = get_tool_cmd(merge_tool_dirt)
  
  #
  # createxbl single
  #
  single_dll =""
  if len(dlls) == 1:
    single_dll = dlls[0]
    
  if single_dll:
    # Skip createxbl step to avoid stripping symbols
    if output_name.split('.')[0].lower() in COPY_IMAGES_AS_IS:
      merged_output_elf = dlls[0]
    else:
      # Single merge
      ARGUMENTS['$FIRSTDLL']= single_dll
      ARGUMENTS['$SECONDDLL']= ""
      ARGUMENTS['$XBLSEC']= get_xbl_sec_path(process_action)

      cxbl_cmd = get_cmd_params(options,variant_flavor,process_action,merge_tool_dirt)
                  
      # Run merge command
      try:
        subprocess.check_output( cxbl_tool_cmd + cxbl_cmd)
      except Exception as error:
        raise NameError("ERROR: buildex::perform_merges: " + \
                        "Failed to merge with error: " + str(error))
    
  #
  # createxbl multiple merge
  #
  else:
       
    first_dll = dlls[0]
    num_dlls = len(dlls)
    ARGUMENTS['$FIRSTDLL'] = first_dll
    ARGUMENTS['$XBLSEC']= get_xbl_sec_path(process_action)
    for x in xrange(1,num_dlls):
      # grab the next dll
      second_dll = dlls[x]
      ARGUMENTS['$SECONDDLL'] = second_dll

      # Everything is always merged into 64-bit elfs
      cxbl_cmd = get_cmd_params(options,variant_flavor,process_action,merge_tool_dirt)
      # if not final merge
      if x < (num_dlls - 1):
        cxbl_cmd.append('-n')
                
      # Run merge command
      try:
        cmd_output = subprocess.check_output( cxbl_tool_cmd + cxbl_cmd)
      except Exception as error:
        raise NameError("ERROR: buildex::perform_merges: " + \
                        "Failed to merge with error: " + str(error))
      first_dll = find_next_param_from_list(cxbl_cmd,"-o")
      ARGUMENTS['$FIRSTDLL'] = first_dll
      ARGUMENTS['$XBLSEC']= ""
  variant_flavor.merged_output_elf = output_name
  variant_flavor.output_name = output_name
  output_dir_root = os.path.dirname(find_next_param_from_list(cxbl_cmd,'-o'))
  return [output_dir_root, output_name]




##############################################################################
# generate_merge_items
#   merge_dict= merge_book()
##############################################################################
def generate_merge_item(merge_dict,dsc_path,variant_flavor,process_dirt):
  # Check for normal merges of images inside dsc
  # check for fdf
  #dsc=[dsc_path,variant_flavor.aarch]
  fdf_file = find_tag_in_descriptor_file(dsc_path, "Defines", "FLASH_DEFINITION")
  if fdf_file:
    build_dir = find_tag_in_descriptor_file(dsc_path, "Defines", "OUTPUT_DIRECTORY")
    if not build_dir:
      build_dir = find_tag_in_descriptor_file(dsc_path, "Defines." + variant_flavor.aarch, "OUTPUT_DIRECTORY")
      if not build_dir:
        raise NameError("ERROR: buildex::handle_fdf_file: " + \
                      "Unable to find OUTPUT_DIRECTORY tag in dsc: " + dsc_path)
      return os.path.join(os.environ["WORKSPACE"],
                          build_dir,
                          variant_flavor.release_mode + "_" + variant_flavor.compiler,
                          "FV","FVMAIN_COMPACT.elf")
    
  #Check for any merge entries inside DSC
  with open(os.path.join(os.environ["WORKSPACE"],dsc_path), 'r') as d:
    # skip to [Components.common]
    while not re.search('\[Components.common\]', d.readline()):
      pass
    
    # Search for infs now
    inf =  ""
    bracket_count = 0
    
    for line in d.readlines():
      
      # 0 implies not inside inf block yet
      if bracket_count == 0:
        
        # skip commented lines if outside of an inf block
        if line.lstrip().find("#") == 0:
          continue
  
        # look for open bracket, start of inf block
        if line.find(".inf") != -1 and line.find("{") != -1:
          bracket_count = 1
          inf = re.search("(\S+.inf)", line).group(1)
          
      elif bracket_count > 0:
        #Inside INF
        if line.find("{") != -1:
          bracket_count = bracket_count + 1
        elif line.find("}") != -1:
          bracket_count = bracket_count - 1
          # Upon closing brackets, validate and add merge item

            
        if bracket_count < 0:
          raise NameError("ERROR: buildex::generate_merge_items: " + \
                          "Malformed dsc file: " + dsc_path)
          
      if bracket_count == 0 and inf:
        return get_dll_from_dsc_inf(variant_flavor,dsc_path,inf)  

##############################################################################
# get_dll_from_dsc_inf
##############################################################################
def get_dll_from_dsc_inf(variant_flavor,dsc_path,inf):
  arch = variant_flavor.aarch
  output_dir = ""
  for section in ["Defines", "Defines." + arch]:
    result = find_tag_in_descriptor_file(dsc_path, section, "OUTPUT_DIRECTORY")
    if result:
      output_dir = result
  if not output_dir:
    raise NameError("ERROR: buildit::get_dll_from_dsc_inf: " + \
                    "Missing OUTPUT_DIRECTORY tag in dsc: " + dsc_path)
  

  dll_path = os.path.join(os.environ["WORKSPACE"],
                              output_dir,
                              variant_flavor.release_mode + "_" + variant_flavor.compiler,
                              arch,
                              inf.split(".inf")[0],
                              "DEBUG",
                              find_tag_in_descriptor_file(inf, "Defines", "BASE_NAME") + ".dll")
  return dll_path


##############################################################################
# append_build_products
##############################################################################
def append_build_products(built_items):
  for item in built_items:
    output_dir = item[0]
    file_name = item[1]
    bp_cmd = ['python', os.path.join(os.environ['WORKSPACE'],'QcomPkg','Tools',
                                   'scripts','buildproducts_builder.py'),
            '-t', os.path.join(os.environ["WORKSPACE"],'..'),
            '-s', output_dir,
            '-i', file_name]
    subprocess.check_output(bp_cmd)
    
if __name__ == "__main__":
  main()
