#============================================================================
#
# QuRT Multi PD image builder rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to create multi PD image and relocated ELF files
#
# Copyright  2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1.c4/bsp/build/scripts/qurt_mpd_image_builder.py#1 $
#  $DateTime: 2018/07/30 01:21:36 $
#  $Author: pwbldsvc $
#  $Change: 16759227 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 04/06/12   NG        Converted the kernel script into a scons compatible builder
#============================================================================

import os
import sys
from SCons.Script import *

#get arguments
isOutput = 0;
isPaddr = 0;

def exists(env):
   return env.Detect('generate_mpd_boot_image')

#-----------------------------------------------------------------------------
#Description:
#This builder is used by Multi PD build
#This builder takes the input Elfs and creates the merged output image [bootimage.pbn]
#and relocates the input elfs based on the physical address mentioned in the scons.
#!!Note: The firs input has to be the guest OS elf
#--------------------------------------------------------------------------------

def generate(env):
   rootenv = env.get('IMAGE_ENV')
   #rootenv.AddMethod(query_section_by_name_util, "QuerySectionByName")
   mpd_image_act = env.GetBuilderAction(generate_mpd_boot_image)
   mpd_image_bld = env.Builder(action = mpd_image_act, 
                         emitter = mpd_image_emitter
                         )
   rootenv.Append(BUILDERS = {'MultiPDImageBuilder' : mpd_image_bld})

def mpd_image_emitter(target, source, env):
    return (target, source)

def generate_mpd_boot_image(target, source, env):
   #import pdb;pdb.set_trace()
   guest_os = env.RealPath(str(source[0]))
   output=env.RealPath(str(target[0]))
   paddr=str("${QDSP6_MPD_PHYS_ADDR}");
   images = [];

   for src in source:
      if os.path.isfile(env.RealPath(str(src))):
        images.append(env.RealPath(str(src)));
      else:
        print(src+" is not a valid file.");
        exit();

   if (os.name == 'posix') : #linux
       obj_copy = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-objcopy";
       obj_dump = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-objdump";
       strip = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-strip";
       MPD_BOOT_IMG_BLDR = "${QDSP6_QURT_TOOLDIR}/qdsp6-image-build";
       
   else:                     #windows
       obj_copy = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-objcopy${EXE_EXT}";
       obj_dump = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-objdump${EXE_EXT}";
       strip = "${QDSP6_RELEASE_DIR}/gnu/bin/hexagon-strip${EXE_EXT}";
       MPD_BOOT_IMG_BLDR = "${QDSP6_QURT_TOOLDIR}/qdsp6-image-build${EXE_EXT}"
       guest_os = guest_os.replace("/", "\\");
       output = output.replace("/", "\\");
       for image in images:
           images[images.index(image)] = image.replace("/", "\\");
   
   #address temp file
   tmp_file = guest_os.replace("elf", "addr");
   
   #strip symbols from QuRT os and app images and save in temp pbn files
   for image in images:
       strip_cmd=strip+" -s "+image+" -o "+image.replace("elf","pbn");
       env.ExecCmds(strip_cmd)
   
   #weave images into boot image with given physical address
   img_bld_cmd = MPD_BOOT_IMG_BLDR;
   for image in images:
       img_bld_cmd += " "+image.replace("elf","pbn");
   img_bld_cmd += " -p "+ paddr + " -o "+output+">"+tmp_file;
   env.ExecCmds(img_bld_cmd, target=output)
   
   #create temp file with address offsets to create relocated images
   delta_paddr= [];
   f=open(tmp_file, "r");
   lines = f.readlines()
   for line in lines:
       list = line.split()
       if len(list) >= 2:
           delta_paddr.append(str(int(list[0], 16) - int(list[1], 16))) 
   f.close();
   
   # delete all temp files
   #import pdb;pdb.set_trace() #ng_dbg
   os.remove(guest_os.replace("elf", "addr"));
   for image in images:
       os.remove(image.replace("elf", "pbn"));
   
   count = 0;
   # updated symbol files (ELFs) with correct physical addresses
   for image in images:
	if len(delta_paddr) >= 1:
           start_addr = delta_paddr[count];
	if int(start_addr) >= 0:
		start_addr = "+"+start_addr;
	obj_copy_cmd = obj_copy+" --change-start "+start_addr+" --change-section-lma .sdata"+start_addr+" --change-section-lma .start"+ \
                       start_addr+" --change-section-lma .init"+start_addr+" --change-section-lma .plt"+start_addr+\
                       " --change-section-lma .text"+start_addr+" --change-section-lma .fini"+\
                       start_addr+" --change-section-lma .rodata"+start_addr+" --change-section-lma uapp"+start_addr+\
                       " --change-section-lma .dynsym"+start_addr+" --change-section-lma .dynstr"+start_addr+" --change-section-lma .hash"+start_addr+\
                       " --change-section-lma .data"+start_addr+" --change-section-lma .dynamic"+start_addr+" --change-section-lma .eh_frame"+\
                       start_addr+" --change-section-lma .gcc_except_table"+start_addr+" --change-section-lma .ctors"+start_addr+\
                       " --change-section-lma .dtors"+start_addr+" --change-section-lma .got"+start_addr+" --change-section-lma .interp"+start_addr+\
                       " --change-section-lma .bss"+start_addr+" --change-section-lma .sdata"+start_addr+\
                       " --change-section-lma .sbss"+start_addr+" --change-section-lma .DEVCFG_DATA"+\
                       start_addr+" "+image+" "+str(target[count+1]);
	env.ExecCmds(obj_copy_cmd)
        #os.system(obj_dump+" -dClx "+image+" > "+image.replace("elf","dump"));
	count += 1;
