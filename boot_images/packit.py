#!/usr/bin/env python


#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 05/17/17   c_vkuppa Copying the manifest.xml file to default path
# 05/16/17   vk      Add SDM support
# 03/27/17   dhaval  Added new commons.py XBLConfig tool packing
# 03/17/17   yps     Added support for packing XBLConfig tools  
# 12/30/16   yps     Added support for packing buildex.py
# 07/12/16   bh      Added support for cleaning generated files after building
# 02/18/16   et      Added support for packing multiple targets
# 01/20/16   et      Added QcomAppPkg to exceptions
# 01/14/16   et      Added buildit.py and QcomModulePkg to exceptions
# 10/14/15   bh      Change command line arg to variant, make LZB default pack
# 08/07/15   ck      Added manifest.xml to release_exceptions
# 07/29/15   ck      Added support for configurable binary destination folder
# 07/07/15   bh      Fix merge issue in script
# 07/02/15   vk      Add 32 bit build script to exceptions
# 06/25/15   bh      Add LA build scripts to release list
# 06/19/15   bh      Add support for variant-specific packing
# 06/03/15   ck      Fixed OS agnostic bugs
# 05/27/15   ck      Added boot_images prefix to copytree exclude
# 05/22/15   sy      Added boot_images to HK11 binary copy.
# 05/08/15   ck      Added source dir check to copy_to_pack_dir.  Updated release_directory_recopy.
# 04/07/15   ck      Added BaseTarget to package list.  Fixed .inf file discovery bug.
# 03/30/15   ck      Improved PackIn/PackOut option by allowing directory names
# 03/26/15   ck      Added -e --eval-dir option
# 03/24/15   ck      Added CryptoPkg to release_directory_recopy list
# 03/20/15   ck      Added default to architecture argument
# 03/19/15   ck      Added copy_release_binaries logic
# 03/16/15   ck      Added UserExtensions.PackIt.PackOut .inf feature
# 03/12/15   ck      Fixed os seperator bug in release_entry_assembler
# 03/09/15   ck      Added UserExtensions.PackIt.PackIn .inf feature
# 03/09/15   ck      Added -a --arch option
# 03/09/15   ck      Added -r --read-write option
# 07/07/14   ck      Initial release


from optparse import OptionParser
import shutil
import os
import sys
import locale
import stat


# Following list is a collection of file extensions or file names that are not
# to be deleted even if the file is not in the generated release list.
release_exceptions = [".inf", ".dec", ".dsc", ".fdf", ".cfg", ".h", "sbl1.ld", ".inc", ".json",\
                      "buildit.py","buildex.py","delcommonts.py","edksetup.bat", "edksetup.sh", "fvinfo_creator.py", \
                      ".bmp", ".png", ".jpg", ".inc", "manifest.xml", "commons.py", "elf_gen.py", "elf_gen_tools.py", "sbl_parser.py", \
                      "GenXBLConfig.py", "XBLconfig_metadata_generator.py"]

# Following list is a collection of directories that should be recopied after
# parsing of the release directory.  This keeps the exception list small
# as some directories must be released in their entirety and can be quite large
release_directory_recopy = ["boot_images/QcomPkg/Docs",
                            "boot_images/QcomPkg/QcomModulePkg",]

