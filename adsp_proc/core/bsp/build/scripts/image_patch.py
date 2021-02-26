#!/usr/bin/env python
#
# image_patch.py
#
# GENERAL DESCRIPTION
#   Used to add/patch an elf file with an additional program header segment to
#   reflect the full memory usage based on a DEFAULT_PHYSPOOL specified in 
#   the provided xml configuration file. 
#
# Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
from optparse import OptionParser
import os
import struct
from xml.dom.minidom import parse

# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = '\x7f'
ELFINFO_MAG1              = 'E'
ELFINFO_MAG2              = 'L'
ELFINFO_MAG3              = 'F'
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = '\x01'
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = '\x01'
ELF_BLOCK_ALIGN           = 0x1000

# ELF Program Header Types
NULL_TYPE                 = 0x0
LOAD_TYPE                 = 0x1
DYNAMIC_TYPE              = 0x2
INTERP_TYPE               = 0x3
NOTE_TYPE                 = 0x4
SHLIB_TYPE                = 0x5
PHDR_TYPE                 = 0x6
TLS_TYPE                  = 0x7

#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in order
# to fill out the appropriate class in Python, or they are used to package up
# the Python class so that we may write the binary data out. 
#----------------------------------------------------------------------------
"""
      Format | C Type         | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *         | string      | 'X' bytes
    2) H     | unsigned short | integer     | 2 bytes
    3) I     | unsigned int   | integer     | 4 bytes
     
"""

#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIIIIIHHHHHH') 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Ehdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]
      self.e_entry        = unpacked_data[4]
      self.e_phoff        = unpacked_data[5]
      self.e_shoff        = unpacked_data[6]
      self.e_flags        = unpacked_data[7]
      self.e_ehsize       = unpacked_data[8]
      self.e_phentsize    = unpacked_data[9]
      self.e_phnum        = unpacked_data[10]
      self.e_shentsize    = unpacked_data[11]
      self.e_shnum        = unpacked_data[12]
      self.e_shstrndx     = unpacked_data[13]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
      values = [self.e_ident,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]
      
      return (Elf32_Ehdr.s).pack(*values)


class Elf32_Phdr:
   # Structure object to align and package the ELF Program Header 
   s = struct.Struct('I' * 8) 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_offset       = unpacked_data[1]
      self.p_vaddr        = unpacked_data[2]
      self.p_paddr        = unpacked_data[3]
      self.p_filesz       = unpacked_data[4]
      self.p_memsz        = unpacked_data[5]
      self.p_flags        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value
            
   def getPackedData(self):
      values = [self.p_type,
         self.p_offset,
         self.p_vaddr,
         self.p_paddr,
         self.p_filesz,
         self.p_memsz,
         self.p_flags,
         self.p_align
      ]
      return (Elf32_Phdr.s).pack(*values)
   
def OPEN(file_name, mode):   
   try:
      fp = open(file_name, mode)
   except IOError:
      raise RuntimeError, "The file could not be opened: " + file_name

   # File open has succeeded with the given mode, return the file object
   return fp

#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
   if (elf_header.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0) or \
      (elf_header.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1) or \
      (elf_header.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2) or \
      (elf_header.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3) or \
      (elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_32) or \
      (elf_header.e_ident[ELFINFO_VERSION_INDEX] != ELFINFO_VERSION_CURRENT):

      return False
   else:
      return True

#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def preprocess_elf_file(elf_file_name):

   # Initialize
   print elf_file_name
   elf_fp = OPEN(elf_file_name, 'rb')
   elf_header = Elf32_Ehdr(elf_fp.read(ELF_HDR_SIZE))
   phdr_table = []

   # Verify ELF header information
   if verify_elf_header(elf_header) is False:
      raise RuntimeError, "ELF file failed verification: " + elf_file_name

   # Get program header size
   phdr_size = elf_header.e_phentsize

   # Find the program header offset 
   file_offset = elf_header.e_phoff
   elf_fp.seek(file_offset) 
   
   # Read in the program headers
   for i in range(elf_header.e_phnum):
      phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

   elf_fp.close()
   return [elf_header, phdr_table]

