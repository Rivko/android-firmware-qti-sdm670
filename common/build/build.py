#!/usr/bin/env python
#@PydevCodeAnalysisIgnore
'''
=======================================================================================================
gen_build_manifest.py:

Usage:
   This script performs task that are common across all builds, and which must be done after all builds are complete.
   
   build.py [--wflow_filter= <filter1, filter2 ...n> --nonhlos, --hlos, --cmm, --imf, --clean]
   Examples:
      build.py --nonhlos  (generates NON_HLOS.bin alone )
      build.py --hlos     (generates sparse images if rawprogram0.xml exists)
	  build.py --wflow_filter= <filter1, filter2.... > ( Execute given filter steps compilation time)
      build.py            (generates NON-HLOS.bin,sparse images and other binaries define work flow step)
      build.py --imf      (Ignore file path validation. Generates NON-HLOS.bin,sparse images and other binaries define work flow step)
      build.py --clean    (Remove all generated binaries created at the time of meta compilation)

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR 
-----------------------------------------------------------------------------------------------
  $Header: //components/rel/build.glue/1.0/build.py#29 $
  $DateTime: 2018/03/30 13:27:39 $
===============================================================================================
'''
import sys
import os
import os.path
import shutil
import traceback
import re
import stat
import string
import subprocess
import json
import time
import datetime
from optparse import OptionParser
from glob import glob

import lib.meta_lib as ml
import lib.meta_log as lg
import lib.update_meta as um
from xml.etree import ElementTree as et
bin_file_dests = dict()


#-------------------------------------------------------------------------#
#------------------------------ SUBROUTINES ------------------------------#
#-------------------------------------------------------------------------#

# Function to create gen_build_flavor.cmm file
def create_gen_build_flavor_cmm ():
   common_dir = os.path.dirname(os.path.dirname(__file__))
   destn_dir  = join_paths(os.path.dirname(__file__), "app")
   if not os.path.exists(destn_dir):
      os.makedirs(destn_dir)
   lg.log("update_common_info.py: Calling gen_cmm_data3 to create " + join_paths(destn_dir, "gen_buildflavor.cmm"))
   lg.log_exec( ["python", "app/gen_cmm_data3.py", os.path.dirname(common_dir), join_paths(destn_dir, "gen_buildflavor.cmm")] )
# End of create_gen_build_flavor_cmm function


# Function to create partition
def  use_fatgen_toget_partitionsize ():
   lg.log("fatgen_tool path:" + fatgen_tool)
   if os.path.exists( fatgen_tool ):
      lg.log('fatgen tool exists, using it to create new NON-HLOS.bin')
         #get modem partition size from partition.xml
      try:
         modem_partition_size = ml.getPartitionSize('../config/partition.xml','modem')
         str_modem_partition_size = str( modem_partition_size )
         #print str_modem_partition_size
         
      except ml.PartitionSizeNotFoundException , ex:
         sys.exit(ex.partition_name + ' PartitionSizeNotFoundException raised')
   else:
      sys.exit('fatgen tool does not exist.')
   return str_modem_partition_size
# End of use_fatgen_toget_partitionsize function


