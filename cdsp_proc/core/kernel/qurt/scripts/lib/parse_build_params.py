#=============================================================================
#
#                                    parse_build_params.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
import os, sys
import re

def parse_build_params(cfg):
   pathname = os.path.dirname(sys.argv[0]) 
   pathname = os.path.join(pathname,"Input","build_params.txt")
   f_build_param = open(pathname, "r")
   asic_pattern = re.compile("Q6VERSION=")
   config_flags_pattern = re.compile("BUILD_CONFIG_FLAGS=")
   return_pattern = re.compile("\n")

   for line in f_build_param:
     if asic_pattern.search(line) != None:
       asic = re.sub(asic_pattern,'',line)
       asic = re.sub(return_pattern,'',asic)
       asic = asic.strip()
     if config_flags_pattern.search(line) != None:
       build_flags = re.sub(config_flags_pattern,'',line)
       build_flags = re.sub(return_pattern,'',build_flags)

   cfg.build_flags.extend(build_flags.split())
   cfg.asic = asic