##############################################################################
# main
##############################################################################
def main():
  release_list = []
  packout_list = ['HY11_1/boot_images/QcomPkg/QnadPkg']
  packin_list = []

  parser = OptionParser()

  parser.add_option("-b", "--branch-dir",
    		    action="store", type="string", dest="branchdir_name",
		    default=".",
		    help="Branch directory name.  Default is '.'")

  parser.add_option("-p", "--pack-dir",
                    action="store", type="string", dest="packdir_name",
		    default="HY11_1",
		    help="Pack directory name.  Default is HY11_1")

  parser.add_option("-r", "--read-write",
    		    action="store_true", dest="read_write",
                    default=False,
		    help="Changes file permissions of pack directory to rw.")

  parser.add_option('-a', '--architectures',
                    type='string',
                    action='callback',
                    callback=architecture_parser_callback,
                    default=["arm","aarch64","ipf"],
		    help="Architecture(s).  Comma seperated.  I.E. ARM,AARCH64" + \
                         "Default is ARM,AARCH64,IPF")

  parser.add_option("-t", "--target",
    		    action="store", type="string", dest="target_name",
		    help="Target to pack.  I.E. Msm8996")

  parser.add_option("--build-dir",
    		    action="store", type="string", dest="build_directory",
		    default="./boot_images/Build",
		    help="Build directory.  Default is './boot_images/Build'")

  parser.add_option("-e", "--eval-dir",
                    action="store", type="string", dest="evaldir_name",
		    default="boot_images/QcomPkg",
		    help="Base directory of where to start pack evaluation.  " + \
                         "Default is boot_images/QcomPkg")

  parser.add_option("--binary-dir",
                    action="store", type="string", dest="binarydir_name",
		    default="HK11",
		    help="Binary pack directory name.  Default is HK11")
 
  parser.add_option("--variant",
                     action="store", type="string", dest="variant", default="none")

  parser.add_option("--post-build",
                     action="store_true", dest="post_build_clean", default=False)

  (options, args) = parser.parse_args()


  # Build dir and eval dir defaults need to be OS agnostic.
  options.build_directory = options.build_directory.replace("/", os.sep)
  options.build_directory = options.build_directory.replace("\\", os.sep)
  options.evaldir_name = options.evaldir_name.replace("/", os.sep)
  options.evaldir_name = options.evaldir_name.replace("\\", os.sep)

  # Clean generated files in pack directory post-build
  if (options.post_build_clean):
    clean_pack_dir_post_build(options.packdir_name, 
                              options.variant.lower())

  # Copy branch to pack directory
  copy_to_pack_dir(options.packdir_name,
   		   options.branchdir_name)
		   
  # Copy manifest file to default directory
  copy_manifest_file()

  # If read-write option was supplied change file permissions of pack dir
  if (options.read_write):
    change_pack_dir_permissions(options.packdir_name)


  # If target name was specified then delete all other target directories
  if (options.target_name):
    delete_target_folders(options.packdir_name,
                          options.target_name)

  # Check each .inf file for basic sanity.  .inf must either contain a Sources
  # or Binaries section but not both.
  sanity_check_inf_files(os.path.join(options.packdir_name,
                                      options.evaldir_name),
                         options.architectures)


  # Generate a list of files to include in release based on .inf file entries.
  generate_packit_list(packout_list,
                 packin_list,
                 os.path.join(options.packdir_name,
                              options.evaldir_name),
                 options.architectures,
                 options.variant.lower())

  # Recursively scan pack dir for module .inf files.  Replace module .inf with
  # release version if found.
  prepare_release_inf_files(os.path.join(options.packdir_name,
                                 options.evaldir_name))
  
  # Generate a list of files to pack out based on .inf file entries
  generate_release_list(release_list,
                 packout_list,
                 packin_list,
                 os.path.join(options.packdir_name,
                              options.evaldir_name),
                 options.architectures,
                 options.variant.lower())
 
  # Check if there are conflicting PackIn/PackOut rules specificed in any .inf file
  check_for_conflicts_in_lists(packin_list, 
                               packout_list)
  
  # Add PackIn files to the final relase list
  release_list += packin_list

  # Evaluate the release_list and copy binaries to source tree.
  # I.E. .efi, .lib, .obj files listed in .inf files
  # This can only be done if the target name and build dir are specified
  if (options.target_name) and (options.build_directory):
    copy_release_binaries(options.build_directory,
                          options.target_name,
                          release_list)


  # Traverse pack directories deleting files not in release list
  # and files and folders in packout_list.
  cleanse_pack_dir(os.path.join(options.packdir_name,
                                options.evaldir_name),
                   release_list,
                   packout_list)


  # Recopy the directories that need to be shipped in their entirty
  restore_full_directories(options.packdir_name,
                           options.branchdir_name)


  # Create binary deliverable directory
  create_binary_deliverable(options.branchdir_name,
                            options.packdir_name,
                            options.binarydir_name,
                            options.target_name)

  # Remove UserExtensions.PackIt rule lines from .inf files in pack folder
  cleanse_existing_inf_files(os.path.join(options.packdir_name,
                                options.evaldir_name))
                            
  # Recopied files can have read only attributes.
  # Execute change_pack_dir_permissions if applicable
  if (options.read_write):
    change_pack_dir_permissions(options.packdir_name)


  return

