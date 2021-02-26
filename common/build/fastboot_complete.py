'''
=======================================================================================================
fastboot_complete.py:

Description:
   Wrapper script around fastboot.exe to flash required binaries onto a device

Usage:
   fastboot_complete.py -h

Options:
   --version      show program's version number and exit
   -h, --help     show this help message and exit
   -a, --ap       APPS_PATH (to be replaced)
   -r, --rp       RESULT_PATH (to save output)
   -p, --pf       PRODUCT_FLAVOR
   -s, --st       STORAGE_TYPE
   -n, --sn       SERIAL_NUMBER (device's adb serial id)
   -i, --il       IMAGE_LIST (comma separated list of image tags to flash)
   -b, --sb       FLASH PERF/SECONDARY BINS
   --bponly       Flash only list of files with back_partition attribute
   --bp           Along with regular binaries flash list of bins with back_partition  attribute
   -c             CREATE LOCAL COPY (and flash from there)
   -e             DISABLE ERASE (load without erasing partitions)
   -j             JTAG DONE (to skip loading core binaries)
   -w             ENABLE WAIT (at the end of script execution)
   -d             ENABLE AUDIBLE SIGNAL
   --nearest     Picks geographically nearest build location

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/fastboot_complete.py#24 $
 $DateTime: 2018/09/17 12:22:19 $
===============================================================================================
'''
#---------------------------------------------------------#
# Import libraries                                        #
#---------------------------------------------------------#
import sys
import os
import shutil
import subprocess
import signal
import time
import tempfile
from glob import glob
from optparse import OptionParser
from pprint import pprint

import lib.meta_lib as ml


mi = None
options = None
fastboot = "fastboot"
adb = "adb"
fatbinary_attr_string = "(fat_file)||(fat_file_btfm)"


###########################################################
###                     SUBROUTINES                     ###
###########################################################

#---------------------------------------------------------#
# Function to find fastboot utility from APPS build       #
#---------------------------------------------------------#
def find_fastboot_from_build(la_path):
   global fastboot
   global adb   
   if "win" in sys.platform:
      print "Executing fastboot on windows"
      fastboot = os.path.join(la_path, 'vendor/qcom/proprietary/usb/host/windows/prebuilt/fastboot.exe')
      if not os.path.exists(fastboot):
         print "Cannot find fastboot.exe in Apps path"
         print "Will try using local environment"
         fastboot = 'fastboot.exe'
   elif "linux" in sys.platform:
      print "Executing fastboot on Linux"
      fastboot = os.path.join(la_path, 'out/host/linux-x86/bin/fastboot')
      if not os.path.exists(fastboot):
         print "Cannot find fastboot.exe in Apps path"
         print "Will try using local environment"  # default value
         fastboot = 'fastboot'
   
   adb = os.path.join(os.path.split(fastboot)[0],(os.path.split(fastboot)[1]).replace('fastboot','adb'))
   if not os.path.exists(adb): #in case if adb is not in apps build and fastboot is in apps build.
      if "win" in sys.platform:
         adb = 'adb.exe'
      elif "linux" in sys.platform:
         adb = adb	  
   print "adb path:",adb
#---------------------------------------------------------#
# Function to verify the target state                     #
#---------------------------------------------------------#
def verify_fastboot():
   retry_count = 0
   device_found = 0
   print "Checking target state..."
   print  "Please set device to fastboot mode and check if USB is connected." 
   st_time = time.time()
   while 1:
      # check if fastboot utility is available. If not, ask user to install it.
      try:
         stuff = subprocess.Popen([fastboot, 'devices'], stdout=subprocess.PIPE).stdout.read()
      except:
         print "\nFastboot is not installed."
         print "Please install fastboot utilities and add to system path. Then reload the build."
         fastboot_file_create("FAILURE_FASTBOOT")
         exit_with_failure(terminate=True)
      # check if device is in fastboot mode
      if stuff.count('fastboot') >= 1:
         if not options.serial_number:  # serial number not provided
            device_found = 1  # assume one device in fastboot
         else:
            if stuff.count(options.serial_number) > 0:  # required device is found
               device_found = 1
      if device_found:
         print "Found device in fastboot mode"
         return True
      else:
         if time.time() - st_time > 120:
            print  "Unable to find device in FastBoot state."
            print  "Retry Timeout !!! Exiting"
            fastboot_file_create("FAILURE_FASTBOOT")
            exit_with_failure(terminate=True)
         if time.time() - st_time > 10:
            print str(time.time() - st_time) + "sec - Retry attempt ", retry_count
            time.sleep(19)
         time.sleep(1)
         retry_count += 1

