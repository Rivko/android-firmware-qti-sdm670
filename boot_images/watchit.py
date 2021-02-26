#!/usr/bin/env python


#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 01/30/18   c_vkuppa Update build file hash
# 08/03/16   vk      Update build file hash
# 08/03/16   yps     Update build file hash
# 06/22/16   bh      Update build file hash
# 01/12/16   et      Update build file hash
# 10/21/15   ck      Added one comdef.h check
# 09/30/15   ck      Added cross target logic
# 09/29/15   ck      Added warning as error logic
# 09/24/15   ck      Added logic to print error count after successful copyright check
# 09/22/15   ck      Added Qcom Copyright feature
# 09/10/15   ck      Added buildit.py protection
# 09/08/15   ck      Added missing CR LF removal logic from self hash check
# 08/26/15   ck      Added logic to create temp file without CR or LF when checking hash
# 08/24/15   ck      Initial release


from optparse import OptionParser
import shutil
import os
import sys
import locale
import stat
import platform
import subprocess
import tempfile


GENERAL_ERROR_CODE = 1
WATCHIT_SELF_CHECK_ERROR_CODE = 2
BUILD_SCRIPTS_CHECK_ERROR_CODE = 3
CROSS_TARGET_CHECK_ERROR_CODE = 4
WARNING_AS_ERROR_CHECK_ERROR_CODE = 5
DAL_TARGET_CHECK_ERROR_CODE = 6
TARGET_DRIVER_REFERENCE_COMMON_CODE_ERROR_CODE = 7
TARGET_SPECIFIC_COMMON_CODE_ERROR_CODE = 8
GENERATE_WATCHIT_HASH_VALUE_ERROR_CODE = 9
QCOM_COPYRIGHT_CHECK_ERROR_CODE = 10
ONE_COMDEF_CHECK_ERROR_CODE = 11
FOLDER_FILES_CHECK_ERROR_CODE = 12


# build_files_sha1 structure, <File Name>, <Hash>, <Mandatory Existance>
build_files_sha1=[["buildex.py", "3199c1584d9e2695b87c08b6358aab7e52da7d4d", True]]

if (platform.system().lower() == "linux"):
  # folder_files_sha structure, <Folder Name>, <Hash>, <Mandatory Existance>
  folder_files_sha=[["boot_images/QcomPkg/Include/Protocol", "ac4fe6bb729085db6ba55976892e04ad182c205b", True]]
else:
  # folder_files_sha structure, <Folder Name>, <Hash>, <Mandatory Existance>
  folder_files_sha=[["boot_images/QcomPkg/Include/Protocol", "df9f63d29f64b5fcddeb16b053fe2137e61cc7b6", True]]

# Target prefixes.  Folders that start with these strings are possible targets.
target_prefixes=["Msm", "Mdm", "Qdf", "BaseTarget"]

# Target suffixes. Folders that begin with a target prefix and end with one of
# these suffixes denotes a target folder.
target_suffixes=["Pkg"]


