#============================================================================
#
#/** @file elf_gen.py
#
# GENERAL DESCRIPTION
#   
#
# Copyright 2016 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#**/
#
#----------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who                   what, where, why
# --------   ---------------       ------------------------------------------------------
# 03/15/17   dhaval                Added disassembling elf support and moved common 
#                                  functions to commons.py.
# 03/13/17   dhaval                Added support for disassembling elf without GUI, 
#                                  separated gui related logic to separate .py 
# 02/10/17   kpa                   Fix elf entry address population for config file input
# 01/17/16   kpa                   Fix segment type to ensure input segments show up as loadable
# 12/05/16   kpa                   Update page size, seg_start_offset computation
# 11/04/16   ck                    Replaced linux directory separators with os.path.join()
# 08/12/16   Dallas Delaney        Initial tool completion
#
#============================================================================
from optparse import OptionParser
import os
import sys
import shutil
from shutil import copyfile
import ntpath
import elf_gen_tools
from elf_gen_tools import *
import subprocess
import ConfigParser 
import json
import collections
import commons
from commons import *

# Add path to elf_gen_tools
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                'sectools','sectools','features','isc','parsegen'))

class elfStruct:
  def __init__(self):
      self.e_type = 0x1
      self.e_entryaddr = 0x0
      self.e_phoff = 0x0
      self.e_shoff = 0x0
      self.e_flags = 0x0
      self.e_phsize =0x0
      self.e_phnum = 0x1
      self.e_shentsize = 0x0
      self.e_shnum = 0x0
      self.is_elf_64_bit = False

class segInfoStruct:
  def __init__(self):
      self.old_seg_num = 0x1
      self.num_segs_added = 0x0
      self.num_deleted = 0x0                                                                                                                                        
      self.seg_num = 0

class phdrStruct:
  def __init__(self):
      self.s_type = []
      self.offset = []
      self.vaddr = []                                                                                                                                       
      self.paddr = []
      self.fsize = []
      self.msize = []
      self.flags = []
      self.acc_bits = []
      self.seg_bits = []
      self.align = []
      self.binary = []
      
###########################################################################################################
# findConfigFile - takes a json_file and returns the filename (with path). Also creates directories needed
###########################################################################################################
def findConfigFile(json_file):

  #Open config (JSON) file
  with open (json_file) as data_file: #open config file if it exists
    data = json.load(data_file)

  #Create Directories for 'elf_files' and 'config_files' if they don't exist
  if not data["config_output_path"]: #if no path input, default to working directory
    if not os.path.exists("elf_files"):
      os.makedirs("elf_files")
    if not os.path.exists(os.path.join("elf_files",
                                       "create_cli")):
      os.makedirs(os.path.join("elf_files",
                               "create_cli"))
  else:   #path given
    if not os.path.exists(os.path.join(data["config_output_path"],
                                       "elf_files")):
      os.makedirs(os.path.join(data["config_output_path"],
                               "elf_files"))
    if not os.path.exists(os.path.join(data["config_output_path"],
                                            "elf_files",
                                            "create_cli")):
      os.makedirs(os.path.join(data["config_output_path"],
                               "elf_files",
                               "create_cli"))

  #Create ELF File
  if not data["config_output_path"]: #if no path input, default to working directory
    if not data["config_output_file"]:  #no path given, no file given
      elf_file = os.path.join("elf_files",
                              "create_cli",
                              "config_elf.elf")
    else:                               #no path given, file given
      elf_file = os.path.join("elf_files",
                              "create_cli",
                              data["config_output_file"])
  else: #path given
    if not data["config_output_file"]:  #path given, no file given
      elf_file = os.path.join(data["config_output_path"],
                              "elf_files",
                              "create_cli",
                              "config_elf.elf")
    else:                               #path given, file given
      elf_file = os.path.join(data["config_output_path"],
                              "elf_files",
                              "create_cli",
                              data["config_output_file"])


  return elf_file

#####################################################################################
# config_error - Prints basic error message and termination message
#####################################################################################
def config_error():
  # print "\nFile NOT Created"
  print "\n******* Program Terminated Due To Errors *******\n"

  return

