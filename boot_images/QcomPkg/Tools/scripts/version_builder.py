
#!/usr/bin/python
#============================================================================
#
#/** @file version_builder.py
#
# GENERAL DESCRIPTION
#   Generate image version output files : oem_version.c and qc_version.c 
#   base on manifest.xml input   
#
#
#  Copyright (c) 2014 , 2018, 2019 Qualcomm Technologies, Inc. All Rights Reserved
#
#**/
#
#----------------------------------------------------------------------------
#
#  $Change: 20263961 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 12/27/16   kvs       Update the QC_IMAGE_VERSION_STRING with the BUILD_ID
# 10/19/15   ck        Removed hardwire to 8996 target and variant type LAB
# 08/13/15   ck        Return error code 2 to indicate manifest.xml cannot be found
# 08/07/15   ck        Added logic to find manifest.xml if it isn't supplied
# 05/27/15   ck        Added oem_uuid_builder
# 7/23/14    sy        initial version
#============================================================================

import os
import sys
import re
import socket
import uuid
from datetime import datetime
from xml.etree import ElementTree as et
from optparse import OptionParser
import string

def find_xml():
  # Search for manifest.xml file.  The file must reside in a directory called 'build'.
  # Start searching from boot_images directory.
 
  # Make sure current path has a boot_images directory
  if not 'boot_images' in os.getcwd():
	return

  base_directory = os.getcwd().split('boot_images',1)[0] + 'boot_images'
  for root, directories, files in os.walk(base_directory):  
	for current_file in files:
	   if current_file == 'manifest.xml':
		 # Parent directory must be 'build'
		 if os.path.basename(os.path.normpath(root)).lower() == 'build':
		   return os.path.join(root,
							   'manifest.xml')
			
  return


def get_oem_version_string():
   '''-------------------------------------------------------------------------
   Get OEM version string

   utility function to get OEM version string from multiple sources, used from
   multiple functions
   -------------------------------------------------------------------------'''

   # create oem version string
   # this string can coem from multiple sources, in order of priority
   # cmd line: OEM_BUILD_VER 
   # os env:  SCONS_OEM_BUILD_VER 
   # if nothing found then use machine hostname
   # if that fails use static string
#   oem_ver = SCons.Script.ARGUMENTS.get('OEM_BUILD_VER')
   oem_ver = socket.gethostname()
   if oem_ver is None:
	  oem_ver = "Engineering build"

   oem_ver = str(oem_ver).strip().strip('"')

   return oem_ver


def get_oem_uuid_string():
   '''-------------------------------------------------------------------------
   Get OEM uuid string

   utility function to get OEM uuid string from multiple sources, used from
   multiple functions
   -------------------------------------------------------------------------'''

   # create oem uuid string

   oem_uuid =  "".join(["Q_SENTINEL_{", str(uuid.uuid4()).upper(), "}", datetime.now().strftime("_%Y%m%d_%H%M")])

   return oem_uuid


##-------------------------------------------------------------------------
##create_file_banner
##creates a string that can be use as a banner for auto generated files
##-------------------------------------------------------------------------'''
def CreateFileBanner(fname, description="None", start_comment="#",

   
   #The standard Qualcomm header 
   end_comment="", start_block="", end_block="", style='none'):
   banner_str = \
