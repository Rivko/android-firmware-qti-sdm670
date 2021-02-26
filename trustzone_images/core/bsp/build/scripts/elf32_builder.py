#===============================================================================
#
# HEX file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2014 Qualcomm Technologies, Inc .
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: 
#  $DateTime: 2018/02/06 02:57:17 $
#  $Author: pwbldsvc $
#  $Change: 15399902 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 10/31/14    aa     Add Ext API for merging 3 elfs (qsee, mon and devcfg)
# 01/28/14    ygui   convert 32bit image to 64bit, and merge two 64bit images
#===============================================================================
import os
import subprocess
import string
import re
import tempfile

#======================================================================
#ELF Converter

import struct

ELF32_HEAD_SIZE = 52
ELF64_HEAD_SIZE = 64
ELF32_PHDR_SIZE = 32
ELF64_PHDR_SIZE = 56
ELF32_SHDR_SIZE = 40
ELF64_SHDR_SIZE = 64
PHDR_OFFSET     = 0x100
SEG_OFFSET      = 0x1000

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
      Format | C Type             | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *             | string      | 'X' bytes
    2) H     | unsigned short     | integer     | 2 bytes
    3) I     | unsigned int       | integer     | 4 bytes
    4) Q     | unsigned long long | integer     | 8 bytes
