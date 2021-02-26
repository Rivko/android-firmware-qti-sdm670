# @PydevCodeAnalysisIgnore

'''
====================================================================================================

 fat_creation.py:

 GENERAL DESCRIPTION
    This script is a wrapper around fatgen.py and fatadd.py scripts
    to create fat file binaries based on passed arguments.

 Usage:
    fat_creation.py  -a <attributes> -c <fat container name with relative path> -f <flavor> -t <fat tool name> 

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------------
  $Header: //components/rel/build.glue/1.0/app/fat_creation.py#12 $
  $DateTime: 2018/02/28 13:19:29 $
=====================================================================================================
'''

import sys, os, shutil
import traceback, re
# Hack until modules deliver split files
import stat, string, subprocess
import json
import time
import datetime
from optparse import OptionParser
from glob import glob

if os.path.exists('lib/meta_lib.py'):
   print "Componentized build format detected. Importing libraries from /common/build/lib"
   sys.path.append('lib')
else:
   sys.path.append(__file__)
   print ("Importing libraries from " + __file__)
import meta_lib as ml
import meta_log as lg
from xml.etree import ElementTree as et
bin_file_dests = dict()


#------------------------------------------#
# Execute when run as a file               #
#------------------------------------------#
if __name__ == '__main__': 
   parser = OptionParser()
   parser.add_option("-a", "--attr", action="store", type="string", dest="attr", help="file attributes")
   parser.add_option("-c", "--fatContainers", action="store", type="string", dest="container", help="containers with full paths")
   parser.add_option("-f", "--flavor", action="store", type="string", dest="flav", help="flavor")
   parser.add_option("--fatgen", action="store", type="string", dest="fatgen", help="fatgen.py path")
   parser.add_option("--fatadd", action="store", type="string", dest="fatadd", help="fatadd.py path")
   parser.add_option("--ps", action="store", type="string", dest="partsize", help="partition size")
   parser.add_option("--ss", action="store", type="string", dest="secsize", help="sectorsize")
   (options, args) = parser.parse_args()
   
   if not (options.attr or options.container or options.flav or options.fatgen or options.fatadd or options.partsize):
      print "Error: Invalid Syntax."
      print __doc__
      sys.exit(1)
   # Initialize log file
   logfile = 'fat_creation' 
   fcname = os.path.splitext(os.path.basename(options.container))[0]
   if len(fcname) > 0:
      logfile += '_' + fcname.lower()
   lg = lg.Logger(logfile)   
   mi = ml.meta_info(logger=lg)
   on_linux = sys.platform.startswith("linux")
   # Create fat container
   #---------------------
   lg.log("fat_creation.py: Specified container location:")
   lg.log("                 " + options.container)
   # fatgen can't take float partition size, consider only int part
   lg.log("fat_creation.py: Specified partition size: " + options.partsize)
   options.partsize = options.partsize.split('.')[0]
   lg.log("fat_creation.py: Float partition can't be used for fat creation, hence truncating it. New partition size : " + options.partsize)
   if os.path.exists(options.container):  # check and delete existing file
      lg.log("                 Deleting existing file")
      os.remove(options.container)
   if not os.path.exists(options.fatgen):  # check for fatgen tool
      lg.log("                 fatgen tool not found in")
      lg.log("                    " + options.fatgen)
      sys.exit(1)
   lg.log("                 Using fatgen from " + options.fatgen)
   # execute fatgen
   fatgen_cmd = ['python', options.fatgen, '-f', '-s', options.partsize, '-n', options.container]
   if options.secsize:
      fatgen_cmd.extend(['--sectorsize', options.secsize])
   return_code = lg.log_exec(fatgen_cmd, verbose=0)
   if (return_code != 0):
      err_msg = "                  " + fatgen_cmd + ":Failed"   
      lg.log(err_msg)
      sys.exit(1)	  
   
   # change container permissions
   os.chmod(options.container, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
         	
   # Add each "fat_file" from contents.xml
   #--------------------------------------
   lg.log("fat_creation.py: Adding files into fat container:")
   lg.log("                 " + options.container)
   fatFiles = mi.get_files(attr=options.attr, flavor=options.flav)
   lg.log("fat_creation.py: Found " + str(len(fatFiles)) + " files")
   # Create sub directories in NON-HLOS.bin for each "sub_dir" from contents.xml
   subdir_file_vars = mi.get_file_vars(attr='sub_dir', flavor=options.flav, abs=False)
   subDirFatFiles = {}
   if len(subdir_file_vars) > 0:
      lg.log("Files found with 'sub_dir' attributes")
      for subdir_var, subdir_files in subdir_file_vars.iteritems():
         # default dest directory path
         destdir_path = ''
         create_subdirs = subdir_var.lower() == 'true'
         if subdir_var.endswith('...'):
            destdir_path = subdir_var[:-3]
            create_subdirs = True
         elif subdir_var.lower() != 'true' and subdir_var.lower() != 'false':
            destdir_path = subdir_var
         # Iterates sub_dir var in content xml file to figure-out fatfile dest folder 
         for relfile in subdir_files:
            for absfile in fatFiles:
               tempabsfile = absfile
               if on_linux == False:
                  tempabsfile = absfile.replace('/', '\\')
               if tempabsfile.find(relfile) != -1:
                  if(create_subdirs):
                     subDirFatFiles[absfile] = os.path.join(destdir_path, os.path.dirname(relfile))
                  else:
                     subDirFatFiles[absfile] = os.path.join(destdir_path, '')

   # Adding fat files to NON-HLOS.bin
   for fat_file in fatFiles:
      fat_dest_dir = 'image'
      lg.log(fat_file)
      if fat_file in subDirFatFiles:
          fat_dest_dir = os.path.join(fat_dest_dir, subDirFatFiles[fat_file])
      fatadd_cmd = ['python', options.fatadd, '-n', options.container, '-f' , fat_file, '-d', fat_dest_dir]
      if options.secsize:
         fatadd_cmd.extend(['--sectorsize', options.secsize])
      return_code = lg.log_exec(fatadd_cmd, verbose=0)
      if (return_code != 0):
         err_msg = "                  " + fatadd_cmd + ":Failed"   
         lg.log(err_msg)
         sys.exit(1)
      st = os.stat(options.container)
      lg.log(str(options.container) + " size after adding   " + str(fat_file) + ": " + str(st.st_size) + " bytes")
      lg.log(str(options.container) + " last modified time stamp   " + ": " + str(time.ctime(st.st_mtime)))
      

   if  (options.fatgen and options.container and (options.container.find('NON-HLOS.bin') != -1)):
      bid_file = options.container
      bid_file = bid_file.replace('NON-HLOS.bin', 'Ver_info.txt')
      if os.path.exists(bid_file):
         lg.log("                 Deleting existing " + bid_file)
         os.remove(bid_file)
      if os.path.exists('Ver_info.txt'):
         lg.log("  Deleting existing common/build/Ver_info.txt file")
         os.remove('Ver_info.txt')
      lg.log("Creating the file Ver_info.txt  to hold the Meta information at " + bid_file)
      Product_Info = {}
      Metabuild_Info = {}
      Image_Info = {}
      BuildID = mi.get_build_id('common', 'builds_flat')
      Product_Info['Version'] = '1.0'
      Metabuild_Info['Meta_Build_ID'] = BuildID
      ts = time.time()
      Metabuild_Info['Time_Stamp'] = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
      Metabuild_Info['Product_Flavor'] = options.flav
      Product_Info['Metabuild_Info'] = Metabuild_Info
      build_ids = mi.get_build_list()
      build_ids = build_ids
      for  tag in build_ids:
         si_buildid = mi.get_build_id(tag)
         if si_buildid == None or len(si_buildid)<=0:
            si_buildid = mi.get_build_path(tag)
         Image_Info[tag] = si_buildid 
         # Image_Info = OrderedDict(sorted(Image_Info.items(), key=lambda t: t[0]))
      Product_Info['Image_Build_IDs'] = Image_Info
      with open(bid_file, 'w') as outfile:
         json.dump(Product_Info, outfile, sort_keys=True, indent=4)
      lg.log("fat_creation.py: Adding ver_info.txt into fat container:")
      fat_dest_dir = 'verinfo'
      fatadd_cmd = ['python', options.fatadd, '-n', options.container, '-f' , bid_file, '-d', fat_dest_dir]
      if options.secsize:
         fatadd_cmd.extend(['--sectorsize', options.secsize])
      
      lg.log_exec(fatadd_cmd, verbose=0)
      if (return_code != 0):
         err_msg = "                  " + fatadd_cmd + ":Failed"   
         lg.log(err_msg)
         sys.exit(1)
      st = os.stat(options.container)
      lg.log(str(options.container) + " size after adding   " + str(bid_file) + ": " + str(st.st_size) + " bytes")
      lg.log(str(options.container) + " last modified time stamp   " + ": " + str(time.ctime(st.st_mtime)))
      if ((int(options.partsize)*1024*1024) < st.st_size):
        err_msg = "Error : " + str(options.container) + " size exceeded the partition size hence failing the build"
        lg.log(err_msg)
        sys.exit(1)