'''$SB$SCM============================================================================$ECM
$SCM  Name:                                                                     $ECM
$SCM    $FILE_NAME $ECM
$SCM
$SCM  Description:                                                              $ECM
$SCM    $DESCRIPTION $ECM
$SCM                                                                            $ECM
$SCM Copyright (c) $YEAR Qualcomm Technologies, Inc. All Rights Reserved        $ECM
$SCM============================================================================$ECM
$SCM                                                                            $ECM
$SCM *** AUTO GENERATED FILE - DO NOT EDIT                                      $ECM
$SCM                                                                            $ECM
$SCM GENERATED: $DATE $ECM
$SCM============================================================================$ECM$EB
'''
   if style == 'C':
	  start_comment = "#"
	  end_comment = ""
	  start_block = "/*\n"
	  end_block = "\n*/"
   elif style == 'C++':
	  start_comment = "//"
	  end_comment = ""
	  start_block = ""
	  end_block = ""

   elif style == 'asm':
	  start_comment = ";"
	  end_comment = ""
	  start_block = ""
	  end_block = ""

   elif style == 'make' or style == 'shell':
	  start_comment = "#"
	  end_comment = ""
	  start_block = ""
	  end_block = ""

   elif style == 'dos':
	  start_comment = "REM "
	  end_comment = ""
	  start_block = ""
	  end_block = ""

   banner_str = banner_str.replace('$SCM', start_comment)
   banner_str = banner_str.replace('$ECM', end_comment)
   banner_str = banner_str.replace('$SB', start_block)
   banner_str = banner_str.replace('$EB', end_block)
   banner_str = banner_str.replace('$YEAR', str(datetime.now().strftime('%Y')))
   banner_str = banner_str.replace('$DATE', str(datetime.now().ctime()))
   banner_str = banner_str.replace('$FILE_NAME', fname)
   banner_str = banner_str.replace('$DESCRIPTION', description)

   return banner_str

def qc_version_builder(target, source):
   '''-------------------------------------------------------------------------
   QC Version builder action
   This is the actual builder that generates qc_vesion.c. target is the
   location of the *.c file and source is where the manifest.xml is
  -------------------------------------------------------------------------'''
  # The array design is ported from SCons system 
  # targets and sources are ALWAYS arrays (is a builder standard interface)
  # this builder needs to know if it deals with multiple targes and sources
  # or singles, or mix (common use case: one target multiple sources).
   target_full = str(target[0])
   source_full = str(source[0])

   
   # Get XML data
   XmlCfg = et.ElementTree(file=source_full).getroot()

   XmlImageTree = XmlCfg.find('image_tree')

   if XmlImageTree is not None:
	  XmlImageTreeBuildId = XmlImageTree.find('build_id')
   else:
	  PrintError("")
	  PrintError("-------------------------------------------------------------------------------")
	  PrintError("*** The configuration XML file: %s is missing 'image_tree' tag" % os.path.split(source_full)[1])
	  PrintError("-------------------------------------------------------------------------------")
	  PrintError("")

   build_id = "QC_SCONS_DEFAULT_BUILD_ID" 
   if XmlImageTreeBuildId is not None: 
	   build_id = XmlImageTreeBuildId.text.strip()

   # crete version string
   # embedd the Build ID and limit the Build ID to 72 characters
   qc_version_str = "".join([
	   "const char QC_IMAGE_VERSION_STRING_AUTO_UPDATED[]=" + 
	   "\"QC_IMAGE_VERSION_STRING=", build_id[:72], "\";\n"]) 

   # create QC version file
   banner = CreateFileBanner(os.path.split(target_full)[1], style='C')
   qc_version_file = open (target_full, "w")
   qc_version_file.write(banner)
   qc_version_file.write(qc_version_str)
   qc_version_file.close()

def oem_version_builder(target,
						build_type):
   '''-------------------------------------------------------------------------
   OEM Version builder action

   The actual builder action this gets called only if the targets needs to
   be build or rebuild. Otherwise this will not be called.For now, this is
   called all the time.  
   -------------------------------------------------------------------------'''

   # The array design is ported from SCons system 
   # targets is ALWAYS arrays (is a builder standard interface)
   # this builder needs to know if it deals with multiple targets 
   # or singles, or mix (common one target multiple sources).
   target_full = str(target[0])

   
   # create variant version string
   variant_version_str = "".join([ "const char IMAGE_VARIANT_STRING_AUTO_UPDATED[]=\"IMAGE_VARIANT_STRING=", build_type, "\";\n"])

   oem_ver = get_oem_version_string()
   oem_version_str = "".join([ "const char OEM_IMAGE_VERSION_STRING_AUTO_UPDATED[]=\"OEM_IMAGE_VERSION_STRING=", oem_ver, "\";\n"])

   # create OEM version file
   banner = CreateFileBanner(os.path.split(target_full)[1], style='C')
   oem_version_file = open (target_full, "w")
   oem_version_file.write(banner)
   oem_version_file.write(variant_version_str)
   oem_version_file.write(oem_version_str)
   oem_version_file.close()


