'''
===============================================================================
 update_meta.py: Update Meta information

 GENERAL DESCRIPTION
    This is an internal script used by build.py and other tools to parse the 
    build pairs for meta_info, generate goto scripts and validate file paths 
    in contents.xml

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/build.glue/1.0/lib/update_meta.py#19 $
  $DateTime: 2018/03/28 19:05:37 $

===============================================================================
'''

import sys
import os
import os.path
import meta_lib as ml
from glob import glob

def update_meta (lg_obj, pairs, fb_nearest=False):
   #---------------------------------------------------------#
   # Print some diagnostic info to the log                   #
   #---------------------------------------------------------#
   import meta_lib as ml
   lg_obj.log("update_meta.py: Platform is " + sys.platform)
   lg_obj.log("update_meta.py: Python Version is " + sys.version)
   lg_obj.log("update_meta.py: Current directory is " + os.getcwd())
   lg_obj.log("update_meta.py: Parameters are ")
   for pair in pairs:
      lg_obj.log("update_meta.py: " + "   " + pair.strip())
   
   #---------------------------------------------------------#
   # Update/Create the Meta-Info file                        #
   #---------------------------------------------------------#

   mi = ml.meta_info(logger=lg_obj, fb_nearest=fb_nearest)
   lg_obj.log_highlight("UPDATE META - Processing Parameters")
   lg_obj.log("update_meta.py: Updating/Creating Meta-Info file")
   builds = []  # Save this info for later
   cleanallvars = False    
   for p in pairs:
      lg_obj.log("update_meta.py: Processing " + p)
      if ('=' in p):
         var, val = p.strip().split('=')
         if "--flavors" in var:  # Flavor Filtering
            selected_pf_list = val.strip().split(',')
            mi.filter_product_flavors(selected_pf_list)
         elif "--wflow_filter" in var:  # Workflow Filters
            selected_steps_list = val.strip().split(',')
            mi.remove_workflow_steps(selected_steps_list)
         elif "--remove_images" in var:  # Workflow Filters
            remove_image_list = val.strip().split(',')
            mi.remove_images(remove_image_list)			
         elif "--cleanallvars" in var:  # Check content vars need to be clean-up
            if val.lower() == 'true' :
               cleanallvars = True
         else:  # Var:Value Pairs            
            mi.update_var_values(var, val)
      elif (':' in p):
         tag, loc = p.strip().split(':', 1)
         if tag_not_there(tag, mi):  # Unconfigured build 'tag'
            lg_obj.log_error('Tag "' + tag + '" not found')
            return 2  # EC 2
         builds.append(tag)  # Save valid build tags for later
         if mi.update_build_info(tag, loc):  # Invalid build ID/loc
            lg_obj.log_error('Build "' + loc + '" not found')
            return 3  # EC 3
         if tag == 'common' :
            lg_obj.log("update_meta.py: Updating ProductLine using " + p + " build info")
            mi.update_pl_from_buildid(loc)
      else:
         lg_obj.log_error('Please rerun with the format: modem:M8960AAAAAGAAM0000 var=value')
         return 1  # EC 1
   if cleanallvars :
      mi.clean_content_vars()
   mi.save()
 
   #---------------------------------------------------------#
   # Create goto scripts                                     #
   #---------------------------------------------------------#
   
   lg_obj.log_highlight("UPDATE META - Creating goto scripts")

   # Define template for goto scripts
   goto_template = '''#! /usr/bin/python

import sys
import os
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), 'common/build/lib'))
import meta_lib as ml
mi = ml.meta_info()
path = mi.get_build_path('%s')

on_linux = sys.platform.startswith('linux')

if on_linux:
   if path is not None:
      try:
         subprocess.Popen(['gnome-open', path]).wait()
      except:
         try:
            subprocess.Popen(['xdg-open', path]).wait()
         except:
            ErrorMessageBox("Command not supported by Linux. Only Gnome/xdg are supported")   
   else:
      ErrorMessageBox('goto_%s.py', 'Cannot find path for %s.')

else:
   if path is not None:
      subprocess.Popen(['start', path], shell=True).wait()
   else:
      ErrorMessageBox('goto_%s.py', 'Cannot find path for %s.')

'''

   for tag in mi.get_build_list():
      # Don't process the current build
      if tag != 'common':
         path = mi.get_build_path(tag)
         image_dir = mi.get_image_dir(tag)
         # Don't process builds that don't have image directories
         # (This tests for builds that haven't been updated)
         lg_obj.log("update_meta.py: goto " + tag);
         if path and image_dir and os.path.exists(os.path.join(path, image_dir)):
            goto_script = open(os.path.join(os.path.dirname(__file__), '../../../goto_' + tag + '.py'), 'w')
            goto_script.write(goto_template % (tag, tag, tag, tag, tag))
            goto_script.close()
   
   lg_obj.log_highlight("UPDATE META COMPLETE")
   return 0