##############################################################################
# main
##############################################################################
def main():

  parser = OptionParser()

  parser.add_option("-a", "--all",
                    action="store_true",
                    dest="enable_all_features",
                    default=False,
                    help="Enables all features except self check hash.  " + \
                         "That must be enabled on its own.  " + \
                         "Default is Off")

  parser.add_option("-b", "--build-hash-check",
                    action="store_true",
                    dest="enable_build_hash_check",
                    default=False,
                    help="Enables hash check of build files.  " + \
                         "Default is Off")

  parser.add_option("-f", "--folder-hash-check",
					action="store_true",
 	 		        dest="enable_Folder_hash_check",
 	 		        default=False,
 	 		        help="Checks for protocol update.  " + \
 	 		             "Default is Off")  

  parser.add_option("-c", "--cross-target-check",
                    action="store_true",
                    dest="enable_cross_target_check",
                    default=False,
                    help="Enables check to ensure targets are not referencing " + \
                         "code from other targets.  " + \
                         "Default is Off")

  parser.add_option("-d", "--dal-target-check",
                    action="store_true",
                    dest="enable_dal_target_check",
                    default=False,
                    help="Enables check to ensure DAL config files are set " + \
                         "to proper target.  " + \
                         "Default is Off")

  parser.add_option("-e", "--eval-dir",
                    action="store",
                    type="string",
                    dest="evaluation_dir",
                    default="boot_images/QcomPkg",
                    help="Base directory of where to start evaluation.  " + \
                         "Default is boot_images/QcomPkg")

  parser.add_option("-g", "--generate-watchit-hash-value",
                    action="store_true",
                    dest="enable_generate_watchit_hash_value",
                    default=False,
                    help="Displays this file's hash value.  " + \
                         "Default is Off")

  parser.add_option("-o", "--one-comdef",
                    action="store_true",
                    dest="enable_one_comdef_check",
                    default=False,
                    help="Ensure there is only one comdef.h file in branch.  " + \
                         "Default is Off")

  parser.add_option("-q", "--qcom-copyright-dir",
                    action="store",
                    type="string",
                    dest="qcom_copyright_dir",
                    default="",
                    help="Enables the Qcom Copyright scan option and starts " + \
                         "evaluating from directory specified.  Evaluation " + \
                         "is recursive.")

  parser.add_option("--qcom-copyright-threshold",
                    action="store",
                    type="string",
                    dest="qcom_copyright_threshold",
                    default="0",
                    help="Number of Qcom Copyright errors allowed before fault " + \
                         "is triggered.")

  parser.add_option("-s", "--self-hash-check",
                    action="store",
                    type="string",
                    dest="watchit_hash_value",
                    help="Enables watchit self check.  Hash value supplied " + \
                         "is compared against this file's hash.")

  parser.add_option("-t", "--target-drivers-check",
                    action="store_true",
                    dest="enable_target_drivers_check",
                    default=False,
                    help="Enables target driver schema check.  " + \
                         "Default is Off")

  parser.add_option("-u", "--update-build-hash-values",
                    action="store_true",
                    dest="enable_update_build_hash_values",
                    default=False,
                    help="Automatically updates watchit's build file hash values." + \
                         "Default is Off")

  parser.add_option("-w", "--warning-as-error-disable-check",
                    action="store_true",
                    dest="enable_warning_as_error_check",
                    default=False,
                    help="Enables check to ensure warning as error is not " + \
                         "disabled in any INF file.  " + \
                         "Default is Off")

  (options, args) = parser.parse_args()


  # All defaults need to be OS agnostic.
  options.evaluation_dir = convert_directory_seperators(options.evaluation_dir)

  # Assign openssl_path based on operating system
  try:
    openssl_path = determine_openssl_path(options.evaluation_dir)
  except Exception as error:
    print error
    sys.exit(GENERAL_ERROR_CODE)


  # Generate and display the hash value of this file
  if (options.enable_generate_watchit_hash_value):
    try:
      print "WatchIt hash value" + ("." * 4),
      result = generate_watchit_hash_value(openssl_path)
      print result
    except Exception as error:
      print error
      sys.exit(GENERATE_WATCHIT_HASH_VALUE_ERROR_CODE)


  # Check hash of this watchit.py file.
  if (options.watchit_hash_value):
    try:
      print "Executing watchit self check" + ("." * 27),
      watchit_self_check(openssl_path,
                         options.watchit_hash_value)
      print "PASSED"
    except Exception as error:
      print error
      sys.exit(WATCHIT_SELF_CHECK_ERROR_CODE)


  # Check the hash of the build script files to ensure 
  if (options.enable_all_features or options.enable_build_hash_check):
    try:
      print "Executing build scripts check" + ("." * 26),
      build_scripts_check(options.evaluation_dir,
                          openssl_path)
      print "PASSED"
    except Exception as error:
      print error
      sys.exit(BUILD_SCRIPTS_CHECK_ERROR_CODE)

  # Check the hash of the build script files to ensure
  if (options.enable_all_features or options.enable_Folder_hash_check):
      try:
        print "Executing folder check : Checking if made updates any in intended folder" + ("." * 12),
        folder_files_check(openssl_path)
        print "PASSED"
      except Exception as error:
        print error
        sys.exit(FOLDER_FILES_CHECK_ERROR_CODE)

  # Check that targets are not referencing code from other targets
  if (options.enable_all_features or options.enable_cross_target_check):
    try:
      print "Executing cross target check" + ("." * 27),
      failure_list = cross_target_check(options.evaluation_dir)
    except Exception as error:
      print error
      sys.exit(CROSS_TARGET_CHECK_ERROR_CODE)

    if len(failure_list) == 0:
      print "PASSED"
    else:
      print "ERROR"
      for entry in failure_list:
        print "   File: " + entry[0] + "  Line:" + str(entry[1])
      sys.exit(CROSS_TARGET_CHECK_ERROR_CODE)


  # Check that warning as error is not turned off in any INF files
  if (options.enable_all_features or options.enable_warning_as_error_check):
    try:
      print "Executing warning as error disable check" + ("." * 15),
      failure_list = warning_as_error_check(options.evaluation_dir)
    except Exception as error:
      print error
      sys.exit(WARNING_AS_ERROR_CHECK_ERROR_CODE)

    if len(failure_list) == 0:
      print "PASSED"
    else:
      print "ERROR"
      for entry in failure_list:
        print "   File: " + entry[0] + "  Line:" + str(entry[1])
      sys.exit(WARNING_AS_ERROR_CHECK_ERROR_CODE)


  # Check that dal config files for a target are proper
  if (options.enable_all_features or options.enable_dal_target_check):
    try:
      print "Executing DAL target check" + ("." * 29),
      dal_target_check()
      print "PASSED"
    except Exception as error:
      print error
      sys.exit(DAL_TARGET_CHECK_ERROR_CODE)


  # Check target drivers
  if (options.enable_all_features or options.enable_target_drivers_check):
    try:
      print "Executing target driver reference common code check" + ("." * 4),
      target_driver_reference_common_code_check()
      print "PASSED"
    except Exception as error:
      print error
      sys.exit(TARGET_DRIVER_REFERENCE_COMMON_CODE_ERROR_CODE)

    try:
      print "Executing target specific common code check" + ("." * 12),
      target_specific_common_code_check()
      print "PASSED"
    except Exception as error:
      print error
      sys.exit(TARGET_SPECIFIC_COMMON_CODE_ERROR_CODE)


  # Check that only one comdef.h file exists
  if (options.enable_all_features or options.enable_one_comdef_check):
    try:
      print "Executing one comdef.h check" + ("." * 4),
      comdef_list = one_comdef_check(options.evaluation_dir)
    except Exception as error:
      print error
      sys.exit(ONE_COMDEF_CHECK_ERROR_CODE)

    if len(comdef_list) == 1:
      print "PASSED"
    elif len(comdef_list) < 1:
      print "ERROR"
      print "  comdef.h file could not be found"
      sys.exit(ONE_COMDEF_CHECK_ERROR_CODE)
    else:
      print "ERROR"
      for entry in comdef_list:
        print "  File: " + entry
      sys.exit(ONE_COMDEF_CHECK_ERROR_CODE)


  # Check Qcom Copyrights
  # Only execute copyright check if directory is given.  This is why it cannot
  # be executed with the -a (all) argument.
  if (len(options.qcom_copyright_dir)):
    try:
      print "Executing Qcom Copyright check starting from: " + \
            options.qcom_copyright_dir + ("." * 6),
      error_count = qcom_copyright_check(options.qcom_copyright_dir,
                                         options.qcom_copyright_threshold)
      print "PASSED (" + error_count + ")"
    except Exception as error:
      print error
      sys.exit(QCOM_COPYRIGHT_CHECK_ERROR_CODE)


  return