##############################################################################
# copy_manifest_file
##############################################################################
def copy_manifest_file():
  # Search for manifest.xml file.  The file must reside in a directory called 'build'.
  # Start searching from boot_images directory.
  root_path = os.path.join(os.getcwd(), 'HY11_1', 'boot_images')
  # Make sure current path has a boot_images directory
  if not 'boot_images' in root_path :
	return

  base_directory = root_path.split('boot_images',1)[0] + 'boot_images'
  for root, directories, files in os.walk(base_directory):  
	for current_file in files:
	   if current_file == 'manifest.xml':
		 # Parent directory must be 'build'
		 if os.path.basename(os.path.normpath(root)).lower() == 'build':
		   source_path = os.path.join(root,
							   'manifest.xml')
							   
  dest_path = os.path.join(root_path,
				"BaseTools",
				"Source",
				"Python",
				"build",
				"manifest.xml")
  
  if source_path != dest_path:
            shutil.copyfile(source_path, dest_path)

			
  return

##############################################################################
# architecture_parser_callback
##############################################################################
def architecture_parser_callback(option, opt, value, parser):
  setattr(parser.values, option.dest, value.split(','))


##############################################################################
# cleanse_existing_inf_files
##############################################################################
def cleanse_existing_inf_files(base_directory):
  print "Removing PackIt rules from .inf files"
  for root, directories, files in os.walk(base_directory):
    for file_name in files:
      if file_name.lower().endswith(".inf"):
        file_path = os.path.join(root, file_name)
        if '.packit.' in open(file_path).read().lower():
          remove_packit_rules(file_path)
          

##############################################################################
# remove_packit_rules
##############################################################################
def remove_packit_rules(file_path):
  new_file_lines = []
  inf_lines = [x.replace('\r','').replace('\n','') \
               for x in open(file_path).readlines()]
  delete = False
  for line in inf_lines:
    # if the line has [] brackets, it is a section header
    # non-PackIt sections need to be preserved
    if '[' in line and ']' in line:
      if '.packit.' in line.lower():
        # Don't remove the post-build packing instructions
        if 'postbuildpackout' in line.lower():
          delete=False
          new_file_lines.append(line)
        else:
          delete=True
      else:
        delete=False
        new_file_lines.append(line)
    else:
      if delete:
        continue
      else:
        new_file_lines.append(line)

  # Save contents of stripped .inf to new .inf file
  new_file_path = file_path.replace(".inf", "_new.inf") 
  f1 = open(new_file_path, 'w+')
  for line in new_file_lines:
    f1.write(line+"\r\n")
  f1.close()

  # Remove previous original .inf file
  if os.path.exists(file_path):
    try:
      os.remove(file_path)
    except:
      print "Error encountered while deleting " + file_path
      print "Error info: ", sys.exc_info()[0]
      sys.exit()
  # Rename newly stripped .inf file as original .inf
  try:
    os.rename(new_file_path, file_path)
  except:
    print "Error encountered while renaming " + new_file_path + \
          " to " + file_path
    print "Error info: ", sys.exc_info()[0]
    sys.exit()
  
  return


##############################################################################
# copytree_ignore
##############################################################################
def copytree_ignore(exclude_path, directory):
  def copytree_ignore_evaluation(eval_directory, eval_files):
    if exclude_path == eval_directory:
      return directory
    return []
  return copytree_ignore_evaluation