#----------------------------------------------------------------------------
# Add a dummy segment
# input elf
# input xml file to retrieve DEFAULT_PHYSPOOL
#----------------------------------------------------------------------------
def add_dummy_seg(input_elf, input_xml):   
   
   #Total size/Default physpool size, get from input_xml
   dom = parse(input_xml)
   default_physpool = "0x0"
   for node in dom.getElementsByTagName('physical_pool'):
      if node.getAttribute('name') == 'DEFAULT_PHYSPOOL':
         for region in node.getElementsByTagName('region'):
            default_physpool = int(default_physpool, 16) + int(region.getAttribute('size'),16)

   
   #Open ELF file
   elf_in_fp = OPEN(input_elf, "r+b")
   
   #Initialize
   [elf_header, phdr_table] = preprocess_elf_file(input_elf)

   
   #find all legitimate load segments
   loadable_segment_phdr_table = []
   for i in range(0, len(phdr_table)):
       if (\
            phdr_table[i].p_type is LOAD_TYPE and \
            ((phdr_table[i].p_flags) & (0x7 << 24)) != (0x2 << 24) and \
            phdr_table[i].p_memsz != 0\
          ):
            
            loadable_segment_phdr_table.append(phdr_table[i]) 
            
   

   
   addrtable = []
   for i in range(0, len(loadable_segment_phdr_table)):
       addrtable.append(loadable_segment_phdr_table[i].p_paddr)
   minaddr = min(addrtable)  
   maxaddr = max(addrtable)  
   
   #index of max address
   index_of_maxaddr = addrtable.index(maxaddr)
   
   #fill values for new phdr
   #ph = phdr_table[len(phdr_table)-1]
   ph = loadable_segment_phdr_table[index_of_maxaddr]
   alignsz =  ph.p_align

   next_fstart = ((ph.p_offset + ph.p_filesz) + alignsz) & ~(alignsz - 1)
   next_vstart = ((ph.p_vaddr + ph.p_memsz) + alignsz) & ~(alignsz - 1)
   next_pstart = ((ph.p_paddr + ph.p_memsz) + alignsz) & ~(alignsz - 1)
   
   new_phdr = Elf32_Phdr('\0' * ELF_PHDR_SIZE)
   new_phdr.p_type   = LOAD_TYPE
   new_phdr.p_offset = next_fstart
   new_phdr.p_vaddr  = next_vstart
   new_phdr.p_paddr  = next_pstart
   new_phdr.p_align  = ph.p_align
   new_phdr.p_flags  = ph.p_flags
       
   
   
   #current image size: difference of max and min base addresses + size of the highest addressed segment, which was calculated in next_pstart above
   current_image_size = next_pstart - minaddr
	   
   if current_image_size > default_physpool:
      print "Error: current image size (", hex(current_image_size), ") greater than default_physpool size (", hex(default_physpool), ")"
      sys.exit(2)

   #subtract that number from physpool size. this is the size of the dummy segment.      
   new_phdr.p_memsz  = default_physpool - current_image_size
   
   #alignsz should have been at least 1k, but in case it wasn't, align the new segment size to at least 1k.  
   new_phdr.p_memsz = new_phdr.p_memsz & 0xFFFFF000 
   
   print "Creating Dummy Segment to account for full image size"
   print "Image size: ", hex(current_image_size)
   print "PhysPool size: ", hex(default_physpool)
   print "Dummy segment size: ", hex(new_phdr.p_memsz)
   print "Dummy segment address: ", hex(new_phdr.p_paddr)
   
   new_phdr.p_filesz = 0
   
   elf_header.e_phnum = elf_header.e_phnum + 1
   
   elf_in_fp.write(elf_header.getPackedData())

   file_offset = elf_header.e_phoff
   elf_in_fp.seek(file_offset + (len(phdr_table)*elf_header.e_phentsize))
   elf_in_fp.write(new_phdr.getPackedData())

   elf_in_fp.close()      
       

def main(args):   
   add_dummy_seg(args[1], args[2])   
   
try:
   main(sys.argv)
except KeyboardInterrupt:
   pass
except SystemExit, exit_code:
   sys.exit(exit_code)