##############################################################################
# one_comdef_check
##############################################################################
# Make sure only one comdef.h file exists.
def one_comdef_check(evaluation_dir):

  comdef_file_locations = []

  for root, directories, files in os.walk(evaluation_dir):  
    for current_file in files:
      if current_file == 'comdef.h':
        comdef_file_locations.append(os.path.join(root,
                                                  current_file))

  return comdef_file_locations          


##############################################################################
# cross_target_check
##############################################################################
# Make sure a target does not have any pointers to another target.
# I.E. Referencing headers in 8996 from 8998.
def cross_target_check(evaluation_dir):

  failure_list = []
  target_list = []

  # Scan evaluation_dir for target directories.  When one is found investigate
  # all files inside of that folder for pointers to another target.
  directory_list = next(os.walk(evaluation_dir))[1]

  # Create a list of targets
  for current_directory in directory_list:
    if any(current_directory.startswith(prefix) for prefix in target_prefixes):
      if any(current_directory.endswith(suffix) for suffix in target_suffixes):
        target_list.append(current_directory)


  # Walk each target directory inspecting each file for references to
  # other targets
  for current_target in target_list:
    # Create a error target list which is target list minus target under test
    error_target_list = target_list
    error_target_list.remove(current_target)
    
    for root, directories, files in os.walk(os.path.join(evaluation_dir,
                                                         current_target)):
      for current_file in files:
        current_file = os.path.join(root, current_file)
        with open(current_file) as file:
          current_line = 0
          comment_block = False
          for line in file:
            current_line += 1
            # Determine if line contains a reference to a target
            if any(target in line for target in error_target_list):
              failure_list.append([current_file, current_line])                  
              
  return failure_list
               
               