##############################################################################
# copy_to_pack_dir
##############################################################################
def copy_to_pack_dir(destination_directory,
		     source_directory):

  # Delete old destination directory if it exists
  if os.path.isdir(destination_directory):
    print "Directory " + destination_directory + " exists.  Deleting."
    
    try:
      shutil.rmtree(destination_directory)
    except:
      print "Error encountered while deleting directory " + \
            destination_directory
      print "Error info: ", sys.exc_info()[0]
      sys.exit()


  # Verify source directory exists.  Error out if it doesn't.
  if not os.path.isdir(source_directory):
    print "    ERROR: Directory " + source_directory + " does not exist...exiting"
    sys.exit()


  # Recursively copy source directory to destination
  # Do not include the Build or pack directories
  print "Copying " + source_directory + " to " + destination_directory
  try:
    shutil.copytree(source_directory,
	            destination_directory,
                    ignore=copytree_ignore(os.path.join(source_directory,
                                                        "boot_images"),
                                           ["Build", \
                                            "pack", \
                                            "HY11", \
                                            "HY11_1", \
                                            "HK11"]))
  except:
    print "Error encountered while copying " + source_directory + " to " + \
          destination_directory
    print "Error info: ", sys.exc_info()[0]
    sys.exit()

  return


##############################################################################
# change_pack_dir_permissions
##############################################################################
def change_pack_dir_permissions(destination_directory):

  # Attempt to change the file permissions of destination_directory
  # to read write.
  print "Attempting to change file permissions of " + destination_directory

  # shutil does not have a recursive option.  Walk the directory.
  try:
    for root, directories, files in os.walk(destination_directory):  
      for current_directory in directories:
        os.chmod(os.path.join(root,
                              current_directory),
                 stat.S_IREAD | stat.S_IWRITE)

      for current_file in files:
        os.chmod(os.path.join(root,
                              current_file),
                 stat.S_IREAD | stat.S_IWRITE)
  except:
    print "Error while changing file permissions."
    print "Error info: ", sys.exc_info()[0]
    sys.exit()
  return


###############################################################################
# delete_target_folders
###############################################################################
def delete_target_folders(base_directory,
                          target_names):

  print "Discovering and deleting unused target folders."

  # The logic below should be updated/added to work on different source trees.
  # Using just the target name discover other targets under the base
  # directory and delete them.
  for root, directories, files in os.walk(base_directory):
    for current_directory in directories:

      # Current logic work on "XF" warehouses.
      # Target directory begins with Msm, Mdm, Qcom, or Qdf and ends with Pkg
      # Delete directories matching that and are not the target specified
      if current_directory.lower().startswith("msm") or \
         current_directory.lower().startswith("mdm") or \
         current_directory.lower().startswith("sdm") or \
         current_directory.lower().startswith("qdf") or \
         current_directory.lower().startswith("server") or \
         current_directory.lower().startswith("qcommodule") or \
         current_directory.lower().startswith("qcomapp") or \
         current_directory.lower().startswith("basetarget"):
        if current_directory.lower().endswith("pkg"):
          if not any(target.lower() in current_directory.lower() for target in target_names.split(',')):
            try:
              shutil.rmtree(os.path.join(root, current_directory))
            except:
              print "Error encountered while deleting: " + \
                os.path.join(root, current_directory)
              print "Error info: ", sys.exc_info()[0]
              sys.exit()
        
  return


###############################################################################
# prepare_release_inf_files
###############################################################################
def prepare_release_inf_files(base_directory):

  print "Preparing release inf files"

  # Recursively scan directories starting from base_directory looking
  # for "*release.inf" files.  If one is found then delete the non release
  # inf file (*.inf) and replace with release version.
  for root, directories, files in os.walk(base_directory):
    for file_name in files:
      if file_name.lower().endswith("release.inf"):
        # Get module name.  -11 is release.inf
        module_name = file_name[:-11]
	release_inf_path = os.path.join(root, file_name)
	original_inf_path = os.path.join(root, module_name + ".inf")
        
        # Delete original inf
        if os.path.exists(original_inf_path):
          try:
            os.remove(original_inf_path)
          except:
            print "Error encountered while deleting " + original_inf_path
	    print "Error info: ", sys.exc_info()[0]
	    sys.exit()

        # Rename release inf to original name
	try:
          os.rename(release_inf_path,
		    original_inf_path)
        except:
          print "Error encountered while renaming " + release_inf_path + \
                " to " + original_info_path
          print "Error info: ", sys.exc_info()[0]
	  sys.exit()

  return


