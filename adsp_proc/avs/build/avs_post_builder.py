#===============================================================================
#
# Common Builders
#
# GENERAL DESCRIPTION
#    Common builders.
#
# Copyright (c) 2009-2010 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Technologies Proprietary.
#
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/30/2014 svutukur      Creation of avs_post_builder
#
#===============================================================================
import os
import glob
import re
import hashlib
import subprocess
import string
import fnmatch
import shutil
import textwrap
import imp
import SCons.Action
from SCons.Script import *
from datetime import datetime
import sys 
import SCons.Action
from SCons.Script import *

#-------------------------------------------------------------------------------
# Append the required paths
#-------------------------------------------------------------------------------






def exists(env):
  
   return env.Detect('avs_post_builder')
   
def generate(env):
   # init methods
   env.AddMethod(list_file_create_avs_txt, "list_file_create_avs_txt")

   # init builders
   get_inc_root=env.subst('${INC_ROOT}')
   join_path="".join([get_inc_root,'/','avs','/build/'])
   path_to_insert=os.path.join(env.RealPath(join_path),'%s') % env['CHIPSET']
   print path_to_insert
   if os.path.exists('../../../../audio_avs'):
      sys.path.insert(0, path_to_insert)
   if os.path.exists('../../../../avs'):
      sys.path.insert(0, path_to_insert)
   import_build_config = 'from build_config_'+env.get('CHIPSET')+' '+'import chipset_specific_build_config'
   # ListFileBuilder
   list_file_builder_generate_avs_test(env)
   exec(import_build_config)
   chipset_specific_build_config(env)

  
   return None

def list_file_builder_generate_avs_test(env):
   list_file_act_temp = env.GetBuilderAction(list_file_builder_avs_test)
   list_file_bld_temp = env.Builder(action = list_file_act_temp)
   env.Append(BUILDERS = {'AVSListFileBuilder' : list_file_bld_temp})

def list_file_builder_avs_test(target, source, env):
   list_fname = str(target[0])
   #import pdb;pdb.set_trace()   
   list_file_create_avs_txt(env, list_fname, source, env.get('add_header', False), 
      env.get('relative_path'), env.get('posix', False), env.get('sort', False))
   
def list_file_create_avs_txt(env, list_fname, file_list, add_header=False, rel_path_target=None, posix=False, sort=False):
    import re
    import os
    import sys
    import string
    banner = "" 
    if add_header:
      banner = env.CreateFileBanner(os.path.split(list_fname)[1])
      
    list_file = open (list_fname, "w")
    list_file.write(banner)
    
    file_list = [str(fname) for fname in file_list]
   
    if sort:
      file_list = sorted(file_list)

    if rel_path_target is None:
      for fname in file_list:
         list_file.write(env.RealPath(fname, posix) + '\n')    
         
    else:
      for fname in file_list:
         list_file.write(env.RelPath(os.path.realpath(fname), rel_path_target, posix) + '\n')
    #import pdb;pdb.set_trace()         
    print list_fname
    
    list_file.close()
    
    print list_fname
    
    #Copy all the needed shared libraries to avs\aDSPTest\Media\shared_object folder
    #Create a folder shared_object
    if 'USES_AVS_TEST' in env:
       print "mypath is %s" % os.getcwd()
       print env.subst('${AVS_ROOT}')
       #Create the directory
       sim_gen_so=''.join([env.subst('${INC_ROOT}'),'/avs','/main/aDSPTest','/Media','/shared_objects'])
       #import pdb;pdb.set_trace()
       dest_so=''.join([env.subst('${INC_ROOT}'),'/avs','/build/dynamic_modules/',env.subst('${BUILDPATH}')])
       if os.path.exists(sim_gen_so):
          
          for file in os.listdir(dest_so):
             shutil.copy(dest_so+'/'+file,sim_gen_so+'/'+file)
       
    
	
    

