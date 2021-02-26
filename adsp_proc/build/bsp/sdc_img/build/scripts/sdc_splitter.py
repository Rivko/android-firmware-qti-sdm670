#===============================================================================
#
# Split SDC image to code and data binaries
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2017 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#
#===============================================================================
import os
import subprocess
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sdc_splitter')

def generate(env):
   sdc_splitter_generate(env)

#===============================================================================
# SDC Splitter
#===============================================================================
def sdc_splitter_generate(env):

   rootenv = env.get('IMAGE_ENV')

   #-------------------------------------------------------------------------------
   # L4 SCL Header builder definition
   # 
   split_act = env.GetBuilderAction(sdc_splitter)
   split_bld = env.Builder(action = split_act)

   env.Append(BUILDERS = {'SdcSplitter' : split_bld})

   # Determine whether we are building on Windows or Linux, and set the
   # objcopy version accordingly.
   rootenv['objcopy'] = None
   if sys.platform.startswith('lin'):
      rootenv['objcopy'] = '/usr/bin/objcopy'
   elif sys.platform.startswith('win'):
      try:
         p = subprocess.Popen("objcopy --version ", stdout=subprocess.PIPE, shell=False)
         rootenv['objcopy'] = 'objcopy'
      except:
         try:
             p = subprocess.Popen("C:\\utils\\Cygwin\\bin\\objcopy.exe --version ", stdout=subprocess.PIPE, shell=False)
             rootenv['objcopy'] = 'C:\\utils\\Cygwin\\bin\\objcopy.exe'
         except:
            pass
   else:
      env.PrintError("sdc_splitter.py: Unable to detect whether this platform")
      env.PrintError("is Windows or Linux, and cannot set the objdump tool")
      env.PrintError("appropriately.")
      exit(1)

   if rootenv['objcopy'] is None:
      env.PrintError("Unable to detect objcopy.  Please ensure that it is")
      env.PrintError("available on your workstation.")
      exit(1)

#-------------------------------------------------------------------------------
# SDC splitter action
# 
def sdc_splitter(target, source, env):
   # Get platform hexagon-elfcopy
   # Create coderam bin and dataram bin

   env.PrintInfo("Inside sdc_splitter")
   # Two output files
   code_ram_bin = str(target[0])
   data_ram_bin = str(target[1])
   # Input elf file
   input_sdc_elf = str(source[0])

   objcopy = env.get('objcopy') 

   # Generate the code_ram_bin file.
   p = subprocess.Popen(objcopy + " --output-target=binary -I elf32-little --only-section=CODE_RAM " + input_sdc_elf + " " + code_ram_bin, stdout=subprocess.PIPE, shell=True)
   p.communicate()
   if p.returncode:
      sys.exit(1)

   # Generate the data_ram_bin file.
   p = subprocess.Popen(objcopy + " --output-target=binary -I elf32-little --strip-debug --remove-section=CODE_RAM " + input_sdc_elf + " " + data_ram_bin, stdout=subprocess.PIPE, shell=True)
   p.communicate()
   if p.returncode:
      sys.exit(1)

   return None