###############################################################################
# sanity_check_inf_files
###############################################################################
def sanity_check_inf_files(base_directory,
                           architecture_list):

  print "Sanity checking inf files"

  # Recursively scan directories starting from base_directory looking
  # for ".inf" files.  If one is found then ensure it contains a Sources
  # section or Binaries section but not both.  If architecture is specified
  # then only look at Sources, Sources.common, Sources.<architecture>,
  # Binaries, Binaries.common and Binaries.<architecture>
  for root, directories, files in os.walk(base_directory):
    for file_name in files:
      if file_name.lower().endswith(".inf"):
        source_exists = False
        binary_exists = False

        file_path = os.path.join(root, file_name)

        try:
          if "[sources]" in open(file_path).read().lower():
            source_exists = True

          if "[sources.common]" in open(file_path).read().lower():
            source_exists = True

          if architecture_list:
            for architecture in architecture_list:
              if "[sources." + architecture.lower() in open(file_path).read().lower():
                source_exists = True

          if "[binaries]" in open(file_path).read().lower():
            binary_exists = True

          if "[binaries.common]" in open(file_path).read().lower():
            binary_exists = True

          if architecture_list:
            for architecture in architecture_list:
              if "[binaries." + architecture.lower() in open(file_path).read().lower():
                binary_exists = True

        except:
          print "Error encountered while reading " + file_path
          print "Error info: ", sys.exc_info()[0]
	  sys.exit()

        # Ensure one section or the other exists, but not both
        if source_exists == False and binary_exists == False:
          print "ERROR: Invalid .inf file.  " + file_path + " does not " + \
                "contain a valid Sources or Binaries section."
          sys.exit()

        if source_exists == True and binary_exists == True:
          print "ERROR: Invalid .inf file.  " + file_path + " contains a " + \
                "Sources and Binaries section."
          sys.exit()
  return


###############################################################################
# generate_release_list
###############################################################################
def generate_release_list(release_list,
                   packout_list,
                   packin_list,
                   base_directory,
                   architecture_list,
                   variant):

  print "Parsing source/binaries to generate release list"

  # Recursively scan directories starting from base_directory looking
  # for ".inf" files.  If one is found then add the paths under source,
  # source.common, source.<architecture>,  binaries, binaries.common,
  # binaries.<architecture>, or UserExtensions.PackIt.PackIn to the release
  # list. 
  for root, directories, files in os.walk(base_directory):
    for file_name in files:
      if file_name.lower().endswith(".inf"):
        file_path = os.path.join(root, file_name)

        # Read file line by line looking for sources or binaries
        try:
          release_entry = False

          with open(file_path) as inf_file:
            for line in inf_file:
              # Ignore lines that begin with #
              if line.strip().startswith("#"):
                continue

              if release_entry == True:
                if line == "" or "[" in line:
                  release_entry = False
                else:
                  # Line needs to be parsed and assembled
                  assembled_line = list_entry_assembler(line,
                                                        root)
                  if len(assembled_line) > 0:
                    if release_entry == True:
                      release_list.append(assembled_line)
                    
              if "[sources]" in line.lower() or \
                 "[sources.common]" in line.lower() or \
                 "[binaries]" in line.lower() or \
                 "[binaries.common]" in line.lower():
                release_entry = True
              
              if architecture_list:
                for architecture in architecture_list:
                  if "[sources." + architecture.lower() in line.lower() or \
                     "[binaries." + architecture.lower() in line.lower():
                    release_entry = True
        except:
          print "Error encountered while reading " + file_path
          print "Error info: ", sys.exc_info()[0]
	  sys.exit()

  return


###############################################################################
# check_for_conflicts_in_lists
###############################################################################
def check_for_conflicts_in_lists(packin_list, 
                                 packout_list):
  print "Checking for conflicts"
  # Checking for common files in release and packout lists
  common = list( set(packin_list) & set(packout_list) )
  if (common):
    print "Error: the following file is defined with conflicting PackIt rules"
    for conflict in common:
      print conflict

    sys.exit()
  return