##############################################################################
# warning_as_error_check
##############################################################################
# Ensure warnings as errors are not being disabled via INF and DSC files.
def warning_as_error_check(evaluation_dir):

  failure_list = []

  # Find all .dsc and .inf files
  for root, directories, files in os.walk(evaluation_dir):
    for current_file in files:
      if current_file.lower().endswith(".dsc") or \
         current_file.lower().endswith(".inf"):
        current_file = os.path.join(root, current_file)
        with open(current_file) as file:
          current_line = 0
          for line in file:
            current_line += 1
            # Determine if line contains "-Wno-error"
            if "-wno-error" in line.lower():
              failure_list.append([current_file, current_line])                  
              
  return failure_list


##############################################################################
# update_build_scripts_sha1_values
##############################################################################
# Updates watchit's internal build_script hash values
def update_build_scripts_sha1_values():
  return


##############################################################################
# build_scripts_check
##############################################################################
# Check digest of build scripts and compare against hardwired values.
# Reject if they do not match.
def build_scripts_check(evaluation_dir,
                        openssl_path):

  # Generate base directory of where build files exist
  base_directory = determine_boot_images_path(evaluation_dir)
  if not base_directory:
    raise NameError("ERROR: watchit::build_scripts_check: " + \
                    "Unable to determine base directory from " + \
                    "evaluation directory: " + evaluation_dir)
  base_directory = os.path.join(base_directory,
                                "QcomPkg")


  # Check that each build file exists and its digest is correct
  for current_entry in build_files_sha1:
    # Convert directory seperator to proper one for os.
    current_entry[0] = convert_directory_seperators(current_entry[0])

	# Check if file exists, if it doesn't and Mandatory existance field 
    # is true then error
    if (not os.path.isfile(os.path.join(base_directory,
                                        current_entry[0]))):
      if (current_entry[2]):
        raise NameError("ERROR: watchit::build_scripts_check: " + \
                        "File does not exist but marked Mandatory: " + \
                        os.path.join(base_directory, current_entry[0]))
      else:
        continue


    # Create temp file stripped of CR and LF
    temp_file = create_cr_lf_stripped_temp_file(os.path.join(base_directory,
                                                             current_entry[0]))
	
	
	# If an exception occurs delete the temp file before leaving function
    try:
      calculated_value = calculate_sha1(openssl_path,
                                        temp_file.name)
      #Uncomment to print hash for updating
      #print calculated_value
    except Exception as error:
      os.remove(temp_file.name)
      raise error

    # Delete the temp file
    os.remove(temp_file.name)

    # Check if hash value is present in output
    if not current_entry[1] == calculated_value:
      raise NameError("FAILED")


  return