"""

#----------------------------------------------------------------------------
# ELF32 Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:    
    # Structure object to align and package the ELF Header
    s = struct.Struct('16sHHIIIIIHHHHHH')
   
     
    def __init__(self, data):
        unpacked_data       = (Elf32_Ehdr.s).unpack(data)
        self.unpacked_data  = unpacked_data
        self.e_indent       = unpacked_data[0]
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
        
    def printValue(self):
        print("ATTRIBUTE / VALUE")
        for attr, value in self.__dict__.items():
            print("{0} {1}".format(attr, value))
            
    def getPackedData(self):
        values = [self.e_indent,
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

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

    # Structure object to align and package the ELF Program Header 
    s = struct.Struct('I' * 8) 
    
    def __init__(self, data, file):
        unpacked_data       = (Elf32_Phdr.s).unpack(data)
        self.file           = file
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
        print("ATTRIBUTE / VALUE")
        for attr, value in self.__dict__.items():
            print(attr, value)

    def getPackedData(self):
        values = [
                  self.p_type,
                  self.p_offset,
                  self.p_vaddr,
                  self.p_paddr,
                  self.p_filesz,
                  self.p_memsz,
                  self.p_flags,
                  self.p_align
                  ]

        return (Elf32_Phdr.s).pack(*values)
            
                    
def load_data(file_handler, offset, size):   
    file_handler.seek(offset)
    return file_handler.read(size)
             
def roundup(num, align):
    return (num + (align-1)) & ~(align-1)

#========================================================================


#==========================================================================
#combine TZ with monitor image to form a new one

import copy


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
      Format | C Type             | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *             | string      | 'X' bytes
    2) H     | unsigned short     | integer     | 2 bytes
    3) I     | unsigned int       | integer     | 4 bytes
    4) Q     | unsigned long long | integer     | 8 bytes
"""
#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf64_Ehdr:
    # Structure object to align and package the ELF Header
    s = struct.Struct('16sHHIQQQIHHHHHH') 
   
    def __init__(self, data): 
        unpacked_data       = (Elf64_Ehdr.s).unpack(data)
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
        print("ATTRIBUTE / VALUE")
        for attr, value in self.__dict__.iteritems():
            print(attr, value)

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
      
        return (Elf64_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf64_Phdr:

    # Structure object to align and package the ELF Program Header 
    s = struct.Struct('IIQQQQQQ') 
   
    def __init__(self, data, file): 
        unpacked_data       = (Elf64_Phdr.s).unpack(data)
        self.file           = file
        self.unpacked_data  = unpacked_data
        self.p_type         = unpacked_data[0]
        self.p_flags       = unpacked_data[1]
        self.p_offset        = unpacked_data[2]
        self.p_vaddr        = unpacked_data[3]
        self.p_paddr       = unpacked_data[4]
        self.p_filesz        = unpacked_data[5]
        self.p_memsz        = unpacked_data[6]
        self.p_align        = unpacked_data[7]

    def printValues(self):
        print("ATTRIBUTE / VALUE")
        for attr, value in self.__dict__.items():
            print(attr, value)
            
    def getPackedData(self):
        values = [
                  self.p_type,
                  self.p_flags,
                  self.p_offset,
                  self.p_vaddr,
                  self.p_paddr,
                  self.p_filesz,
                  self.p_memsz,
                  self.p_align
                  ]
      
        return (Elf64_Phdr.s).pack(*values)


def roundup(num, align):
    return (num + (align-1)) & ~(align-1)

def check_overlap(src1, len1, src2, len2):
    return not (src1 + len1 -1 < src2 or src2 + len2 -1 < src1)
    
def load_data(file_handler, offset, size):   
    file_handler.seek(offset)
    return file_handler.read(size)    
    
    
def combine_ELF32(destination, source1, source2):
    target = open(destination, "wb")
    
    src1 = open(source1, "rb")
    src2 = open(source2, "rb")
    
    # read ELF Header from source1 file
    elf32_Ehdr1 = Elf32_Ehdr(load_data(src1, 0, ELF32_HEAD_SIZE)) 

    # read ELF program header table from source1 
    Phdr_tbl1 = []
    for i in range(elf32_Ehdr1.e_phnum):
        data = load_data(src1, elf32_Ehdr1.e_phoff + i * elf32_Ehdr1.e_phentsize, elf32_Ehdr1.e_phentsize)
        elf32_Phdr = Elf32_Phdr(data, src1)
        Phdr_tbl1.append(elf32_Phdr)
        
#         target.write(data)
        
    # read ELF Header from source2 file
    elf32_Ehdr2 = Elf32_Ehdr(load_data(src2, 0, ELF32_HEAD_SIZE)) 
    
    # read ELF program header table from source2
    Phdr_tbl2 = []
    for i in range(elf32_Ehdr2.e_phnum):
        data = load_data(src2, elf32_Ehdr2.e_phoff + i * elf32_Ehdr2.e_phentsize, elf32_Ehdr2.e_phentsize)
        elf32_Phdr = Elf32_Phdr(data, src2)
        Phdr_tbl2.append(elf32_Phdr)
        
#         target.write(data)

    # combine two program header table, and sort it ascending by VA
    Phdr_list = Phdr_tbl1 + Phdr_tbl2
     
    Phdr_list.sort(key=lambda phdr_vaddr: phdr_vaddr.p_vaddr) 

# TODO: enable for --verbose=3 builds maybe?
#    for phdr in Phdr_list:
#        phdr.printValues()
#        print()
#    print(len(Phdr_list))
    
    # checking overlap 
    for i in range(len(Phdr_list) - 1):
        if True == check_overlap(Phdr_list[i].p_vaddr, Phdr_list[i].p_memsz, Phdr_list[i-1].p_vaddr, Phdr_list[i-1].p_memsz):
            error_overlap = ValueError("There is overlap between the segment (address:0x{0:x}, len:0x{1:x}) and segment (address:0x{2:x}, len:0x{3:x})".format(Phdr_list[i].p_vaddr, Phdr_list[i].p_memsz, Phdr_list[i-1].p_vaddr, Phdr_list[i-1].p_memsz))
            raise error_overlap
            return 
        
    # update phnum in new ELF header.  
    new_Ehdr = copy.deepcopy(elf32_Ehdr1)
    new_Ehdr.e_phnum = elf32_Ehdr1.e_phnum + elf32_Ehdr2.e_phnum

    # Use the QSEE's ELF header as the base so the correct entry
    # point is preserved for badger family.
    new_Ehdr.e_entry = elf32_Ehdr1.e_entry
    
    # dump ELF header
    target.write(new_Ehdr.getPackedData())
    
    prev_pfilesz = 0
    prev_align = 0  
    prev_address = 0    
    
    
    # dump ELF segments
    for i in range(len(Phdr_list)):
        segment = load_data(Phdr_list[i].file, Phdr_list[i].p_offset, Phdr_list[i].p_filesz)
        
        f = open("test.txt","wb")
        f.write(segment)
        f.close()
    
        if i == 0:
            address = SEG_OFFSET

        else:
            address =  roundup(prev_address + prev_pfilesz, prev_align)
       
        prev_pfilesz = Phdr_list[i].p_filesz    
        prev_align = Phdr_list[i].p_align    
        prev_address = address
        
        target.seek(address)
        target.write(segment)
        
        # dump the program header table at specified offset address 

        target.seek(elf32_Ehdr1.e_phoff + i*ELF32_PHDR_SIZE, 0)
        Phdr_list[i].p_offset = address
        target.write(Phdr_list[i].getPackedData())

    src1.close()
    src2.close()
    target.close()

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('elf32_builder')

def generate(env):
   elf32_generate(env)

#===============================================================================
# ELF32 build rules
#===============================================================================
def elf32_generate(env):
   elf32_act = env.GetBuilderAction(elf32_builder)
   elf32_bld = env.Builder(action = elf32_act)

   env.Append(BUILDERS = {'ELF32Builder' : elf32_bld})
   
   
#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#-------------------------------------------------------------------------------
def elf32_emitter(target, source, env):

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------

def elf32_builder(target, source, env):

   source_qsee32_str = str(source[0])
   source_mon32_str = str(source[1])
   target_str = str(target[0])
   
   combine_ELF32(target_str, source_qsee32_str, source_mon32_str)
   
   return None