###############################################################################
# generate_packit_list
###############################################################################
def generate_packit_list(packout_list,
                   packin_list,
                   base_directory,
                   architecture_list,
                   variant):

  print "Parsing PackIt rules"

  # Recursively scan directories starting from base_directory looking
  # for ".inf" files.  If one is found then add the paths under source,
  # source.common, source.<architecture>,  binaries, binaries.common,
  # binaries.<architecture>, or UserExtensions.PackIt.PackIn to the release
  # list.  Also look for UserExtensions.PackIt.Packout and add entries to
  # packout_list.  Pruning of entires might be necessary.
  for root, directories, files in os.walk(base_directory):
    for file_name in files:
      if file_name.lower().endswith(".inf"):
        file_path = os.path.join(root, file_name)

        # Read file line by line looking for sources or binaries
        try:
          packout_entry = False
          packin_entry = False
          with open(file_path) as inf_file:
            for line in inf_file:

              # Ignore lines that begin with #
              if line.startswith("#"):
                continue

              if packout_entry == True or packin_entry == True:
                if line == "" or "[" in line:
                  packout_entry = False
                  packin_entry = False
                else:
                  # Line needs to be parsed and assembled
                  assembled_line = list_entry_assembler(line,
                                                        root)
                  if len(assembled_line) > 0:
                    if packout_entry == True:
                      packout_list.append(assembled_line)
                    if packin_entry == True:
                      packin_list.append(assembled_line)
              
              #Parse common/variant-specific PackIt rules
              if "[userextensions.packit.packout]" in line.lower():
                packout_entry = True

              if variant != "none":
                if "[userextensions.packit.packout."+variant+"]" in line.lower():
                  packout_entry = True

              if "[userextensions.packit.packin]" in line.lower():
                packin_entry = True

              if variant != "none":
                if "[userextensions.packit.packin."+variant+"]" in line.lower():
                  packin_entry = True

        except:
          print "Error encountered while reading " + file_path
          print "Error info: ", sys.exc_info()[0]
	  sys.exit()

  return


###############################################################################
# list_entry_assembler
###############################################################################
def list_entry_assembler(line,
                         path_prefix):

  # Line is raw data from inf file.  It must be parsed as it can contain
  # extra data seperated by |.  Extract file name/path and trim it.
  # The parsed data can contain ".."'s.  This must be taken off the end
  # of path_prefix before final assembly.
  split_line = line.split("|")

  raw_data = ""
  for entry in split_line:
    # Look for "..", or ".c", etc
    if "." in entry:
      raw_data = entry
      break

  if len(raw_data) == 0:
    # Valid data was not found.
    return raw_data

  
  # Strip raw_data of leading and training spaces
  raw_data = raw_data.strip()

  # Convert "/" and "\" to proper os specific delimiter.  Do this for not only
  # raw_data but path_prefix.
  raw_data = raw_data.replace("/", os.sep)
  raw_data = raw_data.replace("\\", os.sep)
  path_prefix = path_prefix.replace("/", os.sep)
  path_prefix = path_prefix.replace("\\", os.sep)

  # Join path_prefix and raw_data.  raw_data can have ".." so normalize the path
  release_entry = os.path.normpath(os.path.join(path_prefix, raw_data))
  
  return release_entry
  