def oem_uuid_builder(target):
   '''-------------------------------------------------------------------------
   OEM UUID builder action

   The actual builder action this gets called only if the targets needs to
   be build or rebuild.
   -------------------------------------------------------------------------'''

   # targets and sources are ALWAYS arrays (is a builder standard interface)
   # this builder needs to know if it deals with multiple targets and sources
   # or singles, or mix (common one target multiple sources).
   target_full = str(target[0])
   #source_full = str(source[0])

   # suppose you need to call an the external tool use the following
   #cmd = "".join([TOOL_CMD, " -o=", target_full, " -i=", source_full])
   #std_out, std_err, return_val = env.ExecCmds(cmds)

   # another possibility for the builder is that does everything here in python
   # and then just use open file and write file to generate the target.
   # <fancy/magical python code here>

   # create variant version string
   oem_uuid = get_oem_uuid_string()
   oem_uuid_str = "".join([
	  "const char OEM_IMAGE_UUID_STRING_AUTO_UPDATED[]=\"OEM_IMAGE_UUID_STRING=",
	  oem_uuid, "\";\n"])


   # create OEM version file
   banner = CreateFileBanner(os.path.split(target_full)[1], style='C')
   oem_version_file = open (target_full, "w")
   oem_version_file.write(banner)
   oem_version_file.write(oem_uuid_str)
   oem_version_file.close()


def main():

  parser = OptionParser()

  parser.add_option("-b", "--build-type",
					action="store", type="string", dest="build_type",
		    help="Build type placed into image variant string. I.E. M8996LAB' ")

  parser.add_option("-x", "--xml-dir",
			    action="store", type="string", dest="xml",
		    help="xml location. Default is to search for manifest.xml. ")

  parser.add_option("-t", "--target-dir",
					action="store", type="string", dest="targetdir",
		    default="Library/VersionBuilderLib",
		    help="Target directory name. Default is 'Library/VersionBuilderLib' ")

  
  (options, args) = parser.parse_args()


  # Build type must be given
  if not options.build_type:
	print "Error, build type must be given."
	sys.exit(3)

  # If xml was not provided then search for it
  if not options.xml:
	 options.xml = find_xml()
	 # If xml was not found then exit
	 if not options.xml:
		print "Error encountered, unable to locate manifest.xml file"
		sys.exit(2)


  # Convert xml and target dir path to OS.  I.E. / vs \
  options.xml = options.xml.replace('/', os.sep)
  options.xml = options.xml.replace('\\', os.sep)
  options.targetdir = options.targetdir.replace('/', os.sep)
  options.targetdir = options.targetdir.replace('\\', os.sep)


  if not os.path.isdir(options.targetdir):
	 print options.targetdir
	 try:
		os.makedirs(options.targetdir)
	 except:
		print "Error encountered while creating directory " + options.targetdir
		print "Error info: ", sys.exc_info()[0]
		sys.exit(1)
  

  qc_version_path = os.path.join(options.targetdir, "qc_version.c")
  print "Generating " + qc_version_path
  
  qc_version_builder([os.path.join(options.targetdir,
								   'qc_version.c')],

					 [options.xml])

  oem_version_builder([os.path.join(options.targetdir,
									'oem_version.c')],
					  options.build_type)

  oem_uuid_builder([os.path.join(options.targetdir,
								 'oem_uuid.c')])


if __name__ == "__main__":
   main()