#####################################################################################
# config_parser - Creates an ELF file with information from the config (JSON) file
#####################################################################################
def config_parser(json_file, gui_option):

  hash_seg_num = 0
  entry_addr_valid = False

  #ERROR CHECKING **************************************************************
  if not os.path.isfile(json_file):  #make sure file is valid
    print "ERROR: Config file does not exist"
    config_error()
    return
  with open (json_file) as data_file: #open config file if it exists
    data = json.load(data_file)

  #Check binary, class, address overlap
  for i in range (int(data["elf_header"]["ph_num"],16)):  
    #binary check
    if data["program_header"]["segment"][i]["binary"]:  #if binary given, it must be valid. Else, error out
      if not os.path.isfile(data["program_header"]["segment"][i]["binary"]):  
        print "ERROR: Binary file does not exist for segment", i
        config_error()
        return
    else: #no binary given, load with 0xFF
      print "WARNING: Binary file not given for segment", i,"- segment will be filled with 0xFF"
    #class check
    if not (int(data["elf_header"]["e_ident"]["ei_class"],16) == int(data["program_header"]["segment"][i]["binary_class"],16)): 
      print "ERROR: Elf header class differs from binary class of segment", i
      config_error()
      return
    #hash check
    if(int(data["program_header"]["segment"][i]["segment_type"],16) == 0x2):
      hash_seg_num += 1
      if(hash_seg_num > 1):
        print "ERROR: Only one hash segment is allowed. Hash Segment is specified with value 0x2 in 'segment_type' field"
        config_error()
        return
    #entry_address check - must be between vaddr and file_size
    if ((int(data["elf_header"]["entry_addr"],16)) >= int(data["program_header"]["segment"][i]["vaddr"],16) and int(data["elf_header"]["entry_addr"],16) <int( data["program_header"]["segment"][i]["vaddr"],16) + int(data["program_header"]["segment"][i]["file_size"],16)):
      entry_addr_valid = True

    #address overlap check
    for j in range (int(data["elf_header"]["ph_num"],16)):
      if(i != j):  #dont check for overlap on same segment
        memsize = int(data["program_header"]["segment"][i]["mem_size"],16)
        tmp_memsize = int(data["program_header"]["segment"][j]["mem_size"],16)

        vaddr_start = int(data["program_header"]["segment"][i]["vaddr"],16)
        tmp_vaddr_start = int(data["program_header"]["segment"][j]["vaddr"],16)
        paddr_start = int(data["program_header"]["segment"][i]["paddr"],16)
        tmp_paddr_start = int(data["program_header"]["segment"][j]["paddr"],16)
        vaddr_end = vaddr_start + memsize             #Should only consider mem size, not file
        paddr_end = paddr_start + memsize
        tmp_vaddr_end = tmp_vaddr_start + tmp_memsize
        tmp_paddr_end = tmp_paddr_start + tmp_memsize
        #Vaddr check
        if((vaddr_start < tmp_vaddr_end and vaddr_start >= tmp_vaddr_start) or (vaddr_end < tmp_vaddr_end and vaddr_end > tmp_vaddr_start) or (vaddr_start >= tmp_vaddr_start and vaddr_end < tmp_vaddr_end) or (vaddr_start <= tmp_vaddr_start and vaddr_end > tmp_vaddr_end)): # FIND OVERLAP LOGIC
          print "ERROR: Virtual address overlap (vaddr + mem_size) with segments:", i, j
          config_error()
          return
        #Paddr check - COMMMENTED OUT SINCE VADDR AND PADDR LIKELY TO BE 1:1
        # if((paddr_start < tmp_paddr_end and paddr_start >= tmp_paddr_start) or (paddr_end < tmp_paddr_end and paddr_end >= tmp_paddr_start) or (paddr_start >= tmp_paddr_start and paddr_end < tmp_paddr_end) or (paddr_start <= tmp_paddr_start and paddr_end > tmp_paddr_end)): # FIND OVERLAP LOGIC
        #   print "Error: Physical address and mem size overlap with segments:" i, j
        #   config_error()
        #   return  
  #make sure entry address is valid
  if (entry_addr_valid == False):
    print "ERROR: Invalid entry point address. Must be between a segment's vaddr and file size"
    config_error()
    return      

  #make sure ph_num in elf header is less than or equal to segment number given in config file
  if len(data["program_header"]["segment"]) < int(data["elf_header"]["ph_num"],16):
    print "ERROR: ph_num is greater than number segments given in configuration file"
    return

  #ELF CREATION *********************************************************************

  #Place ELF File in specified location and create all directories needed
  elf_file = findConfigFile(json_file)   #creates directories for 'elf_files' and 'config_files' if they don't exist, and returns ELF filename
  elf_fp = elf_gen_tools.OPEN(elf_file, "wb+")

  #ELF HEADER INTILIALIZATION WITH JSON FILE
  if not data["config_output_path"]: #if no path input, default to working directory
    eh_file = os.path.join("elf_files",
                           "create_cli",
                           "config_eh.bin")
  else:                              #path input given
    eh_file = os.path.join(data["config_output_path"],
                           "elf_files",
                           "create_cli",
                           "config_eh.bin")
  eh_fp = elf_gen_tools.OPEN(eh_file, "wb+") 

  elf_struct = elfStruct()
  elf_struct.e_type = int(data["elf_header"]["type"],16)
  elf_struct.e_entryaddr = int(data["elf_header"]["entry_addr"],16)
  elf_struct.e_phoff = int(data["elf_header"]["ph_offset"],16)
  elf_struct.e_shoff = int(data["elf_header"]["sh_offset"],16)
  elf_struct.e_flags = int(data["elf_header"]["flags"],16)
  # elf_struct.e_phsize = int(data["elf_header"]["ph_size"],16)
  elf_struct.e_phnum = int(data["elf_header"]["ph_num"],16)
  elf_struct.e_shentsize = int(data["elf_header"]["sh_size"],16)
  elf_struct.e_shnum = int(data["elf_header"]["sh_num"],16)
  if int(data["elf_header"]["e_ident"]["ei_class"],16) == 0x1:
    elf_struct.is_elf_64_bit = False
  elif int(data["elf_header"]["e_ident"]["ei_class"],16) == 0x2:
    elf_struct.is_elf_64_bit = True
  else:
    print "Invalid elf header class. Must be value '1' for 32 bit or '2' for 64 bit"
    return
    
  #Create ELF Header (PARTIAL HEADER)
  elf_gen_tools.create_elf_header_gui(eh_file, 0, 0, elf_struct)  #creates an elf header 

  #create blank ELF image
  [elf_header, phdr_table] = \
    elf_gen_tools.preprocess_elf_file(eh_file) #creates headers

  #More ELF header editing
  elf_header.e_phnum = elf_struct.e_phnum
  elf_header.e_flags = elf_struct.e_flags
  elf_header.e_shentsize = elf_struct.e_shentsize
  elf_header.e_shnum = elf_struct.e_shnum
  elf_header.e_entry = elf_struct.e_entryaddr

  del phdr_table[0] #delete first ph entry given from 'create_elf_header_gui' function

  seg_start_offset = roundup(elf_header.e_phoff + elf_header.e_phentsize*elf_header.e_phnum, PAGE_SIZE)   
  
  #CREATE PROGRAM HEADERS AND APPEND TO PHDR_TABLE
  for i in range (elf_struct.e_phnum):
    #Update phdr. First entry is already there
    if(elf_struct.is_elf_64_bit): 
      new_phdr = elf_gen_tools.Elf64_Phdr('\0' * ELF64_PHDR_SIZE)
    else:
      new_phdr = elf_gen_tools.Elf32_Phdr('\0' * ELF32_PHDR_SIZE)
    new_phdr.p_offset = int(int(data["program_header"]["segment"][i]["offset"], 16) + seg_start_offset)
    new_phdr.p_vaddr  = int(data["program_header"]["segment"][i]["vaddr"],16)
    new_phdr.p_paddr  = int(data["program_header"]["segment"][i]["paddr"],16)
    new_phdr.p_filesz = int(data["program_header"]["segment"][i]["file_size"],16)
    new_phdr.p_memsz  = int(data["program_header"]["segment"][i]["mem_size"],16)
    new_phdr.p_align  = int(data["program_header"]["segment"][i]["addr_alignment"],16)
    new_phdr.p_type   = int(data["program_header"]["segment"][i]["segment_type"],16)
    #Decode flags
    pflags_var = int(data["program_header"]["segment"][i]["flags"],16)
    access_type_var = 0x0
    segment_type_var = 0x0
    if( int(data["program_header"]["segment"][i]["access_type"],16) & 0x1 == 0x1): #check first bit
      access_type_var += 0x200000
    if( int(data["program_header"]["segment"][i]["access_type"],16) & 0x2 == 0x2): #check first bit
      access_type_var += 0x400000
    if( int(data["program_header"]["segment"][i]["access_type"],16) & 0x4 == 0x4): #check first bit
      access_type_var += 0x800000
    if( int(data["program_header"]["segment"][i]["segment_type"],16) & 0x1 == 0x1): #check first bit
      segment_type_var += 0x1000000
    if( int(data["program_header"]["segment"][i]["segment_type"],16) & 0x2 == 0x2): #check first bit
      segment_type_var += 0x2000000
    if( int(data["program_header"]["segment"][i]["segment_type"],16) & 0x4 == 0x4): #check first bit
      segment_type_var += 0x4000000

    new_phdr.p_flags  = pflags_var + access_type_var + segment_type_var

    phdr_table.append(new_phdr)

  #WRITE ELF HEADER INTO FILE
  if elf_struct.is_elf_64_bit == False:
    elf_header.e_phoff = ELF32_HDR_SIZE
    elf_header.e_phentsize = ELF32_PHDR_SIZE
    elf_fp.write(elf_gen_tools.Elf32_Ehdr.getPackedData(elf_header))
  else:
    elf_header.e_phoff = ELF64_HDR_SIZE 
    elf_header.e_phentsize = ELF64_PHDR_SIZE
    elf_fp.write(elf_gen_tools.Elf64_Ehdr.getPackedData(elf_header))


  #WRITE PHDR_TABLE INTO FILE - update phdr file location and write individual phdrs in phdr table
  phdr_offset = elf_header.e_phoff
  for i in range (elf_header.e_phnum):
    elf_fp.seek(phdr_offset)
    if elf_struct.is_elf_64_bit == False:
      elf_fp.write(elf_gen_tools.Elf32_Phdr.getPackedData(phdr_table[i]))
    else:
      elf_fp.write(elf_gen_tools.Elf64_Phdr.getPackedData(phdr_table[i]))
    phdr_offset += elf_header.e_phentsize # increment phdr_offset to next location


  #WRITE EACH SEGMENT INTO FILE
  for i in range (elf_header.e_phnum):      
    segment_offset = roundup(int(data["program_header"]["segment"][i]["offset"],16) + seg_start_offset, int(data["program_header"]["segment_alignment_offset"],16))
    seg_size = int(data["program_header"]["segment"][i]["file_size"],16)
    if os.path.isfile(data["program_header"]["segment"][i]["binary"]):  #binary check                 
      bin_fp = elf_gen_tools.OPEN(data["program_header"]["segment"][i]["binary"], "rb")
      seg_bytes_written = elf_gen_tools.file_copy_offset(bin_fp,
                 0,
                                               elf_fp,
                 segment_offset,
                                               seg_size )
      #Append with 0's if size of binary is smaller than 'fielsize' entry
      binary_size = os.stat(data["program_header"]["segment"][i]["binary"]).st_size    #gives size of binary listed in bytes
      if(binary_size < int(data["program_header"]["segment"][i]["file_size"],16)):       #specified offset is larger than file size - append with 0's
        elf_fp.seek(segment_offset + binary_size)  #go to line in modified ELF
        for i in range (seg_size - binary_size):
          elf_fp.write(b'\xFF')
      bin_fp.close()

    else: #No binary given - appened segment with 0xFF per byte
      elf_fp.seek(segment_offset)  #go to correct offset in modified ELF
      for i in range (seg_size):
          elf_fp.write(b'\xFF')


  #Delete elf header file
  eh_fp.close()
  if not data["config_output_path"]:
    os.remove(os.path.join("elf_files",
                           "create_cli",
                           "config_eh.bin"))
  else:
    os.remove(os.path.join(data["config_output_path"],
                           "elf_files",
                           "create_cli",
                           "config_eh.bin"))

  if not (gui_option):  #program not termintaed if GUI still running after this function
    print "\n" + ELF_GENERATOR_SCRIPT + ": Generated elf \"" + elf_file + "\"\n"  
  elf_fp.close()

  return 

