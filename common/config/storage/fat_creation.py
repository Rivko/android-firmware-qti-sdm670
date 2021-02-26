#@PydevCodeAnalysisIgnore

'''
===============================================================================

 Fat Binary Creation

 GENERAL DESCRIPTION
    This script is a wrapper around fatgen.py and fatadd.py scripts
    to create fat file binaries based on passed arguments.

 Copyright (c) 2011-2012 by QUALCOMM, Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-----------------------------------------------------------------------------------------------------------------

  $Header: //components/rel/config.glue/30.0/storage/fat_creation.py#1 $
  $DateTime: 2017/12/12 09:30:16 $

------------------------------------------------------------------------------------------------------------------

fat_creation.py:

Usage:

      fat_creation.py  -a <attributes> -c <fat container name with relative path> -f <flavor> -t <fat tool name> 

=====================================================================================================================
'''

import sys, os, shutil
import traceback, re
# Hack until modules deliver split files
import stat, string, subprocess
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
   parser.add_option("-a","--attr",action="store",type="string",dest="attr",help="file attributes")
   parser.add_option("-c","--fatContainers",action="store",type="string",dest="container",help="containers with full paths")
   parser.add_option("-f","--flavor",action="store",type="string",dest="flav",help="flavor")
   parser.add_option("--fatgen",action="store",type="string",dest="fatgen",help="fatgen.py path")
   parser.add_option("--fatadd",action="store",type="string",dest="fatadd",help="fatadd.py path")
   parser.add_option("--ps",action="store",type="string",dest="partsize",help="partition size")
   parser.add_option("--ss",action="store",type="string",dest="secsize",help="sectorsize")
   (options, args) = parser.parse_args()

   mi = ml.meta_info(logger=lg)
   on_linux = sys.platform.startswith("linux")
   lg = lg.Logger('fat_creation')
   
   if not (options.attr or options.container or options.flav or options.fatgen or options.fatadd or options.partsize):
      print "Error: Invalid Syntax."
      print __doc__
      sys.exit(1)
   
   # Create fat container
   #---------------------
   lg.log("fat_creation.py: Specified container location:")
   lg.log("                 " + options.container)
   lg.log("fat_creation.py: Specified partition size: " + options.partsize)
   if os.path.exists(options.container):  # check and delete existing file
      lg.log("                 Deleting existing file")
      os.remove(options.container)
   if not os.path.exists(options.fatgen):     # check for fatgen tool
      lg.log("                 fatgen tool not found in")
      lg.log("                    " + options.fatgen)
      sys.exit(1)
   lg.log("                 Using fatgen from " + options.fatgen)
   # execute fatgen
   fatgen_cmd = ['python',options.fatgen,'-f','-s',options.partsize,'-n',options.container]
   if options.secsize:
      fatgen_cmd.extend(['--sectorsize', options.secsize])
   lg.log_exec(fatgen_cmd, verbose=0)
   
   # change container permissions
   os.chmod(options.container, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
      
   # Add each "fat_file" from contents.xml
   #--------------------------------------
   lg.log("fat_creation.py: Adding files into fat container:")
   lg.log("                 " + options.container)
   fatFiles = mi.get_files(attr = options.attr, flavor=options.flav)
   lg.log("fat_creation.py: Found " + str(len(fatFiles)) + " files")
   # Create sub directories in NON-HLOS.bin for each "sub_dir" from contents.xml
   subDirFiles = mi.get_files(attr = 'sub_dir', flavor=options.flav,abs=False)
   subDirFatFiles = {}
   if len(subDirFiles) > 0:
      lg.log("                 " + str(len(subDirFiles)) + "are 'subdir' files")
      for relfile in subDirFiles:
         for absfile in fatFiles:
            tempabsfile = absfile
            if on_linux == False:
               tempabsfile = absfile.replace('/','\\')
            if tempabsfile.find(relfile) != -1:
               subDirFatFiles[absfile] = os.path.dirname(relfile)

   for fat_file in fatFiles:
      fat_dest_dir = 'image'
      print fat_file
      if fat_file in subDirFatFiles:
          fat_dest_dir = os.path.join(fat_dest_dir,subDirFatFiles[fat_file])
      fatadd_cmd = ['python',options.fatadd,'-n',options.container, '-f' , fat_file,'-d', fat_dest_dir]
      if options.secsize:
         fatadd_cmd.extend(['--sectorsize',options.secsize])
      lg.log_exec(fatadd_cmd, verbose=0)
