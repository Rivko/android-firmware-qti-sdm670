#===============================================================================
#
# Common Builders
#
# GENERAL DESCRIPTION
#    Common builders.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Change: 14402555 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/21/10   wd      Create
#
#===============================================================================
import os
import glob
import re
import sys
import hashlib
import subprocess
import string
import fnmatch
import shutil
import textwrap
import imp
from datetime import datetime
from optparse import OptionParser

def build_producst_file(target, source, image):
   # Well this is all very hacky since we do everything in the emitter,
   # what does this mean? It means that SCons has no clue about what
   # dependencies are needed for creating or updating this file and/or
   # explain why this got build or updated with options like
   # --debug=explain. Also it has the bad habit to update or create files
   # even if this rule is not needed. Another unbecoming of SCons behavior.
   # we should really change how the CRM infrastructure works instead
   # of hacking this to adapt to it!!

   #Future input variables (TODO)
  
   target_full_str = str(target)
   source_full_str = str(source)
   print ("source_full_str: " + source_full_str)

   TARGET_ROOT = os.path.realpath(os.path.join(os.path.dirname(__file__), "../../.."))
   
   if ( os.path.abspath(TARGET_ROOT) == os.path.abspath(target) ):
     BUILD_PRODUCT_FILE = TARGET_ROOT + "/BuildProducts.txt"
   else:
     BUILD_PRODUCT_FILE = target + "/BuildProducts.txt"
     TARGET_ROOT = target
   
   build_products_file_path  = os.path.realpath(BUILD_PRODUCT_FILE)
   print ("build_products_file_path: " + build_products_file_path)  
   print ("target root: " + TARGET_ROOT) 
   image_file = "./" + os.path.relpath(source,TARGET_ROOT) + "/" +(image) 
   print ( "image_file: " + image_file)

   #env.SideEffect(target, build_products_file_path)
   
   if os.path.exists(build_products_file_path):
      build_prod_file = open(build_products_file_path, "r")
      build_prod_data = build_prod_file.read()
      build_prod_file.close()
   else:
      build_prod_data = ""

   # add lines for this image.
   if build_prod_data.find(image_file) == -1:
      build_prod_file = open (build_products_file_path, "w")
      build_prod_data = "".join([build_prod_data, image_file, "\n"])
      build_prod_file.write(build_prod_data)
      build_prod_file.close()
   else:
      build_prod_data = ""

#   return (target, source)

def printHelpMenu():
  print "Usage: generic_builder.py <target location> <source location> <image name> \n"

def main():
   if len(sys.argv) < 2:
     printHelpMenu()
     sys.exit()
   elif '-h' in ' '.join(sys.argv):
     printHelpMenu()
     sys.exit()

   # Parsing input 
   parser = OptionParser()

   parser.add_option("-t", "--target-dir",
                    action="store", type="string", dest="targetdir_name",
                    default=".",
                    help="Target Directory name.  Default is root directory")

   parser.add_option("-s", "--source-dir",
                    action="store", type="string", dest="sourcedir_name",
                    default=".",
                    help="Image Source directory .  Default is root directory")

   parser.add_option("-i", "--image",
                    action="store", type="string", dest="image_name",
                    default="uefi.mbn",
                    help="Image name. Default is 'uefi.mbn'")

   (options, args) = parser.parse_args()

   build_producst_file(options.targetdir_name, 
                      options.sourcedir_name, 
                      options.image_name)   

if __name__=="__main__":
  main()

#build_producst_file("/local/mnt/workspace/shirleyy/builds/boot.xf.1.0.xblbuildproduct", "/local/mnt/workspace/shirleyy/builds/boot.xf.1.0.xblbuildproduct/QcomPkg/Msm8996Pkg/bin" , "uefi.mbn")