##############################################################################
# folder_files_check
##############################################################################
# Check for updates in the protected folder
# Compare the folder hash and proceed
def folder_files_check(openssl_path):
  
  # Check for updates in the protected folder
  for current_entry in folder_files_sha:
    cwd = os.getcwd()
    basedir = os.path.join(cwd, current_entry[0])
    # Convert directory seperator to proper one for os.
    basedir = convert_directory_seperators(basedir)
    hash_file_path = os.path.join(basedir, "hash_file.txt")
    fo = open(hash_file_path,'w')
    for dir_file in os.listdir(basedir):
      if dir_file == "hash_file.txt" :
        print ""
      else :
        # Create temp file stripped of CR and LF
        temp_file = create_cr_lf_stripped_temp_file(os.path.join(basedir, dir_file))
    
        # If an exception occurs delete the temp file before leaving function
        try:
          file_hash_value = calculate_sha1(openssl_path,temp_file.name)
          #Uncomment to print hash for updating
          #print file_hash_value for each file
          #print file_hash_value
          fo.write(file_hash_value)
          fo.write("\r")
        except Exception as error:
          os.remove(temp_file.name)
          raise error
      
        # Delete the temp file 
        os.remove(temp_file.name)
        
    fo.close()        
    # Create temp file stripped of CR and LF
    hash_file = create_cr_lf_stripped_temp_file(hash_file_path)
    folder_hash_value = calculate_sha1(openssl_path,hash_file.name)
    #Uncomment to print hash for updating
    #print folder_hash_value for entire folder
    #print folder_hash_value
    
    # Check if hash value is present in output
    if not current_entry[1] == folder_hash_value:
      print 'Failure is due to update in' + current_entry[0]
      raise NameError("FAILED")

  return

##############################################################################
# dal_target_check
##############################################################################
# Evaluate DAL XML files for proper target
def dal_target_check():
  return


##############################################################################
# generate_watchit_hash_value
##############################################################################
def generate_watchit_hash_value(openssl_path):

 # Create temp file stripped of CR and LF
  temp_file = create_cr_lf_stripped_temp_file(os.path.realpath(__file__))


  # If an exception occurs delete the temp file before leaving function
  try:
    calculated_value = calculate_sha1(openssl_path,
                                      temp_file.name)
  except Exception as error:
    os.remove(temp_file.name)
    raise error


  # Delete the temp file
  os.remove(temp_file.name)

  return calculated_value


##############################################################################
# watchit_self_check
##############################################################################
# Watchit self hash check using user supplied hash value  
def watchit_self_check(openssl_path,
                       user_hash_value):

  # Create temp file stripped of CR and LF
  temp_file = create_cr_lf_stripped_temp_file(os.path.realpath(__file__))

  calculated_value = calculate_sha1(openssl_path,
                                    temp_file.name)

  # Check if user supplied hash matches
  if not user_hash_value == calculated_value:
    raise NameError("FAILED")


  return


##############################################################################
# target_driver_reference_common_code_check
##############################################################################
# Check target specific drivers for pulling in common code directly.
def target_driver_reference_common_code_check():
  return


##############################################################################
# target_specific_common_code_check
##############################################################################
# Check for target specific drivers in common folder
def target_specific_common_code_check():
  return


##############################################################################
# create_cr_lf_stripped_temp_file
##############################################################################
# Because of OS CR vs CRLF differences create a temp file that is
# stripped of CR and LF.
def create_cr_lf_stripped_temp_file(original_file):
  temp_file = tempfile.NamedTemporaryFile(delete=False)
  with open(original_file, "r") as file:
    for line in file:
      temp_file.write(line.replace("\n", "").replace("\r", ""))
  temp_file.flush()
  temp_file.close()

  return temp_file


##############################################################################
# calculate_sha1
##############################################################################
def calculate_sha1(openssl_path,
                   file_path):

  if not os.path.isfile(openssl_path):
    raise NameError("ERROR: watchit::calculate_sha1: " + \
                    "OpenSSL could not be found.")


  # Use openssl to calculate sha1 value of file
  try:
    openssl_result = subprocess.check_output([openssl_path,
                                              "dgst",
                                              "-sha1",
                                              file_path])
  except:
    raise NameError("ERROR: watchit::calculate_sha1: " + \
                    "OpenSSL returned an error.")


  # Result will have file name, equal size, then hash value
  # Make sure equal sign is in result
  if not "=" in openssl_result:
    raise NameError("ERROR: watchit::calculate_sha1: " + \
                    "OpenSSL result is not in proper format.")


  return openssl_result.split('=',1)[1].lstrip().rstrip()