#---------------------------------------------------------#
# Function to verify the target state                     #
#---------------------------------------------------------#
def verify_adb():
   global adb
   adb_args = [adb]
   if options.serial_number:
      adb_args.extend(["-s", options.serial_number])
   adb_args.extend(['devices']) 
   stuff = subprocess.Popen(adb_args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
   print stuff  
   fastboot_args = [fastboot]
   fastboot_args.extend(['devices'])  
   fastboot_output = subprocess.Popen(fastboot_args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read() 
   if options.serial_number and stuff.count(options.serial_number) > 0:
     print "Device is in adb mode. Moving the device to fastboot mode"
     get_fastboot_mode()
   #adb devices output: "List of devices attached<\n>serial_number device", hence two 'device' for one device	 
   elif options.serial_number == None and fastboot_output.count('fastboot') == 0 and stuff.count('device') == 2:
     print "Device is in adb mode. Moving the device to fastboot mode"
     get_fastboot_mode()       
   

#---------------------------------------------------------#
# Function to erase partitions listed in contents         #
#---------------------------------------------------------#
def erase_extra_partitions():
   partition_list = mi.get_contents_parti_list(attr="fastboot_erase")
   if len(partition_list) > 0:
      print "Found list of partitions to erase:"
      print partition_list
      print 
      for partition_name in partition_list:
         fastboot_erase(partition_name)
   else:
      print "No partitions specified for erase"
      print "Proceeding with load...\n"

#---------------------------------------------------------#
# Function to erase a particular partition                #
#---------------------------------------------------------#
def fastboot_erase(partition_name):
   print "Erasing ", partition_name
   if options.serial_number :
      fastboot_command = [fastboot, '-s', options.serial_number, 'erase', partition_name]
   else :
      fastboot_command = [fastboot, 'erase', partition_name]
   print ' '.join(fastboot_command)
   stuff = subprocess.Popen(fastboot_command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
   print stuff
   fastboot_check(stuff, ok_count=1)

#---------------------------------------------------------#
# Function to flash                                       #
#---------------------------------------------------------#
def fastboot_flash(fastboot_command, erase=False):
   partition = fastboot_command[-2]
   if erase:  # separate flag to control when calling
      fastboot_erase(partition)  # command is a list
   print time.strftime("%c") + " : Starting to flash " + partition
   print ' '.join(fastboot_command)
   stuff = subprocess.Popen(fastboot_command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
   print stuff
   fastboot_check(stuff)
   print time.strftime("%c") + " : Complete"

#---------------------------------------------------------#
# Function to get fastboot command                        #
#---------------------------------------------------------#
def get_fastboot_cmd(var, file):
   file_name_w_ext = os.path.basename(file)
   file_name = os.path.splitext(file_name_w_ext)[0]
   partition_name = file_name if (var == 'true') else var
   # print "Command to load ", file_name_w_ext," in to ", partition_name, " partition..."
   fb_args = [fastboot]
   if options.serial_number:
      fb_args.extend(["-s", options.serial_number])
   if options.sparse_size:
      fb_args.extend(["-S", options.sparse_size + "M"])  # take in MB
   fb_args.extend(['flash', partition_name, file])
   return fb_args
      
#---------------------------------------------------------#
# Function to get fastboot command                        #
#---------------------------------------------------------#
def get_fastboot_mode():
   adb_args = [adb]
   if options.serial_number:
      adb_args.extend(["-s", options.serial_number])
   adb_args.extend(['reboot', 'bootloader']) 
   print "Trying to move the device to fastboot mode\n",adb_args  
   stuff = subprocess.Popen(adb_args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.read()
   print stuff

#---------------------------------------------------------#
# Function to handle fastboot failure case                #
#---------------------------------------------------------#
def fastboot_check(stuff, ok_count=2):
   if stuff.count('OKAY') < ok_count:
      exit_with_failure(False)
      print "Exiting"
      fastboot_file_create("FAILURE_FASTBOOT")
      time.sleep(60)
      sys.exit(1)
   if stuff.count('FAILED') > 0:
      exit_with_failure(False)
      print "Exiting"
      fastboot_file_create("FAILURE_FASTBOOT")
      time.sleep(60)
      sys.exit(1)
   return

#---------------------------------------------------------#
# Function to create success/failure fastboot files       #
#---------------------------------------------------------#
def fastboot_file_create(result):
   if result_path:
      outcome_fastboot = os.path.join(result_path, result + '.txt')
      FILE = open(outcome_fastboot, 'w')
      FILE.close()
   return

#---------------------------------------------------------#
# Define a handler to exit when Ctrl+C is pressed         #
#---------------------------------------------------------#
def interrupt_handler(signum, frame):
   print "Exiting"
   sys.exit(1)

#---------------------------------------------------------#
# Logging functions                                       #
#---------------------------------------------------------#
def print_linebreak():
   print "============================================================"

def exit_with_failure(terminate=True):
   print
   print_linebreak()
   print "  _          __     ___       __    _     ___     _  _"
   print " |_ |   /\  (_  |_|  |  |\ | /__   |_ /\   |  |  |_ | \ | | |"
   print " |  |_ /--\ __) | | _|_ | \| \_|   | /--\ _|_ |_ |_ |_/ o o o "
   print
   print_linebreak()
   if terminate:
      sys.exit(1)

def exit_with_success():
   print
   print_linebreak()
   print "FLASHING SUCCESSFUL!"
   # print "  _          __     ___       __    __      _  _  _  __  __  _"
   # print " |_ |   /\  (_  |_|  |  |\ | /__   (_  | | /  /  |_ (_  (_  |_ | | |  |"
   # print " |  |_ /--\ __) | | _|_ | \| \_|   __) |_| \_ \_ |_ __) __) |  |_| |_ o"
   # print
   print_linebreak()

#---------------------------------------------------------#
# Option parser function                                  #
#---------------------------------------------------------#
def option_parser():
   parser = OptionParser(usage="usage: %prog [options] filename", version="%prog 1.0")
   parser.add_option("--ap", "-a", action="store", type="string", dest="apps_path", help="APPS PATH")
   parser.add_option("--rp", "-r", action="store", type="string", dest="result_path", help="RESULT PATH")
   parser.add_option("--pf", "-p", action="store", type="string", dest="product_flavor", help="PRODUCT FLAVOR")
   parser.add_option("--st", "-s", action="store", type="string", dest="storage_type", help="STORAGE TYPE")
   parser.add_option("--sn", "-n", action="store", type="string", dest="serial_number", help="SERIAL NUMBER")
   parser.add_option("--il", "-i", action="store", type="string", dest="image_list", help="IMAGE LIST TO FLASH")
   parser.add_option("--mb", "-m", action="store", type="string", dest="sparse_size", help="SPARSE SIZE LIMIT (MB)")
   parser.add_option("--sb", "-b", action="store_true", default=False, dest="secondary_boot", help="FLASH BINS AND PERF/SECONDARY BINS")
   parser.add_option("--sbonly", action="store_true", default=False, dest="secondary_boot_only", help="FLASH ONLY PERF/SECONDARY BINS")
   parser.add_option("--bponly", action="store_true", default=False, dest="backup_partition_only", help="FLASH BINS to BACKUP PARTITIONS")
   parser.add_option("--bp", action="store_true", default=False, dest="backup_partition", help="FLASH BINS to BACKUP PARTITIONS")
   parser.add_option("-c", action="store_true", default=False, dest="copy_local", help="CREATE LOCAL COPY")
   parser.add_option("-e", action="store_true", default=False, dest="no_erase", help="DISABLE ERASE")
   parser.add_option("-j", action="store_true", default=False, dest="jtagged", help="JTAGGED (SKIP CORE/PARTITIONS)")
   parser.add_option("-w", action="store_true", default=False, dest="wait_enable", help="ENABLE WAIT")
   parser.add_option("-d", action="store_true", default=True, dest="audible_signal", help="ENABLE AUDIBLE SIGNAL")
   parser.add_option("-f", action="store_true", default=False, dest="fastboot_mode", help="MOVE DEVICE TO FASTBOOT")
   parser.add_option("--nearest", action="store_true", default=False,dest="fb_nearest", help="PICK GEOGRAPHICALLY NEAREST LOCATION")   
   return parser.parse_args()

#---------------------------------------------------------#
# Function to get file list on required attributes        #
#---------------------------------------------------------#
def get_fastboot_files(images_to_load, attr_list, storage_type):
   var_dict = {}
   for each_attr in attr_list:
      # loop through each attribute
      attr_var_list = {}
      if not images_to_load:
         # get this attr vars for all images at once
         attr_var_list = mi.get_file_vars(attr=each_attr,
                                          flavor=options.product_flavor,
                                          critical=True,
                                          storage=storage_type)
      else:
         # get this attr vars for each image separately
         for each_image in images_to_load:
            image_attr_var_list = mi.get_file_vars(attr=each_attr,
                                                   build=each_image,
                                                   flavor=options.product_flavor,
                                                   critical=True,
                                                   storage=storage_type)
            # merge each image vars to attr vars
            for k, v in image_attr_var_list.iteritems():
               attr_var_list.setdefault(k, [])
               attr_var_list[k] += v
      # merg each attr vars to full vars
      for k, v in attr_var_list.iteritems():  # add each item to common dict
         var_dict.setdefault(k, [])
         var_dict[k] += v
   return var_dict


def myfilecopy(src, dst):
   fname = os.path.basename(src)
   dfile = os.path.join(dst, fname)
   if os.path.exists(dfile):
      print fname + " already exists in " + dst
      stime = os.stat(src).st_mtime
      dtime = os.stat(dfile).st_mtime
      print "Src file timestamp - ", stime
      print "Dst file timestamp - ", dtime
      if round(stime - dtime, 5) == 0:  # till 5 decimals
         # to avoid last digit truncation/rounding issues from shutil.copy2
         print "Files identical, so skipping copy \n"
         return
   print "Copying ", src
   print "     to ", dst
   shutil.copy2(src, dst)
   print


###########################################################
###                MAIN FASTBOOT LOGIC                  ###
###########################################################
def main(jtagged=False):
   global mi, options

   (options, args) = option_parser()
   attr_list = ["fastboot"]
   # support setting jtagged flag both ways
   jtagged = options.jtagged or jtagged
   if not jtagged:
      if options.backup_partition_only:
         attr_list[:] = []
         attr_list.append("backup_partition")
      else :
         attr_list.append("fastboot_complete")
         if options.backup_partition:
            attr_list.append("backup_partition")
   # print attr_list
   # Print some diagnostic info
   # --------------------------
   print
   print_linebreak()
   print
   print "Platform is:", sys.platform
   print "Python Version is:", sys.version
   print "Current directory is:", os.getcwd()

   
   # Load the Meta-Info file
   # -----------------------
   mi = ml.meta_info(fb_nearest=options.fb_nearest)
   common_path = mi.get_build_path('common')
   common_path = os.path.join(common_path, 'common', 'build')

   
   # Process the input arguments
   # ---------------------------
   print
   print_linebreak()
   print "fastboot_complete.py: Processing input arguments"
   print_linebreak()
   print "Argument List : " + str(sys.argv[1:])

   apps_path = ""
   new_apps = False
   # atleast 1 args provided
   if len(args) >= 2:
      apps_path = args[1]
      new_apps = True
      print "Specified new APPS PATH: " + apps_path
   elif options.apps_path:  # provided as option
      apps_path = options.apps_path
      new_apps = True
      print "Specified new APPS PATH: " + apps_path
   else:
      print "No new APPS PATH specified. Taking from META..."
      apps_path = mi.get_build_path('apps')  # not provided
   
   # atleast 2 args provided
   global result_path
   if len(args) >= 3:         
      result_path = args[2]
   else:
      result_path = options.result_path  # provided value or 'None'
   if result_path:
      print "Specified RESULT PATH: " + result_path
   
   # OPTIONS
   if options.product_flavor :
      print "Specified product flavor: " + options.product_flavor
   if options.storage_type:
      print "Specified storage type: " + options.storage_type
   else:
      supportedd_storage_types = mi.get_storage_types()
      options.storage_type = supportedd_storage_types[0] if len(supportedd_storage_types) > 0 else None
      print ("Storage type not specified, defaulting to " + str(options.storage_type))

   if options.serial_number:
      print "Specified serial number: " + options.serial_number
   
   images = options.image_list
   if images:
      print "Specified list of images to flash: " + images
   
   if options.copy_local:
      print "Specified option to copy binaries in local folder before flashing"
      temp_dir = tempfile.gettempdir()
      try:
         local_dir = os.path.join(temp_dir, 'fastboot_' + mi.get_build_id('common', 'builds_flat'))
      except TypeError:  # exception because of empty BuildID 
         local_dir = os.path.join(temp_dir, 'fastboot')
      print "   Creating directory: ", local_dir
      try: 
         if os.path.exists(local_dir):
            print "   Directory with same name exists"
            print "   Will copy updated binaries based on timestamp"
            os.remove(os.path.join(local_dir, "fastboot_cmds"))
         else:
            os.mkdir(local_dir)
         options.local_dir = local_dir  # save to options for accessing in 'main'
         options.local_ff = open(os.path.join(local_dir, "fastboot_cmds"), 'w')
      except OSError as e:
         print "   Unable to delete " + local_dir
         print e
         print "   Continuing without local copy..."
         options.copy_local = False

   print
   print "Apps path is:  ", apps_path
   print "Common path is:", common_path

   
   # Check apps path type
   #---------------------
   la_path = os.path.join(apps_path, 'LINUX', 'android')
   if os.path.exists(la_path):
      la_path_exists = True
   else:
      la_path_exists = False

      
   # Get fastboot location
   #----------------------
   global fastboot
   find_fastboot_from_build(la_path)  # will set to apps path or just 'fastboot'
   if options.copy_local and fastboot != "fastboot":
      print "Fastboot executable: " + fastboot
      # copy from apps path to local directory
      fbdir = os.path.dirname(fastboot)
      if fbdir:  # will be empty for local fastboot
         for f in (glob(os.path.join(fbdir, "*.exe")) + glob(os.path.join(fbdir, "*.dll"))):
            myfilecopy(f, options.local_dir) 
      # once copied point to local fastboot
      fastboot = "fastboot" 

   
   # -----------------------------------------------------------------------------------------------------#
   # If a list of image builds is provided to flash, need to check if any of those builds
   # have fat binaries that are pushed into non-hlos and add common to the list accordingly
   # -----------------------------------------------------------------------------------------------------#
   print
   print_linebreak()
   print "fastboot_complete.py: List of builds to flash"
   print_linebreak()
   images_to_load = None
   if images:
      images_to_load = images.split(',')
      print "Builds specified: ",
      print images_to_load
      mi_build_list = mi.get_build_list()
      invalid_builds = [x for x in images_to_load if x not in mi_build_list]
      if len(invalid_builds) > 0:
         print "One or more builds provided are invalid: ",
         print invalid_builds
         print "Please choose from: ",
         print mi_build_list
         exit_with_failure()
      if "common" not in images_to_load:
         for each_image in images_to_load:
            image_fat_files = mi.get_files(build=each_image, attr=fatbinary_attr_string, flavor=options.product_flavor)
            if len(image_fat_files) > 0:
               print each_image + " has fat binaries picked up in 'common' build"
               print "Need to flash 'common'(meta) build as a dependency"
               images_to_load.append("common")
               break
   else:
      print "No image builds specified. Continuing to load all..."

   # set flag whether to flash partitions
   # should do when loading full
   flag_flash_partis = not (images_to_load or jtagged or options.backup_partition_only or options.secondary_boot_only)
   
   
   #------------------------------------------------------------------------------------------------------#
   # Get a list of all file_paths with "fastboot" or "fastboot_complete" attribute
   # If the file contains "apps root path" and if user specified a new apps :
   #   then remove the apps root path from the file
   #   If LINUX/android is not there in the user specified apps path , then remove it from file
   #   append user specied path to file to make it a complete usable path
   # If fastboot attribute value is "true"
   #   partition name = file name
   # else
   #   partition name = fastboot attribute value
   #------------------------------------------------------------------------------------------------------#
   print
   print_linebreak()
   print "fastboot_complete.py: List of files"
   print_linebreak()
   try:
      var_dict = get_fastboot_files(images_to_load, attr_list, options.storage_type)
      gpt_var_dict = get_fastboot_files(None, ['gpt_file'], options.storage_type)
   except ml.MyException, e:
      print "One or more files in the build are missing"
      print e
      print "Cannot continue with full build flashing"
      exit_with_failure()

   # if options.storage_type:
   #   var_dict     = storage_type_filter(var_dict)
   #   gpt_var_dict = storage_type_filter(gpt_var_dict)

   if flag_flash_partis:
      pprint(gpt_var_dict)
      print
   pprint(var_dict)

   
   #--------------------------------------------------
   # Adjust the list
   # Might need to modify paths based on flags specied
   # Like secondary boot, copy local
   #--------------------------------------------------
   if options.secondary_boot or options.secondary_boot_only or options.copy_local or new_apps:
      print
      print_linebreak()
      print "fastboot_complete.py: File path modifications"
      print_linebreak()
      # Copy partition file to flash locally       
      if flag_flash_partis and options.copy_local:
         for var in gpt_var_dict:
            new_filelist = []
            for file in gpt_var_dict[var]:
               dpath, fname = os.path.split(file)
               myfilecopy(file, options.local_dir)
               file = fname  # keep just filename to be used in flashing
               options.local_ff.write(' '.join(get_fastboot_cmd(var, file)) + "\n")  # save commands
               new_filelist.append(file)
            gpt_var_dict[var] = new_filelist
      # Check list of secondary apps image from contents XML
      secondary_list = [sfile for sfile in mi.get_files(attr='secondary_boot', flavor=options.product_flavor, 
                       storage=options.storage_type,invalid_path=True)]
      # Legacy secondary apps boot image
      if(len(secondary_list) <= 0):
         secondary_list = ["boot.img", "system.img"]	  
      meta_appspath = mi.get_build_path('apps')
      # Filter file for secondary boot flashing.	  
      for var in var_dict.copy():
         new_filelist = []
         for file in var_dict[var]:
            dpath, fname = os.path.split(file)
            # Replacing new apps image
            if new_apps and meta_appspath in file:
               print "Changing " + file  # dummy os.path.join below to add trailing slash if not there
               file = os.path.join(dpath.replace(meta_appspath, os.path.join(apps_path, '')), fname)
               print "      to " + file
               if ('LINUX/android/' in file) and not(la_path_exists) :
                  file = file.replace('LINUX/android/', '')
                  print "      to " + file
               dpath, fname = os.path.split(file)  # update dpath since file path changed
            # Replace primary images with perf/secondary image files
            if options.secondary_boot or options.secondary_boot_only:
               secfile_match = False
               for secfile in secondary_list:
                  if os.path.basename(secfile) == fname:
                     # updating secondary boot file path for new apps
                     if new_apps and meta_appspath in secfile:
                        secfile = secfile.replace(meta_appspath, os.path.join(apps_path, ''))
                        if ('LINUX/android/' in secfile) and not(la_path_exists) :
                           secfile = secfile.replace('LINUX/android/', '')		
                     if(len(secfile) == len(fname)):  # only file name, consider Legecy secondary image path
                        secfile = os.path.join(dpath, "secondary-boot", fname)
                     if os.path.exists(secfile):
                        print "Replacing {0} with {1}".format(file, secfile)
                        file = secfile
                        dpath, fname = os.path.split(file)
                     else:
                        print "{1} does not exist. Keeping {0}".format(file, secfile)
                     secfile_match = True
                     break
               # Skip all files except apps secondary images 
               if options.secondary_boot_only and secfile_match is False:
                  print 'Skipping :' + file
                  continue
            # Copy flashable binaries to local drive
            if options.copy_local:
               myfilecopy(file, options.local_dir)
               file = fname  # keep just filename to be used in flashing
               options.local_ff.write(' '.join(get_fastboot_cmd(var, file)) + "\n")  # save commands
            # Updated file consider for flashing
            new_filelist.append(file)
         # Updating var dictionary to adjust modified flashing file list
         if(len(new_filelist) > 0):
            var_dict[var] = new_filelist
         else:
            var_dict.pop(var)
      
   if hasattr(options, 'local_ff'):  # same as using if options.copy_local
      options.local_ff.close()
   
   if options.fastboot_mode == True:   
      verify_adb()    
   # Flash partition layout
   #-----------------------
   if flag_flash_partis:
      # should flash partition only when
      # jtag is not done and flashing all images
      print
      print_linebreak()
      print "fastboot_complete.py: Flashing partition layout"
      print_linebreak()
      if options.copy_local:
         print "Changing working dir to " + options.local_dir
         os.chdir(options.local_dir)
      for var in sorted(gpt_var_dict.keys()):
         for file in gpt_var_dict[var] :
            verify_fastboot()
            fastboot_flash (get_fastboot_cmd(var, file))
      if options.no_erase:
         print "\n'NO_ERASE' flag is set. Skipping partition erase..."
      else:
         print
         print_linebreak()
         print "fastboot_complete.py: Erasing listed partitions"
         print_linebreak()
         erase_extra_partitions()

   
   # Flash binaries
   #---------------
   print
   print_linebreak()
   print "fastboot_complete.py: Flashing binaries"
   print_linebreak()
   if options.copy_local:
      print "Changing working dir to " + options.local_dir
      os.chdir(options.local_dir)
   for var in sorted(var_dict.keys()):
      for file in var_dict[var] :
         verify_fastboot()
         fastboot_flash (get_fastboot_cmd(var, file), not options.no_erase)
   
   
   # ------------------------------------------------------- #
   # Signal the interrupt handler to handle ctrl + C         #
   # ------------------------------------------------------- #
   signal.signal(signal.SIGINT, interrupt_handler)
   fastboot_file_create("SUCCESS_FASTBOOT")

   exit_with_success()
   if (options.wait_enable):
      print "fastboot_complete.py: Loading complete,Window will be closed in 1 minute"
      print "Hit Ctrl+C to exit"
      time.sleep(60)
   else :
      print "fastboot_complete.py: Loading complete. "
      time.sleep(5)
   print


###########################################################
###                   DEFAULT EXECUTE                   ###
###########################################################
if __name__ == "__main__":
   # execute only if run as a script
   main()
