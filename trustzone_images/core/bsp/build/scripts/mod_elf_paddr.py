#! /usr/bin/env python
"""
usage: python mod_elf_padd.py vss.elf <output_elf_name>
increment each(program headers) ph.p_paddr by 0x8F500000.
change elf.entry to 0x0F500000

"""

import sys
import stat
import csv 
import subprocess
import re
import struct
import itertools
import os
import shutil

# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
    # Structure object to align and package the ELF Header
    #s = struct.Struct('16sHHIIIIIHHHHHH') 
    s = struct.Struct('4sHHIIIIIHHHHHH') 
    
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
            print attr,value

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

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

    # Structure object to align and package the ELF Program Header 
    s = struct.Struct('IIIIIIII') 
    
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

    def printStructValues(self):
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
        
def die(message):
    print message
    exit(1)

def usage():
    print "Usage: %s <elf> <prefix>" % sys.argv[0]
    exit(1)
    
def is_elf(file):
    """Verifies a file as being an ELF file"""
    file.seek(0)
    magic = file.read(4)
    return magic == '\x7fELF'


if __name__ == "__main__":

    if len(sys.argv) != 3:
        usage()

        # Initialize
    image = open(sys.argv[1], 'rb')
    image_out = open(sys.argv[2], "wb+")
    
    # Copy input file to output file
    shutil.copyfileobj(image, image_out, -1)
    
    if not is_elf(image):
        usage()
            
    #Read the 1st 16 bytes (e_ident)
    image.seek(12)
    #elf_header = Elf32_Ehdr(image.read(ELF_HDR_SIZE))
    elf_header = Elf32_Ehdr(image.read(40))
    phdr_table = []
    #elf_header.printValues()
    #set elf header entrypoint to 0x0F500000
    elf_header.e_entry = 0x8F500000
    #print "entrypoint =", elf_header.e_entry
    image_out.seek(12)
    image_out.write(elf_header.getPackedData())
    
    # Get program header size
    phdr_size = elf_header.e_phentsize
    #print phdr_size

    # Find the program header offset 
    file_offset = elf_header.e_phoff
    #print file_offset
    image.seek(file_offset) 
    image_out.seek(file_offset)
    
    # Read in the program headers
    for i in range(elf_header.e_phnum):
        curr_phdr = Elf32_Phdr(image.read(32))
        
    
    #for i in range(elf_header.e_phnum):
        #curr_phdr = phdr_table[i]
        
        #curr_phdr.printStructValues()
        # incr current_phdr lma by 0x0F500000
        curr_phdr.p_paddr += 0x8F500000
        #print curr_phdr.p_paddr
        image_out.write(curr_phdr.getPackedData())