##############################################################################
# convert_directory_seperators
##############################################################################
# Convert all directory seperators to proper one for executing OS
def convert_directory_seperators(path_to_convert):
  converted_path = path_to_convert.replace("/", os.sep)
  converted_path = converted_path.replace("\\", os.sep)

  return converted_path


##############################################################################
# determine_boot_images_path
##############################################################################
def determine_boot_images_path(evaluation_dir):

  # Make sure evaluation directory has a boot_images directory
  if not 'boot_images' in evaluation_dir:
    raise NameError("ERROR: watchit::determine_boot_images_path: " + \
                    "evaluation directory not in repository: " + \
                    evaluation_dir);


  return evaluation_dir.split('boot_images',1)[0] + 'boot_images'


##############################################################################
# determine_openssl_path
##############################################################################
def determine_openssl_path(evaluation_dir):

  # If executing on a windows machine set openssl_path to point to exe in
  # branch.  Linux assign to return value of "which".
  openssl_path = ""

  if (platform.system().lower() == "linux"):
    try:
      which_result = subprocess.check_output(["which",
                                              "openssl"]).lstrip().rstrip()
    except:
      raise NameError("ERROR: watchit::determine_openssl_path: " + \
                      "which returned an error.")

    if len(which_result):
      openssl_path = which_result
    else:
      raise NameError("ERROR: watchit::determine_openssl_path: " + \
                      "Unable to locate OpenSSL on system.")


  elif (platform.system().lower() == "windows"):
    # Obtain boot_images (base) path from evaluation_dir
    base_directory = determine_boot_images_path(evaluation_dir)
    if not base_directory:
      raise NameError("ERROR: watchit::determine_openssl_path: " + \
                      "Unable to determine base directory from " + \
                      "evaluation directory: " + evaluation_dir)

    openssl_path = os.path.join(base_directory,
                                "QcomPkg",
                                "Tools",
                                "sectools",
                                "bin",
                                "WIN",
                                "openssl.exe")
  else:
    # OpenSSL could not be found, raise error
    raise NameError("ERROR: watchit::determine_openssl_path: " + \
                    "Unable to locate OpenSSL on system.")


  return openssl_path


##############################################################################
# qcom_copyright_check
##############################################################################
def qcom_copyright_check(qcom_copyright_dir,
                         qcom_copyright_threshold):

  # Make sure starting directory exists
  if not os.path.isdir(qcom_copyright_dir):
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "Directory specified does not exist: " + \
                    qcom_copyright_dir)


  # Ensure threshold is greater than or equal to 0
  if qcom_copyright_threshold < 0:
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "Threshold supplied is invalid: " + \
                    qcom_copyright_threshold)


  # Watchit is closer to the base than the copyright script so it is ok
  # to start searching for the copyright script from here.
  script_path = ""
  base_directory = os.path.dirname(os.path.realpath(__file__))
  for root, directories, files in os.walk(base_directory):  
    for current_file in files:
      if current_file == 'QcomCopyright.py':
        script_path = os.path.join(root,
                                   'QcomCopyright.py')

  if not len(script_path):
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "Unable to locate QcomCopyright.py script")


  # Execute copyright script with appropriate arguments and collect output
  try:
    # Absolute path must be given as execution location of this script and
    # copyright script are different
    script_result = subprocess.check_output(["python",
                                             script_path,
                                             "-s",
                                             "-c",
                                             "-d" + os.path.abspath(qcom_copyright_dir),
                                             "-r"])
  except:
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "QcomCopyright.py returned an error.")
    
    
  # Result will have "Total errors: " and then value
  # Make sure line is in the output
  if not "Total errors:" in script_result:
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "QcomCopyright.py result is not in proper format.")
    
    
  total_errors = script_result.split('Total errors:',1)[1].lstrip().rstrip()


  # Compare total_errors against threshold
  if total_errors > qcom_copyright_threshold:
    raise NameError("ERROR: watchit::qcom_copyright_check: " + \
                    "Total errors (" + total_errors + ") exceeds " + \
                    "threshold of (" + qcom_copyright_threshold + ")")

  return total_errors
  

main()
