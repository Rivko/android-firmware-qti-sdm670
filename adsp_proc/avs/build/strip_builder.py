#===============================================================================
#
# Common Builders
#
# GENERAL DESCRIPTION
#    Common builders.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Technologies Proprietary.
#
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 03/20/2014 svutukur      Creation of strip_builder
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

#-------------------------------------------------------------------------------
# Append the required paths
#-------------------------------------------------------------------------------
sys.path.insert(0, '../../../../audio_avs/build/msm8909')



def exists(env):
   return env.Detect('strip_builder')
   
def generate(env):
   # init methods
   env.AddMethod(list_file_create_avs_txt, "list_file_create_avs_txt")

   # init builders
   
   # ListFileBuilder
   list_file_builder_generate_avs_test(env)
   #Sim mode flag addition.
   if ARGUMENTS.get('SIM') == "1" or ARGUMENTS.get('SIM') == "TRUE":
      env.PrintInfo("SIM is in the argument, build SIM AVS")
      env.AddUsesFlags(['USES_AVS_TEST'])
      env.Replace(AVS_TEST = True)
   #from build_config import chipset_specific_build_config
   #chipset_specific_build_config(env)

  
   return None

def list_file_builder_generate_avs_test(env):
   list_file_act_temp = env.GetBuilderAction(list_file_builder_avs_test)
   list_file_bld_temp = env.Builder(action = list_file_act_temp)
   env.Append(BUILDERS = {'ListFileBuilder_avs' : list_file_bld_temp})

def list_file_builder_avs_test(target, source, env):
   list_fname = str(target[0])
   print "check here"
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
         print "from here"		 
    else:
      for fname in file_list:
         list_file.write(env.RelPath(os.path.realpath(fname), rel_path_target, posix) + '\n')
         print "check here 2"
    list_file.close()
    
    print list_fname
    
	#Enforce rules from the following list. Add all the algorithm locations here
    algos_list=['afe/algos','aud/algorithms','ultrasound/algorithms','listen/algos','voice/algos']
    get_algos_list=[]
    lib_not_in_env=[]

    #Open the libs/objs txt file.
    #Check for the existence of the libs/objs txt file
    uses_lib=[]
    searchfile = open(list_fname, "r")
    print searchfile
    #Logic goes on here
    for line in searchfile:
        for algos in algos_list:
          if algos in line: 
            get_lib=line.split('/')[-1].upper().split('.')[0] #Get the lib name & convert to upper case
            get_algos_list.append(get_lib)
            #print get_lib
            # Remove .lib from the library name fetched && append 'USES_' to the every lib
            #Push the libs to the array
    print get_algos_list

    for every_algo_list in get_algos_list:
        uses_lib.append(''.join(['USES_', every_algo_list]))
        uses_lib.append(''.join(['USES_', every_algo_list,'_ONLY']))
    print uses_lib
    #print env.Dump()
    #Compare the grabbed libs with the libs sent to env using build_config.
    for eachlib in uses_lib:
        if eachlib in env:
            print "found in the env"
            print eachlib
        else:
            lib_not_in_env.append(eachlib)
            print eachlib
            print "check this too"
    if len(lib_not_in_env) > 0:
       print "Warning:: The below libs are algorithms, which needs to  be specifially mentioned with either strip/stip_only/stub in the build_config"
       print lib_not_in_env
       searchfile.close()
       #sys.exit(-1) #Exit with the error code
    searchfile.close()   

    #If it is ZQ based build then iteratively strip all the avs libs, including servies also
    if 'USES_ZM_STRIP_AUDIO_AVS' in env:
        print "Start to strip the avs_libs.txt if ZQ based builds"
        searchfile = open(list_fname, "rb").read().splitlines()
        #searchfile.close() #close the file
        print searchfile
        print "Debug 0"
        for line in searchfile:
            skip_stripping=False
            print "Current path from where strip happens:"
            print os.getcwd()
            #Folder paths which are added here will skip to strip. Currently all the avs services are un-stripped...
            skip_strip=['aud/services','afe/services','audio_avs/elite','listen/services','voice/services','audio_avs/vsd','ultrasound/services']
            for skip in skip_strip:
                if skip in line:
                   print "Services code not stripping:%s" %line
                   skip_stripping = True
                   
            get_path=os.getcwd()
            #os.chdir("../myps/tools")
            print os.getcwd()
            print line
            print "lib which is being stripped:"
            #check whether the lib path which we grabbed exists, if not send the error exit call to the build system
            if not os.path.exists(line):
               print "Error::This lib does not exist but grabbed from the avs_libs.txt", line
               sys.exit(-1)
            strip_cmd = ''.join([ '${STRIP} -x -w -K *xx_msg_* -K *xx_err_msg_* ', line])
            if skip_stripping == False:
               data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripped by avs builder on every library as it is ZM based build')
            #os.chdir(get_path)
            print os.getcwd() 
    
    

