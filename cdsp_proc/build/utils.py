#===============================================================================
#                    Copyright 2009 QUALCOMM Technologies Incorporated.
#                           All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0.c3/utils.py#1 $
#  $DateTime: 2018/08/07 05:36:15 $
#  $Change: 16823152 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when       who            what, where, why
# --------   ---        ---------------------------------------------------------
#   06/12/13   corinc    Script to call the QC-SCons build System for Badger    

#===============================================================================
import os
import re
import sys
import datetime
import subprocess
import socket
import shutil
import platform
import fileinput
from time import sleep
from datetime import timedelta

variant = os.environ.get('VARIANT_NAME')


def adspsobin_create():
   cwd_dir = os.getcwd()      
   os.system('chmod +x ./build/ext4fs_tools/ubuntu/make_ext4fs')
   os.system('chmod +x ./build/ext4fs_tools/ubuntu/simg2img')
   output_16_folder = './build/dynamic_signed/%s'%(variant)
   output_bin_folder = ''.join([cwd_dir + "/build/dynamic_signed/%s"%(variant)])
   output_bin_folder_client = os.path.abspath(output_bin_folder)
   if not os.path.exists(output_bin_folder_client):
      os.makedirs(output_bin_folder_client)
   print '\nCreating the binary out of the Dynamic Shared Objects'
   if platform.system() == 'Linux':
      proc = subprocess.Popen("./build/ext4fs_tools/ubuntu/make_ext4fs -s -T -1 -S ./build/ext4fs_tools/ubuntu/file_contexts -L dsp -l 16777216 -a dsp "+output_16_folder+"/adspso_sparse.bin ./build/dynamic_signed/%s/shared_obj"%(variant), shell=True)
      (out, err) = proc.communicate()
      if err:
         print err
      proc = subprocess.Popen("./build/ext4fs_tools/ubuntu/simg2img "+output_16_folder+"/adspso_sparse.bin "+output_16_folder+"/adspso.bin", shell=True)
      (out, err) = proc.communicate()
      if err:
         print err
   else: 
      print platform.system()
      proc = subprocess.Popen(".\\build\\ext4fs_tools\\windows\\make_ext4fs_win -s -T -1 -S .\\build\\ext4fs_tools\\windows\\file_contexts -L dsp -l 16777216 -a dsp .\\build\\dynamic_signed\\%s\\adspso_sparse.bin .\\build\\dynamic_signed\\%s\\shared_obj"%(variant,variant), shell=True)
      (out, err) = proc.communicate()
      if err:
         print err
      proc = subprocess.Popen(".\\build\\ext4fs_tools\\windows\\simg2img_win .\\build\\dynamic_signed\\%s\\adspso_sparse.bin .\\build\\dynamic_signed\\%s\\adspso.bin"%(variant,variant), shell=True)
      (out, err) = proc.communicate()
      if err:
         print err

def oem_root_shared_libs_copy():
   print '\nCopying shared libraries from OEM_ROOT to adsp_proc/build/dynamic_signed/shared_obj'
   if platform.system() == 'Linux':
      if os.environ.get('OEM_ROOT')!=None:
        proc = subprocess.Popen("cp -r "+os.environ.get('OEM_ROOT')+"build/dynamic_modules_hash/%s/* ./build/dynamic_signed/%s/shared_obj"%(variant,variant), shell=True)
        (out, err) = proc.communicate()
        if err:
            print err
   else: 
      print platform.system()
      if os.environ.get('OEM_ROOT')!=None:
        proc = subprocess.Popen("xcopy "+os.environ.get('OEM_ROOT')+"\\build\\dynamic_modules_hash\\%s .\\build\\dynamic_signed\\%s\\shared_obj /Y"%(variant,variant), shell=True)
        (out, err) = proc.communicate()
        if err:
             print err