#************************************************************* GUI CODE ****************************************************************************

#################################################################################
# create_modified_elf - Creates ELF with GUI entries in specified output path
#################################################################################
def create_modifided_elf(original_offset_list, elf_fp, phdr_table, elf_header, is_out_elf_64_bit, segment_added, val_list, original_elf_filename, file_struct, entry_struct, seg_struct, initial_phdr_struct, phdr_struct):

  num_segs_added = seg_struct.num_segs_added
  seg_num = seg_struct.seg_num
  # print "segnum", seg_num

  #Create 'elf_files' and 'elf_files/generated' directories if they don't exist
  if not file_struct.output_file.get(): #if nothing in entry, default to working directory
    if not os.path.exists("elf_files"):
      os.makedirs("elf_files")
    if not os.path.exists(os.path.join("elf_files",
                                       "modified")):
      os.makedirs(os.path.join("elf_files",
                               "modified"))
  else:
    if not os.path.exists(os.path.join(str(file_struct.output_file.get()),
                                       "elf_files")):
      os.makedirs(os.path.join(str(file_struct.output_file.get()),
                               "elf_files"))
    if not os.path.exists(os.path.join(str(file_struct.output_file.get()),
                                       "elf_files",
                                       "modified")):
      os.makedirs(os.path.join(str(file_struct.output_file.get()),
                               "elf_files",
                               "modified"))

  #Create file in given or default path
  if not file_struct.output_file.get(): #if nothing in entry, default to working directory
    elf_path = os.path.join("elf_files",
                            "modified",
                            "out_" + ntpath.basename(file_struct.input_file.get()))
  else:
    elf_path = os.path.join(str(file_struct.output_file.get()),
                            "elf_files",
                            "modified",
                            "out_" + ntpath.basename(file_struct.input_file.get()))
  m_elf_fp = elf_gen_tools.OPEN(elf_path, "wb+")  
  m_elf_fp.seek(0)
  m_elf_fp.truncate(0)  #clear contents of file before using (without this, contents remain in file if file size of segments are changed to be smaller)

  prev_offset_list = []

  # print "added segs: ", num_segs_added
  # print "phdr count: ", len(phdr_table)
  # print "offset count: ", len(original_offset_list), "\n"

  #update ELF header (only e_phnum), but save original number of segments
  old_seg_num = elf_header.e_phnum 
  elf_header.e_phnum = seg_num

  #write ELF header into file
  if is_out_elf_64_bit == False:
    m_elf_fp.write(elf_gen_tools.Elf32_Ehdr.getPackedData(elf_header))
  else:
    m_elf_fp.write(elf_gen_tools.Elf64_Ehdr.getPackedData(elf_header))

  #UPDATE PHDR (p_filesz, p_offset)
  phdr_offset = elf_header.e_phoff  # offset from file to start of phdrs
  for i in range (seg_num): 
    tmp = phdr_table[i].p_offset
    prev_offset_list.append(tmp)
    #Decode 'type' in GUI
    if val_list[i*11].get() == 'null' :
      ptype = 0x0
    elif val_list[i*11].get() == 'load' :
      ptype = 0x1
    elif val_list[i*11].get() == 'dynamic' :
      ptype = 0x2
    elif val_list[i*11].get() == 'interp' :
      ptype = 0x3
    elif val_list[i*11].get() == 'note' :
      ptype = 0x4
    elif val_list[i*11].get() == 'shlib' :
      ptype = 0x5
    elif val_list[i*11].get() == 'phdr' :
      ptype = 0x6
    elif val_list[i*11].get() == 'loproc' :
      ptype = 0x7000000
    elif val_list[i*11].get() == 'hiproc' :
      ptype = 0x7ffffff
    else :
      ptype = 0x1 #default is load 

    #Decode flags
    pflags_var = int(phdr_struct.flags[i].get(), 16) #bottom 3 bits - no need to decode
    access_type_var = 0x0
    segment_type_var = 0x0
    if(int(phdr_struct.acc_bits[i].get(), 16) & 0x1 == 0x1): #check first bit
      access_type_var += 0x200000
    if(int(phdr_struct.acc_bits[i].get(), 16) & 0x2 == 0x2): #check first bit
      access_type_var += 0x400000
    if(int(phdr_struct.acc_bits[i].get(), 16) & 0x4 == 0x4): #check first bit
      access_type_var += 0x800000
    if(int(phdr_struct.seg_bits[i].get(), 16) & 0x1 == 0x1): #check first bit
      segment_type_var += 0x1000000
    if(int(phdr_struct.seg_bits[i].get(), 16) & 0x2 == 0x2): #check first bit
      segment_type_var += 0x2000000
    if(int(phdr_struct.seg_bits[i].get(), 16) & 0x4 == 0x4): #check first bit
      segment_type_var += 0x4000000

    #phdr_table[i].p_flags = (pflags_var)
    phdr_table[i].p_flags = (pflags_var + access_type_var + segment_type_var)
    phdr_table[i].p_type = ptype  #type
    phdr_table[i].p_offset = int(Entry.get(phdr_struct.offset[i]), 16)  #offset
    phdr_table[i].p_vaddr = int(Entry.get(phdr_struct.vaddr[i]), 16)  #vaddr
    phdr_table[i].p_paddr = int(Entry.get(phdr_struct.paddr[i]), 16)  #paddr
    phdr_table[i].p_filesz = int(Entry.get(phdr_struct.fsize[i]), 16)  #filesize
    phdr_table[i].p_memsz = int(Entry.get(phdr_struct.msize[i]), 16)  #memsize
    phdr_table[i].p_align = int(Entry.get(phdr_struct.align[i]), 16)  #align
    # flag_var = 0
    # if(check_list[i*3].var.get() == 1):
    #   flag_var += 0x4
    # if(check_list[i*3+1].var.get() == 1):
    #   flag_var += 0x2
    # if(check_list[i*3+2].var.get()== 1):
    #   flag_var += 0x1
    # phdr_table[i].p_flags = flag_var

    # update output file location and write each phdr
    m_elf_fp.seek(phdr_offset)
    if is_out_elf_64_bit == False:
      m_elf_fp.write(elf_gen_tools.Elf32_Phdr.getPackedData(phdr_table[i]))
    else:
      m_elf_fp.write(elf_gen_tools.Elf64_Phdr.getPackedData(phdr_table[i]))

    # increment phdr_offset to next location
    phdr_offset += elf_header.e_phentsize #** this or file size?? **   

  #SEGMENT COPYING
  phdr_size = elf_header.e_phentsize*seg_num
  seg_start_offset = elf_header.e_phoff + phdr_size  #first seg offset is phdr offset + phdr size
  
  # offset the start of the segments just after the program headers
  [original_elf_header, original_phdr_table] = \
  elf_gen_tools.preprocess_elf_file(original_elf_filename) #need to use offsets from original ELF for adding segments

  #COPY SEGMENTS INTO ELF - added segments not included here
  original_seg_start_offset = roundup(original_elf_header.e_phoff + original_elf_header.e_phentsize*original_elf_header.e_phnum, PAGE_SIZE) #MAY NOT ALWAYS BE ALIGNED TO 4KB (PAGE_SIZE)
  seg_start_offset = roundup(elf_header.e_phoff + elf_header.e_phentsize*elf_header.e_phnum, PAGE_SIZE) 
  for i in range (seg_num - num_segs_added):
    segment_offset_in = (original_offset_list[i] + original_seg_start_offset) #CHECK IF CORRECT TO USE ALIGNMENT***************
    segment_offset_out = roundup((int(Entry.get(phdr_struct.offset[i]), 16) + seg_start_offset), int(entry_struct.segalign.get(), 16)) #VARIABLE SEGMENT ALIGNMENT
    seg_size = int(Entry.get(phdr_struct.fsize[i]), 16)
    if not phdr_struct.binary[i].get(): #if no binary given for segment, use original ELF file
      seg_bytes_written = elf_gen_tools.file_copy_offset(elf_fp,
                 segment_offset_in,
                                               m_elf_fp,
                 segment_offset_out ,
                                               seg_size )
      # # update data segment offset to be aligned after previous segment
      # segment_offset += roundup(new_phdr.p_filesz, SEGMENT_ALIGN);

    else: #use binary if one is given
      # print 'BINARY HERE', i
      bin_fp = elf_gen_tools.OPEN(phdr_struct.binary[i].get(), "rb")
      seg_bytes_written = elf_gen_tools.file_copy_offset(bin_fp,
                 0,
                                               m_elf_fp,
                 segment_offset_out ,
                                               seg_size )
      #Append with F's if size of binary is smaller than 'fielsize' entry
      binary_size = os.stat(phdr_struct.binary[i].get()).st_size    #gives size of binary listed in bytes
      if(binary_size < int(Entry.get(phdr_struct.fsize[i]), 16)):       #specified offset is larger than file size - append with 0's
        m_elf_fp.seek(segment_offset_out + binary_size)  #go to line in modified ELF
        for i in range (seg_size - binary_size):
          m_elf_fp.write(b'\xFF')
      bin_fp.close()

  #COPY EACH ADDED SEGMENT INTO ELF - MAKES SURE 
  for i in range (num_segs_added):                          #start at first added segment entry
    # path_idx = (seg_num - num_segs_added + i)*11
    path_idx = (seg_num - num_segs_added + i)
    segment_offset_out = roundup((int(Entry.get(phdr_struct.offset[path_idx]), 16) + seg_start_offset), int(entry_struct.segalign.get(), 16))
    # print "offset", hex(segment_offset_out)
    seg_size = int(Entry.get(phdr_struct.fsize[path_idx]), 16)
    # print "added path", phdr_struct.binary[path_idx].get()
    bin_fp = elf_gen_tools.OPEN(phdr_struct.binary[path_idx].get(), "rb")
    seg_bytes_written = elf_gen_tools.file_copy_offset(bin_fp,
               0,
                                             m_elf_fp,
               segment_offset_out,
                                             seg_size )
    #Append with F's if size of binary is smaller than 'fielsize' entry
    binary_size = os.stat(phdr_struct.binary[path_idx].get()).st_size    #gives size of binary listed in bytes
    if(binary_size < int(Entry.get(phdr_struct.fsize[path_idx]), 16)):       #specified offset is larger than file size - append with 0's
      m_elf_fp.seek(segment_offset_out + binary_size)  #go to line in modified ELF
      for i in range (seg_size - binary_size):
        m_elf_fp.write(b'\xFF')
    bin_fp.close()

  print "\nELF File Created Successfully"
  elf_fp.close()
  m_elf_fp.close()

  return m_elf_fp  