def validate_filepaths(mi, lg_obj):
   '''
   This function is the entry point to test the validity of files mentioned in contents.xml in the elements like <download_file> , <file_ref>, <dload_boot_image>
   '''

   lg_obj.log_underline("update_meta: Verifying mandatory partition files against contents.xml")
   list_invalid_files = []  # to save files which are not found
   list_ignored_partition_files = []  # to save files in partition.xml listed in contents with 'ignore'
   list_not_with_download_file_types = []   # to save files in partition.xml listed in contents not with 'download_file' type
   master_partition_files = []   
   
   # Loop through each flavor
   #-------------------------
   flavors = mi.get_product_flavors()
   if len(flavors) == 0:
      # this product does not have flavors,adding None as a flavor.   
      flavors.append(None)   
   for flavor in flavors:
      if flavor != None:   
         lg_obj.log("\nupdate_meta.py: FLAVOR - " + flavor.upper())
      # locate partition.xml file(s)
      # contents.xml has partition_file and partition_patch_file listed, which are rawprogram0 and patch0 respectively
      # ignoring patch files for now
      # getting file vars with storage type to see if we need to check for each type separately
      partition_file_vars = mi.get_file_vars(file_types=['partition_file'], flavor=flavor, attr="storage_type")
      if (len(partition_file_vars) == 0):
         # this product does not have storage types, take all files in single list
         partition_file_vars[None] = mi.get_files(file_types=['partition_file'], flavor=flavor)
      # Optional partition file list, which defined partition XML, but will not be part of SI/meta build. 
      parti_info = mi.get_contents_parti_info_var(attr="skip_loading")
      skip_loading_file_list = [attr_val for partition, attr_val in parti_info.iteritems()]
      lg_obj.log('Skipping partition file validation :' + str(skip_loading_file_list))
      # Now loop through each partition file by storage type
      #-----------------------------------------------------
      master_contents_flavor_download_type_files = []
      for st_type in partition_file_vars:
         for pfile in partition_file_vars[st_type]:
            # get file names list
            parsed_files_set = set(ml.get_filenames_from_partitionfile(pfile)) - set(skip_loading_file_list)
            master_partition_files = master_partition_files + list(parsed_files_set)
            lg_obj.log("------------------------------------------------------------")
            lg_obj.log(pfile)
            lg_obj.log("update_meta.py: " + str(len(parsed_files_set)) + " files to check")
            # Now verify all these files are listed in contents.xml
            #------------------------------------------------------
            if st_type:
               lg_obj.log("   STORAGE TYPE = " + st_type)
            contents_flavor_files = mi.get_files(flavor=flavor, storage=st_type)
            contents_flavor_files = [os.path.basename(p) for p in contents_flavor_files]  # keep only filename
            contents_flavor_ignored_files = mi.get_files(flavor=flavor, attr="ignore", storage=st_type)
            contents_flavor_ignored_files = [os.path.basename(p) for p in contents_flavor_ignored_files]
			#get File list with <download_file> tag
            contents_flavor_download_type_files = mi.get_files(file_types=['download_file'],flavor=flavor, storage=st_type)
            contents_flavor_download_type_files = [os.path.basename(p) for p in contents_flavor_download_type_files]
            for filename in parsed_files_set:
               if filename not in contents_flavor_files:
                  if flavor != None:			   
                     lg_obj.log("   *** ERROR: " + filename + " is NOT listed in contents.xml for " + flavor + " flavor.")
                  else:
                     lg_obj.log("   *** ERROR: " + filename + " is NOT listed in contents.xml  ")				  
                  list_invalid_files.append(filename)
               elif filename in contents_flavor_ignored_files:
                  lg_obj.log("   *** WARN: " + filename + " is listed in contents.xml BUT with 'ignore' attribute")
                  # list_invalid_files.append(filename)         # changint this to warning
                  list_ignored_partition_files.append(filename)  # will fail when actual file is missing
               if filename not in contents_flavor_download_type_files:
                  lg_obj.log("   *** ERROR: " + filename + " please update the file element type of " + filename + " as download_file")		
                  list_not_with_download_file_types.append(filename)			  	  
            lg_obj.log("   Done")
   
   master_contents_flavor_download_type_files = mi.get_files(file_types=['download_file'],attr='!(((fastboot)||(fastboot_complete))||(gpt_file))')
   master_contents_flavor_download_type_files = [os.path.basename(p) for p in master_contents_flavor_download_type_files]
   lg_obj.log("\t _________________ ______________________________________________________________")
   lg_obj.log("\t||         ||     ||       || _____|| ||      ||  ||  ||      || ||----------|| ")
   lg_obj.log("\t||         ||    || ||     ||      || || |    ||  ||  || |    || ||             ")
   lg_obj.log("\t||   ||    ||   ||   ||    ||______|| ||   |  ||  ||  ||  |   || ||     _ _ _   ")
   lg_obj.log("\t||   ||    ||  ||_____||   ||||       ||    | ||  ||  ||   |  || ||    ||    || ")
   lg_obj.log("\t||   ||    || ||       ||  ||  ||     ||     |||  ||  ||    | || ||    ||    || ")
   lg_obj.log("\t||___||____||||         || ||     ||  ||      ||  ||  ||     ||| ||_ _ ||    || ")  
   lg_obj.log("\t _________________ ______________________________________________________________\n")   
   for file in set(master_contents_flavor_download_type_files):
       if file not in master_partition_files and (not file.startswith('gpt_')):
          lg_obj.log(" *** WARN:Please change the file element type of "+ file + " from 'download_file' to 'file_ref' to increase QFIL performance")
   lg_obj.log("\t _________________ _______________________________________________________________")
   lg_obj.log("\t||         ||     ||       || _____|| ||      ||  ||  ||      || ||----------|| ")
   lg_obj.log("\t||         ||    || ||     ||      || || |    ||  ||  || |    || ||             ")
   lg_obj.log("\t||   ||    ||   ||   ||    ||______|| ||   |  ||  ||  ||  |   || ||     _ _ _   ")
   lg_obj.log("\t||   ||    ||  ||_____||   ||||       ||    | ||  ||  ||   |  || ||    ||    || ")
   lg_obj.log("\t||   ||    || ||       ||  ||  ||     ||     |||  ||  ||    | || ||    ||    || ")
   lg_obj.log("\t||___||____||||         || ||     ||  ||      ||  ||  ||     ||| ||_ _ ||    || ")  
   lg_obj.log("\t _________________ ______________________________________________________________\n")  
   lg_obj.log("\t Please review above warnings")  
      

   if(len(list_invalid_files) > 0):
      lg_obj.log_error("File list validation check failed for mandatory files")
      raise ml.InvalidFilePathException(list_invalid_files)
   lg_obj.log("------------------------------------------------------------\n")   
   
   if (len(list_not_with_download_file_types) > 0):
      list_not_with_download_file_types = list(set(list_not_with_download_file_types))
      lg_obj.log("**ERROR**:Found partitions files with different file type that 'download_type'\n")
      raise ml.InvalidFileElementTypeException(list_not_with_download_file_types)
   lg_obj.log("------------------------------------------------------------\n")   
      
   lg_obj.log_underline("update_meta.py: Validating File paths in contents.xml")
   master_file_list = mi.get_files(expand_wildcards=False, invalid_path=True)  # get the list of all files
   ignored_file_list = mi.get_files(attr='ignore', expand_wildcards=False, invalid_path=True)  # get the list of files that have ignore attribute

   lg_obj.log("update_meta.py: Total number of file paths found in contents.xml = " + str(len(master_file_list)))
   
   # remove all critical files from ignored_file_list
   ignored_file_list = [x for x in ignored_file_list if os.path.basename(x) not in list_ignored_partition_files]
   
   # remove ignored files from the master_file_list
   file_set_to_check = set(master_file_list) - set(ignored_file_list)  # convert lists to sets and subtract to remove ignored files 
      
   # iterate through the file_set_to_check and process each file
   for filepath in file_set_to_check:
      # as the file path may have a wild character, check using glob if there is a least one file which matches the pattern specified by the filepath
      matching_file_list = glob(filepath)
      if not matching_file_list:
         list_invalid_files.append(filepath)  # no match for filename pattern: treating this as invalid file, add path to list of invalid files
         lg_obj.log("   *** ERROR: " + filepath + " is NOT listed in contents.xml  ")
   # done processing all files

   # if the list of invalid files has atleast one entry, raise an exception
   if(len(list_invalid_files) > 0):
      raise ml.InvalidFilePathException(list_invalid_files)
   else:
      lg_obj.log("update_meta.py: file path validation passed.")

# end of function validate_filepaths()


def tag_not_there(tag, mi):
   for t in mi.get_build_list():
      if t == tag:
         return False
   return True