def create_timestamp(env):
   print "==================Adding Domain Name to TimeStamp=================="
   domain=socket.getfqdn()
   Dname = domain.partition('.')[2]
   if Dname:
      DOMAIN_NAME=Dname
   else:
      DOMAIN_NAME="NODOMAIN"
   print DOMAIN_NAME
   
   print "==================Creating timestamp and q6_build_version header files=================="
   print env['BUILD_VERSION']
   str_build_version = "#define QDSP6_BUILD_VERSION " + env.subst('$BUILD_VERSION') 
   file = open('./q6_build_version.h', 'w')
   file.write(str_build_version)
   file.close()
   
   timestamp_dir = os.getcwd()
   print "\n\n Time Stamp working directory is %s" % timestamp_dir

   current_time = datetime.datetime.now()
   timestamp_value = current_time.strftime("Q6_BUILD_TS_%a_%b_%d_%H:%M:%S_PST_%Y")
   if os.path.exists('./qcom_timestamp.h'):
      print "qcom_timestamp header file already generated from CRM build, check the build version from existing header file"
      ReadFile = open('./qcom_timestamp.h', 'r')
      for line in ReadFile:
         match = re.split('QCOM time:', line)[1]
         if match: 
            os.environ['OS_ENV_QCOM_TIME_STAMP'] = match[:-2]
      ReadFile.close()  
   else:
      QTime= "QCOM time:" + timestamp_value + "_" + env.subst('$BUILD_VERSION') 
      os.environ['OS_ENV_QCOM_TIME_STAMP'] = timestamp_value + "_" + env.subst('$BUILD_VERSION')
      str_timestamp_qcom = "const char qcom_timestampstring[] = \"" + QTime + "\";"
      file = open('./qcom_timestamp.h', 'w')
      file.write(str_timestamp_qcom)
      file.close()
   
   target_name =os.environ.get('CHIPSET')
   EnggTime= "ENGG time:" + timestamp_value + "_" + target_name + "_" + DOMAIN_NAME
   os.environ['OS_ENV_ENGG_TIME_STAMP'] = timestamp_value
   str_timestamp_engg = "const char engg_timestampstring[] = \"" + EnggTime + "\";"   
   filepath = './engg_timestamp.h'
   if os.path.exists(filepath):
      os.system('chmod 777 '+filepath+'')
      os.remove(filepath)
   file = open(filepath, 'w')
   file.write(str_timestamp_engg)
   file.close()
   
#   current_time = date | sed 's/ /_/g'
#   file.write(current_time)

def init_chipset(env):
  
  #print env.Dump()
  print "current PLATFORM is " + env['PLATFORM']
  chipset=env.subst('$CHIPSET')
  print chipset
  # userargs = os.environ.get('BUILD_USERARGS',None)
  # print userargs
  build_flavor=env.subst('$BUILD_FLAVOR')
  print build_flavor
  build_act=env.subst('$BUILD_ACT')
  print build_act
  build_flags=env.subst('$BUILD_FLAGS')
  print "Printing the build Flags information \n"
  print build_flags
  build_verbose=env.subst('$BUILD_VERBOSE')
  print build_verbose
  build_numcore=env.subst('$BUILD_NUMCORE')
  print build_numcore
  build_component=env.subst('$BUILD_COMPONENT')
  print build_component
  build_filter=env.subst('$BUILD_FILTER')
  print build_filter
  build_sconsargs=env.subst('$BUILD_SCONSARGS')
  print build_sconsargs
 
  

  if env['PLATFORM'] == 'posix':
     env.Replace(SCONS_CMD_SCRIPT = "python build.py")
  else:
     env.Replace(SCONS_CMD_SCRIPT = "python build.py")
  
  env.Replace(IMAGE_ALIAS = "${CHIPSET}_MPD")   
  variant = os.environ.get('VARIANT_NAME')
  if 'adsp' in variant:
     env.Replace(IMAGE_ALIAS = "${CHIPSET}_ADSP")  
  elif 'cdsp' in variant:
     env.Replace(IMAGE_ALIAS = "${CHIPSET}_CDSP")  
  elif 'slpi' in variant:
     env.Replace(IMAGE_ALIAS = "${CHIPSET}_SLPI")  
  
  pl = os.environ.get('PRODUCT_LINE')
  if variant == None:
    variant = 'AAAAAAAA'
  env.Replace(SCONS_CMD = "${SCONS_CMD_SCRIPT} ${IMAGE_ALIAS} BUILD_ID=%s BUILD_VER=1234 PRODUCT_LINE=%s" %(variant, pl))
  if build_component:
     build_component = build_component.replace(',', ' ')
     env.Replace(IMAGE_ALIAS = build_component)
  if build_filter:
     env.Append(SCONS_CMD = " --filter="+build_filter)
  
  if build_numcore:
     env.Replace(SCONS_CMD_NUMCORE = "cd build/ms && ${SCONS_CMD} -j "+build_numcore)
  else:
     env.Replace(SCONS_CMD_NUMCORE = "cd build/ms && ${SCONS_CMD} -j 8")

  if build_verbose:
     env.Replace(SCONS_FLAVOR_CMD = "${SCONS_CMD_NUMCORE} --verbose="+build_verbose)
  else:
     env.Replace(SCONS_FLAVOR_CMD = "${SCONS_CMD_NUMCORE} --verbose=1")
     
  if build_sconsargs:
     opts_sconsargs = build_sconsargs.replace(',', ' ')
     env.Append(SCONS_FLAVOR_CMD = " "+opts_sconsargs)
  if os.path.exists('./obj'):
      splitelf_dir = './obj'
      shutil.rmtree(splitelf_dir)
  if build_act=='clean':
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD} -c")
     env.Replace(CLEAN_CMD = "clean")
     print "clean engg_timestamp"
     engg_timestamp_file = './engg_timestamp.h'
     os.remove(engg_timestamp_file)
     print "clean split images"
  else:
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD}")
  
  # if build_flags:
     # env.Append(SCONS_FLAVOR_ACT_CMD = " USES_FLAGS="+build_flags)
       