# Use cpfatfs tool to add binares
def use_cpfatfs():
   for flavor in bin_file_dests:
      fat_file_dest = os.path.join(bin_file_dests[flavor], 'NON-HLOS.bin')
      
      #if NON-HLOS.bin exists delete it, a new NON-HLOS.bin will be created
      #if NON-HLOS.bin already exists, fatgen.py doesn't create it
      #deleting this file so that it is created each time
      if os.path.exists( fat_file_dest ):
         os.remove( fat_file_dest )
         lg.log("Existing " + fat_file_dest + " has been deleted.") 
      if (fatgen_build):
         # Create NON-HLOS.bin of container size as specified in partition.xml
         modem_partition_size = use_fatgen_toget_partitionsize ()
         ret_code_fatgen_tool = lg.log_exec(['python',fatgen_tool,'-f','-s',modem_partition_size,'-n',fat_file_dest],verbose=0)
         if ret_code_fatgen_tool != 0:
             err_msg = 'update_common_info.py:'+'python'+' '+fatgen_tool+' '+'-f'+' '+'-s'+' '+modem_partition_size+' '+'-n'+' '+fat_file_dest +':Faled'
             lg.log(err_msg)
             raise Exception(err_msg)
      else:
         # Copy fat.bin to our root directory.  Assume fat.bin is in same directory as cpfatfs.exe   
         shutil.copy(cpfatfs_path + fat_file_src, fat_file_dest)   
         
   
      os.chmod(fat_file_dest, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
      fat_dest_dir = 'image'
      for fat_file in mi.get_files(attr = 'fat_file', flavor = flavor):
         lg.log('update_common_info.py:' + 'cpfatfs' + ' ' +  fat_file +  ' ' + fat_file_dest,verbose=0)
         ret_code_cpfatfs_tool = lg.log_exec([cpfatfs_tool, fat_file_dest, fat_dest_dir, fat_file],verbose=0)
         if ret_code_cpfatfs_tool != 0:
            err_msg = 'update_common_info.py:' + cpfatfs_tool +' '+ fat_file_dest +' '+ fat_dest_dir+' '+fat_file+':Failed'
            lg.log(err_msg)
            raise Exception(err_msg)
# End of use_cpfatfs function


# Function to use fat add tool to add binares
def   use_fat_add ():
   for flavor in bin_file_dests:
      fat_file_dest = os.path.join(bin_file_dests[flavor], 'NON-HLOS.bin')

      #if NON-HLOS.bin exists delete it, a new NON-HLOS.bin will be created
      #if NON-HLOS.bin already exists, fatgen.py doesn't create it
      #deleting this file so that it is created each time
      if os.path.exists( fat_file_dest ):
         os.remove( fat_file_dest )
         lg.log("Existing " + fat_file_dest + " has been deleted.")
      if (fatgen_build):
         # Create NON-HLOS.bin of container size as specified in partition.xml
         modem_partition_size = use_fatgen_toget_partitionsize ()
         ret_code_fatgen_tool = lg.log_exec(['python',fatgen_tool,'-f','-s',modem_partition_size,'-n',fat_file_dest],verbose=0)
         if ret_code_fatgen_tool != 0:
             err_msg = 'update_common_info.py:'+'python'+' '+fatgen_tool+' '+'-f'+' '+'-s'+' '+modem_partition_size+' '+'-n'+' '+fat_file_dest +':Faled'
             lg.log(err_msg)
             raise Exception(err_msg)         
      else:
         # Copy fat.bin to our root directory.  Assume fat.bin is in same directory as cpfatfs.exe   
         shutil.copy(fatadd_path + fat_file_src, fat_file_dest)
               
      os.chmod(fat_file_dest, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
      
      # Add each "fat_file" from contents.xml
      fatFiles = mi.get_files(attr = 'fat_file', flavor=flavor)
      # Create sub directories in NON-HLOS.bin for each "sub_dir" from contents.xml
      subDirFiles = mi.get_files(attr = 'sub_dir', flavor=flavor,abs=False)
      subDirFatFiles = {}
      if len(subDirFiles) > 0:
          for relfile in subDirFiles:
              for absfile in fatFiles:
                  tempabsfile = absfile
                  if on_linux == False:     
                      tempabsfile = absfile.replace('/','\\')
                  if tempabsfile.find(relfile) != -1:
                      subDirFatFiles[absfile] = os.path.dirname(relfile)
                            

      for fat_file in fatFiles:
         fat_dest_dir = 'image'
         lg.log(fat_file)
         if fat_file in subDirFatFiles:
             fat_dest_dir = os.path.join(fat_dest_dir,subDirFatFiles[fat_file])
         lg.log('update_common_info.py:' + 'fatadd' + ' ' +  fat_file +  ' ' + fat_file_dest,verbose=0)
         ret_code_fatadd_tool = lg.log_exec(['python',fatadd_tool,'-n',fat_file_dest,'-f' + fat_file,'-d'+ fat_dest_dir],verbose=0)
         if ret_code_fatadd_tool != 0:
            err_msg = 'update_common_info.py:' + 'python'+' '+fatadd_tool+' '+'-n'+' '+fat_file_dest+' '+'-f' + ' '+fat_file+' '+'-d'+' '+ fat_dest_dir+':Failed'
            lg.log(err_msg)
            raise Exception(err_msg)
      
      # Add ver_info.txt
      fat_dest_dir = 'verinfo'
      bid_file = os.path.join(os.path.dirname(__file__), 'Ver_Info.txt')
      if os.path.exists(bid_file):
         lg.log('update_common_info.py:' + 'fatadd' + ' ' +  bid_file +  ' ' + fat_file_dest,verbose=0)
         ret_code_fatadd_tool = lg.log_exec(['python',fatadd_tool,'-n',fat_file_dest,'-f' + bid_file,'-d'+ fat_dest_dir],verbose=0) 
         if ret_code_fatadd_tool != 0:
             err_msg = 'update_common_info.py:' + 'python'+' '+fatadd_tool+' '+'-n'+' '+fat_file_dest+' '+'-f' + bid_file+' '+'-d'+ fat_dest_dir+':Failed'
             lg.log(err_msg) 
             raise Exception(err_msg)
# End of use_fat_add function



# Function to implement join_paths with "None"
def join_paths(*arg):
   arg = [x for x in arg if x is not None]
   return os.path.join(*arg)
# End of function join_paths



# Function to make a copy of step with updated flavor values
def create_flavor_dict (step_dict, flavor):
   # Update flavor with current flavor value in step
   step_dict_flavor = {}
   step_dict_flavor = step_dict.copy()
   contains_flavor_flag = False
   for key in step_dict_flavor:
      if ('$[flavor' in step_dict_flavor[key]):
         contains_flavor_flag = True  # To check whether to loop on flavors while executing
      step_dict_flavor[key] = ml.update_var_values(step_dict[key], "flavor", flavor)
   return (step_dict_flavor, contains_flavor_flag)
# End of function create_flavor_dict()


# Function to process the standard keys in each step before running
def preprocess_step (step_dict_flavor, flavor):
   # If step is "exec" process the tool name to get path
   if ('tool_name' in step_dict_flavor):
      tool_name = step_dict['tool_name']
      tool_build = mi.get_tool_build(tool_name, 'workflow')
      if tool_build:
         # If valid build exists, get the full path to tool
         tool_pfn = join_paths(mi.get_build_path(tool_build), 
                               mi.get_tool_path(tool_name, 'workflow'), 
                               tool_name)
         step_dict_flavor['tool_name'] = tool_pfn
      else:
         step_dict_flavor['tool_name'] = None
         # The tool for this exec step is not present. Need not proceed...
         return
   
   # If destination directory is specified, create if not found
   if ('destn_dir' in step_dict_flavor):
      destn_dir = ml.clean_wvars(step_dict_flavor['destn_dir'])
      if not os.path.exists(destn_dir):
         lg.log("update_common_info.py: Creating directory " + destn_dir)
         os.makedirs(destn_dir)
      # After evaluating map it back to the dictionary
      step_dict_flavor['destn_dir'] = destn_dir
   
   # If execution directory is specified, create and cd to it
   if ('exec_dir' in step_dict_flavor):
      exec_dir = ml.clean_wvars(step_dict_flavor['exec_dir'])
      if not os.path.exists(exec_dir):
         os.makedirs(exec_dir)
      # Save the current path and change to exec_dir
      start_dir = os.getcwd()
      step_dict_flavor['start_dir'] = start_dir
      os.chdir(exec_dir)
      step_dict_flavor['exec_dir'] = exec_dir
      lg.log("Changed working directory to " + step_dict_flavor['exec_dir']);
      
   for step_elem in step_dict_flavor.keys():
      if step_elem.startswith('root_dir'):
         build = ml.clean_wvars(step_dict_flavor[step_elem])
         root_dir_si = mi.get_build_path(build)
         step_dict_flavor[step_elem] = root_dir_si 
      
   # Get list of source files based on the given key
   src_files = []
   if ('src_files' in step_dict_flavor):
      src_files = step_dict_flavor['src_files']
      if '$' in src_files:     
         # If source_files is listed as var:value pair, use get_files and filter with provided attr
         # This will return a list of files filtered with flavor and attribute
         attr = ml.clean_wvars(src_files)
         src_files = mi.get_files(attr=attr, flavor=flavor)
      else:
         # Treat is just as a file or a directory path and make it a list
         src_files = [ml.clean_wvars(src_files)]
      step_dict_flavor['src_files'] = src_files
   src_file_vars = []
   if ('src_file_vars' in step_dict_flavor):
      # If source is listed as file_vars, use get_file_vars
      # This will return a dictionary with var:[file] pairs
      attr = ml.clean_wvars(step_dict_flavor['src_file_vars'])
      src_file_vars = mi.get_file_vars(attr, flavor = flavor)
      step_dict_flavor['src_file_vars'] = src_file_vars
# End of preprocess_step()


# Functions to run each step in workflow based on its type
def workflow_copy(step_dict):
   # Execution step should specify the tool name to run
   if not ('destn_dir' in step_dict or 'src_files' in step_dict):
      lg.log("update_common_info.py: Invalid copy step! Source Files or Destination not specified")
   else:
      for flavor in flavors:
         # Make a copy of step_dict for current flavor
         (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)
         
         # Preprocess the standard items in the step
         preprocess_step(step_dict_flavor, flavor)
         
         lg.log("build.py: --------------------------------------------------")
         lg.log("build.py:  Starting copy")
         
         if ('src_files' in step_dict_flavor.keys()):
            src_files = step_dict_flavor['src_files']
            destn_dir = step_dict_flavor['destn_dir']
            # Copy each file in the list
            for src_file in src_files:
               # Directory vs File
               if ('@' in src_file):
                  src_file = ml.evaluate_params(step_dict_flavor,src_file)
               if (os.path.isdir(src_file)):
                  lg.log("update_common_info: Copying directory tree " + src_file + " to " + destn_dir)
                  copytree(src_file, destn_dir)
               else:
                  for file in glob(src_file): # for wildcards
                     lg.log("Copying " + file + " to " + destn_dir)
                     shutil.copy(file, destn_dir)

         if ('src_file_vars' in step_dict_flavor.keys()) and (len(step_dict_flavor['src_file_vars']) > 0):
            src_file_vars = step_dict_flavor['src_file_vars']
            for key in src_file_vars.keys():
               destn_dir = step_dict_flavor['destn_dir']
               for src_file in src_file_vars[key]:
			      # Directory vs File
                  if (os.path.isfile(src_file)): # No wildcards
                     destloc = 	destn_dir			  
                     if(key.lower() != 'true'): # Rename file based on attribute value
                        destloc = os.path.join(destn_dir, key)
                        base_dir = os.path.dirname(destloc)
                        if os.path.exists(base_dir) is False:
                           os.mkdir(base_dir)
                     lg.log("Copying " + src_file + " to " + destloc)
                     shutil.copy(src_file, destloc)
                  elif (os.path.isdir(src_file)):
                     lg.log("update_common_info: Copying directory tree " + src_file + " to " + destn_dir)
                     copytree(src_file, destn_dir)
                  else:
                     for file in glob(src_file): # for wildcards
                        lg.log("Copying " + file + " to " + destn_dir)
                        shutil.copy(file, destn_dir)
				  
            lg.log("build.py: --------------------------------------------------\n")
      
         # Don't have to repeat if the step doesn't contain flavor info
         if (len(flavors) > 0) and (not flavor_flag):
            break

def workflow_delete(step_dict):
   for flavor in flavors:
      # Make a copy of step_dict for current flavor
      (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)

      # Preprocess the standard items in the step
      preprocess_step(step_dict_flavor, flavor)
      
      # Once destination path is processed, try deleting it
      for key in step_dict_flavor:
         # Delete step should contain only keys of destinations to delete
         destn = ml.clean_wvars(step_dict_flavor[key])
         try:
            if os.path.isdir(destn):
               shutil.rmtree(destn)
               lg.log("update_common_info.py: Deleted the directory " + destn);
            else:
               os.remove(destn)
               lg.log("update_common_info.py: Deleted the file " + destn);
         except:
            lg.log("update_common_info.py: Unable to delete " + destn);

      # Don't have to repeat if the step doesn't contain flavor info
      if (len(flavors) > 0) and (not flavor_flag):
         break

def workflow_exec(step_dict):
   # Execution step should specify the tool name to run
   if not ('tool_name' in step_dict or 'params' in step_dict):
      lg.log("update_common_info.py: Invalid exec step! Tool name or command not specified")
   else:
      for flavor in flavors:
         # Make a copy of step_dict for current flavor
         (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)
         
         # Preprocess the standard items in the step
         preprocess_step(step_dict_flavor, flavor)
         
         # After preprocess check if the tool is available and execute
         if step_dict_flavor['tool_name']:
            lg.log("build.py:  --------------------------------------------------")
            lg.log("build.py:   Calling " + step_dict_flavor['tool_name'].split('/')[-1] + " " + (flavor or ""))
            
            # Check if the step has file_vars or files and loop accordingly
            if ('src_file_vars' in step_dict_flavor.keys()) and (len(step_dict_flavor['src_file_vars']) > 0):
               src_file_vars = step_dict_flavor['src_file_vars']
               for key in src_file_vars.keys():
                  step_dict_flavor['src_file_var'] = key
                  for file in src_file_vars[key]:
                     step_dict_flavor['src_file'] = file
                     params = ml.evaluate_params(step_dict_flavor)
                     lg.log("build.py:  --------------------------------------------------\n")

                     return_code = lg.log_exec(params)
                     if (return_code != 0 and (('tool_name' in step_dict) and (step_dict['tool_name'] in  ("fatadd.py","fatgen.py","ptool.py","pil-splitter.py","checksparse.py","match_si_var.py","fat_creation.py","ext_tool_wrapper.py") ))):
                        err_msg = "build.py:  "+params+":Failed"
                        lg.log(err_msg)
                        raise Exception(err_msg)
            elif ('src_files' in step_dict_flavor.keys()) and (len(step_dict_flavor['src_files']) > 0):
               src_files = step_dict_flavor['src_files']
               for src_file in src_files:      
                  step_dict_flavor['src_file'] = src_file
                  params = ml.evaluate_params(step_dict_flavor)
                  lg.log("build.py:  --------------------------------------------------\n")

                  return_code = lg.log_exec(params)
                  if (return_code != 0 and (('tool_name' in step_dict) and (step_dict['tool_name'] in  ("fatadd.py","fatgen.py","ptool.py","pil-splitter.py","checksparse.py","match_si_var.py","fat_creation.py","ext_tool_wrapper.py") ))):
                     err_msg = "build.py:  "+params+":Failed"
                     lg.log("err_msg")
                     raise Exception(err_msg)
            else:
               params = ml.evaluate_params(step_dict_flavor)
               lg.log("build.py:  --------------------------------------------------\n")
               return_code =  lg.log_exec(params)

               if (return_code != 0 and (('tool_name' in step_dict) and (step_dict['tool_name'] in  ("fatadd.py","fatgen.py","ptool.py","pil-splitter.py","checksparse.py","match_si_var.py","fat_creation.py","ext_tool_wrapper.py") ))):
                  err_msg = "build.py:  "+params+":Failed"
                  lg.log(err_msg)
                  raise Exception(err_msg)
            if ('start_dir' in step_dict_flavor.keys()):
               # cd'd to exec_dir in preprocessing, so revert back
               os.chdir(step_dict_flavor['start_dir'])
               del step_dict_flavor['start_dir']
         
         # Don't have to repeat if the step doesn't contain flavor info
         if (len(flavors) > 0) and (not flavor_flag):
            break

def workflow_setenv(step_dict):
   # Preprocess the standard items in the step
   preprocess_step(step_dict, flavor)
   var       = step_dict["var"]
   new_value = ml.evaluate_params(step_dict, step_dict["value"])
   cur_value = "%"+var+"%"
   if cur_value in new_value:
      if var in os.environ:   # try fetching current value only when present
         new_value = new_value.replace(cur_value, os.environ[var], 1)   # only one
      else:                   # just keep the new value
         pass
   lg.log("Setting OS environment variable")
   lg.log(var + " = " + new_value)
   os.environ[var] = new_value
# End of workflow type functions


def copytree(src, dst, symlinks=False, ignore=None): # To copy directory trees
   # native shutil.copytree fn errors out when dest dir is already present
   # creating this function as a workaround
   for item in os.listdir(src):
      s = os.path.join(src, item)
      d = os.path.join(dst, item)
      if os.path.isdir(s):
         shutil.copytree(s, d, symlinks, ignore)
      else:
         shutil.copy2(s, d)
# End of copytree


#-------------------------------------------------------------------------#
#--------------------------------- MAIN ----------------------------------#
#-------------------------------------------------------------------------#

#---------------------------------------------------------#
# Option Parser                                           #
#---------------------------------------------------------#
parser = OptionParser()
parser.add_option("--cmm",action="store_true",dest="cmm",help="Create cmm script",default=False)
parser.add_option("--nonhlos",action="store_true",dest="non_hlos",help="NON HLOS",default=False)
parser.add_option("--hlos",action="store_true",dest="hlos",help="HLOS",default=False)
parser.add_option("--wflow_filter",action="store",type="string",dest="wflow_filter",help="Workflow Filter",default=None)
parser.add_option("--clean",action="store_true",dest="clean_build",help="Clean binaries",default=False)
parser.add_option("--imf",action="store_true",dest="ignore_missing_files",help="Ignore path validation",default=False)
parser.add_option("--variant",action="store", type="string", dest="variant_contents_xml", help="build variant",default="")
(options, args) = parser.parse_args()

# Clean build   	  
if options.clean_build:
   ml.CleanBuild()
   sys.exit()
# Create snapshot file list to track generated files list in build process.
ml.SaveFilesSnapshot()

#---------------------------------------------------------#
# Get bearings                                            #
#---------------------------------------------------------#
on_linux = sys.platform.startswith("linux")

#---------------------------------------------------------#
# Initialize logger and print some diagnostic info        #
#---------------------------------------------------------#
lg = lg.Logger('update_common')
lg.log("build.py: Platform is:" + sys.platform,verbose=0)
lg.log("build.py: Python Version is:" + sys.version,verbose=0)
lg.log("build.py: Current directory is:" + os.getcwd(),verbose=0)

#---------------------------------------------------------------------#
# Check "common/config/" folder if multi-variant contents XML exist   #
#---------------------------------------------------------------------#
contents_xml = '../../contents.xml'
var_contents_xml = '../config/contents.xml' #Default variant
if os.path.exists(var_contents_xml) or len(options.variant_contents_xml) > 0: #Might be support multi-variant
   if len(options.variant_contents_xml) > 0:
      var_contents_xml = '../config/contents_' + options.variant_contents_xml + '.xml'
   if not os.path.exists(var_contents_xml):
      lg.log("Error : " + var_contents_xml + " not found!!!")
      sys.exit(1)
   lg.log("Selected variant:  " + var_contents_xml)
   #Restore selected variant contents XML
   shutil.copy(var_contents_xml, contents_xml)

#---------------------------------------------------------#
# Load the Meta-Info file                                 #
#---------------------------------------------------------#
lg.log("build.py: Loading the Meta-Info file",verbose=0)

mi = ml.meta_info(logger=lg)

#---------------------------------------------------------#
# Get flavor information                                  #
#---------------------------------------------------------#
flavors = mi.get_product_flavors()
# Returns empty list [] if no flavors found
if len(flavors) == 0:
   flavors.append(None)
else:
   lg.log('flavors found')

#---------------------------------------------------------#
# Create a file to store the version info
#---------------------------------------------------------#
verinfo_file = os.path.join(os.path.dirname(__file__), 'Ver_Info.txt')
lg.log("Creating the file verinfo.txt to hold the buildID information at " + verinfo_file)
product_info = {}
metabuild_info = {}
image_Info = {}
buildID = mi.get_build_id('common', 'builds_flat')
product_info['Version'] = '1.0'
metabuild_info['Meta_Build_ID'] = buildID
ts = time.time()
metabuild_info['Time_Stamp'] = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
metabuild_info['Product_Flavor'] = str(flavors)
product_info['Metabuild_Info'] = metabuild_info
build_ids = mi.get_build_list()
build_ids = build_ids
for  tag in build_ids:
   si_buildid = mi.get_build_id(tag)
   if si_buildid == None or len(si_buildid)<=0:
      si_buildid = mi.get_build_path(tag)
   image_Info[tag] = si_buildid 
   # Image_Info = OrderedDict(sorted(Image_Info.items(), key=lambda t: t[0]))
product_info['Image_Build_IDs'] = image_Info
with open(verinfo_file, 'w') as outfile:
   json.dump(product_info, outfile, sort_keys=True, indent=4)

#---------------------------------------------------------#
# Initialize workflow filters
#---------------------------------------------------------#
workflow_filter_list = []
if options.wflow_filter:
   workflow_filter_list = options.wflow_filter.strip().split(',')
   
# Translate legacy filters to workflow
if (len(workflow_filter_list) == 0):   # only when --wflow_filter is not used
   if options.non_hlos:
      workflow_filter_list.extend(['non_hlos','non-hlos','nonhlos'])
   if options.hlos:
      workflow_filter_list.extend(['hlos'])
lg.log('Filters specified for compilation:')
lg.log(str(workflow_filter_list))

# If not arguments are passed, then set all to True
# Maintaining these for backward compatibility if not using workflow
if not (options.cmm or options.non_hlos or options.hlos):
   options.cmm = True
   options.non_hlos = True
   options.hlos = True
    
#---------------------------------------------------------#
# Setup our bin directory                                 #
#---------------------------------------------------------#
for flavor in flavors:
   path = join_paths('./bin/', flavor)
   # For no flavors will just create ./bin/
   bin_file_dests[flavor] = path
   try:
      os.makedirs(path)
   except:
      # Ignore the exception that gets raised if the directory already exists.
      pass

#---------------------------------------------------------#
# Run PIL-Splitter                                        #
#---------------------------------------------------------#
pil_splitter_tool_name = 'pil-splitter.py'
# From contents.xml get the build in which tool is present
pil_splitter_build  = mi.get_tool_build(pil_splitter_tool_name)

# Run only if the tool is present
if pil_splitter_build:
   # Get the full path for tool
   pil_splitter = os.path.join(mi.get_build_path(pil_splitter_build),
                               mi.get_tool_path(pil_splitter_tool_name),
                               pil_splitter_tool_name)
   pil_splitter = pil_splitter.replace('\\', '/')
   lg.log("pil-splitter path: "+ pil_splitter)



   dest_dir  = '/app'
   if os.path.exists(pil_splitter):
      # Copy it into the meta-build, and run it from there.
      if not os.path.exists(dest_dir):
         os.mkdir(dest_dir)
      lg.log("Copying pil-splitter from host build.")
      shutil.copy(pil_splitter, dest_dir)

   # Always use local copy of pil_splitter
   pil_splitter = os.path.join (dest_dir, pil_splitter_tool_name)
   
   # Loop through each flavor and run the tool. Takes "None" when no flavor is present
   for flavor in flavors:
      # Pil-Splitter requires a source file, and target prefix as parameters
      # The target prefix can include path information.  Files in contents.xml
      # that need to be split will have a 'pil-split' attribute set to the
      # prefix to use for that file.  When we call get_file_vars, it will
      # return a dictionary mapping the prefix to the file to be split.
      # We must then prepend any path information to the prefix.
      pil_split_bins_dir = join_paths('bin', flavor, 'pil_split_bins')
      if flavor:
         lg.log('Processing the flavor ' + flavor)
      lg.log('pil_split_bins_dir: ' + pil_split_bins_dir)
      if not os.path.exists(pil_split_bins_dir):
         os.makedirs(pil_split_bins_dir)
      pil_splitter_files = mi.get_file_vars(attr = 'pil_split', flavor = flavor)
       
      for prefix in pil_splitter_files.keys():
         if not pil_splitter_files[prefix]:
            lg.log('Source file for prefix '+prefix+' is Null')
            sys.exit('Unable to call pil-splitter on prefix '+prefix)
         prefix_path = os.path.join(pil_split_bins_dir, prefix)
         # There should only be one source file per prefix
         pil_split_src = pil_splitter_files[prefix][0]
         lg.log('build.py:  Calling pil-splitter on ' + pil_split_src)
         ret_code_pil_spiltter = lg.log_exec(['python', pil_splitter, pil_split_src, prefix_path])
         if ret_code_pil_spiltter != 0:
             err_msg = 'build.py: '+'python'+' '+ pil_splitter+' '+ pil_split_src+' '+ prefix_path+ ':Failed'
             lg.log(err_msg)
             raise Exception(err_msg)


#---------------------------------------------------------#
# NON-HLOS Operations                                     #
#---------------------------------------------------------#
if options.non_hlos:
   #---------------------------------------------------------#
   # Write the FAT image                                     #
   #---------------------------------------------------------#
   lg.log("build.py: Writing FAT images")

   fat_file_src  = 'fat.bin'
   fatfiles = []
   # check for fat file exist or not.
   # if flavors exists, we pass one flavor element to mi.get_files() 
   # this is done just to avoid DataIntegrity exception
   # because we are only checking if there is atleast one file type element with 'fat_file' attribute
   fatfiles = mi.get_files(attr = 'fat_file', flavor = flavors[0] )
   if len(fatfiles) > 0:
      fatbin_exists = False
      fatadd_build = None
      fatgen_build = None
      cpfatfs_build = None  
      try:
         #cheking if fat add tool exists or not
         fatadd_build  = mi.get_tool_build('fatadd')
         fatadd_path   = mi.get_build_path(fatadd_build)
         fatadd_path  += mi.get_tool_path('fatadd')
         fatadd_tool  =  fatadd_path + 'fatadd.py'
         temp_path = os.path.abspath( os.path.join( fatadd_path , 'fat.bin' ) )
         lg.log("fat.bin location: " + temp_path)
         if os.path.exists( temp_path ):
            fatbin_exists = True
      except:
         pass

      try:
         #checking if fatgen tool exists or not
         fatgen_build  = mi.get_tool_build('fatgen')
         fatgen_path   = mi.get_build_path( fatgen_build )
         fatgen_path  += mi.get_tool_path('fatgen')
         fatgen_tool  =  os.path.join(fatgen_path, 'fatgen.py')
         lg.log("fatgen tool location: " + fatgen_tool)
      except:
         pass 

      try:
         #checking if cpfatfs tool exists or not
         cpfatfs_build  = mi.get_tool_build('cpfatfs')
         if cpfatfs_build:
            cpfatfs_path   = mi.get_build_path(cpfatfs_build)
            cpfatfs_path  += mi.get_tool_path('cpfatfs')
            cpfatfs_tool   = cpfatfs_path + 'cpfatfs'
            temp_path = os.path.abspath( os.path.join( cpfatfs_path , 'fat.bin' ) )
            lg.log("cpfatfs tool location: " + temp_path)
            if os.path.exists( temp_path ):
               fatbin_exists = True
            if not on_linux:
               cpfatfs_tool += '.exe'
      except:
         pass 
  
      if(fatgen_build or fatbin_exists) and (fatadd_build or cpfatfs_build):
         if (fatadd_build):
            use_fat_add ()
         elif (cpfatfs_build):
            use_cpfatfs ()  
         else:
            lg.log('Warning: No tool found to add component binaries to NON-HLOS.bin.')
      else:
         lg.log('Warning: Unable to generate / process NON-HLOS.bin.')
      
      
   #---------------------------------------------------------#
   # Encode NON-HLOS.build                                   #
   #---------------------------------------------------------#
   try :
      ptool = mi.get_tool_build('enc_img')
   except :
      pass
      ptool = ''
   if ptool:  # Returns a build
      run_tool = True
      # Get the root path to the build
      enc_img_build_root = mi.get_build_path('tz')
      # Get rel path to tool
      enc_img_tool_path = mi.get_tool_path('enc_img')
      # Start processing parameters
      params = mi.get_tool_params('enc_img')
      for i in range(0, len(params)):
         if params[i].startswith("--tools_dir="):
            params[i] = params[i] % enc_img_build_root
      
         elif params[i].startswith("--keys="):
            key_dir = params[i].split('=')[1]
            if not key_dir:
               run_tool = False
   
      if run_tool:
         lg.log("Running Encoder on NON-HLOS.bin")
         cmd = ['python', os.path.join(enc_img_build_root, enc_img_tool_path, "enc_img.py"), "--ip_file=NON-HLOS.bin", "--op_file=NON-HLOS.enc"] + params
         lg.log_exec(cmd)
   
   #---------------------------------------------------------#
   # Process partition.xml                                   #
   # Only run ptool at build time.  msp is run at download   #
   # time.                                                   #
   #---------------------------------------------------------#

   lg.log("update_common_info.py: Processing partition.xml",verbose=0)
   # Find the tools
   ptool_build  = mi.get_tool_build('ptool')
   if ptool_build:
      ptool_path   = mi.get_build_path(ptool_build)
      ptool_path  += mi.get_tool_path('ptool')
      ptool_path   = os.path.abspath(ptool_path)
      ptool_tool   = ptool_path + '/ptool.py'
      ptool_params = mi.get_tool_params('ptool')

      # Define our file names
      partition_xml   = '../config/partition.xml'
      rawprogram0_xml = 'rawprogram0.xml'

      if ptool_params:
         for i in range(len(ptool_params)):
            ptool_params[i] = ptool_params[i] % partition_xml   # Replace %s with path\name of partition.xml file
            ptool_params[i] = ptool_params[i].split()             # Split the command line into a list of arguments
      else:
         ptool_params = [['-x', partition_xml]]

      # Execute the commands
      for params in ptool_params:
         lg.log('build.py' + ' ' +  'ptool.py'+ ' ' + partition_xml + ' ' + 'gpt',verbose=0)
         ret_code_ptool = lg.log_exec(['python', ptool_tool] + params,verbose=0)
         if ret_code_ptool != 0:
             paramString = ''
             for param in params:
                 paramString += param + ' '
             err_msg = 'update_comon_info:' + 'python'+' '+ptool_tool+' ' + paramString+':Failed'
             lg.log(err_msg)
             raise Exception(err_msg)

   nand_build  = mi.get_tool_build('nand_mbn_generator')
   if nand_build:
      # Define our file names
      partition_xml   = 'partition_nand.xml'
      rawprogram0_xml = 'rawprogram0.xml'
      partition_mbn = 'partition.mbn'
      nand_path   = mi.get_build_path(nand_build)
      nand_path  += mi.get_tool_path('nand_mbn_generator')
      nand_path   = os.path.abspath(nand_path)
      if os.path.exists(nand_path):
         nand_tool   = nand_path + '/nand_mbn_generator.py'
         nand_partition_xml=nand_path+'/partition_nand.xml'
         lg.log(nand_partition_xml)
         lg.log('build.py:' + ' ' +  'nand_mbn_generator.py'+ ' ' + partition_xml + ' ' + 'gpt',verbose=0)
         lg.log_exec(['python', nand_tool,partition_xml,partition_mbn],verbose=0)

if options.hlos:

#---------------------------------------------------------#
# Run checksparse, if available.                          #
#---------------------------------------------------------#

   start_dir = os.getcwd()
   rawprogram_unsparse_xml = 'rawprogram_unsparse.xml'
   raw_program_xml = os.path.join(start_dir, 'rawprogram0.xml')
   if os.path.exists(raw_program_xml):
      checksparse_build  = mi.get_tool_build('checksparse')
      if checksparse_build:
         for flavor in bin_file_dests:
            sparse_image_dest = os.path.join(bin_file_dests[flavor], 'sparse_images')
            sparse_image_info = mi.get_file_vars(attr='sparse_image_path')
            if sparse_image_info and len(sparse_image_info['true']):
               # There should only be one file, and the attr value is 'true'
               sparse_image_path = os.path.dirname(sparse_image_info['true'][0])
               if os.path.isdir(sparse_image_path):
                  checksparse_path   = mi.get_build_path(checksparse_build)
                  checksparse_path  += mi.get_tool_path('checksparse')
                  checksparse_path   = os.path.abspath(checksparse_path)
                  checksparse_tool   = checksparse_path + '/checksparse.py'
               if not os.path.exists(sparse_image_dest):
                  os.makedirs(sparse_image_dest,mode = 0755)
               os.chdir(sparse_image_dest)
               lg.log("build.py: Executing checksparse tool",verbose=0)
               ret_code_checksparse = lg.log_exec(['python', checksparse_tool,'-i',raw_program_xml,'-s',sparse_image_path,'-o',rawprogram_unsparse_xml],verbose=0)
               if ret_code_checksparse != 0:
                    err_msg = 'python'+' '+ checksparse_tool+' '+'-i'+' '+raw_program_xml+' '+'-s'+' '+sparse_image_path+' '+'-o'+' '+rawprogram_unsparse_xml+':Failed'
                    lg.log(err_msg)
                    raise Exception(err_msg)
               os.chdir(start_dir)
      else:
         lg.log("build.py:  Checksparse tool does not exist. Could not create sparse images.")
   else:
      lg.log("build.py:  " + raw_program_xml + " path does not exist. Could not create sparse images.")


#-------------------------------------------------------------------------#
# WORKFLOW                                                                #
#-------------------------------------------------------------------------#

# Get the workflow block from contents.xml
workflow_elem = mi.find_root_element('workflow')

# Define a dictionary mapping for type of steps in workflow
run_step = {
   'copy'  : workflow_copy,
   'exec'  : workflow_exec,
   'delete': workflow_delete,
   'setenv': workflow_setenv
}

# Loop through each step in the workflow if found
if workflow_elem is not None:
   for step_elem in workflow_elem:
        if step_elem.tag == "step":
            # Process only 'step's
            lg.log(et.tostring(step_elem))
            # Each step should have a type
            if 'type' in step_elem.attrib:
                if ('platform' not in step_elem.attrib) or sys.platform.startswith(step_elem.attrib['platform']):
                    if ('filter' not in step_elem.attrib) or (len(workflow_filter_list) == 0) or (step_elem.attrib['filter'] in workflow_filter_list):
                        step_dict = ml.XmlToDict(step_elem)
                        step_func = run_step[step_elem.attrib['type']]
                        step_func(step_dict)
                else:
                    lg.log("Skipping step since it doesn't match current platform")
            else:
                lg.log("Skipping step since there's no 'type' specified") 


#---------------------------------------------------------#
# Creating gen_buildflavor.cmm file                       #
#---------------------------------------------------------#
if options.cmm:
   create_gen_build_flavor_cmm()

#---------------------------------------------------------#
# Call function to validate file paths in contents.xml
#---------------------------------------------------------#
if not options.ignore_missing_files:
   mi = ml.meta_info(logger=lg)
   lg.log_highlight("update_common_info.PY - VALIDATING FILE PATHS")
   try:
      um.validate_filepaths(mi, lg)
   except ml.InvalidFilePathException as ex:
      lg.log("update_common_info.py:InvalidFilePathException exception!!!, invalid file paths found in contents.xml ,count of invalid file paths = " + str(len(ex.invalid_files_list)))
      for invalidfile in ex.invalid_files_list:
         lg.log("update_common_info.py:Invalid file-path: " + invalidfile)
      lg.log( "update_common_info.py:InvalidFilePathException occurred")  # exit the program
      sys.exit(1)
   except ml.InvalidFileElementTypeException as ex:
      lg.log("update_common_info.py:InvalidFileElementTypeException exception!!!, please change the file type of below files to 'download_type' in contents.xml= " + str(len(ex.list_not_with_download_file_types)))      
      for invalidfile in ex.list_not_with_download_file_types:
        lg.log("update_common_info.py:Invalid file-element-type: " + invalidfile +" Change this file type to 'download_type'")
      lg.log( "update_common_info.py:InvalidFileElementTypeException occurred")  # exit the program
      sys.exit(1)        
   lg.log("============== CHECK COMPLETE ===============\n")

lg.log("update_common_info.py:============ UPDATE COMMON INFO COMPLETE====================",verbose=0)