###############################################################################
# copy_release_binaries
###############################################################################
def copy_release_binaries(build_directory,
                          target_directory,
                          release_list):

  print "Copying Release Binaries"
  
  # Search the release_list looking for .efi, .lib, and .obj files.
  # These files must be copied from the build directory and placed in the
  # source tree so post pack compilations succeed.

  for release_list_file in release_list:
    if ".efi" in release_list_file.lower() or \
       ".lib" in release_list_file.lower() or \
       ".obj" in release_list_file.lower():

      binary_split = release_list_file.split(os.sep)
      binary_file = binary_split[len(binary_split) - 1]
      binary_directory = binary_split[len(binary_split) - 2]

      # Find file in build.  If the path contains the proper target,
      # release, and parent folder name then copy the file to the source tree.
      for root, directories, files in os.walk(build_directory):
        for possible_file in files:
          if binary_file.lower() == possible_file.lower():

            # Check binary_directory
            if binary_directory.lower() in root.lower():

              # Make sure this is right target
              if any(os.sep + target_dir.lower() in root.lower() for target_dir in target_directory):

                # Make sure this is a RELEASE build
                if "RELEASE" in root:
                  try:
                    # release_list_file can contain a path.  Add the missing
                    # directories if needed before copying.
                    if not os.path.isdir(os.path.dirname(release_list_file)):
                      os.makedirs(os.path.dirname(release_list_file))

                    shutil.copyfile(os.path.join(root, possible_file),
                                    release_list_file)

                  except:
                    print "Error encountered while copying " + \
                      os.path.join(root, possible_file)
                    print "Error info: ", sys.exc_info()[0]

                    sys.exit()

  return


###############################################################################
# cleanse_pack_dir
###############################################################################
def cleanse_pack_dir(base_directory,
                     release_list,
                     packout_list):

  print "Cleansing pack directory"
  
  # Recursively scan directories in base_directory starting from highest
  # inspecting each file.  If the file exists in the release list then keep it.
  # If not then delete it.  Exception is made for files listed in
  # the release_exceptions list.  If a directory is empty then
  # delete it too.

  # New feature added where entries in release list can be a directory instead
  # of an individual file.  After file check is done then compare directory
  # for entry in release list.

  try:
    for root, directories, files in os.walk(base_directory,
                                            topdown=False):
      for current_file in files:
        # If file does not exist in release list or exceptions list, delete it.
        if os.path.join(root, current_file) not in release_list:
          # Check if directory is in release list
          if root not in release_list:
            if not any(ext in current_file for ext in release_exceptions):
              try:
                os.remove(os.path.join(root, current_file))
              except:
                print "Error encountered while deleting " + \
                  os.path.join(root, current_file)
                print "Error info: ", sys.exc_info()[0]
                sys.exit()

      # All files have been evaluated.  Check to see if directory is empty
      # and delete it if it is.
      if not os.listdir(root):
        try:
          os.rmdir(root)
        except:
          print "Error encountered while deleting directory " + root
          print "Error info: ", sys.exc_info()[0]
          sys.exit()
  except:
    print "Error encountered while reading " + root
    print "Error info: ", sys.exc_info()[0]
    sys.exit()
  
 
  # Iterate through the packout list and remove files or directories
  try:
    for packout_entry in packout_list:
      # Check and see if the entry is a file or a directory.  If file just
      # delete it.  If directory delete it and everything underneath it.
      if os.path.isfile(packout_entry):
        os.remove(packout_entry)
      elif os.path.isdir(packout_entry):
        shutil.rmtree(packout_entry)
  except:
    print "Error encountered"
    print "Error info: ", sys.exc_info()[0]
    sys.exit()


  return


###############################################################################
# restore_full_directories
###############################################################################
def restore_full_directories(pack_directory,
                             branch_directory):

  print "Restoring full ship directories"

  # Recopy the directories specified by release_directory_recopy.  These are
  # directories that are to be shipped in their entirety.
  for recopy_directory in release_directory_recopy:
    # Change path seperator for proper os
    recopy_directory = recopy_directory.replace("/", os.sep)
    recopy_directory = recopy_directory.replace("\\", os.sep)

    copy_to_pack_dir(os.path.join(pack_directory, recopy_directory),
                     os.path.join(branch_directory, recopy_directory))

  return