def execute_buildcmd(env):
   variant = os.environ.get('VARIANT_NAME')
   print "==================delete the elfs from previous build if exist=================="
   dspelf_file = './dsp_%sQ.elf'%variant
   if os.path.exists(dspelf_file):
      os.remove(dspelf_file)
   adsp_link_dir = './build/bsp/adsp_link/build/%s/'%(variant)
   if os.path.exists(adsp_link_dir):
      adsp_link_files = os.listdir(adsp_link_dir)
      print adsp_link_files
      os.chdir(adsp_link_dir)
      for file in adsp_link_files:
         os.remove(file)
      os.chdir('../../../../..')

   print "==================execute the build startup command=================="
   cwd_dir = os.getcwd()
   print "\n\nBuild working directory is %s" % cwd_dir
   chipset = env.subst('$CHIPSET')
   external_pack = ''.join([cwd_dir + "/build/ms/tbc_cleanpack.py"])
   if not os.path.exists(external_pack) and not 'cleanpack' in env['BUILD_SCONSARGS']:
       variant = os.environ.get('VARIANT_NAME')
       if not env.subst('$BUILD_COMPONENT') and 'BUILD_BAREBONE' not in env.subst('$BUILD_FLAGS'):
           scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD')
           os.system(scons_flavor_act_cmd)
       else:
           if 'BUILD_BAREBONE' not in env.subst('$BUILD_FLAGS'):
                print "Alias given from the top Build Command\n"
           else:
                print "BAREBONE Build Being Triggered\n"
           scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD')
           os.system(scons_flavor_act_cmd)
   else: #This is External Build Just perform Static Image Generation
       print "External Build Being Triggered\n" 
       if (os.environ.get('OEM_ROOT', None)) and not env.subst('$BUILD_COMPONENT') and ('GEN_SHARED_LIBS' in env.subst('$BUILD_FLAGS')):
            print "Compiling for shared libs"
            saved_alias = env.subst('$IMAGE_ALIAS')
            env.Replace(IMAGE_ALIAS = 'custom_shared_libs')
            scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD')
            os.system(scons_flavor_act_cmd)
            env.Replace(IMAGE_ALIAS = saved_alias)
       oem_root_shared_libs_copy()
       scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD')
       os.system(scons_flavor_act_cmd)
       # if os.path.exists('./build/ext4fs_tools'):
          # adspsobin_create()
       # else:
          # print "Please download ext4fs tools to add the oem shared library to adspso.bin"
   cwd_dir = os.getcwd()
   print "\n\nCurrent working directory is %s" % cwd_dir

