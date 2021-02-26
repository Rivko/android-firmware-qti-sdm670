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
            
    def convert_to_64(self, file_handler):
        s64 = struct.Struct('16sHHIQQQIHHHHHH')
#         s64 = struct.Struct('16sHHIIIIIHHHHHH')
        # modify EI_CLASS to 64bit in e_indent
        self.e_indent = b'\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00'
        self.e_ehsize = ELF64_HEAD_SIZE
        self.e_phentsize = ELF64_PHDR_SIZE
        self.e_shentsize = ELF64_SHDR_SIZE
        self.e_phoff = PHDR_OFFSET
        self.e_shnum = 0
        self.e_shoff = 0
        self.e_shstrndx = 0
#         print(self.e_ehsize)
        data = [self.e_indent,
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
                self.e_shstrndx,
                ]
        packed_data = s64.pack(*data)
        
        file_handler.write(packed_data)

        return s64.size

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
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
        print("ATTRIBUTE / VALUE")
        for attr, value in self.__dict__.items():
            print(attr, value)
            
    def convert_to_64(self,file_handler, offset):
        s64 = struct.Struct('IIQQQQQQ') 
#         s64 = struct.Struct('I' * 8) 
        self.p_offset = offset
        data = [self.p_type,
                self.p_flags,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_align,
                ]
        packed_data = s64.pack(*data)
        file_handler.write(packed_data)
            
def load_data(file_handler, offset, size):   
    file_handler.seek(offset)
    return file_handler.read(size)
             
def roundup(num, align):
    return (num + (align-1)) & ~(align-1)

def convert_Elf(elf64, source):
    elf32 = open(source, "rb")
    #elf64 = open(destination, "wb")
    
    # read ELF Header from input ELF file
    elf32_Ehdr = Elf32_Ehdr(load_data(elf32, 0, ELF32_HEAD_SIZE)) 
    
    # program header table address will be modified in class later, so keep it at this point
    phdr_tbl_addr = elf32_Ehdr.e_phoff

    # convert ELF header to 64-bit 
    elf32_Ehdr.convert_to_64(elf64)
    
    prev_pfilesz = 0
    prev_align = 0  
    prev_address = 0
    
    # conver ELF program header table to 64-bit
    for i in range(elf32_Ehdr.e_phnum):
        data = load_data(elf32, phdr_tbl_addr + i * ELF32_PHDR_SIZE, ELF32_PHDR_SIZE)
        elf32_Phdr = Elf32_Phdr(data)
        
        # for each program header, copy its corresponding segment into ELF64
        segment = load_data(elf32, elf32_Phdr.p_offset, elf32_Phdr.p_filesz )

        f = open("test.txt","wb")
        f.write(segment)
        f.close()
        
        if i == 0:
            address = SEG_OFFSET

        else:
#             address = SEG_OFFSET + elf32_Phdr.p_offset - prev_offset
            address =  roundup(prev_address + prev_pfilesz, prev_align)
       
        prev_pfilesz = elf32_Phdr.p_filesz    
        prev_align = elf32_Phdr.p_align    
        prev_address = address
        
        elf64.seek(address)
        elf64.write(segment)
        
        # store the program header table at specified offset address 

        elf64.seek(elf32_Ehdr.e_phoff + i*ELF64_PHDR_SIZE, 0)
            
        elf32_Phdr.convert_to_64(elf64, address)
    
    elf32.close()
    #elf64.close()


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
    
    
def combine_ELF64(destination, source1, source2):
    target = open(destination, "wb")
    
    src1 = open(source1, "rb")
    src2 = open(source2, "rb")
    
    # read ELF Header from source1 file
    elf64_Ehdr1 = Elf64_Ehdr(load_data(src1, 0, ELF64_HEAD_SIZE)) 

    # read ELF program header table from source1 
    Phdr_tbl1 = []
    for i in range(elf64_Ehdr1.e_phnum):
        data = load_data(src1, elf64_Ehdr1.e_phoff + i * elf64_Ehdr1.e_phentsize, elf64_Ehdr1.e_phentsize)
        elf64_Phdr = Elf64_Phdr(data, src1)
        Phdr_tbl1.append(elf64_Phdr)
        
#         target.write(data)
        
    # read ELF Header from source2 file
    elf64_Ehdr2 = Elf64_Ehdr(load_data(src2, 0, ELF64_HEAD_SIZE)) 
    
    # read ELF program header table from source2
    Phdr_tbl2 = []
    for i in range(elf64_Ehdr2.e_phnum):
        data = load_data(src2, elf64_Ehdr2.e_phoff + i * elf64_Ehdr2.e_phentsize, elf64_Ehdr2.e_phentsize)
        elf64_Phdr = Elf64_Phdr(data, src2)
        Phdr_tbl2.append(elf64_Phdr)
        
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
    new_Ehdr = copy.deepcopy(elf64_Ehdr1)
    new_Ehdr.e_phnum = elf64_Ehdr1.e_phnum + elf64_Ehdr2.e_phnum

    # Use the monitor's ELF header as the base so the correct entry
    # point is preserved.
    new_Ehdr.e_entry = elf64_Ehdr2.e_entry
    
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

        target.seek(elf64_Ehdr1.e_phoff + i*ELF64_PHDR_SIZE, 0)
        Phdr_list[i].p_offset = address
        target.write(Phdr_list[i].getPackedData())

    src1.close()
    src2.close()
    target.close()