#############################################################################################
# disassemble_elf_raw - Parses an ELF file and places sections of file into different binaries
#                       called by command line directly or disassemble_elf grid_dispatcher
#############################################################################################
def disassemble_elf_raw(elf_fp, input_elf_filename, elf_info_out_json, output_path):
  hash_seg_found = False
  hash_idx = 0
  elf_info_dict = collections.OrderedDict({})
  
  #load elf header and phdr table
  [elf_header, phdr_table] = \
    elf_gen_tools.preprocess_elf_file(input_elf_filename) 
  #Create directory if it doesn't exist
  if not output_path: #if nothing in entry, default to working directory
    output_path = os.path.abspath(os.getcwd())
    output_path = os.path.join(str(output_path), AUTO_GEN_ITEM_FOLDER)
  elif output_path == os.getcwd():
    output_path = os.path.join(str(output_path), AUTO_GEN_ITEM_FOLDER)
  if not os.path.isdir(output_path):
    os.makedirs(output_path)

  # store input file name with path to be disassembled.
  elf_info_dict[INPUT_ELF] = input_elf_filename
  # store output path where disassembled files are stored.
  elf_info_dict[DISASSEMBLE_PATH] = output_path
  
  #write ELF header
  elf_hdr_path = os.path.join(output_path, EHDR_BIN)
  elf_header_fp = elf_gen_tools.OPEN(elf_hdr_path, "wb+")
  if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_32:
    elf_header_fp.write(elf_gen_tools.Elf32_Ehdr.getPackedData(elf_header))
    elf_class = str(ELFINFO_CLASS_32)
  else:
    elf_header_fp.write(elf_gen_tools.Elf64_Ehdr.getPackedData(elf_header))
    elf_class = str(ELFINFO_CLASS_64)
  elf_entry_point = elf_header.e_entry

  # store disassembled elf's class(32-bit or 64-bit).
  elf_info_dict[E_CLASS] = hex(ord(elf_class))
  # store disassembled elf's elf_entry_address.
  elf_info_dict[E_ENTRY] = hex(elf_entry_point)
  # store disassembled elf-header filename with path.
  elf_info_dict[EHDR] = EHDR_BIN

  #write PHDR
  phdr_size = elf_header.e_phentsize * elf_header.e_phnum
  phdr_path = os.path.join(str(output_path),
                           PHDR_BIN)
  phdr_fp = elf_gen_tools.OPEN(phdr_path, "wb+")
  phdr_bytes_written = elf_gen_tools.file_copy_offset(elf_fp,
                                                      elf_header.e_phoff,
                                                      phdr_fp,
                                                      0,
                                                      phdr_size)
  
  # store disassembled program-header filename with path.
  elf_info_dict[PHDR] = PHDR_BIN

  #Copy each 'original' segment into separate files
  seg_start_offset = roundup(elf_header.e_phoff + phdr_size, PAGE_SIZE) #must be on one line, cant += string and int
  for i in range (elf_header.e_phnum):
    segment_name = os.path.join("segment_" + str(i) + ".bin")
    # store disassembled segment filename with path.
    elf_info_dict[SEGMENT+str(i)] = segment_name
    segment_path = os.path.join(str(output_path), segment_name)
    segment_fp = elf_gen_tools.OPEN(segment_path, "wb+")  #create files 
    seg_size = phdr_table[i].p_filesz  #segment size
    segment_offset = phdr_table[i].p_offset
    seg_bytes_written = elf_gen_tools.file_copy_offset(elf_fp,
                                                       segment_offset,
                                                       segment_fp,
                                                       0 ,
                                                       seg_size )
    segment_fp.close()
    #find hash segment for elf metadata binary
    if(MI_PBT_PAGE_MODE_VALUE(phdr_table[i].p_flags) == MI_PBT_PAGED_SEGMENT):
      hash_idx = i
      hash_seg_found = True

  #ELF METADATA BINARY
  metadata_path = os.path.join(str(output_path),ELF_METADATA_BIN)
  metadata_fp = elf_gen_tools.OPEN(metadata_path, "wb+")

  #write elf header in metadata file
  if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_32:
    metadata_fp.write(elf_gen_tools.Elf32_Ehdr.getPackedData(elf_header))
  else:
    metadata_fp.write(elf_gen_tools.Elf64_Ehdr.getPackedData(elf_header))

  #write phdr in metadata file
  metadata_bytes_written = elf_gen_tools.file_copy_offset(elf_fp,
                                                          elf_header.e_phoff,
                                                          metadata_fp,
                                                          elf_header.e_phoff,
                                                          phdr_size)
  # store disassembled elf_meta_data filename with path.
  elf_info_dict[ELF_METADAT] = ELF_METADATA_BIN

  #write hash segment in metadata file
  if(hash_seg_found):
    seg_start_offset = roundup(elf_header.e_phoff + phdr_size, PAGE_SIZE) #offset into start of segments
    output_hash_offset = elf_header.e_phoff + phdr_size   #no alignment of here PAGE_SIZE
    hash_offset = phdr_table[hash_idx].p_offset
    hash_size = phdr_table[hash_idx].p_filesz  #segment size
    metadata_bytes_written = elf_gen_tools.file_copy_offset(elf_fp,
                                                            hash_offset,
                                                            metadata_fp,
                                                            output_hash_offset,
                                                            hash_size)
  #metadata_fp.name
  metadata_fp.close()
  elf_header_fp.close()
  phdr_fp.close()
  if not elf_info_out_json:
    elf_info_out_json = os.path.join(output_path, DISASSEMBLED_ELF_INFO_JSON)
  with open(elf_info_out_json, "w") as elf_info_out_json_fp:
    json.dump(collections.OrderedDict(elf_info_dict), elf_info_out_json_fp, separators=(',', ':'), indent=4)
  elf_info_out_json_fp.close()

  print "\n" + ELF_GENERATOR_SCRIPT + ": Disassembled elf info generated in:\"" + elf_info_out_json + "\"" 
  return 0