def split_proc(env):
   curr_path = os.environ['PATH']
   os.environ['PATH']=':'.join([curr_path, os.environ['HEXAGON_ROOT']+'/'+os.environ['HEXAGON_RTOS_RELEASE']+'/Tools/bin'])
   print "================== Running splitter ==================="
   if os.path.exists('./dsp_%sQ.elf'%variant):
     split_dir = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/LA/system/etc/firmware/')

     split_dir_unsigned = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/unsigned/LA/system/etc/firmware/') 
     split_dir_signed = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/signed/LA/system/etc/firmware/')
     split_dir_signed_encrypted = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/signed_encrypted/LA/system/etc/firmware/')

     obj_dir_unsigned = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/unsigned/') 
     obj_dir_signed = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/signed/')
     obj_dir_signed_encrypted = os.path.dirname('./obj/qdsp6v5_ReleaseG/'+variant+'/signed_encrypted/')


     root_dir = os.getcwd()
     image_name = os.environ.get('IMAGE')

     if not os.path.exists(split_dir):
         os.makedirs(split_dir)
     print "================== Running splitter on MBN at default locationn (/obj/qdsp6v5_ReleaseG/)==================="
     SPLIT_CMD = "cd "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/LA/system/etc/firmware && python "+root_dir+"/qdsp6/scripts/pil-splitter.py "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/%s.mbn %s"%(image_name,image_name)
     os.system(SPLIT_CMD)
     print "================== Running splitter on non-reloc==================="
     os.system(SPLIT_CMD)

     if os.path.exists(obj_dir_unsigned):
        if not os.path.exists(split_dir_unsigned):
           os.makedirs(split_dir_unsigned)
        print "================== Running splitter on unsigned==================="
        SPLIT_CMD = "cd "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/unsigned/LA/system/etc/firmware && python "+root_dir+"/qdsp6/scripts/pil-splitter.py "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/unsigned/%s.mbn %s"%(image_name,image_name)
        os.system(SPLIT_CMD)
     if os.path.exists(obj_dir_signed):
        if not os.path.exists(split_dir_signed):
           os.makedirs(split_dir_signed)
        print "================== Running splitter on signed==================="
        SPLIT_CMD = "cd "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/signed/LA/system/etc/firmware && python "+root_dir+"/qdsp6/scripts/pil-splitter.py "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/signed/%s.mbn %s"%(image_name,image_name)
        os.system(SPLIT_CMD)
     if os.path.exists(obj_dir_signed_encrypted):
        if not os.path.exists(split_dir_signed_encrypted):
           os.makedirs(split_dir_signed_encrypted)
        print "================== Running splitter on signed_encryted==================="   
        SPLIT_CMD = "cd "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/signed_encrypted/LA/system/etc/firmware && python "+root_dir+"/qdsp6/scripts/pil-splitter.py "+root_dir+"/obj/qdsp6v5_ReleaseG/"+variant+"/signed_encrypted/%s.mbn %s"%(image_name,image_name)
        os.system(SPLIT_CMD)
   else:
     print "dsp.elf is not generated!!!"
     sys.exit(0)

def elf_extractor_proc(env):
   if os.path.exists('./build/bsp/adsp_link/build/%s/ADSP_PROC_IMG_%sQ.elf.map'%(variant,variant)): 
      print "================== Running hexagon elf extractor script ==================="
      env.Replace(ELF_EXTRACTOR_PROC_CMD = "python ./qdsp6/scripts/hexagon_elf_extractor.py --target=adsp --elf=./build/bsp/adsp_link/build/%s/ADSP_PROC_IMG_%s.elf"%(variant,variant))
      elf_extractor_proc_cmd=env.subst('$ELF_EXTRACTOR_PROC_CMD')
      os.system(elf_extractor_proc_cmd)