def combine_ELF64_ext(destination, source1, source2, source3):
    target = open(destination, "wb")
    
    src1 = open(source1, "rb")
    src2 = open(source2, "rb")
    src3 = open(source3, "rb")
    
    # read ELF Header from source1 file
    elf64_Ehdr1 = Elf64_Ehdr(load_data(src1, 0, ELF64_HEAD_SIZE)) 

    # read ELF program header table from source1 
    Phdr_tbl1 = []
    for i in range(elf64_Ehdr1.e_phnum):
        data = load_data(src1, elf64_Ehdr1.e_phoff + i * elf64_Ehdr1.e_phentsize, elf64_Ehdr1.e_phentsize)
        elf64_Phdr = Elf64_Phdr(data, src1)
        Phdr_tbl1.append(elf64_Phdr)
        
    #target.write(data)
        
    # read ELF Header from source2 file
    elf64_Ehdr2 = Elf64_Ehdr(load_data(src2, 0, ELF64_HEAD_SIZE)) 
    
    # read ELF program header table from source2
    Phdr_tbl2 = []
    for i in range(elf64_Ehdr2.e_phnum):
        data = load_data(src2, elf64_Ehdr2.e_phoff + i * elf64_Ehdr2.e_phentsize, elf64_Ehdr2.e_phentsize)
        elf64_Phdr = Elf64_Phdr(data, src2)
        Phdr_tbl2.append(elf64_Phdr)
        
    # read ELF Header from source2 file
    elf64_Ehdr3 = Elf64_Ehdr(load_data(src3, 0, ELF64_HEAD_SIZE)) 
    
    # read ELF program header table from source2
    Phdr_tbl3 = []
    for i in range(elf64_Ehdr3.e_phnum):
        data = load_data(src3, elf64_Ehdr3.e_phoff + i * elf64_Ehdr3.e_phentsize, elf64_Ehdr3.e_phentsize)
        elf64_Phdr = Elf64_Phdr(data, src3)
        Phdr_tbl3.append(elf64_Phdr)
        
    #target.write(data)

    # combine two program header table, and sort it ascending by VA
    Phdr_list = Phdr_tbl1 + Phdr_tbl2 + Phdr_tbl3
     
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
    new_Ehdr = copy.deepcopy(elf64_Ehdr1)
    new_Ehdr.e_phnum = elf64_Ehdr1.e_phnum + elf64_Ehdr2.e_phnum + elf64_Ehdr3.e_phnum

    # Use the monitor's ELF header as the base so the correct entry
    # point is preserved.
    new_Ehdr.e_entry = elf64_Ehdr2.e_entry
    
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

        target.seek(elf64_Ehdr1.e_phoff + i*ELF64_PHDR_SIZE, 0)
        Phdr_list[i].p_offset = address
        target.write(Phdr_list[i].getPackedData())

    src1.close()
    src2.close()
    src3.close()
    target.close()

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('elf64_builder')

def generate(env):
   elf64_generate(env)
   elf64_ext_generate(env)

#===============================================================================
# ELF64 build rules
#===============================================================================
def elf64_generate(env):
   elf64_act = env.GetBuilderAction(elf64_builder)
   elf64_bld = env.Builder(action = elf64_act)

   env.Append(BUILDERS = {'ELF64Builder' : elf64_bld})
   #env.ELF64Builder('tz_64.elf', 'tz.elf')
   
#===============================================================================
# ELF64 EXT build rules
#===============================================================================
def elf64_ext_generate(env):
   elf64_ext_act = env.GetBuilderAction(elf64_ext_builder)
   elf64_ext_bld = env.Builder(action = elf64_ext_act)

   env.Append(BUILDERS = {'ELF64ExtBuilder' : elf64_ext_bld})
   #env.ELF64Builder('tz_64.elf', 'tz.elf')

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#-------------------------------------------------------------------------------
def elf64_emitter(target, source, env):

   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

#-------------------------------------------------------------------------------

def elf64_builder(target, source, env):

   source_qsee64_str = str(source[0])
   source_mon64_str = str(source[1])
   target_str = str(target[0])
   
   combine_ELF64(target_str, source_qsee64_str, source_mon64_str)
   
   return None

#-------------------------------------------------------------------------------

def elf64_ext_builder(target, source, env):

   source_qsee64_str = str(source[0])
   source_mon64_str = str(source[1])
   source_devcfg64_str = str(source[2])
   target_str = str(target[0])
   
   combine_ELF64_ext(target_str, source_qsee64_str, source_mon64_str, source_devcfg64_str)
   
   return None   