##############################################################################
# main - Parses script arguments and runs elf_gen tool
##############################################################################
if __name__ == "__main__":
  if sys.version_info < (2, 7, 3):
    print "Must use Python 2.7.3 or greater"
    sys.exit(1)

  #parse arguments
  parser = OptionParser(usage='usage: %prog [options] arguments')

  parser.add_option("-f", "--first_filepath",
                    action="store", type="string", dest="elf_input",
                    help="First ELF file to merge.")

  parser.add_option("-c", "--cfg",
                    action="store", type="string", dest="json_file",
                    help="JSON file to create ELF from")

  parser.add_option("-d", "--disassemble_elf",
                    action="store", dest="elf_to_be_disassembled",
                    help="Command to disassemble with input elf file.")
                    
  parser.add_option("-e", "--elf_info_out_json",
                    action="store", dest="elf_info_out_json",
                    help="Output json filename to capture disassembled elf information.")

  parser.add_option("-u", "--gui",
                    action="store_true", dest="u",
                    help="Choose ui (interface) to open GUI with ELF file created from config file")

  parser.add_option("-o", "--output-dir",
                    action="store", dest="output_directory",
                    help="Output directory path. Default is current directory.")

  parser.add_option("-t", "--tools_path",
                    action="store", type="string", dest="tools_path",
                    help="Path to tools directory containing dependent *.py scripts.")

  (options, args) = parser.parse_args()
  
  # Dependent Tools (elf_gen, elf_gen_tools.py etc.) path directory 
  if options.tools_path and os.path.isdir(options.tools_path):
    options.tools_path = os.path.abspath(options.tools_path)
  else:
    options.tools_path = os.getcwd()

  #disassemble input elf
  if options.elf_to_be_disassembled:
    print "\n" + ELF_GENERATOR_SCRIPT + ": Disassembling elf........."
    options.elf_to_be_disassembled = get_abspath_if_exist(options.elf_to_be_disassembled, parser,
                                                          "Input elf file must be supplied and exist for disassembling elf.")
    input_elf_fp = elf_gen_tools.OPEN(options.elf_to_be_disassembled, "rb")
    if (options.output_directory):
      options.output_directory = os.path.abspath(options.output_directory)
    if not options.elf_info_out_json or \
       not (os.path.isdir(os.path.split(os.path.abspath(options.elf_info_out_json))[0])):
      parser.error("Output json filename to capture disassembled elf information must exist.")
    disassemble_elf_raw(input_elf_fp, options.elf_to_be_disassembled, options.elf_info_out_json, options.output_directory)
    input_elf_fp.close()
  elif options.json_file and (not options.u): #Make a new ELF with data from json file
    print "\n" + ELF_GENERATOR_SCRIPT + ": Generating elf........."
    config_parser(options.json_file, False)
  else:
    command = "python " + options.tools_path + "/" + GUI_ELF_GENERATOR_SCRIPT 
    if options.json_file:
      command = command + " -c " + options.json_file
    if options.elf_input:
      command = command + " -f " + options.elf_input
    # Call elf_gen_gui script to handle input options with GUI interface.
    call_os_system( command )
  exit(0)