def memory_proc(env):

   build_flavor=env.subst('$BUILD_FLAVOR')
   chipset_name=env.subst('$CHIPSET')
   target_name=chipset_name[3:]
   tools_version_used = os.environ.get('HEXAGON_RTOS_RELEASE', None)   
   image_name = os.environ.get('IMAGE', None)
   tools_major_version_used = int(tools_version_used[0])      
   elf_name_normal=os.path.join('./build/ms/','ROOT_%sQ.elf'%variant)
   if not os.path.exists(elf_name_normal):
       elf_name_normal="".join(['M',target_name,'AAAAAAAAQ1234.elf'])
       elf_name_normal=os.path.join("./build/ms/",elf_name_normal)
   
   elf_name_audio=os.path.join("./build/ms/",'AUDIO_%sQ.elf'%variant)
   if not os.path.exists(elf_name_audio):
       elf_name_audio="".join(['M',target_name,'AAAAAAAAQ1234_AUDIO.elf'])
       elf_name_audio=os.path.join("./build/ms/",elf_name_audio)   

   elf_name_sensor=os.path.join("./build/ms/",'SENSOR_%sQ.elf'%variant)
   if not os.path.exists(elf_name_sensor):
       elf_name_sensor="".join(['M',target_name,'AAAAAAAAQ1234_SENSOR.elf'])
       elf_name_sensor=os.path.join("./build/ms/",elf_name_sensor)   

   print "================== Running memory profile script ==================="
   if os.path.exists('./build/bsp/adsp_link/build/%s/ADSP_PROC_IMG_%sQ.elf.map'%(variant,variant)):
          env.Replace(MEMORY_PROC_CMD1 = "python ./performance/tools/Memory/Image_Break_Down.py ${CHIPSET} ./build/bsp/adsp_link/build/%s/ADSP_PROC_IMG_%sQ.elf.map CommonPD %s %s"%(variant,variant,tools_major_version_used,image_name))   
          memory_proc_cmd1=env.subst('$MEMORY_PROC_CMD1')
          os.system(memory_proc_cmd1)   	
   if os.path.exists('./build/bsp/avs_adsp_user/build/%s/AUDIO_IMG_%sQ.elf.map'%(variant,variant)):
          env.Replace(MEMORY_PROC_CMD1 = "python ./performance/tools/Memory/Image_Break_Down.py ${CHIPSET} ./build/bsp/avs_adsp_user/build/%s/AUDIO_IMG_%sQ.elf.map AudioPD %s %s"%(variant,variant,tools_major_version_used,image_name))   
          memory_proc_cmd1=env.subst('$MEMORY_PROC_CMD1')
          os.system(memory_proc_cmd1)     
   if os.path.exists('./build/bsp/sensor_img/build/%s/SENSOR_IMG_%sQ.elf.map'%(variant,variant)):
          env.Replace(MEMORY_PROC_CMD1 = "python ./performance/tools/Memory/Image_Break_Down.py ${CHIPSET} ./build/bsp/sensor_img/build/%s/SENSOR_IMG_%sQ.elf.map SensorsPD %s %s"%(variant,variant,tools_major_version_used,image_name))   
          memory_proc_cmd1=env.subst('$MEMORY_PROC_CMD1')
          os.system(memory_proc_cmd1)  
   if os.path.exists('./build/bsp/cdsp_root/build/%s/CDSP_PROC_IMG_%sQ.elf.map'%(variant,variant)):
          env.Replace(MEMORY_PROC_CMD1 = "python ./performance/tools/Memory/Image_Break_Down.py ${CHIPSET} ./build/bsp/cdsp_root/build/%s/CDSP_PROC_IMG_%sQ.elf.map CommonPD %s %s"%(variant,variant,tools_major_version_used,image_name))   
          memory_proc_cmd1=env.subst('$MEMORY_PROC_CMD1')
          os.system(memory_proc_cmd1)
   if os.path.exists('./build/dynamic_modules/%s'%(variant)):
          env.Replace(MEMORY_PROC_CMD1 = "python ./performance/tools/Memory/DynamicModuleSize.py ./build/dynamic_modules/%s/"%(variant))   
          memory_proc_cmd4=env.subst('$MEMORY_PROC_CMD1')
          os.system(memory_proc_cmd4)

   if env['PLATFORM'] == 'posix':
          env.Replace(HEXAGON_ANALYZER_BACKEND_CMD = "hexagon-analyzer-backend")
   else:
          env.Replace(HEXAGON_ANALYZER_BACKEND_CMD = "hexagon-analyzer-backend.exe")
    
   if os.path.exists('./build/bsp/adsp_link/build/%s/ADSP_PROC_IMG_%sQ.elf.map'%(variant,variant)):
          env.Replace(MEMORY_PROC_CMD3 = ("$HEXAGON_ANALYZER_BACKEND_CMD --aidfile ./qdsp6/tools/input/libcAidFile.txt --elffile %s -o ./build/ms > ./build/ms/hexagon_StackAnalyzer_Log.txt "%elf_name_normal))
          memory_proc_cmd3=env.subst('$MEMORY_PROC_CMD3')
          os.system(memory_proc_cmd3)
             
          if os.path.exists('./build/ms/RA_FunctionStackSizes.csv'):
            if os.path.exists('./build/ms/RA_FunctionStackSizes_normal.csv'):
                os.remove('./build/ms/RA_FunctionStackSizes_normal.csv')
            os.renames('./build/ms/RA_FunctionStackSizes.csv','./build/ms/RA_FunctionStackSizes_normal.csv')
          if os.path.exists('%s'%elf_name_audio):
            env.Replace(MEMORY_PROC_CMD4 = ("$HEXAGON_ANALYZER_BACKEND_CMD --aidfile ./qdsp6/tools/input/libcAidFile.txt --elffile %s -o ./build/ms > ./build/ms/hexagon_StackAnalyzer_Log.txt "%elf_name_audio))
            memory_proc_cmd4=env.subst('$MEMORY_PROC_CMD4')
            os.system(memory_proc_cmd4)
          os.system("python ./qdsp6/tools/stack_size.py %s %s"%(build_flavor,elf_name_audio))	


#Initialize environmentn 
def Init(env):

   os.chdir('../')
   env_init_dir = os.getcwd()
   print "\n\n Env init working directory is %s" % env_init_dir

   #InitChipset method
   env.AddMethod(init_chipset,"InitChipset")

   #create timestamp
   env.AddMethod(create_timestamp, "CreateTimestamp")

   #execute the build startup command
   env.AddMethod(execute_buildcmd, "ExecuteBuildCmd")

   #execute the split_proc command
   env.AddMethod(split_proc, "SplitProc")

   #execute the memory_proc command
   env.AddMethod(memory_proc, "MemoryProc")

   #execute the hexagon_elf_extractor command
   #env.AddMethod(elf_extractor_proc, "ElfExtractorProc")
    