##############################################################################
# create_binary_deliverable
##############################################################################
def create_binary_deliverable(base_directory,
                              pack_directory,
                              binary_directory,
                              target_names):

  # Delete old destination directory if it exists
  if os.path.isdir(binary_directory):
    print "Directory " + binary_directory + " exists.  Deleting."
    
    try:
      shutil.rmtree(binary_directory)
    except:
      print "Error encountered while deleting directory " + \
            binary_directory
      print "Error info: ", sys.exc_info()[0]
      sys.exit()


  print "Creating " + binary_directory + " binary directory."
  # Copy all target binary directories
  for target_name in target_names.split(','):
    try:
      # Check for Bin (buildit dirs)
      bin_path = os.path.join(base_directory,
                                "boot_images",
                                "QcomPkg",
                                target_name + "Pkg",
                                "Bin")

      if os.path.isdir(bin_path):
        shutil.copytree(bin_path,
                        os.path.join(binary_directory,
                                     "boot_images",
                                     "QcomPkg",
                                     target_name + "Pkg",
                                     "Bin"))
        
    except:
      print "Error encountered while copying target binaries."
      print "Error info: ", sys.exc_info()[0]
      sys.exit()


  # Copy QcomPkg Tools
  try:
    qcom_tools_path = os.path.join(pack_directory,
                                   "boot_images",
                                   "QcomPkg",
                                   "Tools")

    if os.path.isdir(qcom_tools_path):
      shutil.copytree(qcom_tools_path,
                      os.path.join(binary_directory,
                                   "boot_images",
                                   "QcomPkg",
                                   "Tools"))
  except:
    print "Error encountered while copying QCOM tools."
    print "Error info: ", sys.exc_info()[0]
    sys.exit()


  # Copy target tools
  for target_name in target_names.split(','):
    try:
      target_tools_path = os.path.join(pack_directory,
                                       "boot_images",
                                       "QcomPkg",
                                       target_name + "Pkg",
                                       "Tools")

      if os.path.isdir(target_tools_path):
        shutil.copytree(target_tools_path,
                        os.path.join(binary_directory,
                                     "boot_images",
                                     "QcomPkg",
                                     target_name + "Pkg",
                                     "Tools"))
    except:
      print "Error encountered while copying target tools."
      print "Error info: ", sys.exc_info()[0]
      sys.exit()

  return

##############################################################################
# clean_pack_dir_post_build
##############################################################################
def clean_pack_dir_post_build(packdir, variant):

  print "Running post-build PackIt rules."
    
  if not os.path.isdir(packdir):
    print ("Error finding packed directory!\nPlease ensure packit.py has been "
           "run prior to calling the post-build clean procedure")
    sys.exit()

  build_dir = os.path.join(packdir, "boot_images", "Build")
  if not os.path.isdir(build_dir):
    print ("Packed directory not built!\nPlease ensure the directory has been "
           "built prior to calling the post-build clean procedure")
    sys.exit()

  postbuild_packout_list = []

  for root, directories, files in os.walk(packdir):
    for file_name in files:
      if file_name.lower().endswith(".inf"):
        file_path = os.path.join(root, file_name)
        
        # Read file line by line looking for sources or binaries
        try:
          packout_entry = False
          with open(file_path, 'r') as inf_file:
            for line in inf_file:

              # Ignore lines that begin with #
              if line.startswith("#"):
                continue

              if packout_entry == True:
                if line == "" or "[" in line:
                  packout_entry = False
                else:
                  # Line needs to be parsed and assembled
                  assembled_line = list_entry_assembler(line,
                                                        root)
                  if len(assembled_line) > 0:
                    if packout_entry == True:
                      postbuild_packout_list.append(assembled_line)
                
              #Parse common/variant-specific PackIt rules
              if "[userextensions.packit.postbuildpackout]" in line.lower():
                packout_entry = True

              if variant != "none":
                if "[userextensions.packit.postbuildpackout."+variant+"]" in line.lower():
                  packout_entry = True


        except:
          print "Error encountered while reading " + file_path
          print "Error info: ", sys.exc_info()[0]
          sys.exit()



  # Iterate through the packout list and remove files or directories
  try:
    for packout_entry in postbuild_packout_list:
      # Check and see if the entry is a file or a directory.  If file just
      # delete it.  If directory delete it and everything underneath it.
      if os.path.isfile(packout_entry):
        os.remove(packout_entry)
      elif os.path.isdir(packout_entry):
        shutil.rmtree(packout_entry)
  except:
    print "Error encountered"
    print "Error info: ", sys.exc_info()[0]
    sys.exit()


  print 'Packed out %d files' % len(postbuild_packout_list)

  sys.exit()

main()
